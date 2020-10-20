using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GFHIOHost
{
    public partial class Form1 : Form
    {
        /*   properties   */
        protected string RecentFileListPath { get { return @"RecentFiles.lst"; } }
        protected int RecentFileMax { get { return 64; } }

        /*   members   */




        /*
         * 
         * */
        protected void InitializeRecentFileList()
        {
            try
            {
                using (System.IO.StreamReader reader = new System.IO.StreamReader(RecentFileListPath))
                {
                    while (0 <= reader.Peek())
                    {
                        string line = reader.ReadLine();

                        if (!String.IsNullOrWhiteSpace(line))
                        {
                            /*   新規追加   */
                            AddToRecentFileList(line);
                        }
                    }
                }
            }
            catch (System.IO.FileNotFoundException e)
            {
                WriteRecentFileList();
            }
            finally
            {
                UpdateRecentFileListState();
            }
        }

        /*
         * 
         * */
        protected void WriteRecentFileList()
        {
            using (System.IO.StreamWriter writer = new System.IO.StreamWriter(RecentFileListPath, false))
            {
                foreach (ToolStripMenuItem item in ToolStripMenuItem_RecentFiles.DropDownItems)
                {
                    writer.WriteLine(item.Text);
                }
            }
        }


        /*
         * 
         * */
        protected void AddToRecentFileList(string path)
        {
            ToolStripItemCollection items   = ToolStripMenuItem_RecentFiles.DropDownItems;
            ToolStripMenuItem newItem = new ToolStripMenuItem(path);

            newItem.Click += new EventHandler(this.RecentFileMenuItem_OnClick);
           
            foreach(ToolStripMenuItem item in items)
            {
                if(item.Text.Equals(path))
                {
                    items.Remove(item);
                    break;
                }
            }

            items.Insert(0, newItem);
            if (RecentFileMax < items.Count)
            {
                int diff = items.Count - RecentFileMax;
                for (int count = 0; count < diff; ++count)
                {
                    items.RemoveAt(items.Count - 1);
                }
            }

            UpdateRecentFileListState();
        }

        private void UpdateRecentFileListState()
        {
            ToolStripMenuItem_RecentFiles.Enabled = (0 < ToolStripMenuItem_RecentFiles.DropDownItems.Count);
        }

        private void RecentFileMenuItem_OnClick(object sender, EventArgs e)
        {
            AddRequest(sender.ToString());
        }
    };
}
