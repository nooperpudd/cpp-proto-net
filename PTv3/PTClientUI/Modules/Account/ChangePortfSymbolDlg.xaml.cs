using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace PortfolioTrading.Modules.Account
{
    /// <summary>
    /// Interaction logic for ChangePortfSymbolDlg.xaml
    /// </summary>
    public partial class ChangePortfSymbolDlg : Window
    {
        public ChangePortfSymbolDlg()
        {
            InitializeComponent();
        }

        public string FirstLegSymbol
        {
            get
            {
                return this.txtFirstLeg.Text;
            }
            set 
            {
                this.txtFirstLeg.Text = value;
            }
        }

        public string SecondLegSymbol
        {
            get
            {
                return this.txtSecondLeg.Text;
            }
            set
            {
                this.txtSecondLeg.Text = value;
            }
        }

        public void SetSymbols(string[] symbols)
        {
            if (symbols == null || symbols.Length == 0)
                return;

            if (symbols.Length > 0)
                this.txtFirstLeg.Text = symbols[0];
            if (symbols.Length > 1)
                this.txtSecondLeg.Text = symbols[1];
        }

        public string[] GetSymbols()
        {
            List<string> symobls = new List<string>();
            if (!string.IsNullOrWhiteSpace(this.txtFirstLeg.Text))
                symobls.Add(this.txtFirstLeg.Text);
            if (!string.IsNullOrWhiteSpace(this.txtSecondLeg.Text))
                symobls.Add(this.txtSecondLeg.Text);

            return symobls.ToArray();
        }

        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = true;
        }
    }
}
