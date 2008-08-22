using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
namespace OOLauncher
{
    public partial class Launcher : Form
    {
        public Launcher()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (File.Exists("realmlist.wth"))
                File.Delete("realmlist.wth");
            File.WriteAllText("realmlist.wth","1 "+ipTextBox.Text +" "+portTextBox.Text);
            System.Diagnostics.Process.Start("obse_loader.exe");
        }
    }
}
