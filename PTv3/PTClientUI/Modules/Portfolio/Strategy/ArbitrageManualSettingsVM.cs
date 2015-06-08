using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PortfolioTrading.Modules.Portfolio.Strategy
{
    [Export]
    public class ArbitrageManualSettingsVM : StrategySettingVM
    {

        public ArbitrageManualSettingsVM()
        {
            OffsetFlagItemsSource = new List<PosiOffsetItem>() 
            {
                new PosiOffsetItem{OffsetFlag = PTEntity.PosiOffsetFlag.CLOSE, DisplayText = "平仓"},
                new PosiOffsetItem{OffsetFlag = PTEntity.PosiOffsetFlag.OPEN, DisplayText = "开仓"}
            };
        }

        public IEnumerable<PosiOffsetItem> OffsetFlagItemsSource
        {
            get;
            set;
        }

        public IEnumerable<CompareCondItem> OpenCondItemsSource
        {
            get;
            set;
        }

        public IEnumerable<CompareCondItem> CloseCondItemsSource
        {
            get;
            set;
        }

        protected override StrategySetting CreateSettings()
        {
            _innerSettings = new ArbitrageManualStrategySettings();
            _innerSettings.OnDirectionChange += _innerSettings_OnDirectionChange;
            return _innerSettings;
        }

        void _innerSettings_OnDirectionChange(PTEntity.PosiDirectionType obj)
        {
            InitializeCondItemsSource(obj);
        }

        private ArbitrageManualStrategySettings _innerSettings;

        private void InitializeCondItemsSource(PTEntity.PosiDirectionType direction)
        {
            if (direction == PTEntity.PosiDirectionType.LONG)
            {
                OpenCondItemsSource = LessItemsSource;
                CloseCondItemsSource = GreaterItemsSource;
            }
            else if (direction == PTEntity.PosiDirectionType.SHORT)
            {
                OpenCondItemsSource = GreaterItemsSource;
                CloseCondItemsSource = LessItemsSource;
            }

            RaisePropertyChanged("OpenCondItemsSource");
            RaisePropertyChanged("CloseCondItemsSource");
        }

        protected override void AfterCopySettings()
        {
            base.AfterCopySettings();
            InitializeCondItemsSource(_innerSettings.Direction);
        }
    }
}
