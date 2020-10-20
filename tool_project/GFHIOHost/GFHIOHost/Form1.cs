using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using GFHIOHostFormControl;
using GFHIOHostFormControl.Forms;
using GFHIOHostFormControl.Uility;

namespace GFHIOHost
{
    public partial class Form1 : Form
    {
        /*   properties   */
        protected UCtr_HIODevice HIO { get { return uCtr_HIODevice; } }
        protected SendData_Cam CamData { get; private set; }
        public Form_Log LogForm { get; private set; }

        /*   members   */
        protected Timer m_CamControlTimer = new Timer();
        protected bool m_IsMouseOnCameraControlPanel = false;



        public Form1()
        {
            InitializeComponent();
            
            /*   args   */
            string[]    args    = System.Environment.GetCommandLineArgs();

            if((1 < args.Length) && (!String.IsNullOrWhiteSpace(args[1])))
            {
                temporaryConvertCommnadDirectory    = args[1];
            }
            
            
            /*      */
            InitializeRecentFileList();


            this.MouseWheel += new MouseEventHandler(this.MouseWheelHandler);

            (panel_MouseController as Control).KeyPress += new KeyPressEventHandler(panel_MouseController_OnKeyPress);
            m_CamControlTimer.Tick += new EventHandler(this.OnTick_CamControlTimer);
            m_CamControlTimer.Interval = 100;
            m_CamControlTimer.Start();


            /*  */
            LogForm = new Form_Log();

            /*      */
            HIO.LogForm = LogForm;
            HIO.BindControl(tabControl1);
            HIO.SIOChannels.Add(@"model",   1);
            HIO.SIOChannels.Add(@"camera",  2);
            HIO.SIOChannels.Add(@"light",   3);
            HIO.SIOChannels.Add(@"motion",  4);

            /*      */
            CamData = new SendData_Cam(HIO.SIOChannels[@"camera"]);
        }


        public void Log(string format, params object[] args)
        {
            string str = string.Format(format, args);

            Console.WriteLine(str);
            if (LogForm != null)
            {
                LogForm.WriteLine(str);
            }
        }

        private void ToolStripMenuItem_ViewLog_Click(object sender, EventArgs e)
        {
            if (LogForm != null)
            {
                LogForm.Show();
            }
        }
    }
}




