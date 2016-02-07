﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows;
using PortfolioTrading.Modules.Portfolio.Strategy;

namespace PortfolioTrading.Controls
{
    public class PortfSettingTemplateSelector : DataTemplateSelector
    {
        public DataTemplate ArbitrageTemplate { get; set; }
        public DataTemplate ArbitrageManualTemplate { get; set; }
        public DataTemplate ChangePositionTemplate { get; set; }
        public DataTemplate ScalperTemplate { get; set; }
        public DataTemplate DualScalperTemplate { get; set; }
        public DataTemplate DualQueueTemplate { get; set; }
        public DataTemplate IcebergTemplate { get; set; }
        public DataTemplate MACDHistSlopeTemplate { get; set; }
        public DataTemplate WMATrendTemplate { get; set; }
        public DataTemplate LinerRegressionTemplate { get; set; }
        public DataTemplate ASCTrendTremplate { get; set; }
        public DataTemplate RangeTrendTemplate { get; set; }
        public DataTemplate ManualTemplate { get; set; }

        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            if (item is ArbitrageSettingsVM)
                return ArbitrageTemplate;
            else if (item is ArbitrageManualSettingsVM)
                return ArbitrageManualTemplate;
            else if (item is ChangePositionSettingsVM)
                return ChangePositionTemplate;
            else if (item is ScalperSettingVM)
                return ScalperTemplate;
            else if (item is DualScalperSettingVM)
                return DualScalperTemplate;
            else if (item is DualQueueSettingVM)
                return DualQueueTemplate;
            else if (item is IcebergSettingVM)
                return IcebergTemplate;
            else if (item is MACDHistSlopeSettingsVM)
                return MACDHistSlopeTemplate;
            else if (item is WMATrendSettingsVM)
                return WMATrendTemplate;
            else if (item is LinerRegSettingsVM)
                return LinerRegressionTemplate;
            else if (item is ASCTrendSettingsVM)
                return ASCTrendTremplate;
            else if (item is RangeTrendSettingsVM)
                return RangeTrendTemplate;
            else if (item is ManualStrategySettingVM)
                return ManualTemplate;
            return null;
        }
    }
}
