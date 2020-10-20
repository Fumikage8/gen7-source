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
        protected string TemporaryDirName { get { return @"$tmp$"; } }
        /*   conv   */
        protected string TemporaryConvertCommandDirectory { get { return temporaryConvertCommnadDirectory; } set { temporaryConvertCommnadDirectory = value; } }
        protected string TemoraryConvertCommandFormat { get { return @"/c {0}\GFMdlConvert.bat {1} {2}"; } }
        protected string TemporaryMotConvertCommandFormat { get { return @"/c {0}\GFMotConvert.bat {1} {2}"; } }

        /*      */
        private string temporaryConvertCommnadDirectory = @".";


        /*
         * 
         * */
        private void AddRequest(string path)
        {
            Log(@"{0}:""{1}""", System.Reflection.MethodBase.GetCurrentMethod().Name, path);

            string ext = System.IO.Path.GetExtension(path).ToLower();
            GFTreeNode addedNode = null;
            TreeNodeCollection targetNodes = treeView_Models.Nodes;

            if (ext.Equals(GFTreeNode_ModelBin.Extension))
            {
                /*   model:bin   */
                string headerDescription = System.IO.Path.GetFileNameWithoutExtension(path);

                SendModel(path, headerDescription);
                {
                    addedNode = GFTreeNode_ModelBin.CreateFromFile(path);
                    addedNode.HeaderDescription = headerDescription;
                    targetNodes.Add(addedNode);
                }
            }
            else if (ext.Equals(GFTreeNode_ModelAscii.Extension))
            {
                /*   model:asc   */
                string tmpPath = GFMdlConvert(path);

                if (!String.IsNullOrEmpty(tmpPath))
                {
                    string headerDescription    = System.IO.Path.GetFileNameWithoutExtension(tmpPath);

                    SendModel(tmpPath, headerDescription);

                    {
                        addedNode = GFTreeNode_ModelAscii.CreateFromFile(path);
                        addedNode.HeaderDescription = headerDescription;
                        targetNodes.Add(addedNode);
                        addedNode.Nodes.Add(GFTreeNode_ModelBin.CreateFromFile(tmpPath));
                    }
                }
            }
            else if (ext.Equals(GFTreeNode_MotionBin.Extension))
            {
                /*   motion:bin   */
                GFTreeNode_ModelBase selectedTopNode = GetSelectedTopNode();

                if (selectedTopNode != null)
                {

                    string headerDescription = System.IO.Path.GetFileNameWithoutExtension(path);

                    SendMotion(path, headerDescription, selectedTopNode.HeaderDescription.GetHashCode());

                    selectedTopNode.SetupMotionFromFile(path);
                    addedNode = selectedTopNode.Motion;
                    addedNode.HeaderDescription = headerDescription;
                }
            }
            else if (ext.Equals(GFTreeNode_MotionAscii.Extension))
            {
                /*   motion:bin   */
                GFTreeNode_ModelBase selectedTopNode = GetSelectedTopNode();

                if (selectedTopNode != null)
                {
                    string tmpPath = GFMotConvert(path);

                    if (!String.IsNullOrEmpty(tmpPath))
                    {
                        string headerDescription = System.IO.Path.GetFileNameWithoutExtension(tmpPath);

                        SendMotion(tmpPath, headerDescription, selectedTopNode.HeaderDescription.GetHashCode());

                        {

                            selectedTopNode.SetupMotionFromFile(path);
                            addedNode = selectedTopNode.Motion;
                            addedNode.HeaderDescription = headerDescription;

                            addedNode.Nodes.Add(GFTreeNode_MotionBin.CreateFromFile(tmpPath));
                        }
                    }
                }
            }


            if (addedNode != null)
            {
                AddToRecentFileList(path);
                treeView_Models.SelectedNode = addedNode;
            }
        }


        /*
         * 
         * 
         * */
        private void treeView_Models_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                string[] paths = (string[])e.Data.GetData(DataFormats.FileDrop, false);

                e.Effect = DragDropEffects.None;
                foreach (string path in paths)
                {
                    string ext = System.IO.Path.GetExtension(path).ToLower();

                    if (
                        ext.Equals(GFTreeNode_ModelBin.Extension)
                        || ext.Equals(GFTreeNode_ModelAscii.Extension)
                        || ext.Equals(GFTreeNode_MotionBin.Extension)
                        || ext.Equals(GFTreeNode_MotionAscii.Extension)
                     )
                    {
                        e.Effect = DragDropEffects.Copy;
                        break;
                    }
                }
            }
        }

        private void treeView_Models_DragDrop(object sender, DragEventArgs e)
        {
            string[] paths = (string[])e.Data.GetData(DataFormats.FileDrop, false);

            foreach (string path in paths)
            {
                AddRequest(path);
            }

            WriteRecentFileList();
        }

        /*
         * キー押下
         * */
        private void treeView_Models_KeyDown(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Delete:
                    {
                        GFTreeNode_ModelBase targetNode = GetSelectedTopNode();

                        if (targetNode != null)
                        {
                            treeView_Models.Nodes.Remove(targetNode);
                            SendRemoveModel(targetNode);
                        }
                    }
                    break;

                default:
                    break;
            }
        }


        private GFTreeNode_ModelBase GetSelectedTopNode()
        {
            TreeNode node = treeView_Models.SelectedNode; /*   選択中のNodeは一般Nodeの場合もある   */

            if(node == null)    return null;

            while(node.Parent != null)
            {
                node    = node.Parent;
            }

            return (node as GFTreeNode_ModelBase);    /*   Topは派生Nodeである   */
        }



        private void ToolStripMenuItem_SendAllModel_Click(object sender, EventArgs e)
        {
            TreeNodeCollection  targetNodes = treeView_Models.Nodes;
            TreeNode[] nodes = new TreeNode[targetNodes.Count];

            targetNodes.CopyTo(nodes, 0);
            targetNodes.Clear();
            foreach (TreeNode node in nodes)
            {
                GFTreeNode_ModelBase    modelNode = (node as GFTreeNode_ModelBase);

                if(modelNode == null)   continue;

                SendRemoveModel(modelNode);
                AddRequest(modelNode.FilePath);
                if (modelNode.Motion != null)
                {
                    AddRequest(modelNode.Motion.FilePath);
                }
            }
        }

        private void ToolStripMenuItem_DeleteAllTreeModel_Click(object sender, EventArgs e)
        {
            TreeNodeCollection targetNodes = treeView_Models.Nodes;
            TreeNode[] nodes = new TreeNode[targetNodes.Count];

            targetNodes.CopyTo(nodes, 0);
            foreach (TreeNode node in nodes)
            {
                GFTreeNode_ModelBase modelNode = (node as GFTreeNode_ModelBase);

                if (modelNode == null) continue;

                /*   model削除コマンド送信   */
                SendRemoveModel(modelNode);
                targetNodes.Remove(modelNode);
            }
        }




        private void SendRemoveModel(GFTreeNode_ModelBase node)
        {
            Log(@"{0}:{1}", System.Reflection.MethodBase.GetCurrentMethod().Name, node.HeaderDescription);
            /*
              deleteModel
              [0]:1
              [1]:modelHash
            */
            HIOSendData data = new HIOSendData(HIO.SIOChannels[@"model"]);

            data.Header.Description = node.HeaderDescription;
            data.Header.UserData[0] = 1; /*   remove   */
            data.Header.UserData[1] = node.HeaderDescription.GetHashCode();
            HIO.SendData(data);
        }

        /*
         * gfbmdlpの送信
         * */
        private void SendModel(string path, string headerDescription)
        {
            Log(@"{0}:{1},""{2}""", System.Reflection.MethodBase.GetCurrentMethod().Name, headerDescription, path);
            using (System.IO.FileStream fStream = new System.IO.FileStream(path, System.IO.FileMode.Open, System.IO.FileAccess.Read))
            {
                byte[] bytes = new byte[fStream.Length];

                fStream.Read(bytes, 0, bytes.Length);

                /*
                  addModel
                  [0]:0
                  [1]:modelHash
                */
                {
                    Console.WriteLine(@"write: {0}", System.IO.Path.GetFileName(path));
                    Console.WriteLine(@"  {0}[B]", bytes.Length);

                    HIOSendData data = new HIOSendData(HIO.SIOChannels[@"model"]);

                    data.Header.Description = headerDescription;
                    data.Header.UserData[0] = 0;
                    data.Header.UserData[1] = headerDescription.GetHashCode();
                    data.Bytes = bytes;
                    HIO.SendData(data);
                }
            }
        }
        /*
         * gfbmotの追加
         * */
        private void SendMotion(string path, string headerDescription, int modelHash)
        {
            Log(@"{0}:{1}(#{2}),""{3}""", System.Reflection.MethodBase.GetCurrentMethod().Name, headerDescription, modelHash, path);
            using (System.IO.FileStream fStream = new System.IO.FileStream(path, System.IO.FileMode.Open, System.IO.FileAccess.Read))
            {
                byte[] bytes = new byte[fStream.Length];

                fStream.Read(bytes, 0, bytes.Length);

                /*
                  addMotion
                  [0]:0
                  [1]:modelHash
                  [2]:motionHash
                */
                {
                    Console.WriteLine(@"write: {0}", System.IO.Path.GetFileName(path));
                    Console.WriteLine(@"  {0}[B]", bytes.Length);

                    HIOSendData data = new HIOSendData(HIO.SIOChannels[@"motion"]);

                    data.Header.Description = headerDescription;
                    data.Header.UserData[0] = 0;
                    data.Header.UserData[1] = modelHash;
                    data.Header.UserData[2] = headerDescription.GetHashCode();
                    data.Bytes = bytes;

                    HIO.SendData(data);
                }
            }
        }



        private string GFMdlConvert(string gfmdlPath)
        {
            Log(@"{0}:""{1}""", System.Reflection.MethodBase.GetCurrentMethod().Name, gfmdlPath);

            string tmpPath = @"";

            System.Diagnostics.Process proc = new System.Diagnostics.Process();

            proc.StartInfo.FileName = System.Environment.GetEnvironmentVariable("ComSpec");
            proc.StartInfo.UseShellExecute = false;
            proc.StartInfo.RedirectStandardOutput = true;
            proc.StartInfo.RedirectStandardInput = false;
            //ウィンドウを表示しないようにする
            proc.StartInfo.CreateNoWindow = true;
            //コマンドラインを指定（"/c"は実行後閉じるために必要）
            //            proc.StartInfo.Arguments = @"/c GFMdlConvert.bat " + gfmdlPath;

            proc.StartInfo.Arguments = String.Format(TemoraryConvertCommandFormat, TemporaryConvertCommandDirectory, gfmdlPath, TemporaryDirName);


            //起動
            proc.Start();

            //出力を読み取る
            //            string results = proc.StandardOutput.ReadToEnd();

            //プロセス終了まで待機する
            //WaitForExitはReadToEndの後である必要がある
            //(親プロセス、子プロセスでブロック防止のため)
            proc.WaitForExit();

            Log(@"{0}: code({1})", System.Reflection.MethodBase.GetCurrentMethod().Name, proc.ExitCode);
            if (proc.ExitCode == 0)
            {
                while (0 <= proc.StandardOutput.Peek())
                {
                    tmpPath = proc.StandardOutput.ReadLine();
                }
                Console.WriteLine(@"{0} -> {1}", gfmdlPath, tmpPath);
            }
            proc.Close();

            if (!string.IsNullOrEmpty(tmpPath))
            {
                Log(@"{0}: tmpPath""{1}""", System.Reflection.MethodBase.GetCurrentMethod().Name, tmpPath);
            }

            return tmpPath;
        }

        /*
         * convert motion 
         * */
        private string GFMotConvert(string gfmotPath)
        {
            Log(@"{0}:""{1}""", System.Reflection.MethodBase.GetCurrentMethod().Name, gfmotPath);

            string tmpPath = @"";

            System.Diagnostics.Process proc = new System.Diagnostics.Process();

            proc.StartInfo.FileName = System.Environment.GetEnvironmentVariable("ComSpec");
            proc.StartInfo.UseShellExecute = false;
            proc.StartInfo.RedirectStandardOutput = true;
            proc.StartInfo.RedirectStandardInput = false;
            //ウィンドウを表示しないようにする
            proc.StartInfo.CreateNoWindow = true;
            //コマンドラインを指定（"/c"は実行後閉じるために必要）
            //            proc.StartInfo.Arguments = @"/c GFMdlConvert.bat " + gfmdlPath;

            proc.StartInfo.Arguments = String.Format(TemporaryMotConvertCommandFormat, TemporaryConvertCommandDirectory, gfmotPath, TemporaryDirName);


            //起動
            proc.Start();

            //出力を読み取る
            //            string results = proc.StandardOutput.ReadToEnd();

            //プロセス終了まで待機する
            //WaitForExitはReadToEndの後である必要がある
            //(親プロセス、子プロセスでブロック防止のため)
            proc.WaitForExit();

            Log(@"{0}: code({1})", System.Reflection.MethodBase.GetCurrentMethod().Name, proc.ExitCode);
            if (proc.ExitCode == 0)
            {
                while (0 <= proc.StandardOutput.Peek())
                {
                    tmpPath = proc.StandardOutput.ReadLine();
                }
                Console.WriteLine(@"{0} -> {1}", gfmotPath, tmpPath);
            }
            proc.Close();

            if (!string.IsNullOrEmpty(tmpPath))
            {
                Log(@"{0}: tmpPath""{1}""", System.Reflection.MethodBase.GetCurrentMethod().Name, tmpPath);
            }

            return tmpPath;
        }
    };
}
