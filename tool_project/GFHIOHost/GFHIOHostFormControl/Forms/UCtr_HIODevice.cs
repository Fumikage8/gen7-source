using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GFHIOHostFormControl.Forms
{
    public partial class UCtr_HIODevice : UserControl
    {
         /*
         * 
         * */
        public GFHIOHostDll.GFHIO HIO { get{return mHIO;} private set{mHIO = value;}}
        public Form_Log LogForm { get; set; }
        public Dictionary<string, int> SIOChannels { get { return mSIOChDictionary; } private set { mSIOChDictionary = value; } }
        public string DeviceText { get { return TextBox_DeviceID.Text; } protected set { TextBox_DeviceID.Text = value; } }
        public Control BindedControl { get; private set; }    /**< 接続状態に応じて有効状態を同期させたいControl */
        public Timer HIOTimer { get; private set; }


        protected TextBox TextBox_DeviceID { get { return textBox_DeviceID; } }
        protected Button Button_Connect { get { return button_Connect; } }
        protected Button Button_Disconnect { get { return button_Disconnect; } }
        protected DisplayMode CurrentMode { get; private set; }

        protected enum DisplayMode
        {
            MODE_Offline,
            MODE_Online,
            MODE_Connecting,
        };


        /*
         * 
         * */
        protected GFHIOHostDll.GFHIO mHIO = new GFHIOHostDll.GFHIO();
        protected Dictionary<string, int> mSIOChDictionary = new Dictionary<string,int>();



        /*
         * 
         * */
        public UCtr_HIODevice()
        {
            InitializeComponent();
            SetDisplayMode(DisplayMode.MODE_Offline);
        }

        /*
         * 
         * */
        public void BindControl(Control ctr)
        {
            BindedControl = ctr;
            UpdateBindedControl();
        }

        /*
         * 
         * */
        public void UnbindControl()
        {
            BindedControl = null;
        }

        /*
         * 
         * */
        private void UpdateBindedControl()
        {
            if(BindedControl != null)   BindedControl.Enabled = (CurrentMode == DisplayMode.MODE_Online);
        }

        /*
         * 
         * */
        public void Connect()
        {
            if (HIO == null) return;

            int found;
            uint[] ids = new uint[1];

            /*
             * 接続
             * */
            found = HIO.Find(ref ids, 0);       /*   端末検索＆選択   */
            if (found > 0)
            {
                if (HIO.Connect(ids[0]) >= 0)
                {
                    ConnectSIOChannels();
                }
            }
        }


        /*
         * SIOChannels接続(非同期)
         * 
         * */
        public async void ConnectSIOChannels()
        {
            var sioConnectTasks = new List<Task<int>>();
            int elems = SIOChannels.Count;

            if (SIOChannels.Count < 1)
            {
                DisconnectSIOChannels();
                return;
            }


            for (int index = 0; index < elems; ++index)
            {
                KeyValuePair<string, int> pair = SIOChannels.ElementAt(index);

                Log(@"connectSIO:ch[{0}] for ""{1}""", pair.Value, pair.Key);
                var task = Task.Run(() =>
                {
                    int result = -1;

                    HIO.OpenSio(pair.Value);
                    result = HIO.ConnectSio(pair.Value);
                    return result;
                });
                sioConnectTasks.Add(task);
            }

            /*  */
            SetDisplayMode(DisplayMode.MODE_Connecting);

            /*   join   */
            bool bAllOK = true;
            await Task.WhenAll(sioConnectTasks.ToArray());

            for (int index = 0; index < sioConnectTasks.Count; ++index)
            {
                var task = sioConnectTasks[index];
                KeyValuePair<string, int> pair = SIOChannels.ElementAt(index);

                Log(@"connectResult:SIOch[{0}] for ""{1}"" = {2}", pair.Value, pair.Key, task.Result);

                if (task.Result < 0)
                {
                    bAllOK = false;
                    break;
                }
            }

            if (bAllOK)
            {
                SetDisplayMode(DisplayMode.MODE_Online);
            }
            else
            {
                DisconnectSIOChannels();
            }

        }


        /*
         * 
         * */
        public void DisconnectSIOChannels()
        {
            int elems = SIOChannels.Count;

            for (int index = 0; index < elems; ++index)
            {
                KeyValuePair<string, int> pair = SIOChannels.ElementAt(index);
                Log(@"closeSIO:ch[{0}] for ""{1}""", pair.Value, pair.Key);
                HIO.CloseSio(pair.Value);     /*   disconnectは内部で   */
            }

            HIO.Disconnect();
            SetDisplayMode(DisplayMode.MODE_Offline);
        }


        /*
         * 
         * */
        public void SendData(GFHIOHostFormControl.Uility.HIOSendData data)
        {
            byte[] dataByteArray = data.ToArray();

            Log(@"SendData: ch[{0}], {1}[B], ""{2}""", data.Ch, data.Bytes, data.Header.Description);

            /*  header  */
            data.Header.Bytes   = (dataByteArray != null) ? dataByteArray.Length : 0;
            HIO.WriteSio(data.Ch, data.Header.ToArray(), GFHIOHostFormControl.Uility.HIOSendDataHeader.MyBytes);

            /*  data    */
            if (0 < data.Header.Bytes)
            {
                HIO.WriteSio(data.Ch, dataByteArray, dataByteArray.Length);
            }
        }



        /*
         * 
         * */
        protected void SetDisplayMode(DisplayMode mode)
        {
            CurrentMode = mode;
            
            switch (mode)
            {
                case DisplayMode.MODE_Offline:
                    DeviceText = @"未接続";
                    Button_Connect.Enabled = true;
                    Button_Disconnect.Enabled = !Button_Connect.Enabled;
                    break;

                case DisplayMode.MODE_Online:
                case DisplayMode.MODE_Connecting:
                {
                    uint deviceID = HIO.GetConnectId();

                    {
                        Button_Connect.Enabled = false;
                        Button_Disconnect.Enabled = !Button_Connect.Enabled;
                    }

                    if(deviceID == 0)
                    {
                        SetDisplayMode(DisplayMode.MODE_Offline);
                    }
                    else
                    {
                        DeviceText = deviceID.ToString();
                        if(mode == DisplayMode.MODE_Connecting)
                        {
                            DeviceText += @"に接続中...";
                        }
                    }
                }
                    break;

                default:
                    break;
            }

            UpdateBindedControl();
        }





        protected void Log(string format, params object[] args)
        {
            Console.WriteLine(string.Format(format, args));
            if (LogForm != null)
            {
                LogForm.WriteLine(format, args);
            }
        }


        private void button_Connect_Click(object sender, EventArgs e)
        {
            Connect();
        }

        private void button_Disconnect_Click(object sender, EventArgs e)
        {
            DisconnectSIOChannels();
        }





        public void SetTimer(int msec)
        {
            if (HIOTimer == null) HIOTimer = new Timer();

            HIOTimer.Interval = msec;
        }

        public void PauseTimer(bool isPause)
        {
            if (HIOTimer != null)
            {
                HIOTimer.Enabled = !isPause;
            }
        }

        public void ClearTimer()
        {
            if (HIOTimer != null)
            {
                HIOTimer.Dispose();
                HIOTimer = null;
            }
        }
    
    
    }
}
