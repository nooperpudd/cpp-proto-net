﻿using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    [Export]
    public class DualScalperSettingVM : StrategySettingVM
    {
        protected override StrategySetting CreateSettings()
        {
            return new DualScalperSetting();
        }
    }
}
