using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    [Export]
    public class IcebergSettingVM : StrategySettingVM
    {
        protected override StrategySetting CreateSettings()
        {
            return new IcebergSetting();
        }
    }
}
