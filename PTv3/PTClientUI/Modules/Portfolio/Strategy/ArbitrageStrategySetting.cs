using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Linq;
using Microsoft.Practices.Prism.ViewModel;
using PortfolioTrading.Utils;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    public class ArbitrageStrategySetting : StrategySetting
    {

        #region BollPeriod
        private int _bollPeriod;

        public int BollPeriod
        {
            get { return _bollPeriod; }
            set
            {
                if (_bollPeriod != value)
                {
                    _bollPeriod = value;
                    RaisePropertyChanged("BollPeriod");
                }
            }
        }
        #endregion

        #region StdDevMultiplier
        private int _stdDevMultiplier;

        public int StdDevMultiplier
        {
            get { return _stdDevMultiplier; }
            set
            {
                if (_stdDevMultiplier != value)
                {
                    _stdDevMultiplier = value;
                    RaisePropertyChanged("StdDevMultiplier");
                }
            }
        }
        #endregion

        #region FirstLegSymbol
        private string _firstLegSymbol;

        public string FirstLegSymbol
        {
            get { return _firstLegSymbol; }
            set
            {
                if (_firstLegSymbol != value)
                {
                    _firstLegSymbol = value;
                    RaisePropertyChanged("FirstLegSymbol");
                }
            }
        }
        #endregion

        #region SecondLegSymbol
        private string _secondLegSymbol;

        public string SecondLegSymbol
        {
            get { return _secondLegSymbol; }
            set
            {
                if (_secondLegSymbol != value)
                {
                    _secondLegSymbol = value;
                    RaisePropertyChanged("SecondLegSymbol");
                }
            }
        }
        #endregion

        #region TimeFrame
        private int _timeFrame;

        public int TimeFrame
        {
            get { return _timeFrame; }
            set
            {
                if (_timeFrame != value)
                {
                    _timeFrame = value;
                    RaisePropertyChanged("TimeFrame");
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

        #region UseTargetGain
        private bool _useTargetGain;

        public bool UseTargetGain
        {
            get { return _useTargetGain; }
            set
            {
                if (_useTargetGain != value)
                {
                    _useTargetGain = value;
                    RaisePropertyChanged("UseTargetGain");
                }
            }
        }
        #endregion

        #region AbsoluteGain
        private bool _absoluteGain;

        public bool AbsoluteGain
        {
            get { return _absoluteGain; }
            set
            {
                if (_absoluteGain != value)
                {
                    _absoluteGain = value;
                    RaisePropertyChanged("AbsoluteGain");
                }
            }
        }
        #endregion


        #region TargetGain
        private int targetGain;

        public int TargetGain
        {
            get { return targetGain; }
            set
            {
                if (targetGain != value)
                {
                    targetGain = value;
                    RaisePropertyChanged("TargetGain");
                }
            }
        }
        #endregion

        #region SpecifyBandRange
        private bool _specifyBandRange;

        public bool SpecifyBandRange
        {
            get { return _specifyBandRange; }
            set
            {
                if (_specifyBandRange != value)
                {
                    _specifyBandRange = value;
                    RaisePropertyChanged("SpecifyBandRange");
                }
            }
        }
        #endregion

        #region BandRange
        private int _bandRange;

        public int BandRange
        {
            get { return _bandRange; }
            set
            {
                if (_bandRange != value)
                {
                    _bandRange = value;
                    RaisePropertyChanged("BandRange");
                }
            }
        }
        #endregion

        #region StopLossType
        private PTEntity.ArbitrageStopLossType stopLossType;

        public PTEntity.ArbitrageStopLossType StopLossType
        {
            get { return stopLossType; }
            set
            {
                if (stopLossType != value)
                {
                    stopLossType = value;
                    RaisePropertyChanged("StopLossType");
                }
            }
        }
        #endregion

        #region StopLossCondition
        private PTEntity.CompareCondition _stopLossCond;

        public PTEntity.CompareCondition StopLossCondition
        {
            get { return _stopLossCond; }
            set
            {
                if (_stopLossCond != value)
                {
                    _stopLossCond = value;
                    RaisePropertyChanged("StopLossCondition");
                }
            }
        }
        #endregion

        #region StopLossThreshold
        private double _stopLossThreshold;

        public double StopLossThreshold
        {
            get { return _stopLossThreshold; }
            set
            {
                if (_stopLossThreshold != value)
                {
                    _stopLossThreshold = value;
                    RaisePropertyChanged("StopLossThreshold");
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


        public ArbitrageStrategySetting()
        {
			BollPeriod = 26;
			StdDevMultiplier = 2;
			TimeFrame = 60;
            MaxPosition = 1;
            OpenTimeout = 350;
            RetryTimes = 8;

            UseTargetGain = false;
            AbsoluteGain = false;
            TargetGain = 4;

            SpecifyBandRange = false;
            BandRange = 10;

            StopLossType = PTEntity.ArbitrageStopLossType.STOP_LOSS_Disabled;
            StopLossCondition = PTEntity.CompareCondition.GREATER_THAN;
            StopLossThreshold = 0;
        }

        public override string Persist()
        {
            XElement elem = new XElement("arbitrageStrategySetting");
            elem.Add(
                new XAttribute("useTargetGain", UseTargetGain),
                new XAttribute("absoluteGain", AbsoluteGain),
                new XAttribute("targetGain", TargetGain),
                new XAttribute("specifyBandRange", SpecifyBandRange),
                new XAttribute("bandRange", BandRange),
                new XAttribute("maxPosition", MaxPosition),
                new XAttribute("stopLossType", StopLossType),
                new XAttribute("retryTimes", RetryTimes),
                new XAttribute("openTimeout", OpenTimeout));
			
			XElement elemHistSource = new XElement("histSource",
                new XAttribute("firstLeg", FirstLegSymbol),
                new XAttribute("secondLeg", SecondLegSymbol),
				new XAttribute("timeFrame", TimeFrame));
            elem.Add(elemHistSource);
			XElement elemBollBand = new XElement("bollBand",
                new XAttribute("period", BollPeriod),
                new XAttribute("stdDevMultiplier", StdDevMultiplier));
            elem.Add(elemBollBand);
            XElement elemStopLoss = new XElement("stopLoss",
                new XAttribute("condition", StopLossCondition),
                new XAttribute("threshold", StopLossThreshold));
            elem.Add(elemStopLoss);
            return elem.ToString();
        }

        public override void Load(string xmlText)
        {
            XElement elem = XElement.Parse(xmlText);

            XAttribute attr = elem.Attribute("useTargetGain");
            if (attr != null)
                UseTargetGain = bool.Parse(attr.Value);
            attr = elem.Attribute("absoluteGain");
            if (attr != null)
                AbsoluteGain = bool.Parse(attr.Value);
            attr = elem.Attribute("targetGain");
            if (attr != null)
                TargetGain = int.Parse(attr.Value);
            attr = elem.Attribute("specifyBandRange");
            if (attr != null)
                SpecifyBandRange = bool.Parse(attr.Value);
            attr = elem.Attribute("bandRange");
            if (attr != null)
                BandRange = int.Parse(attr.Value);
            attr = elem.Attribute("maxPosition");
            if (attr != null)
                MaxPosition = int.Parse(attr.Value);
            
            attr = elem.Attribute("stopLossType");
            if (attr != null)
                StopLossType = (PTEntity.ArbitrageStopLossType)Enum.Parse(typeof(PTEntity.ArbitrageStopLossType), attr.Value);
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
				
			XElement elemHistSource = elem.Element("histSource");
			attr = elemHistSource.Attribute("firstLeg");
			if(attr != null)
			{
				FirstLegSymbol = attr.Value;
			}
			attr = elemHistSource.Attribute("secondLeg");
			if(attr != null)
			{
				SecondLegSymbol = attr.Value;
			}
			attr = elemHistSource.Attribute("timeFrame");
			if(attr != null)
			{
				TimeFrame = int.Parse(attr.Value);
			}
			
			XElement elemBollBand = elem.Element("bollBand");
			attr = elemBollBand.Attribute("period");
			if(attr != null)
			{
				BollPeriod = int.Parse(attr.Value);
			}
			attr = elemBollBand.Attribute("stdDevMultiplier");
			if(attr != null)
			{
				StdDevMultiplier = int.Parse(attr.Value);
			}

            XElement elemStopLoss = elem.Element("stopLoss");
            attr = elemStopLoss.Attribute("condition");
            if (attr != null)
            {
                StopLossCondition = (PTEntity.CompareCondition)Enum.Parse(typeof(PTEntity.CompareCondition), attr.Value);
            }
            attr = elemStopLoss.Attribute("threshold");
            if (attr != null)
            {
                StopLossThreshold = double.Parse(attr.Value);
            }
        }

        public override string Name
        {
            get { return ArbitrageStrategyName; }
        }

        public override PTEntity.StrategyItem GetEntity()
        {
            PTEntity.ArbitrageStrategyItem strategyItem = new PTEntity.ArbitrageStrategyItem();
            strategyItem.OpenTimeout = OpenTimeout;
            strategyItem.RetryTimes = RetryTimes;

            strategyItem.BollPeriod = BollPeriod;
            strategyItem.StdDevMultiplier = StdDevMultiplier;
			strategyItem.FirstLegSymbol = FirstLegSymbol;
			strategyItem.SecondLegSymbol = SecondLegSymbol;
			strategyItem.TimeFrame = TimeFrame;
            strategyItem.MaxPosition = MaxPosition;
            strategyItem.StoplossType = StopLossType;
            strategyItem.UseTargetGain = UseTargetGain;
            strategyItem.AbsoluteGain = AbsoluteGain;
            strategyItem.TargetGain = TargetGain;
            strategyItem.SpecifyBandRange = SpecifyBandRange;
            strategyItem.BandRange = BandRange;
            strategyItem.StoplossCondition = StopLossCondition;
            strategyItem.StopLossThreshold = StopLossThreshold;
           
            return strategyItem;
        }

        public override void CopyFrom(StrategySetting settings)
        {
            ArbitrageStrategySetting strategySettings = (ArbitrageStrategySetting)settings;
            this.OpenTimeout = strategySettings.OpenTimeout;
            this.RetryTimes = strategySettings.RetryTimes;
			this.BollPeriod = strategySettings.BollPeriod;
            this.StdDevMultiplier = strategySettings.StdDevMultiplier;
			this.FirstLegSymbol = strategySettings.FirstLegSymbol;
			this.SecondLegSymbol = strategySettings.SecondLegSymbol;
			this.TimeFrame = strategySettings.TimeFrame;
            this.MaxPosition = strategySettings.MaxPosition;
            this.UseTargetGain = strategySettings.UseTargetGain;
            this.AbsoluteGain = strategySettings.AbsoluteGain;
            this.SpecifyBandRange = strategySettings.SpecifyBandRange;
            this.BandRange = strategySettings.BandRange;
            this.TargetGain = strategySettings.TargetGain;
            this.StopLossType = strategySettings.StopLossType;
            this.StopLossCondition = strategySettings.StopLossCondition;
            this.StopLossThreshold = strategySettings.StopLossThreshold;
        }
    }
}
