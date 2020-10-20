using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SymbolResolveManager
{
    /// <summary>
    /// 複数のプロセスを並列で立ち上げ、終了を待機するためのクラス
    /// </summary>
    class ProcessManager
    {
        const int WaitMillSecSpan = 5;

        int waitCounter;
        object waitCounterLock;

        public HashSet<string> FailProcessInfo { get; private set; }
        public bool QuietManagerMessage { get; private set; }

        /// <summary>
        /// コンストラクタ
        /// </summary>
        /// <param name="quietManagerMessage">ProcessManagerからのメッセージを出力するか</param>
        public ProcessManager(bool quietManagerMessage = false)
        {
            this.waitCounter = 0;
            this.waitCounterLock = new object();

            this.FailProcessInfo = new HashSet<string>();

            this.QuietManagerMessage = quietManagerMessage;
        }

        /// <summary>
        /// 指定したProcessを立ち上げる
        /// </summary>
        /// <param name="ps"></param>
        public void DispatchProcess(System.Diagnostics.ProcessStartInfo ps)
        {
            lock (this.waitCounterLock)
            {
                ++this.waitCounter;
            }
            ps.RedirectStandardOutput = true;
            ps.UseShellExecute = false;
            var proc = System.Diagnostics.Process.Start(ps);
            proc.EnableRaisingEvents = true;
            proc.Exited += (sender, arg) =>
            {
                if (proc.ExitCode != 0)
                {
                    this.FailProcessInfo.Add(
                        String.Format(
                            "Fail:[{0}]がコード({1})で終了しました。",
                            proc.StartInfo.FileName + " " + proc.StartInfo.Arguments,
                            proc.ExitCode
                        ) + Environment.NewLine + proc.StandardOutput.ReadToEnd()
                    );
                }
                lock (this.waitCounterLock)
                {
                    --this.waitCounter;
                }
                this.WriteLine("Rest: " + this.waitCounter + " proc.");
            };
        }

        /// <summary>
        /// DispatchProcessで実行したプロセスが全て終了するまで待つ
        /// </summary>
        public void WaitAll()
        {
            while (true)
            {
                lock (this.waitCounterLock)
                {
                    if (this.waitCounter < 1)
                    {
                        break;
                    }
                }

                System.Threading.Thread.Sleep(WaitMillSecSpan);
            }
            this.WriteLine("All Proc End.");
        }

        private void WriteLine(string str)
        {
            if (this.QuietManagerMessage == false)
            {
                Console.WriteLine(str);
            }
        }
    }
}
