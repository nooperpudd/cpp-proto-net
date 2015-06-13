using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class DualScalperSetting : StrategySetting
    {
        #region RetryTimes
        private int _retryTimes;

        public int RetryTimes
        {
            get { return _retryTimes; }
            set
            {
                if (_retryTimes != value)
                {
                    _retryTimes = value;
                    RaisePropertyChanged("RetryTimes");
                }
            }
        }
        #endregion

        #region OpenTimeout
        private int _openTimeout;

        public int OpenTimeout
        {
            get { return _openTimeout; }
            set
            {
                if (_openTimeout != value)
                {
                    _openTimeout = value;
                    RaisePropertyChanged("OpenTimeout");
                }
            }
        }
        #endregion

        #region Threshold
        private double _diff;

        public double Threshold
        {
            get { return _diff; }
            set
            {
                if (_diff != value)
                {
                    _diff = value;
                    RaisePropertyChanged("Threshold");
                }
            }
        }
        #endregion

        #region PriceTick
        private double _priceTick;

        public double PriceTick
        {
            get { return _priceTick; }
            set
            {
                if (_priceTick != value)
                {
                    _priceTick = value;
                    RaisePropertyChanged("PriceTick");
                }
            }
        }
        #endregion

        #region StopLossCloseMethod
        private PTEntity.StopLossCloseMethods _closeMethod;

        public PTEntity.StopLossCloseMethods StopLossCloseMethod
        {
            get { return _closeMethod; }
            set
            {
                if (_closeMethod != value)
                {
                    _closeMethod = value;
                    RaisePropertyChanged("StopLossCloseMethod");
                }
            }
        }
        #endregion


        #region LongSideUserId
        private string _longSideUserId = string.Empty;

        public string LongSideUserId
        {
            get { return _longSideUserId; }
            set
            {
                if (_longSideUserId != value)
                {
                    _longSideUserId = value;
                    RaisePropertyChanged("LongSideUserId");
                }
            }
        }
        #endregion

        #region ShortSideUserId
        private string _shortSideUserId = string.Empty;

        public string ShortSideUserId
        {
            get { return _shortSideUserId; }
            set
            {
                if (_shortSideUserId != value)
                {
                    _shortSideUserId = value;
                    RaisePropertyChanged("ShortSideUserId");
                }
            }
        }
        #endregion


        public DualScalperSetting()
        {
            Threshold = 0.8;
            PriceTick = 0.2;
            StopLossCloseMethod = PTEntity.StopLossCloseMethods.BASED_ON_NEXT_QUOTE;
            RetryTimes = 8;
            OpenTimeout = 400;
        }

        public override string Name
        {
            get { return StrategySetting.DualScalperStrategyName; }
        }

        public override string Persist()
        {
            XElement elem = new XElement("scalperStrategySetting",
                new XAttribute("threshold", Threshold),
                new XAttribute("prickTick", PriceTick),
                new XAttribute("stopLossCloseStrategy", StopLossCloseMethod),
                new XAttribute("retryTimes", RetryTimes),
                new XAttribute("openTimeout", OpenTimeout),
                new XAttribute("longSideUserId", LongSideUserId),
                new XAttribute("shortSideUserId", ShortSideUserId));

            return elem.ToString();
        }

        public override void Load(string xmlText)
        {
            XElement elem = XElement.Parse(xmlText);
            XAttribute attr = elem.Attribute("threshold");
            if (attr != null)
            {
                Threshold = double.Parse(attr.Value);
            }
            attr = elem.Attribute("prickTick");
            if (attr != null)
            {
                PriceTick = double.Parse(attr.Value);
            }
            attr = elem.Attribute("stopLossCloseStrategy");
            if (attr != null)
            {
                StopLossCloseMethod = (PTEntity.StopLossCloseMethods)Enum.Parse(typeof(PTEntity.StopLossCloseMethods), attr.Value);
            }
            attr = elem.Attribute("retryTimes");
            if (attr != null)
            {
                RetryTimes = int.Parse(attr.Value);
            }
            attr = elem.Attribute("openTimeout");
            if (attr != null)
            {
                OpenTimeout = int.Parse(attr.Value);
            }
            attr = elem.Attribute("longSideUserId");
            if (attr != null)
            {
                LongSideUserId = attr.Value;
            }
            attr = elem.Attribute("shortSideUserId");
            if (attr != null)
            {
                ShortSideUserId = attr.Value;
            }
        }

        public override PTEntity.StrategyItem GetEntity()
        {
            PTEntity.DualScalperStrategyItem scalperStrategy = new PTEntity.DualScalperStrategyItem();
            scalperStrategy.PriceTick = PriceTick;
            scalperStrategy.Threshold = Threshold;
            scalperStrategy.OpenTimeout = OpenTimeout;
            scalperStrategy.RetryTimes = RetryTimes;
            scalperStrategy.StopLossStrategy = StopLossCloseMethod;
            scalperStrategy.LongSideUserId = LongSideUserId;
            scalperStrategy.ShortSideUserId = ShortSideUserId;

            return scalperStrategy;
        }

        public override void CopyFrom(StrategySetting settings)
        {
            DualScalperSetting strategySettings = (DualScalperSetting)settings;
            Threshold = strategySettings.Threshold;
            PriceTick = strategySettings.PriceTick;
            StopLossCloseMethod = strategySettings.StopLossCloseMethod;
            RetryTimes = strategySettings.RetryTimes;
            OpenTimeout = strategySettings.OpenTimeout;
            LongSideUserId = strategySettings.LongSideUserId;
            ShortSideUserId = strategySettings.ShortSideUserId;
        }
    }
}
