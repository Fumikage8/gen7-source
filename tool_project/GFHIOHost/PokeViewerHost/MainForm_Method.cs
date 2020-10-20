using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;

using GFHIOHostFormControl;
using GFHIOHostFormControl.Forms;
using GFHIOHostFormControl.Uility;


namespace PokeViewerHost
{
    public partial class MainForm : Form
    {
        /// <summary>
        ///   config設定をアプリケーションに反映する
        /// </summary>
        private void SetupFromConfig()
        {
            /*  pokeinfo    */
            {
                mPokeInfo = new Utility.PokeInfoTable();
                mPokeInfo.SetupNameListFromXml(mConfig.PokeNameListFilePath);
                mPokeInfo.SetupDataTableFromXml(mConfig.PokeInfoDefinitionFilePath);
            }

            //                    dataGridView_PokeList.DataSource = Utility.PokeInfo.CreateDataTableFromXml(mConfig.PokeInfoDefinitionFilePath);
            dataGridView_PokeList.DataSource = mPokeInfo.DataTable;

            /*   personalconfig   */
            PersonalConfig = Utility.PersonalConfig.Deserialize(PersonalConfigDefaultSerializePath);
            if(PersonalConfig == null) PersonalConfig = new Utility.PersonalConfig();
            PersonalConfig.SerializePath = string.Format(@"{0}\{1}", mConfig.WorkspacePath, PersonalConfigDefaultSerializePath);
            uCtr_PokeDexCapture.SetCompositeBackground(PersonalConfig.CompositeBackgoundImagePath);
        }

        /// <summary>
        ///   設定Formを開き、OKで終了されたらアプリケーションに反映させる
        /// </summary>
        private void ShowConfig()
        {
            Forms.Form_Config form = Forms.Form_Config.CreateFromConfig(mConfig);

            form.PokeInfoForPreview = Utility.PokeInfo.CreateFromTableRow(CurrentRow);
            DialogResult result = form.ShowDialog();

            if(result == DialogResult.OK)
            {
                form.ApplyTo(ref mConfig);
                SetupFromConfig();
            }
        }

        /// <summary>
        ///   genderstringを評価する
        /// </summary>
        private int ParseGenderString(string genderStr)
        {
            return (genderStr.Equals(@"female") ? 1 : 0);
        }

        /// <summary>
        ///   現在の選択行からpokeinfoを生成し、その内容で図鑑Viewを更新する
        /// </summary>
        private void UpdatePokedexView()
        {
            UpdatePokedexView(Utility.PokeInfo.CreateFromTableRow(CurrentRow));
        }

        /// <summary>
        ///   pokeinfoの内容で図鑑Viewを更新する
        /// </summary>
        private void UpdatePokedexView(Utility.PokeInfo info)
        {
            string fileName = info.MakeFileName(mConfig.FileNamingRule);
            string bmpName = fileName + @".bmp";
            string pngName = fileName + @".png";

            /*      */
            {
                string format = @"{0}\{1}\{2}";
                string colorPath = string.Format(format, mConfig.WorkspacePath, CapturePath_Color, bmpName);
                string alphaPath = string.Format(format, mConfig.WorkspacePath, CapturePath_Alpha, bmpName);
                string compoPath = string.Format(format, mConfig.WorkspacePath, CapturePath_Composite, pngName);
                Forms.UCtr_PokeDexCapture dexCtrl = uCtr_PokeDexCapture;


                /*  dispose */
                dexCtrl.DisposeAllImage();
                dexCtrl.ClearAllCaption();


                try { dexCtrl.Image_SourceColor = Utility.PokeDexImageTool.CreateImageFromFile(colorPath, true); }
                catch(Exception e) { Log(@"load ""{0}"" failed", colorPath); }

                try { dexCtrl.Image_SourceAlpha = Utility.PokeDexImageTool.CreateImageFromFile(alphaPath, true); }
                catch(Exception e) { Log(@"load ""{0}"" failed", alphaPath); }

                dexCtrl.Image_InvAlpha = Utility.PokeDexImageTool.CreateInvAlphaImage(dexCtrl.Image_SourceAlpha as Bitmap);
                dexCtrl.Image_Composite = Utility.PokeDexImageTool.CreateCompositeImage(dexCtrl.Image_SourceColor as Bitmap, dexCtrl.Image_InvAlpha as Bitmap);

                if(dexCtrl.Image_SourceColor != null) dexCtrl.Caption_SourceColor = colorPath;
                if(dexCtrl.Image_SourceAlpha != null) dexCtrl.Caption_SourceAlpha = alphaPath;
                if(dexCtrl.Image_InvAlpha != null) dexCtrl.Caption_InvAlpha = @"inverse alpha";
                if(dexCtrl.Image_Composite != null)
                {
                    dexCtrl.Caption_Composite = compoPath;

                    try
                    {
                        dexCtrl.Image_Composite.Save(compoPath);
                    }
                    catch(System.Exception ex)
                    {
                        Console.WriteLine(ex.Message);
                    }
                }
            }


            /*   blueprint   */
            {
                string format = @"{0}\{1}\{2}.xml";
                string path = string.Format(format, mConfig.WorkspacePath, BluePrintPath, fileName);
                if((BluePrint != null) && (!string.IsNullOrWhiteSpace(BluePrint.SerializePath)))
                {
                    try
                    {
                        Log(@"save: ""{0}""", BluePrint.SerializePath);
                        BluePrint.Save();
                    }
                    catch(Exception e)
                    {
                        Log(@"書き込みに失敗：""{0}""", BluePrint.SerializePath);
                        Log(e.Message);
                    }
                }

                BluePrint = Utility.BluePrint.SafeLoad(path);
            }

            /*   refresh list   */
            dataGridView_PokeList.Refresh();
        }

        /// <summary>
        ///   ctrにcaptureをリクエストする
        /// </summary>
        protected void CaptureRequest()
        {
            Utility.PokeInfo info = Utility.PokeInfo.CreateFromTableRow(CurrentRow);

            string fileName = info.MakeFileName(mConfig.FileNamingRule);
            string bmpName = fileName + @".bmp";

            string format = @"{0}\{1}\{2}";
            string colorPath = string.Format(format, mConfig.WorkspacePath, CapturePath_Color, bmpName);
            string alphaPath = string.Format(format, mConfig.WorkspacePath, CapturePath_Alpha, bmpName);


            HIOSendData data = new HIOSendData(HIOCtr.SIOChannels[@"capture"]);

            HIOSendDataByteBuilder db = new HIOSendDataByteBuilder();

            Log(@"capture request:");
            Log(@"  color: ""{0}""", colorPath);
            Log(@"  alpha: ""{0}""", alphaPath);
            db.Add(string.Format(@"{0},{1}", colorPath, alphaPath));

            data.Bytes = db.ToArray();
            data.Header.Bytes = data.Bytes.Length;

            BluePrint.Save();

            HIOCtr.SendData(data);

            Cursor.Current = Cursors.WaitCursor;
            System.Threading.Thread.Sleep(1000);
            UpdatePokedexView();
            Cursor.Current = Cursors.Default;
        }

        /// <summary>
        ///   blueprintのコピー
        /// </summary>
        protected void CopyBluePrint()
        {
            Log(@"CopyBluePrint");
            if(BluePrint != null)
            {
                ClippedBP = BluePrint;
            }
        }

        /// <summary>
        ///   blueprintをペースト
        /// </summary>
        protected void PasteBluePrint()
        {
            Log(@"PasteBluePrint");
            if(ClippedBP != null)
            {
                string serializePath = BluePrint.SerializePath;
                BluePrint = ClippedBP;
                BluePrint.SerializePath = serializePath;
                IsDirtyBluePrint = true;
            }
        }

        /// <summary>
        ///   color,alpha,composite画像を削除する
        /// </summary>
        private void DeleteImage()
        {
            Utility.PokeInfo info = Utility.PokeInfo.CreateFromTableRow(CurrentRow);

            string fileName = info.MakeFileName(mConfig.FileNamingRule);
            string bmpName = fileName + @".bmp";
            string pngName = fileName + @".png";

            string format = @"{0}\{1}\{2}";
            string colorPath = string.Format(format, mConfig.WorkspacePath, CapturePath_Color, bmpName);
            string alphaPath = string.Format(format, mConfig.WorkspacePath, CapturePath_Alpha, bmpName);
            string compositePath = string.Format(format, mConfig.WorkspacePath, CapturePath_Composite, pngName);


            string message;
            string title;

            title = @"Imageの削除";
            message = string.Format(
                            @"color: ""{0}""{1}alpha: ""{2}""{3}composite: ""{4}""{5}を削除します。{6}よろしいですか？"
                            , colorPath, Environment.NewLine
                            , alphaPath, Environment.NewLine
                            , compositePath, Environment.NewLine
                            , Environment.NewLine
                       );

            if(MessageBox.Show(message, title, MessageBoxButtons.YesNo) == DialogResult.Yes)
            {
                try
                {
                    Log(@"delete: ""{0}""", colorPath);
                    System.IO.File.Delete(colorPath);

                    Log(@"delete: ""{0}""", alphaPath);
                    System.IO.File.Delete(alphaPath);

                    Log(@"delete: ""{0}""", compositePath);
                    System.IO.File.Delete(compositePath);
                }
                catch(Exception e)
                {
                    Log(e.Message);
                }
                finally
                {
                    UpdatePokedexView();
                }
            }
        }


        /// <summary>
        ///   blueprintをCTRに送信する
        /// </summary>
        private void SendBluePrint(Utility.BluePrint bp)
        {
            if(bp != null)
            {
                HIOCtr.SendData(bp.CreateHIOSendData(HIOCtr.SIOChannels[@"blueprint"]));
            }
        }


        /// <summary>
        ///   blueprint設定Formを表示し、OKで終了されたら変更を反映させる
        /// </summary>
        protected void ShowBluePrintForm()
        {
            /*
             * open BP form
             * */
            using(Forms.Form_BluePrint form = new Forms.Form_BluePrint())
            {
                Utility.BluePrint oldBP = (Utility.BluePrint)BluePrint.Clone();

                form.OnPropertyChanged = OnBluePrintFormPropertyChanged;
                form.LogForm = LogForm;
                form.ClippedBP = ClippedBP;
                form.SetupFromBluePrint(BluePrint);
                if(form.ShowDialog() != DialogResult.OK)
                {
                    BluePrint = oldBP;
                    IsDirtyBluePrint = true;
                }
                ClippedBP = form.ClippedBP;
            }
        }

        /// <summary>
        ///   その他の情報をCTRに送信
        /// </summary>
        private void SendOtherData()
        {
            HIOSendData data = new HIOSendData(HIOCtr.SIOChannels[@"others"]);
            int index = 0;
            int[] userData = data.Header.UserData;

            /*      */
            userData[index++] = GuideGridSize;

            HIOCtr.SendData(data);
        }


        /// <summary>
        ///   ログ出力
        /// </summary>
        public void Log(string format, params object[] args)
        {
            Console.WriteLine(string.Format(format, args));

            if(LogForm != null)
            {
                LogForm.WriteLine(format, args);
            }
        }
    
    }
}
