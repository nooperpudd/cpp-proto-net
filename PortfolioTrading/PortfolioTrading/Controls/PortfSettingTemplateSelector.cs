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
        public DataTemplate ChangePositionTemplate { get; set; }
        public DataTemplate ScalperTemplate { get; set; }

        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            if (item is ArbitrageSettingsVM)
                return ArbitrageTemplate;
            else if (item is ChangePositionSettingsVM)
                return ChangePositionTemplate;
            else if (item is ScalperSettingVM)
                return ScalperTemplate;
            return null;
        }
    }
}
