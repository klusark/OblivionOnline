using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.IO;
namespace OOLauncher
{
    public partial class Patcher : Form
    {
        public Patcher()
        {
            InitializeComponent();
        }

        private void Patcher_Load(object sender, EventArgs e)
        {
            
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Stream XMLStream;
            string PatcherURI = Properties.Settings.Default.PatcherURI;
            if (PatcherURI.StartsWith("http:", true, System.Globalization.CultureInfo.CurrentCulture))
            {
                try
                {
                    eventListBox.Items.Add("Retrieving patch file from HTTP");
                    HttpWebRequest request = (HttpWebRequest)HttpWebRequest.Create(new Uri(PatcherURI));
                    HttpWebResponse resp = (HttpWebResponse)request.GetResponse();
                    XMLStream = resp.GetResponseStream();
                    eventListBox.Items.Add("Retrieved patch from HTTP.");
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Failed to patch from update server. Offline Play might still be possible, although online gaming will be disabled");
                    return;
                }
            }
            else
            {
                try
                {
                    XMLStream = File.Open(PatcherURI, FileMode.Open, FileAccess.Read);
                    eventListBox.Items.Add("Loaded patch file from local filesystem. For development only");
                }
                catch (System.Exception ex)
                {
                    MessageBox.Show("Failed to update from local patch file");
                }
            }
        }
    }
}
