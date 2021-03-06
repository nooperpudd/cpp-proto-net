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
using System.ComponentModel.Composition;
using PortfolioTrading.Modules.Account.HiFreTradeUI.ViewModels;

namespace PortfolioTrading.Modules.Account
{
    /// <summary>
    /// Interaction logic for PositionView.xaml
    /// </summary>
    [Export]
    [PartCreationPolicy(CreationPolicy.NonShared)]
    public partial class PositionView : UserControl
    {
        public PositionDetailVM ViewModel { get; private set;}

        [ImportingConstructor]
        public PositionView(PositionDetailVM viewModel)
        {
            ViewModel = viewModel;
            DataContext = ViewModel;
            InitializeComponent();
        }
        
    }
}
