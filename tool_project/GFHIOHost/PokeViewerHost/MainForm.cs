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

namespace PokeViewerHost
{
    public partial class MainForm : Form
    {
        /*
         * 
         * */
        public Form_Log LogForm { get { return mLogForm; } }
        protected UCtr_HIODevice HIOCtr { get { return uCtr_HIODevice; } }

        protected Utility.BluePrint BluePrint { get; set; }
        protected Utility.BluePrint ClippedBP { get; set; }
        protected bool IsDirtyBluePrint { get; set; }

        protected string CapturePath_Alpha { get { return mConfig.ImageDir_Alpha; } }
        protected string CapturePath_Color { get { return mConfig.ImageDir_Color; } }
        protected string CapturePath_Composite { get { return mConfig.ImageDir_Composite; } }
        protected string BluePrintPath { get { return mConfig.BluePrintDir; } }

        protected string PersonalConfigDefaultSerializePath { get { return @"$PersonalData$\PersonalConfig.xml"; } }
        protected Utility.PersonalConfig PersonalConfig { get; set; }

        protected int GuideGridSize { get; set; }

        /*
         * 
         * */
        /*   members   */
        protected Utility.Config mConfig = new Utility.Config();
        protected Utility.PokeInfoTable mPokeInfo = null;
        public Form_Log mLogForm = new Form_Log();


        public MainForm()
        {
            InitializeComponent();

            /*   hio   */
            HIOCtr.LogForm = LogForm;
            HIOCtr.BindControl(splitContainer_Main);
            HIOCtr.SIOChannels.Add(@"capture", 0);
            HIOCtr.SIOChannels.Add(@"blueprint", 1);
            HIOCtr.SIOChannels.Add(@"pokemon", 2);
            HIOCtr.SIOChannels.Add(@"others", 3);

            HIOCtr.SetTimer(100);
            HIOCtr.HIOTimer.Tick += new EventHandler(OnHIOTimerTick);
            HIOCtr.HIOTimer.Start();

            /*   bp   */
            BluePrint = new Utility.BluePrint();
            IsDirtyBluePrint = false;

            uCtr_PokeDexCapture.OnCompositeBackgroundChanged += CompositeBackgroundChanged;

            GuideGridSize = 0;
        }


        /// <summary>
        ///   
        /// </summary>
        private void MainForm_Load(object sender, EventArgs e)
        {
            var assm = System.Reflection.Assembly.GetExecutingAssembly();
            var name    = assm.GetName();

            this.Text = string.Format(
                            @"{0} {1} {2}",
                            name.Name,
                            name.Version.ToString(),
#if DEBUG
                            @"(debug)"
#else
                            @""
#endif
                        );

            /*
             * args
             * */
            {
                string[] args = System.Environment.GetCommandLineArgs();

                if ((1 < args.Length) && (!String.IsNullOrWhiteSpace(args[1])))
                {
                    mConfig = Utility.Config.Deserialize(args[1]);

                    SetupFromConfig();
                }
            }
        }


        /// <summary>
        ///   合成画像Paneの背景画像の設定が更新された際のハンドラ
        /// </summary>
        private void CompositeBackgroundChanged(Forms.UCtr_PokeDexCapture sender, Forms.UCtr_PokeDexCapture.CompositeBackgroundChangedEventArgs e)
        {
            PersonalConfig.CompositeBackgoundImagePath = e.Path;
            PersonalConfig.Serialize();
        }


        /// <summary>
        ///   
        /// </summary>
        private void 読み込みOToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();

            ofd.Filter  = @"xml(*.xml)|*.xml|all(*.*)|*.*";
            ofd.RestoreDirectory = true;

            if(ofd.ShowDialog() == DialogResult.OK)
            {
                Utility.Config newConfig = Utility.Config.Deserialize(ofd.FileName);

                if (newConfig != null)
                {
                    mConfig = newConfig;
                }
            }
        }

        /// <summary>
        ///   
        /// </summary>
        private void 保存ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            mConfig.Serialize();
        }

        /// <summary>
        ///   configuration
        /// </summary>
        private void 設定CToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ShowConfig();
        }



        /// <summary>
        ///   選択セルが変更されたときのハンドラ
        /// </summary>
        private DataRow CurrentRow { get; set; }
        private void dataGridView_PokeList_CurrentCellChanged(object sender, EventArgs e)
        {
            DataGridView gridView = sender as DataGridView;


            if(gridView == null)   return;
            if (gridView.CurrentRow == null) return;

            {
                DataRow row = (gridView.CurrentRow.DataBoundItem as DataRowView).Row;

                if (row == CurrentRow) return;

                CurrentRow = row;
            }

            Utility.PokeInfo info = Utility.PokeInfo.CreateFromTableRow(CurrentRow);

            UpdatePokedexView(info);

            /*  送信  */
            {
                HIOSendData data = new HIOSendData(HIOCtr.SIOChannels[@"pokemon"]);

                data.Header.UserData[0] = info.MonsNo;
                data.Header.UserData[1] = info.FormNo;
                data.Header.UserData[2] = info.GenderInt;
                data.Header.UserData[3] = info.IsRareInt;

                HIOCtr.SendData(data);
                System.Threading.Thread.Sleep(100);
                IsDirtyBluePrint = true;
            }
        }


        
        private void ToolStripMenuItem_Log_Click(object sender, EventArgs e)
        {
            LogForm.Show();
        }

        /// <summary>
        ///   HIO送信タイマのハンドラ
        /// </summary>
        private void OnHIOTimerTick(object sender, EventArgs e)
        {
            if (IsDirtyBluePrint)
            {
                SendBluePrint(BluePrint);
                IsDirtyBluePrint = false;
            }
        }


        private void ToolStripMenuItem_TestForm_Click(object sender, EventArgs e)
        {
            ShowBluePrintForm();
        }

        /// <summary>
        ///   blueprintのPropertyが変更されたときのハンドラ
        /// </summary>
        private void OnBluePrintFormPropertyChanged(Forms.Form_BluePrint sender)
        {
            IsDirtyBluePrint    = true;
        }


        private void ToolStripMenuItem_Capture_Click(object sender, EventArgs e)
        {
            CaptureRequest();
        }

        private void button_Capture_Click(object sender, EventArgs e)
        {
            ToolStripMenuItem_Capture_Click(sender, e);
        }

        private void button_Reload_Click(object sender, EventArgs e)
        {
            UpdatePokedexView();
        }

        private void ToolStripMenuItem_Copy_Click(object sender, EventArgs e)
        {
            CopyBluePrint();
        }

        private void ToolStripMenuItem_Paste_Click(object sender, EventArgs e)
        {
            PasteBluePrint();
        }


        private void dataGridView_PokeList_DoubleClick(object sender, EventArgs e)
        {
            ShowBluePrintForm();
        }

        private void ToolStripMenuItem_Capture_Click_1(object sender, EventArgs e)
        {
            CaptureRequest();
        }

        private void ToolStripMenuItem_Reload_Click(object sender, EventArgs e)
        {
            UpdatePokedexView();
        }

        private void button_DeleteImage_Click(object sender, EventArgs e)
        {
            DeleteImage();
        }

        private void ToolStripMenuItem_GuideRect_Hide_Click(object sender, EventArgs e)
        {
            GuideGridSize = 0;
            SendOtherData();
        }

        private void ToolStripMenuItem_GuideRect_Size1_Click(object sender, EventArgs e)
        {
            GuideGridSize = mConfig.GuideGridSize_1;
            SendOtherData();
        }

        private void ToolStripMenuItem_GuideRect_Size2_Click(object sender, EventArgs e)
        {
            GuideGridSize = mConfig.GuideGridSize_2;
            SendOtherData();
        }

        private void ToolStripMenuItem_GuideRect_Size3_Click(object sender, EventArgs e)
        {
            GuideGridSize = mConfig.GuideGridSize_3;
            SendOtherData();
        }

        private void dataGridView_PokeList_CellFormatting(object sender, DataGridViewCellFormattingEventArgs e)
        {
            DataGridView dgv = (sender as DataGridView);

            if(dgv.Columns[e.ColumnIndex].Name == PokeViewerHost.Utility.PokeInfo.Pair_CaptureState.Key)
            {
                Utility.PokeInfo info = Utility.PokeInfo.CreateFromTableRow((dgv.Rows[e.RowIndex].DataBoundItem as DataRowView).Row);

                if(info == null) return;


                string fileName = info.MakeFileName(mConfig.FileNamingRule);
                string pngName = fileName + @".png";

                string format = @"{0}\{1}\{2}";
                string compositePath = string.Format(format, mConfig.WorkspacePath, CapturePath_Composite, pngName);

                if(System.IO.File.Exists(compositePath))
                {
                    e.Value = PokeViewerHost.Utility.PokeInfo.CaptureStateString_Captured;
                }
                else
                {
                    e.Value = PokeViewerHost.Utility.PokeInfo.CaptureStateString_Nothing;
                }

                e.FormattingApplied = true;
            }
        }

        private void ToolStripMenuItem_Delete_Click(object sender, EventArgs e)
        {
            DeleteImage();
        }
    }
}
