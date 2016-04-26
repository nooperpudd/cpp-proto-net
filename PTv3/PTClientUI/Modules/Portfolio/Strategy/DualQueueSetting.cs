using System;
using PTEntity;
using System.Xml.Linq;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class DualQueueSetting : StrategySetting
    {
        #region PriceTick
        private double _priceTick;

        public double PriceTick
        {
            get { return _priceTick; }
            set
            {
                if (Math.Abs(_priceTick - value) > 0.00001)
                {
                    _priceTick = value;
                    RaisePropertyChanged("PriceTick");
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
                }
            }
        }
        #endregion

        #region StableTickThreshold
        private int _stableTickThreshold;

        public int StableTickThreshold
        {
            get { return _stableTickThreshold; }
            set
            {
                if (_stableTickThreshold != value)
                {
                    _stableTickThreshold = value;
                    RaisePropertyChanged("StableTickThreshold");
                }
            }
        }
        #endregion

        #region MinWorkingSize
        private int _minWorkingSize;

        public int MinWorkingSize
        {
            get { return _minWorkingSize; }
            set
            {
                if (_minWorkingSize != value)
                {
                    _minWorkingSize = value;
                    RaisePropertyChanged("MinWorkingSize");
                }
            }
        }
        #endregion

        public override string Name => DualQueueStrategyName;

        public override void CopyFrom(StrategySetting settings)
        {
            DualQueueSetting strategySettings = (DualQueueSetting)settings;
            PriceTick = strategySettings.PriceTick;
            Direction = strategySettings.Direction;
            MinWorkingSize = strategySettings.MinWorkingSize;
            StableTickThreshold = strategySettings.StableTickThreshold;
        }

        public override StrategyItem GetEntity()
        {
            DualQueueStrategyItem queueStrategy = new DualQueueStrategyItem
            {
                PriceTick = PriceTick,
                Direction = Direction,
                StableTickThreshold = StableTickThreshold,
                MinWorkingSize = MinWorkingSize
            };

            return queueStrategy;
        }

        public override void Load(string xmlText)
        {
            XElement elem = XElement.Parse(xmlText);
            XAttribute attr = elem.Attribute("stableTickThreshold");
            if (attr != null)
            {
                StableTickThreshold = int.Parse(attr.Value);
            }
            attr = elem.Attribute("prickTick");
            if (attr != null)
            {
                PriceTick = double.Parse(attr.Value);
            }
            attr = elem.Attribute("minWorkingSize");
            if (attr != null)
            {
                MinWorkingSize = int.Parse(attr.Value);
            }
            attr = elem.Attribute("direction");
            if (attr != null)
            {
                Direction = (PTEntity.PosiDirectionType)Enum.Parse(typeof(PTEntity.PosiDirectionType), attr.Value);
            }
        }

        public override string Persist()
        {
            XElement elem = new XElement("dualQueueStrategySetting",
                new XAttribute("prickTick", PriceTick),
                new XAttribute("stableTickThreshold", StableTickThreshold),
                new XAttribute("minWorkingSize", MinWorkingSize),
                new XAttribute("direction", Direction)
                );

            return elem.ToString();
        }
    }
}
