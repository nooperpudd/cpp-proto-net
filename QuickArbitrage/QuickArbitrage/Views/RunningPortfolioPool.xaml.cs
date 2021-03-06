﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;
using QuickArbitrage.Main.ViewModel;
using QuickArbitrage.Common.Enums;
using System.ComponentModel;
using System.Windows.Threading;
using Infragistics.Windows.Controls;
using QuickArbitrage.Common.Contract;

namespace QuickArbitrage.Main.Views
{
    /// <summary>
    /// Interaction logic for RunningPortfolioPool.xaml
    /// </summary>
    public partial class RunningPortfolioPool : UserControl
    {
        private RunningPortfoliosViewModel _runningPortfolios;

        public RunningPortfolioPool()
        {
            InitializeComponent();

            _runningPortfolios = this.FindResource("portfolios") as RunningPortfoliosViewModel;
        }

        public void Add(RunningPortfolioItem portfolio)
        {
            _runningPortfolios.Add(portfolio);
        }

        private void tabTradingType_SelectionChanged(object sender, Controls.TabSelectionChangedEventArgs e)
        {
            TradingTypeOptionItem item = e.SelectedItem as TradingTypeOptionItem;
            this.dgFilter.Conditions.Clear();
            if(item.TradingType != TradingType.Unknown)
            {
                this.dgFilter.Conditions.Add(
                    new Infragistics.Windows.Controls.ComparisonCondition(
                        ComparisonOperator.Equals, item.TradingType));
            }
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
 
        }

        private void EditPortfolioButtonClicked(object sender, RoutedEventArgs e)
        {
            //QuickArbitrage.Connection.TransferTest.StreamFileTest.WriteCustomer("e:\\cus.bin");
            IAccountClient client = QuickArbitrage.Connection.ClientFactory.Instance.GetAccountClient();
            client.Login("0240050002", "888888", new LoginCallback(
                                            (succ, msg) => {

                                                if (succ)
                                                {
                                                    MessageBox.Show("Login Succeeded");
                                                }
                                                else
                                                {
                                                    MessageBox.Show("Login Failed");
                                                }

                                            }));
        }

        private void DeletePortfolioButtonClicked(object sender, RoutedEventArgs e)
        {
//             QuickArbitrage.Connection.TransferTest.Customer customer =
//                 QuickArbitrage.Connection.TransferTest.StreamFileTest.ReadCustomer("e:\\cpp.bin");
            IQuoteClient quoteClient = QuickArbitrage.Connection.ClientFactory.Instance.GetQuoteClient();
            quoteClient.OnQuoteReceived += new EventHandler<OnQuoteReceivedEventArgs>(quoteClient_OnQuoteReceived);
            quoteClient.Subscribe(new string[] { "cu1206", "cu1207" });
        }

        void quoteClient_OnQuoteReceived(object sender, OnQuoteReceivedEventArgs e)
        {
            string qInfo = string.Format("{0}, {1}, {2}", e.Symbol, e.Quote.last, e.Quote.update_time);
            System.Diagnostics.Trace.WriteLine(qInfo);
        }

        private void LegOpenPositionHandler(object sender, ExecutedRoutedEventArgs e)
        {

        }

        private void PortfolioOpenPositionHandler(object sender, ExecutedRoutedEventArgs e)
        {

        }
    }

    class TradingTypeOptionItem
    {
        public TradingType TradingType { get; set; }
        public string DisplayName { get; set; }
    }

    class TradingTypeOptions : List<TradingTypeOptionItem>
    {
        public TradingTypeOptions()
        {
            this.Add(new TradingTypeOptionItem() { TradingType = TradingType.Manual, DisplayName = "手动" });
            this.Add(new TradingTypeOptionItem() { TradingType = TradingType.Auto, DisplayName = "自动" });
            this.Add(new TradingTypeOptionItem() { TradingType = TradingType.Unknown, DisplayName = "全部" });
        }
    }

    class RunningTypeItems : List<TradingType>
    {
        public RunningTypeItems()
        {
            this.Add(TradingType.Manual);
            this.Add(TradingType.Auto);
        }
    }

    class RunningPortfoliosViewModel : ObservableCollection<RunningPortfolioItem>
    {
        public RunningPortfoliosViewModel()
        {
            EquityItem eqA = new EquityItem()
            {
                Code = "cu1203",
                Name = "沪铜1203",
                Side = Side.Short,
                PrevClose = 60990,
                Cost = 61240,
                Ask = 60990,
                Bid = 60970,
                Last = 60980,
                LegStatus = "未开仓"
            };

            EquityItem eqB = new EquityItem()
            {
                Code = "cu1204",
                Name = "沪铜1204",
                Side = Side.Long,
                PrevClose = 61280,
                Cost = 61570,
                Ask = 61300,
                Bid = 61280,
                Last = 61290,
                LegStatus = "未开仓"
            };

            RunningPortfolioItem portfolio = new RunningPortfolioItem();
            portfolio.Equities.Add(eqA);
            portfolio.Equities.Add(eqB);

            portfolio.RunningType = TradingType.Manual;
            portfolio.Quantity = 1;
            portfolio.UpdateDiff();
            portfolio.UpdateBalance();
            portfolio.Strategy = new Strategy() { StrategyID = Guid.NewGuid().ToString(), Name = "Auto1" };

            this.Add(portfolio);

            eqA = new EquityItem()
            {
                Code = "cu1205",
                Name = "沪铜1205",
                Side = Side.Short,
                PrevClose = 61580,
                Cost = 61860,
                Last = 61560,
                Ask = 61580,
                Bid = 61550,
                LegStatus = "未开仓"
            };

            eqB = new EquityItem()
            {
                Code = "cu1206",
                Name = "沪铜1206",
                Side = Side.Long,
                PrevClose = 61740,
                Cost = 62020,
                Last = 61750,
                Ask = 61760,
                Bid = 61740,
                LegStatus = "未开仓"
            };

            portfolio = new RunningPortfolioItem();
            portfolio.Equities.Add(eqA);
            portfolio.Equities.Add(eqB);
            portfolio.RunningType = TradingType.Auto;
            portfolio.Quantity = 1;
            portfolio.UpdateDiff();
            portfolio.UpdateBalance();
            portfolio.Strategy = new Strategy() { StrategyID = Guid.NewGuid().ToString(), Name = "Auto2" };

            this.Add(portfolio);

       }
    }
}
