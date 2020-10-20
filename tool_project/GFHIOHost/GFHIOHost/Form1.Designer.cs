namespace GFHIOHost
{
    partial class Form1
    {
        /// <summary>
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージ リソースが破棄される場合 true、破棄されない場合は false です。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows フォーム デザイナーで生成されたコード

        /// <summary>
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.uCtr_HIODevice = new GFHIOHostFormControl.Forms.UCtr_HIODevice();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage_Model = new System.Windows.Forms.TabPage();
            this.treeView_Models = new System.Windows.Forms.TreeView();
            this.contextMenu_ModelTree = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.ToolStripMenuItem_SendAllModel = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_DeleteAllTreeModel = new System.Windows.Forms.ToolStripMenuItem();
            this.tabPage_Camera = new System.Windows.Forms.TabPage();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.panel_MouseController = new System.Windows.Forms.Panel();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.ToolStripMenuItem_File = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_RecentFiles = new System.Windows.Forms.ToolStripMenuItem();
            this.表示VToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_ViewLog = new System.Windows.Forms.ToolStripMenuItem();
            this.groupBox1.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage_Model.SuspendLayout();
            this.contextMenu_ModelTree.SuspendLayout();
            this.tabPage_Camera.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // statusStrip1
            // 
            this.statusStrip1.Location = new System.Drawing.Point(0, 755);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(805, 22);
            this.statusStrip1.TabIndex = 1;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.uCtr_HIODevice);
            this.groupBox1.Location = new System.Drawing.Point(12, 27);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(781, 45);
            this.groupBox1.TabIndex = 2;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "デバイス";
            // 
            // uCtr_HIODevice
            // 
            this.uCtr_HIODevice.Dock = System.Windows.Forms.DockStyle.Fill;
            this.uCtr_HIODevice.Location = new System.Drawing.Point(3, 15);
            this.uCtr_HIODevice.LogForm = null;
            this.uCtr_HIODevice.Name = "uCtr_HIODevice";
            this.uCtr_HIODevice.Size = new System.Drawing.Size(775, 27);
            this.uCtr_HIODevice.TabIndex = 3;
            // 
            // tabControl1
            // 
            this.tabControl1.AllowDrop = true;
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tabPage_Model);
            this.tabControl1.Controls.Add(this.tabPage_Camera);
            this.tabControl1.Enabled = false;
            this.tabControl1.Location = new System.Drawing.Point(12, 75);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(781, 677);
            this.tabControl1.TabIndex = 3;
            // 
            // tabPage_Model
            // 
            this.tabPage_Model.AllowDrop = true;
            this.tabPage_Model.Controls.Add(this.treeView_Models);
            this.tabPage_Model.Location = new System.Drawing.Point(4, 22);
            this.tabPage_Model.Name = "tabPage_Model";
            this.tabPage_Model.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_Model.Size = new System.Drawing.Size(773, 651);
            this.tabPage_Model.TabIndex = 0;
            this.tabPage_Model.Text = "Model/Motion";
            this.tabPage_Model.UseVisualStyleBackColor = true;
            // 
            // treeView_Models
            // 
            this.treeView_Models.AllowDrop = true;
            this.treeView_Models.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.treeView_Models.ContextMenuStrip = this.contextMenu_ModelTree;
            this.treeView_Models.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeView_Models.HideSelection = false;
            this.treeView_Models.Location = new System.Drawing.Point(3, 3);
            this.treeView_Models.Name = "treeView_Models";
            this.treeView_Models.Size = new System.Drawing.Size(767, 645);
            this.treeView_Models.TabIndex = 4;
            this.treeView_Models.DragDrop += new System.Windows.Forms.DragEventHandler(this.treeView_Models_DragDrop);
            this.treeView_Models.DragEnter += new System.Windows.Forms.DragEventHandler(this.treeView_Models_DragEnter);
            this.treeView_Models.KeyDown += new System.Windows.Forms.KeyEventHandler(this.treeView_Models_KeyDown);
            // 
            // contextMenu_ModelTree
            // 
            this.contextMenu_ModelTree.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_SendAllModel,
            this.ToolStripMenuItem_DeleteAllTreeModel});
            this.contextMenu_ModelTree.Name = "contextMenu_ModelTree";
            this.contextMenu_ModelTree.Size = new System.Drawing.Size(137, 48);
            // 
            // ToolStripMenuItem_SendAllModel
            // 
            this.ToolStripMenuItem_SendAllModel.Name = "ToolStripMenuItem_SendAllModel";
            this.ToolStripMenuItem_SendAllModel.Size = new System.Drawing.Size(136, 22);
            this.ToolStripMenuItem_SendAllModel.Text = "全て再送信";
            this.ToolStripMenuItem_SendAllModel.Click += new System.EventHandler(this.ToolStripMenuItem_SendAllModel_Click);
            // 
            // ToolStripMenuItem_DeleteAllTreeModel
            // 
            this.ToolStripMenuItem_DeleteAllTreeModel.Name = "ToolStripMenuItem_DeleteAllTreeModel";
            this.ToolStripMenuItem_DeleteAllTreeModel.Size = new System.Drawing.Size(136, 22);
            this.ToolStripMenuItem_DeleteAllTreeModel.Text = "全て削除";
            this.ToolStripMenuItem_DeleteAllTreeModel.Click += new System.EventHandler(this.ToolStripMenuItem_DeleteAllTreeModel_Click);
            // 
            // tabPage_Camera
            // 
            this.tabPage_Camera.Controls.Add(this.groupBox3);
            this.tabPage_Camera.Location = new System.Drawing.Point(4, 22);
            this.tabPage_Camera.Name = "tabPage_Camera";
            this.tabPage_Camera.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_Camera.Size = new System.Drawing.Size(773, 651);
            this.tabPage_Camera.TabIndex = 1;
            this.tabPage_Camera.Text = "Camera";
            this.tabPage_Camera.UseVisualStyleBackColor = true;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.panel_MouseController);
            this.groupBox3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox3.Location = new System.Drawing.Point(3, 3);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(767, 645);
            this.groupBox3.TabIndex = 0;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "groupBox3";
            // 
            // panel_MouseController
            // 
            this.panel_MouseController.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel_MouseController.Location = new System.Drawing.Point(3, 15);
            this.panel_MouseController.Name = "panel_MouseController";
            this.panel_MouseController.Size = new System.Drawing.Size(761, 627);
            this.panel_MouseController.TabIndex = 0;
            this.panel_MouseController.MouseClick += new System.Windows.Forms.MouseEventHandler(this.panel_MouseController_OnMouseClick);
            this.panel_MouseController.MouseDown += new System.Windows.Forms.MouseEventHandler(this.panel_MouseController_MouseDown);
            this.panel_MouseController.MouseEnter += new System.EventHandler(this.panel_MouseController_MouseEnter);
            this.panel_MouseController.MouseLeave += new System.EventHandler(this.panel_MouseController_MouseLeave);
            this.panel_MouseController.MouseMove += new System.Windows.Forms.MouseEventHandler(this.panel_MouseController_MouseMove);
            this.panel_MouseController.MouseUp += new System.Windows.Forms.MouseEventHandler(this.panel_MouseController_MouseUp);
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_File,
            this.表示VToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(805, 26);
            this.menuStrip1.TabIndex = 4;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // ToolStripMenuItem_File
            // 
            this.ToolStripMenuItem_File.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_RecentFiles});
            this.ToolStripMenuItem_File.Name = "ToolStripMenuItem_File";
            this.ToolStripMenuItem_File.ShortcutKeyDisplayString = "";
            this.ToolStripMenuItem_File.Size = new System.Drawing.Size(85, 22);
            this.ToolStripMenuItem_File.Text = "ファイル(&F)";
            // 
            // ToolStripMenuItem_RecentFiles
            // 
            this.ToolStripMenuItem_RecentFiles.Name = "ToolStripMenuItem_RecentFiles";
            this.ToolStripMenuItem_RecentFiles.Size = new System.Drawing.Size(201, 22);
            this.ToolStripMenuItem_RecentFiles.Text = "最近使ったファイル(&F)";
            // 
            // 表示VToolStripMenuItem
            // 
            this.表示VToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_ViewLog});
            this.表示VToolStripMenuItem.Name = "表示VToolStripMenuItem";
            this.表示VToolStripMenuItem.Size = new System.Drawing.Size(62, 22);
            this.表示VToolStripMenuItem.Text = "表示(&V)";
            // 
            // ToolStripMenuItem_ViewLog
            // 
            this.ToolStripMenuItem_ViewLog.Name = "ToolStripMenuItem_ViewLog";
            this.ToolStripMenuItem_ViewLog.Size = new System.Drawing.Size(152, 22);
            this.ToolStripMenuItem_ViewLog.Text = "ログ(&L)";
            this.ToolStripMenuItem_ViewLog.Click += new System.EventHandler(this.ToolStripMenuItem_ViewLog_Click);
            // 
            // Form1
            // 
            this.AllowDrop = true;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(805, 777);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "GFHIOHost";
            this.groupBox1.ResumeLayout(false);
            this.tabControl1.ResumeLayout(false);
            this.tabPage_Model.ResumeLayout(false);
            this.contextMenu_ModelTree.ResumeLayout(false);
            this.tabPage_Camera.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage_Model;
        private System.Windows.Forms.TabPage tabPage_Camera;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Panel panel_MouseController;
        private System.Windows.Forms.ContextMenuStrip contextMenu_ModelTree;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_DeleteAllTreeModel;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_SendAllModel;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_File;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_RecentFiles;
        private System.Windows.Forms.TreeView treeView_Models;
        private GFHIOHostFormControl.Forms.UCtr_HIODevice uCtr_HIODevice;
        private System.Windows.Forms.ToolStripMenuItem 表示VToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_ViewLog;
    }
}

