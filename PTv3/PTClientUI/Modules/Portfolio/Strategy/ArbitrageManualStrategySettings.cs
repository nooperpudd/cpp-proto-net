using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class ArbitrageManualStrategySettings : StrategySetting
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

        #region Direction
        private PTEntity.PosiDirectionType _direction;

        public PTEntity.PosiDirectionType Direction
        {
            get { return _direction; }
            set
            {
                if (_direction != value)
                {
                    _direction = value;
                    RaisePropertyChanged("Direction");
                    RaiseDirectionChange(value);
                }
            }
        }
        #endregion

        #region PosiOffset
        private PTEntity.PosiOffsetFlag _posiOffset;

        public PTEntity.PosiOffsetFlag PosiOffset
        {
            get { return _posiOffset; }
            set
            {
                if (_posiOffset != value)
                {
                    _posiOffset = value;
                    RaisePropertyChanged("PosiOffset");
                }
            }
        }
        #endregion

        #region OpenCondition
        private PTEntity.CompareCondition _openCondition;

        public PTEntity.CompareCondition OpenCondition
        {
            get { return _openCondition; }
            set
            {
                if (_openCondition != value)
                {
                    _openCondition = value;
                    RaisePropertyChanged("OpenCondition");
                }
            }
        }
        #endregion

        #region OpenThreshold
        private double _openThreshold;

        public double OpenThreshold
        {
            get { return _openThreshold; }
            set
            {
                if (_openThreshold != value)
                {
                    _openThreshold = value;
                    RaisePropertyChanged("OpenThreshold");
                }
            }
        }
        #endregion

        #region CloseCondition
        private PTEntity.CompareCondition _closeCondition;

        public PTEntity.CompareCondition CloseCondition
        {
            get { return _closeCondition; }
            set
            {
                if (_closeCondition != value)
                {
                    _closeCondition = value;
                    RaisePropertyChanged("CloseCondition");
                }
            }
        }
        #endregion

        #region CloseThreshold
        private double _closeThreshold;

        public double CloseThreshold
        {
            get { return _closeThreshold; }
            set
            {
                if (_closeThreshold != value)
                {
                    _closeThreshold = value;
                    RaisePropertyChanged("CloseThreshold");
                }
            }
        }
        #endregion

        #region MaxPosition
        private int _maxPosition;

        public int MaxPosition
        {
            get { return _maxPosition; }
            set
            {
                if (_maxPosition != value)
                {
                    _maxPosition = value;
                    RaisePropertyChanged("MaxPosition");
                }
            }
        }
        #endregion
        

        public event Action<PTEntity.PosiDirectionType> OnDirectionChange;

        private void RaiseDirectionChange(PTEntity.PosiDirectionType direction)
        {
            if (OnDirectionChange != null)
                OnDirectionChange(direction);
        }

        public ArbitrageManualStrategySettings()
        {
            OpenTimeout = 350;
            RetryTimes = 8;

            Direction = PTEntity.PosiDirectionType.LONG;
            PosiOffset = PTEntity.PosiOffsetFlag.CLOSE;
            MaxPosition = 1;

            OpenCondition = PTEntity.CompareCondition.LESS_EQUAL_THAN;
            OpenThreshold = 5;

            CloseCondition = PTEntity.CompareCondition.GREATER_EQUAL_THAN;
            CloseThreshold = 8;
            
        }

        public override string Name
        {
            get { return ArbitrageManualStrategyName; }
        }

        public override string Persist()
        {
            XElement elem = new XElement("arbitrageManualSetting");
            elem.Add(
                new XAttribute("direction", Direction),
                new XAttribute("posiOffset", PosiOffset),
                new XAttribute("maxPosition", MaxPosition),
                new XAttribute("retryTimes", RetryTimes),
                new XAttribute("openTimeout", OpenTimeout));

            XElement elemOpenPosition = new XElement("openPosition",
                new XAttribute("condition", OpenCondition),
                new XAttribute("threshold", OpenThreshold));
            elem.Add(elemOpenPosition);
            XElement elemClosePosition = new XElement("closePosition",
                new XAttribute("condition", CloseCondition),
                new XAttribute("threshold", CloseThreshold));
            elem.Add(elemClosePosition);
            return elem.ToString();
        }

        public override void Load(string xmlText)
        {
            XElement elem = XElement.Parse(xmlText);

            XAttribute attr = elem.Attribute("direction");
            if (attr != null)
                Direction = (PTEntity.PosiDirectionType)Enum.Parse(typeof(PTEntity.PosiDirectionType), attr.Value);
            attr = elem.Attribute("posiOffset");
            if (attr != null)
                PosiOffset = (PTEntity.PosiOffsetFlag)Enum.Parse(typeof(PTEntity.PosiOffsetFlag), attr.Value);
            attr = elem.Attribute("maxPosition");
            if (attr != null)
            {
                MaxPosition = int.Parse(attr.Value);
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

            XElement elemOpenPosition = elem.Element("openPosition");
            attr = elemOpenPosition.Attribute("condition");
            if (attr != null)
            {
                OpenCondition = (PTEntity.CompareCondition)Enum.Parse(typeof(PTEntity.CompareCondition), attr.Value);
            }
            attr = elemOpenPosition.Attribute("threshold");
            if (attr != null)
            {
                OpenThreshold = double.Parse(attr.Value);
            }

            XElement elemClosePosition = elem.Element("closePosition");
            attr = elemClosePosition.Attribute("condition");
            if (attr != null)
            {
                CloseCondition = (PTEntity.CompareCondition)Enum.Parse(typeof(PTEntity.CompareCondition), attr.Value);
            }
            attr = elemClosePosition.Attribute("threshold");
            if (attr != null)
            {
                CloseThreshold = double.Parse(attr.Value);
            }
            
        }

        public override PTEntity.StrategyItem GetEntity()
        {
            PTEntity.ArbitrageManualStrategyItem strategyItem = new PTEntity.ArbitrageManualStrategyItem();
            strategyItem.OpenTimeout = OpenTimeout;
            strategyItem.RetryTimes = RetryTimes;

            strategyItem.Direction = Direction;
            strategyItem.OffsetFlag = PosiOffset;
            strategyItem.OpenCondition = OpenCondition;
            strategyItem.OpenThreshold = OpenThreshold;
            strategyItem.CloseCondition = CloseCondition;
            strategyItem.CloseThreshold = CloseThreshold;
            strategyItem.MaxPosition = MaxPosition;

            return strategyItem;
        }

        public override void CopyFrom(StrategySetting settings)
        {
            ArbitrageManualStrategySettings strategySettings = (ArbitrageManualStrategySettings)settings;
            this.OpenTimeout = strategySettings.OpenTimeout;
            this.RetryTimes = strategySettings.RetryTimes;
            this.Direction = strategySettings.Direction;
            this.PosiOffset = strategySettings.PosiOffset;
            this.OpenCondition = strategySettings.OpenCondition;
            this.OpenThreshold = strategySettings.OpenThreshold;
            this.CloseCondition = strategySettings.CloseCondition;
            this.CloseThreshold = strategySettings.CloseThreshold;
            this.MaxPosition = strategySettings.MaxPosition;
        }
    }
}
