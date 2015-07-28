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

        #region OpenOffset
        private double _openOffset;

        public double OpenOffset
        {
            get { return _openOffset; }
            set
            {
                if (_openOffset != value)
                {
                    _openOffset = value;
                    RaisePropertyChanged("OpenOffset");
                }
            }
        }
        #endregion

        #region PercentOffset
        private double _percentOffset = 0.2;

        public double PercentOffset
        {
            get { return _percentOffset; }
            set
            {
                if (_percentOffset != value)
                {
                    _percentOffset = value;
                    RaisePropertyChanged("PercentOffset");
                }
            }
        }
        #endregion


        #region CloseOffset
        private double _closeOffset;

        public double CloseOffset
        {
            get { return _closeOffset; }
            set
            {
                if (_closeOffset != value)
                {
                    _closeOffset = value;
                    RaisePropertyChanged("CloseOffset");
                }
            }
        }
        #endregion

        #region OppositeCloseThreshold
        private double _oppositeCloseThreshold;

        public double OppositeCloseThreshold
        {
            get { return _oppositeCloseThreshold; }
            set
            {
                if (_oppositeCloseThreshold != value)
                {
                    _oppositeCloseThreshold = value;
                    RaisePropertyChanged("OppositeCloseThreshold");
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
            OpenOffset = 0.2;
            PercentOffset = 0.2;
            CloseOffset = 0.2;
            OppositeCloseThreshold = 0.4;
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
                new XAttribute("openOffset", OpenOffset),
                new XAttribute("percentOffset", PercentOffset),
                new XAttribute("closeOffset", CloseOffset),
                new XAttribute("oppositeCloseThreshold", OppositeCloseThreshold),
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
            attr = elem.Attribute("openOffset");
            if (attr != null)
            {
                OpenOffset = double.Parse(attr.Value);
            }
            attr = elem.Attribute("percentOffset");
            if (attr != null)
            {
                PercentOffset = double.Parse(attr.Value);
            }
            attr = elem.Attribute("closeOffset");
            if (attr != null)
            {
                CloseOffset = double.Parse(attr.Value);
            }
            attr = elem.Attribute("oppositeCloseThreshold");
            if (attr != null)
            {
                OppositeCloseThreshold = double.Parse(attr.Value);
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
            scalperStrategy.OpenOffset = OpenOffset;
            scalperStrategy.PercentOffset = PercentOffset;
            scalperStrategy.CloseOffset = CloseOffset;
            scalperStrategy.OppositeCloseThreshold = OppositeCloseThreshold;
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
            OpenOffset = strategySettings.OpenOffset;
            PercentOffset = strategySettings.PercentOffset;
            CloseOffset = strategySettings.CloseOffset;
            OppositeCloseThreshold = strategySettings.OppositeCloseThreshold;
            StopLossCloseMethod = strategySettings.StopLossCloseMethod;
            RetryTimes = strategySettings.RetryTimes;
            OpenTimeout = strategySettings.OpenTimeout;
            LongSideUserId = strategySettings.LongSideUserId;
            ShortSideUserId = strategySettings.ShortSideUserId;
        }
    }
}
