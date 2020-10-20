using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GFHIOHostFormControl.Forms
{
    public partial class Form_Log : Form
    {
        private TextBox TextBox { get { return textBox; } }


        public Form_Log()
        {
            InitializeComponent();
            this.FormClosing += Form_Log_FormClosing;
        }

        public void WriteLine(string format, params Object[] args)
        {
            string str  = string.Format(format, args) + Environment.NewLine;

            TextBox.AppendText(str);
//            System.Diagnostics.Debug.WriteLine(str);
            System.Console.Write(str);
        }

        public void Clear()
        {
            TextBox.Clear();
        }

        private void Form_Log_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing)
            {
                this.Hide();
                e.Cancel = true;
            }
        }


    }
}
