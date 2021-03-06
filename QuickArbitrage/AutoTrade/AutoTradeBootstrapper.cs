﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Practices.Prism.MefExtensions;
using System.ComponentModel.Composition.Hosting;
using System.Windows;
using Microsoft.Practices.Prism.Logging;

namespace AutoTrade.MainFrame
{
    class AutoTradeBootstrapper : MefBootstrapper
    {
        protected override void ConfigureAggregateCatalog()
        {
            this.AggregateCatalog.Catalogs.Add(new AssemblyCatalog(typeof(AutoTradeBootstrapper).Assembly));
        }

        protected override void ConfigureContainer()
        {
            base.ConfigureContainer();
        }

        protected override void InitializeShell()
        {
            base.InitializeShell();

            Application.Current.MainWindow = (ShellWindow)this.Shell;
            Application.Current.MainWindow.Show();
        }

        protected override DependencyObject CreateShell()
        {
            return this.Container.GetExportedValue<ShellWindow>();
        }

        protected override ILoggerFacade CreateLogger()
        {
            // Because the Shell is displayed after most of the interesting boostrapper work has been performed,
            // this quickstart uses a special logger class to hold on to early log entries and display them 
            // after the UI is visible.
            return LogManager.Logger as ILoggerFacade;
        }
    }
}
