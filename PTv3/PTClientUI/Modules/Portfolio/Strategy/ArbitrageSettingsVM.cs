using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel.Composition;
using Microsoft.Practices.Prism.ViewModel;
using Microsoft.Practices.Prism.Commands;
using Microsoft.Practices.Prism.Events;
using PortfolioTrading.Modules.Account;
using PortfolioTrading.Events;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    [Export]
    public class ArbitrageSettingsVM : StrategySettingVM
    {
        public ArbitrageSettingsVM()
        {
            StopGainCondItemsSource = GreaterItemsSource;
            StopLossCondItemsSource = GreaterItemsSource;
            StopLossTypeItemsSource = new List<StopLossTypeItem>(new StopLossTypeItem[]
            {
                new StopLossTypeItem { Value = PTEntity.ArbitrageStopLossType.STOP_LOSS_Disabled, DisplayText="不启用"},
                new StopLossTypeItem { Value = PTEntity.ArbitrageStopLossType.STOP_LOSS_Auto, DisplayText="自动"},
                new StopLossTypeItem { Value = PTEntity.ArbitrageStopLossType.STOP_LOSS_Fixed, DisplayText="设定亏损"},
                new StopLossTypeItem { Value = PTEntity.ArbitrageStopLossType.STOP_LOSS_Fixed_Price, DisplayText="设定价差"},
            });
        }

        public IEnumerable<CompareCondItem> OpenCondItemsSource
        {
            get;
            set;
        }

        public IEnumerable<CompareCondItem> StopGainCondItemsSource
        {
            get;
            set;
        }

        public IEnumerable<CompareCondItem> StopLossCondItemsSource
        {
            get;
            set;
        }

        public IEnumerable<StopLossTypeItem> StopLossTypeItemsSource
        { get; set; }

        protected override StrategySetting CreateSettings()
        {
            _innerSettings = new ArbitrageStrategySetting();
            return _innerSettings;
        }

        private ArbitrageStrategySetting _innerSettings;
    }

    public class DirectionItem
    {
        public PTEntity.PosiDirectionType Direction { get; set; }
        public string DisplayText { get; set; }
    }

    public class CompareCondItem
    {
        public PTEntity.CompareCondition Condition { get; set; }
        public string DisplayText { get; set; }
    }

    public class StopLossTypeItem
    {
        public PTEntity.ArbitrageStopLossType Value { get; set; }
        public string DisplayText { get; set; }
    }
}
