using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Drawing;

namespace MakeGUI
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        MakeFileAnalyser makeFileAnalyser;
        MakeDriver makeDriver;

        string currentSelectedTarget = null;
        TargetType currentTargetType;

        Setting setting;

        public MainWindow()
        {
            InitializeComponent();

            // コンバート成功時のみに「終了コード0」を返すために仕込み
            Environment.ExitCode = 1;

            var args = Environment.GetCommandLineArgs();

            this.LoadSettingFile(AppDomain.CurrentDomain.BaseDirectory + "\\setting.xml");

            this.SetUpMakeDriver(args.Contains("-r"));

            // MakeFile解析
            var path = "MakeFile";
            if (args.Length > 1)
            {
                path = args[1];
            }
            

            this.LoadMakeFile(path);
        }

        /// <summary>
        /// MakeGUIに対する設定ファイルの読み込み
        /// </summary>
        /// <param name="path"></param>
        void LoadSettingFile(string path)
        {
            this.setting = new Setting(path);

            // プラットホーム選択UIの設定
            this.platformPanel.Children.Clear();
            foreach (var pair in this.setting.PlatformSettings)
            {
                var radio = new RadioButton();
                radio.Margin = new System.Windows.Thickness(5);
                radio.Content = pair.Value.DisplayName;
                radio.Tag = pair.Value.Flag;
                this.platformPanel.Children.Add(radio);
            }

            (this.platformPanel.Children[0] as RadioButton).IsChecked = true;

        }

        /// <summary>
        /// MakeDriverクラスにイベントや設定ファイルパスを設定する
        /// </summary>
        /// <param name="isRedirectMode"></param>
        void SetUpMakeDriver(bool isRedirectMode)
        {
            this.makeDriver = new MakeDriver();
            this.makeDriver.ConvertStart += this.ConvertStart;
            this.makeDriver.ConvertExited += this.ConvertExited;
            // -rが指定されていた場合、出力を標準出力にリダイレクト
            if (isRedirectMode)
            {
                this.makeDriver.ConvertExited += (exitCode) => App.Current.Dispatcher.Invoke(() => Environment.Exit(exitCode));

                this.makeDriver.OutputStandardMessage += Console.WriteLine;
                this.makeDriver.OutputErrorMessage += Console.Error.WriteLine;
            }
            else
            {
                this.makeDriver.OutputStandardMessage += (line) => this.OutputMessageWithColor(line, Brushes.Black);
                this.makeDriver.OutputErrorMessage += (line) => this.OutputMessageWithColor(line, Brushes.Red);
            }

            this.makeDriver.IBProfileFilePath = this.setting.IBProfileFilePath;
        }

        /// <summary>
        /// Makefileの読み込み
        /// </summary>
        /// <param name="path"></param>
        void LoadMakeFile(string path)
        {
            try
            {
                this.makeFileAnalyser = new MakeFileAnalyser(path, this.setting);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message + Environment.NewLine + e.Data, "MakeFile解析エラー");
                Environment.Exit(1);
            }

            // MakeFileの内容を元にGUIの初期化            
            this.TitleLabel.Text = this.makeFileAnalyser.Title;

            // 動的リストが取得されていなければコントロール無効化
            if (this.makeFileAnalyser.DynamicBuildTargets == null)
            {
                this.DynamicTargetGroup.IsEnabled = false;
                this.DynamicTargetSelectBox.ItemsSource = null;
            }
            else
            {
                this.DynamicTargetGroup.IsEnabled = true;
                this.DynamicTargetSelectBox.ItemsSource = this.makeFileAnalyser.DynamicBuildTargets.Keys;
            }
            this.StaticTargetSelectBox.ItemsSource = this.makeFileAnalyser.StaticBuildTargets.Keys;

            this.currentSelectedTarget = this.StaticTargetSelectBox.SelectedItem as string;
            this.currentTargetType = TargetType.Static;
        }

        private string GetPlatformBatFullPath(uint platformFlag)
        {
            var batPath = this.setting.PlatformSettings.First(item => item.Value.Flag == platformFlag).Value.BatPath;

            return Environment.ExpandEnvironmentVariables(batPath);
        }

        #region UIイベント用

        /// <summary>
        /// ターゲット指定のラジオボタンやドロップダウンリストが操作された時に呼ばれるイベント
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void TargetSelectionChanged(object sender, RoutedEventArgs e)
        {
            // 全コントロールの初期化が終わるまでは無効
            // (ドロップダウンリストの初期化より先にラジオボタンの初期化が走るとエラーが出るのを防ぐ)
            if (this.IsInitialized == false)
            {
                return;
            }

            Dictionary<string, TargetInfo> selection = null;

            switch ((sender as Control).Tag as string)
            {
                case "StaticTarget":
                    this.currentSelectedTarget = this.StaticTargetSelectBox.SelectedItem as string;
                    this.currentTargetType = TargetType.Static;
                    selection = this.makeFileAnalyser.StaticBuildTargets;
                    break;
                case "DynamicTarget":
                    this.currentSelectedTarget = this.DynamicTargetSelectBox.SelectedItem as string;
                    this.currentTargetType = TargetType.Dynamic;
                    selection = this.makeFileAnalyser.DynamicBuildTargets;
                    break;
            }

            if (selection != null)
            {
                this.TargetCommentLabel.Text = selection[this.currentSelectedTarget].Comment;

                var platform = this.makeFileAnalyser.GetAvailablePlatform(this.currentSelectedTarget, this.currentTargetType);

                foreach (RadioButton item in this.platformPanel.Children) {
                    var flag = (uint)item.Tag;
                    item.IsEnabled = (platform & flag) == flag;
                }

            }
        }


        /// <summary>
        /// 「Convert」が押されたときのイベント
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void AcceptButton_Click(object sender, RoutedEventArgs e)
        {
            (this.OutPutBox.Document.Blocks.ElementAt(0) as System.Windows.Documents.Paragraph).Inlines.Clear();

            this.SetStatusInfo("コンバート中", Brushes.Goldenrod);

            var currentPlatform = (uint)this.platformPanel.Children.Cast<RadioButton>().First(item => item.IsChecked.Value).Tag;


            this.makeDriver.Exec(
                this.makeFileAnalyser.FileFullPath,
                this.makeFileAnalyser.ConvertTargetString(this.currentSelectedTarget, this.currentTargetType, currentPlatform),
                this.useIBCheckBox.IsChecked.Value,
                this.GetPlatformBatFullPath(currentPlatform),
                this.verboseCheckBox.IsChecked.Value
            );
        }

        /// <summary>
        /// 「Cancel」が押されたときのイベント
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void AbortButton_Click(object sender, RoutedEventArgs e)
        {
            this.makeDriver.Abort();
        }

        /// <summary>
        /// Windowが閉じられようとしたときのイベント
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (this.makeDriver.IsConverting == false)
            {
                return;
            }

            // コンバート処理中はウィンドウを閉じることが出来ない
            MessageBox.Show("コンバート処理中です");
            e.Cancel = true;
        }

        #endregion

        /// <summary>
        /// コンバートプロセス開始時に呼ばれる処理
        /// </summary>
        void ConvertStart()
        {
            this.Dispatcher.Invoke(() => this.LockConvert(true));
        }

        /// <summary>
        /// コンバートプロセス終了に呼ばれる処理
        /// </summary>
        /// <param name="exitCode"></param>
        void ConvertExited(int exitCode)
        {
            this.Dispatcher.Invoke(() => this.LockConvert(false));

            if (exitCode == 0)
            {
                this.SetStatusInfo("成功", Brushes.Green);
            }
            else
            {
                this.SetStatusInfo("失敗", Brushes.Red);
            }
        }

        #region UI操作用

        void LockConvert(bool value)
        {
            this.ControlPanel.IsEnabled = !value;
            this.AcceptButton.Visibility = (value) ? Visibility.Collapsed : Visibility.Visible;
            this.AbortButton.Visibility = (value) ? Visibility.Visible : Visibility.Collapsed;
        }

        /// <summary>
        /// ステータスメッセージの変更
        /// </summary>
        /// <param name="message"></param>
        /// <param name="brush"></param>
        void SetStatusInfo(string message, Brush brush)
        {
            /// (別スレッドから操作される場合があるのでInvoke)
            this.Dispatcher.Invoke(() =>
            {
                this.StatusLabel.Content = message;
//                this.StatusLabel.Foreground = brush;
                this.StatusLabel.Background = brush;
                this.StatusLabel.Foreground = Brushes.White;
            });

        }

        /// <summary>
        /// 出力エリアに文字列を出力する
        /// 
        /// (スタンドアロンモード時のみ使用)
        /// </summary>
        /// <param name="line"></param>
        /// <param name="brush"></param>
        void OutputMessageWithColor(string line, SolidColorBrush brush)   
        {
            if (line == null)
            {
                return;
            }

            Action act =
                () =>
                {
                    if (this.OutputArea.Visibility != System.Windows.Visibility.Visible)
                    {
                        this.OutputArea.Visibility = System.Windows.Visibility.Visible;
                    }

                    var para = this.OutPutBox.Document.Blocks.ElementAt(0) as System.Windows.Documents.Paragraph;
                    var span = new System.Windows.Documents.Span { Foreground = brush };
                    span.Inlines.Add(line + Environment.NewLine);
                    para.Inlines.Add(span);
                };

            this.Dispatcher.BeginInvoke(act);
        }

        #endregion

    }
}
