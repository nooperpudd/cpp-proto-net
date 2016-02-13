﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.ViewModel;
using System.Collections.ObjectModel;
using PortfolioTrading.Utils;

namespace PortfolioTrading.Modules.Portfolio
{
    public class MultiLegOrderVM : NotificationObject
    {
        private ObservableCollection<OrderVM> _orders = new ObservableCollection<OrderVM>();

        public PTEntity.MultiLegOrder LastOrder { get; private set; }

        public MultiLegOrderVM(string acctId)
        {
            this.AccountId = acctId;
        }

        public string AccountId { get; private set; }

        #region OrderId
        private string _orderId;

        public string OrderId
        {
            get { return _orderId; }
            set
            {
                if (_orderId != value)
                {
                    _orderId = value;
                    RaisePropertyChanged("OrderId");
                }
            }
        }
        #endregion

        #region PortfolioId
        private string _portfolioId;

        public string PortfolioId
        {
            get { return _portfolioId; }
            set
            {
                if (_portfolioId != value)
                {
                    _portfolioId = value;
                    RaisePropertyChanged("PortfolioId");
                }
            }
        }
        #endregion

        #region Quantity
        private int _quantity;

        public int Quantity
        {
            get { return _quantity; }
            set
            {
                if (_quantity != value)
                {
                    _quantity = value;
                    RaisePropertyChanged("Quantity");
                }
            }
        }
        #endregion

        #region Profit
        private double _profit;

        public double Profit
        {
            get { return _profit; }
            set
            {
                if (_profit != value)
                {
                    _profit = value;
                    RaisePropertyChanged("Profit");
                }
            }
        }
        #endregion

        #region IsOpenOrder
        private bool _isOpenOrder;

        public bool IsOpenOrder
        {
            get { return _isOpenOrder; }
            set
            {
                if (_isOpenOrder != value)
                {
                    _isOpenOrder = value;
                    RaisePropertyChanged("IsOpenOrder");
                }
            }
        }
        #endregion

        #region IsAllFinished
        private bool _isAllFinished;

        public bool IsAllFinished
        {
            get { return _isAllFinished; }
            set
            {
                if (_isAllFinished != value)
                {
                    _isAllFinished = value;
                    RaisePropertyChanged("IsAllFinished");
                }
            }
        }
        #endregion

        #region IsCanceled
        private string _isCanceled;

        public string IsCanceled
        {
            get { return _isCanceled; }
            set
            {
                if (_isCanceled != value)
                {
                    _isCanceled = value;
                    RaisePropertyChanged("IsCanceled");
                }
            }
        }
        #endregion


        #region OpenOrderId
        private string _openOrderId;

        public string OpenOrderId
        {
            get { return _openOrderId; }
            set
            {
                if (_openOrderId != value)
                {
                    _openOrderId = value;
                    RaisePropertyChanged("OpenOrderId");
                }
            }
        }
        #endregion

        #region Reason
        private string _reason;

        public string Reason
        {
            get { return _reason; }
            set
            {
                if (_reason != value)
                {
                    _reason = value;
                    RaisePropertyChanged("Reason");
                }
            }
        }
        #endregion

        #region IsPortfolio
        private bool _isPortfolio;

        public bool IsPortfolio
        {
            get { return _isPortfolio; }
            set
            {
                if (_isPortfolio != value)
                {
                    _isPortfolio = value;
                    RaisePropertyChanged("IsPortfolio");
                }
            }
        }
        #endregion
        

        public IEnumerable<OrderVM> Legs
        {
            get { return _orders; }
        }

        public void From(PTEntity.MultiLegOrder mlOrder)
        {
            LastOrder = mlOrder;

            OrderId = mlOrder.OrderId;
            OpenOrderId = mlOrder.OpenOrderId;
            Reason = GetReasonDisplayText(mlOrder.Reason);
            PortfolioId = mlOrder.PortfolioId;
            Quantity = mlOrder.Quantity;
            
            IsPortfolio = mlOrder.Offset < PTEntity.MlOrderOffset.ML_OF_OTHER;

            bool allFinished = false;
            for (int i = 0; i < mlOrder.Legs.Length; ++i )
            {
                if (_orders.Count == i)
                    _orders.Add(new OrderVM());

                var legOrder = mlOrder.Legs[i];
                _orders[i].From(legOrder, true);
                allFinished = _orders[i].IsFinished;
            }

            IsAllFinished = allFinished;

            IsCanceled = ("高频" == Reason ? CheckMlOrderCanceled(_orders) : string.Empty);
            if (IsPortfolio)
                IsOpenOrder = mlOrder.Offset == PTEntity.MlOrderOffset.ML_OF_OPEN;
            else
                IsOpenOrder = !IsAllFinished; // For non-portoflio, display

            if (!string.IsNullOrEmpty(mlOrder.StatusMsg))
            {
                EventLogger.Write("组合下单({0}) - {1}", mlOrder.OrderId, mlOrder.StatusMsg);
            }
        }

        private const string MLORDER_CANCELED = "C";
        // If the first leg order is canceled, the whole Multi leg order is regarded as Canceled
        private static string CheckMlOrderCanceled(IList<OrderVM> orders)
        {
            if (orders.Count > 1)
            {
                if (!orders[0].IsFinished && orders[1].IsCanceled)
                    return MLORDER_CANCELED;
                if (orders[0].IsCanceled && !orders[1].IsFinished)
                    return MLORDER_CANCELED;
            }
            else if (orders.Count > 0)
                return orders[0].IsCanceled ? MLORDER_CANCELED : string.Empty;

            return string.Empty;
        }

        public void From(string ordRef, PTEntity.Order order)
        {
            int idx = Array.FindIndex(LastOrder.Legs, 
                l => l.InstrumentID == order.InstrumentID
                    && l.Direction == order.Direction);
            
            if (idx > -1)
            {
                LastOrder.Legs[idx] = order;

                foreach (var oVm in _orders)
                {
                    if (oVm.OrderUid == OrderVM.GetOrderUid(order))
                    {
                        oVm.From(order);
                        break;
                    }
                }

                // updating the second leg order
                bool allFinished = false;
                foreach(var oVm in _orders)
                {
                    allFinished = oVm.IsFinished;
                }
                IsAllFinished = allFinished;
                IsCanceled =  ("高频" == Reason ? CheckMlOrderCanceled(_orders) : String.Empty);
            }
        }

        public void CalcProfit()
        {
            double profit = 0;
            if (LastOrder == null) return;

            if (LastOrder.Legs.Length > 1)
            {
                var openOrd = LastOrder.Legs[0];
                var closeOrd = LastOrder.Legs[1];

                if (closeOrd.Direction == PTEntity.TradeDirectionType.SELL)
                {
                    profit += (closeOrd.LimitPrice * closeOrd.VolumeTraded - openOrd.LimitPrice * openOrd.VolumeTraded);
                }
                else
                {
                    profit += (openOrd.LimitPrice * openOrd.VolumeTraded - closeOrd.LimitPrice * closeOrd.VolumeTraded);
                }
            }

            Profit = profit;
        }

        public void CalcProfit(MultiLegOrderVM openOrderVm)
        {
            double profit = 0;

            if (LastOrder == null) return;

            foreach (var closeOrd in LastOrder.Legs)
            {
                var openOrd = openOrderVm.LastOrder.Legs.FirstOrDefault(l => closeOrd.InstrumentID == l.InstrumentID);
                if (openOrd != null)
                {
                    if (closeOrd.Direction == PTEntity.TradeDirectionType.SELL)
                    {
                        profit += (closeOrd.LimitPrice * closeOrd.VolumeTraded - openOrd.LimitPrice * openOrd.VolumeTraded);
                    }
                    else
                    {
                        profit += (openOrd.LimitPrice * openOrd.VolumeTraded - closeOrd.LimitPrice * closeOrd.VolumeTraded);
                    }
                }
            }

            Profit = profit;
        }

        private static string GetReasonDisplayText(PTEntity.SubmitReason submitReason)
        {
            switch (submitReason)
            {
                case PTEntity.SubmitReason.SR_AutoOpen:
                    return "开仓";
                case PTEntity.SubmitReason.SR_Manual:
                    return "手动";
                case PTEntity.SubmitReason.SR_StopGain:
                    return "止盈";
                case PTEntity.SubmitReason.SR_StopLoss:
                    return "止损";
                case PTEntity.SubmitReason.SR_AutoSwitch:
                    return "移仓";
                case PTEntity.SubmitReason.SR_Scalpe:
                    return "高频";
                case PTEntity.SubmitReason.SR_Trend:
                    return "趋势";
                case PTEntity.SubmitReason.SR_Queuing:
                    return "排队";
                default:
                    return "未知";
            }
        }
    }
}
