﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Threading;

namespace APInvokeManaged
{
    public abstract class ClientBase
    {
        private static string _authId = "APInvoke Connection";
        private static string _authPwd = "";

        public static string AuthId
        {
            get { return _authId; }
            set { _authId = value; }
        }
        public static string AuthPwd
        {
            get { return _authPwd; }
            set { _authPwd = value; }
        }

        private ConnectionBase _connection;
        private Dictionary<string, Delegate> _requestDic = new Dictionary<string, Delegate>();
        private object _syncObj = new object();

        public event Action<string> OnError;
        public bool IsConnected { get; private set; }
        public string SessionId { get; private set; }

        public ClientBase()
        {
            _connection = new ConnectionBase();
            _connection.OnDataReceived += new Action<MsgType, byte[]>(_connection_OnDataReceived);
            _connection.OnError += new Action<string>(_connection_OnError);
        }

        public bool Connect(string address, int port)
        {
            lock(_syncObj)
            {
                bool connected = false;
                ConnectAsync(address, port, 
                    delegate(bool succ, string err)
                    {
                        lock (_syncObj)
                        {
                            connected = succ;
                            Monitor.Pulse(_syncObj);
                        }
                    });

                Monitor.Wait(_syncObj);
                return connected;
            }
        }

        public void ConnectAsync(string address, int port, Action<bool, string> connectCompletionHandler)
        {
            _connection.Address = address;
            _connection.Port = port;

            _connection.OpenAsync(
                delegate(bool succ, string err)
                {
                    if (succ)
                    {
                        // keep async reading
                        _connection.StartReading();

                        EstablishConnection(connectCompletionHandler);
                    }
                    else
                    {
                        if (connectCompletionHandler != null)
                            connectCompletionHandler(succ, err);
                    }
                });
        }

        public void Disconnect()
        {
            _connection.Close();
        }

        public byte[] Request(string method, byte[] reqData)
        {
            lock (_syncObj)
            {
                byte[] ret_data = null;
                RequestAsync(method, reqData,
                    delegate(bool succ, byte[] retData)
                    {
                        lock (_syncObj)
                        {
                            ret_data = retData;
                            Monitor.Pulse(_syncObj);
                        }
                    });

                Monitor.Wait(_syncObj);
                return ret_data;
            }
        }

        public void RequestAsync(string method, byte[] reqData, Action<bool, byte[]> responseHandler)
        {
            Packet.Request req = new Packet.Request()
            {
                session = SessionId,
                method = method,
                param_data = reqData
            };

            byte[] data = DataTranslater.Serialize(req);

            BookRequest(method, responseHandler);

            _connection.SendAsync(MsgType.REQ, data,
                delegate(bool succ, string msg)
                {
                    if (succ)
                    {
                        Trace.WriteLine(string.Format("Request {0} success.", method));
                    }
                    else
                    {
                        RemoveBookedRequest(method);
                        responseHandler(false, null);
                    }
                });
        }

        protected abstract void DispatchCallback(string method, byte[] paramData);

        void _connection_OnDataReceived(MsgType msgType, byte[] receivedData)
        {
            switch (msgType)
            {
                case MsgType.CONN_ACK:
                    OnEstablishConnectionAck(receivedData);
                    break;
                case MsgType.RSP:
                    OnResponse(receivedData);
                    break;
                case MsgType.CALLBK_REQ:
                    OnCallbackReq(receivedData);
                    break;
                default:
                    RaiseError(string.Format("Unexpected message ({0}) received", msgType));
                    break;
            }
        }


        void _connection_OnError(string errMsg)
        {
            if (!_connection.IsConnected)
                IsConnected = false;

            RaiseError(errMsg);
        }

        private void RaiseError(string errMsg)
        {
            if (OnError != null)
                OnError(errMsg);
        }

        private void EstablishConnection(Action<bool, string> connectCompletionHandler)
        {
            Packet.Connect connReq = new Packet.Connect();
            connReq.userid = AuthId;
            connReq.password = AuthPwd;

            byte[] data = DataTranslater.Serialize(connReq);

            BookRequest(typeof(Packet.Connect).Name, connectCompletionHandler);

            _connection.SendAsync(MsgType.CONN, data, 
                delegate(bool succ, string msg)
                {
                    if (succ)
                    {
                        Trace.WriteLine("Request establish connection");
                    }
                    else
                    {
                        RemoveBookedRequest(typeof(Packet.Connect).Name);
                        connectCompletionHandler(
                            false, string.Format("Attempt to establish connection failed due to {0}", msg));
                    }
                });
        }

        private void OnEstablishConnectionAck(byte[] data)
        {
            if (data != null && data.Length > 0)
            {
                Packet.ConnectAck connAck = DataTranslater.Deserialize<Packet.ConnectAck>(data);
                IsConnected = connAck.success;
                SessionId = connAck.session;
            }

            Delegate callback = GetResponseCallback(typeof(Packet.Connect).Name);
            if (callback != null)
            {
                Action<bool, string> connCompletion = callback as Action<bool, string>;
                if (connCompletion != null)
                    connCompletion(IsConnected, "");
            }
        }

        private void OnResponse(byte[] data)
        {
            bool recvSucc = false;
            byte[] respData = null;
            string method = string.Empty;
            if (data != null && data.Length > 0)
            {
                Packet.Response resp = DataTranslater.Deserialize<Packet.Response>(data);
                method = resp.method;
                respData = resp.return_data;

                recvSucc = !string.IsNullOrEmpty(resp.method);
            }

            Delegate callback = GetResponseCallback(method);
            if (callback != null)
            {
                Action<bool, byte[]> respCompletion = callback as Action<bool, byte[]>;
                if (respCompletion != null)
                    respCompletion(recvSucc, respData);
            }
        }

        private void OnCallbackReq(byte[] callbackReqData)
        {
            byte[] paramData = null;
            string method = string.Empty;
            if (callbackReqData != null && callbackReqData.Length > 0)
            {
                Packet.CallbackReq cbReq = DataTranslater.Deserialize<Packet.CallbackReq>(callbackReqData);
                method = cbReq.method;
                paramData = cbReq.param_data;

                DispatchCallback(method, paramData);                
            }
        }

        private void BookRequest(string method, Delegate rspCallback)
        {
            lock (_requestDic)
            {
                if(_requestDic.ContainsKey(method))
                    throw new InvalidOperationException(
                        string.Format("Last invoke of '{0}' has not completed yet", method));

                _requestDic.Add(method, rspCallback);
            }
        }

        private Delegate GetResponseCallback(string method)
        {
            lock(_requestDic)
            {
                if (_requestDic.ContainsKey(method))
                {
                    Delegate callback = _requestDic[method];
                    _requestDic.Remove(method);
                    return callback;
                }

                RaiseError(string.Format("Unexpected method response ({0})", method));
                return null;
            }
        }

        private void RemoveBookedRequest(string method)
        {
            lock (_requestDic)
            {
                _requestDic.Remove(method);
            }
        }
    }
}