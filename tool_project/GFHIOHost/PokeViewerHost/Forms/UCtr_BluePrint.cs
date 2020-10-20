using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PokeViewerHost.Forms
{
    public partial class UCtr_BluePrint : UserControl
    {
        public delegate void OnPropertyChangedDelegate(UCtr_BluePrint sender);

        /*
         * 
         * */
        protected ColorDialog MyColorDialog { get { return colorDialog; } }

        public Utility.BluePrint BluePrint { get; protected set; }
        public OnPropertyChangedDelegate OnPropertyChanged { get; set; }


        /*
         * controls
         * */
        protected TextBox CtrCamFOVText { get { return textBox_CamFOV; } }
        protected TextBox CtrMotionIndexText { get { return textBox_MotionIndex; } }
        protected TextBox CtrMotionFrameText { get { return textBox_MotionFrame; } }
        protected Panel CtrClearColor { get { return panel_ClearColor; } }
        protected TrackBar CtrCamFOV { get { return trackBar_CameraFOV; } }
        protected TrackBar CtrMotionIndex { get { return trackBar_MotionIndex; } }
        protected TrackBar CtrMotionFrame { get { return trackBar_MotionFrame; } }
        protected CheckBox CtrEnableShadow { get { return checkBox_EnableShadow; } }

        public UCtr_BluePrint()
        {
            InitializeComponent();

            trackBar_CameraFOV.ValueChanged += OnTrackBarPropertyChanged;
            trackBar_MotionIndex.ValueChanged += OnTrackBarPropertyChanged;
            trackBar_MotionFrame.ValueChanged += OnTrackBarPropertyChanged;

            checkBox_EnableShadow.CheckedChanged += OnPropertyChangedEvent;
//            comboBox_CamMode.SelectedIndexChanged += OnPropertyChangedEvent;
        }


        protected void OnControlPropertyChanged(object sender, EventArgs e)
        {
        }




        public void Reset()
        {
            CtrClearColor.BackColor = Color.White;
            CtrCamFOV.Value = 40;
            CtrMotionIndex.Value = 0;
            CtrMotionFrame.Value = 0;
            CtrEnableShadow.Checked = false;
        }









        public void Bind(Utility.BluePrint bp)
        {
            Unbind();
/*
            panel_ClearColor.DataBindings.Add(@"BackColor", bp, @"ClearColor");

            checkBox_EnableShadow.DataBindings.Add(@"Checked", bp, @"EnableShadow");
            comboBox_CamMode.DataBindings.Add(@"SelectedIndex", bp, @"CameraMode");

            textBox_CamFOV.DataBindings.Add(@"Text", bp, @"CameraFOV");
            textBox_CamPosX.DataBindings.Add(@"Text", bp, @"CameraX");
            textBox_CamPosY.DataBindings.Add(@"Text", bp, @"CameraY");
            textBox_CamPosZ.DataBindings.Add(@"Text", bp, @"CameraX");
            textBox_CamAtX.DataBindings.Add(@"Text", bp, @"CameraLookAtX");
            textBox_CamAtY.DataBindings.Add(@"Text", bp, @"CameraLookAtY");
            textBox_CamAtZ.DataBindings.Add(@"Text", bp, @"CameraLookAtZ");

            textBox_MotionIndex.DataBindings.Add(@"Text", trackBar_MotionIndex, @"Value");
            textBox_MotionFrame.DataBindings.Add(@"Text", trackBar_MotionFrame, @"Value");

*/

            


            BluePrint = bp;
            OnBluePrintPropertyChanged();
        }


        private void OnTrackBarPropertyChanged(object sender, EventArgs e)
        {
            TrackBar bar = (sender as TrackBar);

            if (bar == null) return;
            if (BluePrint == null) return;


            if (sender == CtrMotionIndex)
            {
                BluePrint.MotionIndex = bar.Value;
                OnPropertyChangedEvent(sender, e);
            }
            else if (sender == CtrMotionFrame)
            {
                BluePrint.MotionFrame = bar.Value;
                OnPropertyChangedEvent(sender, e);
            }
            else if (sender == CtrCamFOV)
            {
                BluePrint.CameraFOV = bar.Value;
                OnPropertyChangedEvent(sender, e);
            }
        }


        
        public void Unbind()
        {
            BluePrint = null;
            /*
            panel_ClearColor.DataBindings.Clear();
            checkBox_EnableShadow.DataBindings.Clear();
            comboBox_CamMode.DataBindings.Clear();

            textBox_CamFOV.DataBindings.Clear();
            textBox_CamPosX.DataBindings.Clear();
            textBox_CamPosY.DataBindings.Clear();
            textBox_CamPosZ.DataBindings.Clear();
            textBox_CamAtX.DataBindings.Clear();
            textBox_CamAtY.DataBindings.Clear();
            textBox_CamAtZ.DataBindings.Clear();

            textBox_MotionIndex.DataBindings.Clear();
            textBox_MotionFrame.DataBindings.Clear();
             * */

        }

        private void panel_ClearColor_Click(object sender, EventArgs e)
        {
            Control ctrl = (sender as Control);

            MyColorDialog.Color = ctrl.BackColor;
            MyColorDialog.FullOpen = true;

            if (MyColorDialog.ShowDialog() == DialogResult.OK)
            {
                ctrl.BackColor = Color.FromArgb(MyColorDialog.Color.ToArgb());
            }
        }

        private void panel_ClearColor_BackColorChanged(object sender, EventArgs e)
        {
            if (BluePrint != null)
            {
                BluePrint.ClearColor = Color.FromArgb((sender as Control).BackColor.ToArgb());
            }

            OnPropertyChangedEvent(sender, e);
        }

        protected void OnPropertyChangedEvent(object sender, EventArgs e)
        {
            if (OnPropertyChanged != null)
            {
                OnPropertyChanged(this);
            }
        }


        /*
         * 外的要因でBluePrintの内容が変化したことの通知
         * */
        public void OnBluePrintPropertyChanged()
        {
            CtrClearColor.BackColor = BluePrint.ClearColor;

            CtrCamFOV.Text = BluePrint.CameraFOV.ToString();
            CtrCamFOVText.Text = BluePrint.CameraFOV.ToString();
            CtrMotionIndexText.Text = BluePrint.MotionIndex.ToString();
            CtrMotionFrameText.Text = BluePrint.MotionFrame.ToString();

            CtrCamFOV.Value = BluePrint.CameraFOV;
            CtrMotionIndex.Value    = BluePrint.MotionIndex;
            CtrMotionFrame.Value    = BluePrint.MotionFrame;
            CtrEnableShadow.Checked = BluePrint.EnableShadow;
        }
    }
}
