﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using PortfolioTrading.Infrastructure;
using System.Threading;

namespace PortfolioTrading.Utils
{
    class ServerLoginParam
    {
        public string TradeAddress { get; set; }
        public string QuoteAddress { get; set; }
        public string BrokerId { get; set; }
        public string InvestorId { get; set; }
        public string UserId { get; set; }
        public string Password { get; set; }
    }

    delegate ServerLoginParam GetServerLoginInfoDel();
    delegate void OnLoginDoneDel(bool succ, string errorMsg);
    delegate void OnStatusUpdate(string statusMsg);

    class TradeStationConnector
    {
        private readonly Client _client;
        private readonly ClientHandlerImpl _handler;
        private readonly SynchronizationContext _uiContext;
        private readonly GetServerLoginInfoDel _getServerLoginParamFunc;

        private const int LoginTimeout = 30000; // milliseconds
        
        private readonly AutoResetEvent _eventLogin = new  AutoResetEvent(false);
        private bool _connectionEstablished = false;
        private string _connectionEstablishError = string.Empty;

        private readonly AutoResetEvent _eventTradeLogin = new AutoResetEvent(false);
        private bool _tradeLogged = false;
        private string _tradeLoggedError = string.Empty;

        private readonly AutoResetEvent _eventQuoteLogin = new AutoResetEvent(false);
        private bool _quoteLogged = false;
        private string _quoteLoggedError = string.Empty;

        private OnLoginDoneDel _onLoginDone;
        private OnStatusUpdate _onStatusUpdate;

        public TradeStationConnector(Client client, ClientHandlerImpl handler, SynchronizationContext uiContext,
            GetServerLoginInfoDel getServerLoginParam)
        {
            _client = client;
            _handler = handler;
            _handler.OnLogin += new Action<bool, string>(_handler_OnLogin);
            _handler.OnServerLogin += new Action<PTEntity.ServerType, bool, string>(_handler_OnServerLogin);
            _uiContext = uiContext;
            _getServerLoginParamFunc = getServerLoginParam;
        }

        void _handler_OnLogin(bool arg1, string arg2)
        {
            _connectionEstablished = arg1;
            _connectionEstablishError = arg2;
            _eventLogin.Set();

            _client.BeginHeartbeat();
        }

        void _handler_OnServerLogin(PTEntity.ServerType svrType, bool succ, string errorMsg)
        {
            if (svrType == PTEntity.ServerType.SERV_TRADE)    // Trade
            {
                _tradeLogged = succ;
                _tradeLoggedError = errorMsg;
                _eventTradeLogin.Set();
            }
            else    // Quote
            {
                _quoteLogged = succ;
                _quoteLoggedError = errorMsg;
                _eventQuoteLogin.Set();
            }
        }
        

        public void Login(string host, int port, OnLoginDoneDel onLoginDone, OnStatusUpdate onStatusUpdate)
        {
            _onLoginDone = onLoginDone;
            _onStatusUpdate = onStatusUpdate;

            RaiseStatusUpdate(string.Format("正在连接 {0}:{1} ...", host, port), false);

            Func<string, int, bool> connectFunc = _client.Connect;
            connectFunc.BeginInvoke(host, port, 
                delegate(IAsyncResult ar)
                {
                    bool connectSucc = false;
                    try
                    {
                        connectSucc = connectFunc.EndInvoke(ar);
                    }
                    catch (System.Exception ex)
                    {
                        LogManager.Logger.ErrorFormat("Failed to connect {0}:{1} due to {2}", host, port, ex.Message);
                    }

                    if (connectSucc)
                    {
                        // wait for client really connected
                        if (_eventLogin.WaitOne(LoginTimeout))
                        {
                            if (_connectionEstablished)
                            {
                                RaiseStatusUpdate("已经与服务器建立连接.");

                                ServerLoginParam tradeLoginInfo = _getServerLoginParamFunc();
                                RaiseStatusUpdate(string.Format("正在登录CTP交易 {0} ...", tradeLoginInfo.TradeAddress));
                                // begin login trade
                                _client.TradeLogin(tradeLoginInfo.TradeAddress, tradeLoginInfo.BrokerId, tradeLoginInfo.InvestorId, tradeLoginInfo.UserId, tradeLoginInfo.Password);

                                if (_eventTradeLogin.WaitOne(LoginTimeout))
                                {
                                    if (_tradeLogged)
                                    {
                                        RaiseStatusUpdate("登录CTP交易成功.");
                                        RaiseStatusUpdate(string.Format("正在登录CTP行情 {0}...", tradeLoginInfo.QuoteAddress));
                                        _client.QuoteLogin(tradeLoginInfo.QuoteAddress, tradeLoginInfo.BrokerId, tradeLoginInfo.InvestorId, tradeLoginInfo.UserId, tradeLoginInfo.Password);

                                        if(_eventQuoteLogin.WaitOne(LoginTimeout))
                                        {
                                            if (_quoteLogged)
                                            {
                                                RaiseStatusUpdate("登录CTP行情成功.");
                                                RaiseLoginDone(true);
                                            }
                                            else
                                            {
                                                RaiseLoginDone(false, _quoteLoggedError);
                                            }
                                        }
                                        else
                                        {
                                            RaiseLoginDone(false, "登录CTP行情超时");
                                        }
                                    }
                                    else
                                    {
                                        RaiseLoginDone(false, _tradeLoggedError);
                                    }
                                }
                                else
                                {
                                    RaiseLoginDone(false, "登录CTP交易超时");
                                }
                            }
                            else
                            {
                                RaiseLoginDone(false, _connectionEstablishError);
                            }
                        }
                        else
                        {
                            RaiseLoginDone(false, "连接服务器超时");
                        }
                    }
                    else
                    {
                        RaiseLoginDone(false, string.Format("无法连接到 {0}:{1}", host, port));
                    }
                    
                }, null);
        }

        private void RaiseLoginDone(bool succ, string errorMsg = "")
        {
            if (!succ)
                _client.Disconnect();

            if (_onLoginDone != null)
            {
                _uiContext.Send(o => _onLoginDone(succ, errorMsg), null);
            }
        }

        private void RaiseStatusUpdate(string statusMsg, bool switchThread = true)
        {
            if (_onStatusUpdate != null)
            {
                if (switchThread)
                    _uiContext.Post(o => _onStatusUpdate(statusMsg), null);
                else
                    _onStatusUpdate(statusMsg); 
            }
        }

    }
}
