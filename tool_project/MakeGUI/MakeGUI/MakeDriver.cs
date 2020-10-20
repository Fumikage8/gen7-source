using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MakeGUI
{

    /// <summary>
    /// Makeの実行を行うクラス
    /// </summary>
    class MakeDriver
    {
        /// <summary>
        /// コンバートプロセスで標準出力に出力されたときに発生するイベント
        /// </summary>
        public event Action<string> OutputStandardMessage;

        /// <summary>
        /// コンバートプロセスで標準エラー出力に出力されたときに発生するイベント
        /// </summary>
        public event Action<string> OutputErrorMessage;

        /// <summary>
        /// コンバートプロセスが立ち上がった直後に発生するイベント
        /// </summary>
        public event Action ConvertStart;

        /// <summary>
        /// コンバートプロセスが終了した直後に発生するイベント
        /// </summary>
        public event Action<int> ConvertExited;

        /// <summary>
        /// 現在コンバート中かどうか
        /// </summary>
        public bool IsConverting { get; private set; }

        /// <summary>
        /// IBのプロファイルファイルパスの取得と設定
        /// </summary>
        public string IBProfileFilePath { get; set; }

        /// <summary>
        /// 実行中のMakeプロセス
        /// </summary>
        System.Diagnostics.Process process;


        System.IO.StreamWriter logFileWriter;


        /// <summary>
        /// Makeを実行する
        /// プロセスを立ち上げた後、このメソッドはすぐに制御を返す
        /// </summary>
        /// <param name="makefilePath"></param>
        /// <param name="makeArg"></param>
        /// <param name="useIB"></param>
        public void Exec(string makefilePath, string makeArg, bool useIB, string preExecBatPath, bool verbose)
        {
            var preCommand = string.IsNullOrEmpty(preExecBatPath) ? "" : string.Format("call {0}", preExecBatPath);

            var makeCommand = string.Format(
                "make -f {0} {1}",
                makefilePath,
                makeArg
            );
            var command = Util.CombineCommands(preCommand, makeCommand);

            // ログファイル作成
            try
            {
                this.logFileWriter = new System.IO.StreamWriter("log.txt");
            }
            catch (System.Exception e)
            {
                this.logFileWriter = null;
                System.Windows.MessageBox.Show("ログファイルが開けませんでした.");

                this.OnConvertExited(-1);
                return;
            }

            // プロセスの生成
            System.Diagnostics.ProcessStartInfo ps = (useIB) ? Util.CreateIBProcessStartInfo(command, this.IBProfileFilePath) : Util.CreateCmdProcessStartInfo(command);

            ps.StandardOutputEncoding = System.Text.Encoding.UTF8;
            ps.StandardErrorEncoding = System.Text.Encoding.UTF8;

            this.process = new System.Diagnostics.Process();
            this.process.StartInfo = ps;
            this.process.StartInfo.CreateNoWindow = true;
            this.process.StartInfo.UseShellExecute = false;

            // 入出力のリダイレクト
            this.process.StartInfo.RedirectStandardInput = true;
            this.process.StartInfo.RedirectStandardOutput = true;
            this.process.StartInfo.RedirectStandardError = true;

            this.process.OutputDataReceived += (sender, e) => this.logFileWriter.WriteLine(e.Data);
            if (verbose)
            {
                this.process.OutputDataReceived += (sender, e) => this.OutputStandardMessage(e.Data);
            }
            else
            {
                this.process.OutputDataReceived += (sender, e) =>
                {
                    if (e.Data == null)
                    {
                        return;
                    }

                    if (e.Data.Trim().StartsWith("make") || e.Data.Trim().StartsWith("***") || e.Data.Trim().StartsWith("[="))
                    {
                        this.OutputStandardMessage(e.Data);
                    }
                };
            }

            this.process.ErrorDataReceived += (sender, e) => this.logFileWriter.WriteLine(e.Data);
            this.process.ErrorDataReceived += (sender, e) => this.OutputErrorMessage(e.Data);

            // プロセスの終了は非同期で待つ
            this.process.EnableRaisingEvents = true;
            this.process.Exited += (sender, e) => this.OnConvertExited(this.process.ExitCode);

            this.process.Start();
            this.process.BeginOutputReadLine();
            this.process.BeginErrorReadLine();            

            this.OnConvertStart();
        }

        /// <summary>
        /// プロセスを強制終了させる
        /// </summary>
        public void Abort()
        {
            if (this.process.HasExited)
            {
                return;
            }
            this.process.Kill();
            this.process.Close();
            this.process.Dispose();
            this.process = null;
            this.OnConvertExited(-1);
        }

        #region private methods

        private void OnConvertStart()
        {
            if (this.ConvertStart != null)
            {
                this.ConvertStart();
            }
            this.IsConverting = true;
        }

        private void OnConvertExited(int exitCode)
        {
            if (this.logFileWriter != null)
            {
                this.logFileWriter.Close();
                this.logFileWriter = null;
            }

            if (this.ConvertExited != null)
            {
                this.ConvertExited(exitCode);
            }
            this.IsConverting = false;
        }

        #endregion
    }
}
