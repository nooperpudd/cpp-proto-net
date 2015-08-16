using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class IcebergSetting : StrategySetting
    {
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

        #region PriceDiffThreshold
        private double _priceDiffThreshold;

        public double PriceDiffThreshold
        {
            get { return _priceDiffThreshold; }
            set
            {
                if (_priceDiffThreshold != value)
                {
                    _priceDiffThreshold = value;
                    RaisePropertyChanged("PriceDiffThreshold");
                }
            }
        }
        #endregion

        #region SizeDiffThreshold
        private int _sizeDiffThreshold;

        public int SizeDiffThreshold
        {
            get { return _sizeDiffThreshold; }
            set
            {
                if (_sizeDiffThreshold != value)
                {
                    _sizeDiffThreshold = value;
                    RaisePropertyChanged("SizeDiffThreshold");
                }
            }
        }
        #endregion

        #region TargetGainPercent
        private double _targetGainPercent;

        public double TargetGainPercent
        {
            get { return _targetGainPercent; }
            set
            {
                if (_targetGainPercent != value)
                {
                    _targetGainPercent = value;
                    RaisePropertyChanged("TargetGainPercent");
                }
            }
        }
        #endregion

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

        #region UserId
        private string _userId;

        public string UserId
        {
            get { return _userId; }
            set
            {
                if (_userId != value)
                {
                    _userId = value;
                    RaisePropertyChanged("UserId");
                }
            }
        }
        #endregion

        public IcebergSetting()
        {
            PriceTick = 0.2;
            PriceDiffThreshold = 1.2;
            SizeDiffThreshold = 20;
            TargetGainPercent = 0.2;
            UserId = string.Empty;

            RetryTimes = 8;
            OpenTimeout = 300;
        }

        public override string Name
        {
            get { return IcebergStrategyName; }
        }

        public override string Persist()
        {
            XElement elem = new XElement("icebergSetting",
                new XAttribute("prickTick", PriceTick),
                new XAttribute("priceDiffThreshold", PriceDiffThreshold),
                new XAttribute("sizeDiffThreshold", SizeDiffThreshold),
                new XAttribute("targetGainPercent", TargetGainPercent),
                new XAttribute("userId", UserId),
                new XAttribute("retryTimes", RetryTimes),
                new XAttribute("openTimeout", OpenTimeout));

            return elem.ToString();
        }

        public override void Load(string xmlText)
        {
            XElement elem = XElement.Parse(xmlText);
            XAttribute attr = elem.Attribute("prickTick");
            if (attr != null)
            {
                PriceTick = double.Parse(attr.Value);
            }
            attr = elem.Attribute("priceDiffThreshold");
            if (attr != null)
            {
                PriceDiffThreshold = double.Parse(attr.Value);
            }
            attr = elem.Attribute("sizeDiffThreshold");
            if (attr != null)
            {
                SizeDiffThreshold = int.Parse(attr.Value);
            }
            attr = elem.Attribute("targetGainPercent");
            if (attr != null)
            {
                TargetGainPercent = double.Parse(attr.Value);
            }
            attr = elem.Attribute("userId");
            if (attr != null)
            {
                UserId = attr.Value;
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
        }

        public override PTEntity.StrategyItem GetEntity()
        {
            PTEntity.IcebergStrategyItem icebergStrategy = new PTEntity.IcebergStrategyItem();
            icebergStrategy.PriceTick = PriceTick;
            icebergStrategy.PxDiffThreshold = PriceDiffThreshold;
            icebergStrategy.SizeDiffThreshold = SizeDiffThreshold;
            icebergStrategy.TargetGainPercent = TargetGainPercent;
            icebergStrategy.UserId = UserId;
            icebergStrategy.OpenTimeout = OpenTimeout;
            icebergStrategy.RetryTimes = RetryTimes;

            return icebergStrategy;
        }

        public override void CopyFrom(StrategySetting settings)
        {
            IcebergSetting strategySettings = (IcebergSetting)settings;
            PriceTick = strategySettings.PriceTick;
            PriceDiffThreshold = strategySettings.PriceDiffThreshold;
            SizeDiffThreshold = strategySettings.SizeDiffThreshold;
            TargetGainPercent = strategySettings.TargetGainPercent;
            UserId = strategySettings.UserId;

            RetryTimes = strategySettings.RetryTimes;
            OpenTimeout = strategySettings.OpenTimeout;
        }
    }
}
