using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace OOLauncher
{
    public partial class Logon : Form
    {
        public Logon()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            (new Patcher()).ShowDialog();
            (new Launcher()).ShowDialog();
            this.Close();
            return;
        }
    }
}
