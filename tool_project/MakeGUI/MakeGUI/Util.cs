using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MakeGUI
{
    static class Util
    {
        /// <summary>
        /// IBでコマンドを実行するためのプロセス情報を生成
        /// </summary>
        public static System.Diagnostics.ProcessStartInfo CreateIBProcessStartInfo(string command, string profileFilePath)
        {
            return new System.Diagnostics.ProcessStartInfo("IBConsole", String.Format("/profile=\"{0}\" /command=\"{1}\"", profileFilePath, command));
        }

        /// <summary>
        /// Cmd.exeでコマンドを実行するためのプロセス情報を生成
        /// </summary>
        /// <param name="command">実行するコマンド</param>
        /// <returns></returns>
        public static System.Diagnostics.ProcessStartInfo CreateCmdProcessStartInfo(string command)
        {
            return new System.Diagnostics.ProcessStartInfo("cmd", String.Format("/c \"{0}\"", command));
        }


        /// <summary>
        /// 複数のコマンドを連結し1つのコマンドにする
        /// </summary>
        /// <param name="commands"></param>
        /// <returns></returns>
        public static string CombineCommands(params string[] commands)
        {
            return String.Join(" & ", commands).Trim('&', ' ');
        }

        /// <summary>
        /// プロセスの実行結果を文字列で受け取る
        /// </summary>
        public static System.IO.StreamReader GetProcessStream(System.Diagnostics.ProcessStartInfo ps, System.Text.Encoding outEnc = null, int timeoutMillSec = int.MaxValue)
        {
            ps.UseShellExecute = false;
            ps.RedirectStandardOutput = true;
            ps.StandardOutputEncoding = outEnc;

            System.IO.MemoryStream output = null;

            using (var process = System.Diagnostics.Process.Start(ps))
            {                
                var sb = new StringBuilder();

                process.OutputDataReceived += (sender, e) => { if (e.Data != null) { sb.Append(e.Data); } };

                process.BeginOutputReadLine();

                if (process.WaitForExit(timeoutMillSec) == false)
                {
                    process.Kill();
                    throw new TimeoutException(string.Format("{0}がタイムアウトしました.", ps.ToString()));
                }

                process.CancelOutputRead();
                output = new System.IO.MemoryStream(outEnc.GetBytes(sb.ToString()));
            }

            return new System.IO.StreamReader(output);
        }



    }
}
