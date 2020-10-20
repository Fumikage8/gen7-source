namespace PokeViewerHost
{
    partial class MainForm
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
            this.menuStrip_FormMenu = new System.Windows.Forms.MenuStrip();
            this.ファイルToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.読み込みOToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.保存ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.設定CToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_Edit = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_Copy = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_Paste = new System.Windows.Forms.ToolStripMenuItem();
            this.ツールTToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_Capture = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_Reload = new System.Windows.Forms.ToolStripMenuItem();
            this.表示VToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_TestForm = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_Log = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_GudeRect = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_GuideRect_Hide = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_GuideRect_Size1 = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_GuideRect_Size2 = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_GuideRect_Size3 = new System.Windows.Forms.ToolStripMenuItem();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.dataGridView_PokeList = new System.Windows.Forms.DataGridView();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.button_DeleteImage = new System.Windows.Forms.Button();
            this.button_Reload = new System.Windows.Forms.Button();
            this.button_Capture = new System.Windows.Forms.Button();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.splitContainer_Main = new System.Windows.Forms.SplitContainer();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.uCtr_HIODevice = new GFHIOHostFormControl.Forms.UCtr_HIODevice();
            this.colorDialog = new System.Windows.Forms.ColorDialog();
            this.uCtr_PokeDexCapture = new PokeViewerHost.Forms.UCtr_PokeDexCapture();
            this.ToolStripMenuItem_Delete = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStrip_FormMenu.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_PokeList)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer_Main)).BeginInit();
            this.splitContainer_Main.Panel1.SuspendLayout();
            this.splitContainer_Main.Panel2.SuspendLayout();
            this.splitContainer_Main.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip_FormMenu
            // 
            this.menuStrip_FormMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ファイルToolStripMenuItem,
            this.ToolStripMenuItem_Edit,
            this.ツールTToolStripMenuItem,
            this.表示VToolStripMenuItem});
            this.menuStrip_FormMenu.Location = new System.Drawing.Point(0, 0);
            this.menuStrip_FormMenu.Name = "menuStrip_FormMenu";
            this.menuStrip_FormMenu.Size = new System.Drawing.Size(1223, 26);
            this.menuStrip_FormMenu.TabIndex = 0;
            this.menuStrip_FormMenu.Text = "menuStrip1";
            // 
            // ファイルToolStripMenuItem
            // 
            this.ファイルToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.読み込みOToolStripMenuItem,
            this.保存ToolStripMenuItem,
            this.設定CToolStripMenuItem});
            this.ファイルToolStripMenuItem.Name = "ファイルToolStripMenuItem";
            this.ファイルToolStripMenuItem.Size = new System.Drawing.Size(85, 22);
            this.ファイルToolStripMenuItem.Text = "ファイル(&F)";
            // 
            // 読み込みOToolStripMenuItem
            // 
            this.読み込みOToolStripMenuItem.Name = "読み込みOToolStripMenuItem";
            this.読み込みOToolStripMenuItem.Size = new System.Drawing.Size(119, 22);
            this.読み込みOToolStripMenuItem.Text = "開く(&O)";
            this.読み込みOToolStripMenuItem.Click += new System.EventHandler(this.読み込みOToolStripMenuItem_Click);
            // 
            // 保存ToolStripMenuItem
            // 
            this.保存ToolStripMenuItem.Name = "保存ToolStripMenuItem";
            this.保存ToolStripMenuItem.Size = new System.Drawing.Size(119, 22);
            this.保存ToolStripMenuItem.Text = "保存(&S)";
            this.保存ToolStripMenuItem.Click += new System.EventHandler(this.保存ToolStripMenuItem_Click);
            // 
            // 設定CToolStripMenuItem
            // 
            this.設定CToolStripMenuItem.Name = "設定CToolStripMenuItem";
            this.設定CToolStripMenuItem.Size = new System.Drawing.Size(119, 22);
            this.設定CToolStripMenuItem.Text = "設定(&C)";
            this.設定CToolStripMenuItem.Click += new System.EventHandler(this.設定CToolStripMenuItem_Click);
            // 
            // ToolStripMenuItem_Edit
            // 
            this.ToolStripMenuItem_Edit.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_Copy,
            this.ToolStripMenuItem_Paste});
            this.ToolStripMenuItem_Edit.Name = "ToolStripMenuItem_Edit";
            this.ToolStripMenuItem_Edit.Size = new System.Drawing.Size(61, 22);
            this.ToolStripMenuItem_Edit.Text = "編集(&E)";
            // 
            // ToolStripMenuItem_Copy
            // 
            this.ToolStripMenuItem_Copy.Name = "ToolStripMenuItem_Copy";
            this.ToolStripMenuItem_Copy.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.C)));
            this.ToolStripMenuItem_Copy.Size = new System.Drawing.Size(188, 22);
            this.ToolStripMenuItem_Copy.Text = "コピー(&C)";
            this.ToolStripMenuItem_Copy.Click += new System.EventHandler(this.ToolStripMenuItem_Copy_Click);
            // 
            // ToolStripMenuItem_Paste
            // 
            this.ToolStripMenuItem_Paste.Name = "ToolStripMenuItem_Paste";
            this.ToolStripMenuItem_Paste.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.V)));
            this.ToolStripMenuItem_Paste.Size = new System.Drawing.Size(188, 22);
            this.ToolStripMenuItem_Paste.Text = "ペースト(&P)";
            this.ToolStripMenuItem_Paste.Click += new System.EventHandler(this.ToolStripMenuItem_Paste_Click);
            // 
            // ツールTToolStripMenuItem
            // 
            this.ツールTToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_Capture,
            this.ToolStripMenuItem_Reload,
            this.ToolStripMenuItem_Delete});
            this.ツールTToolStripMenuItem.Name = "ツールTToolStripMenuItem";
            this.ツールTToolStripMenuItem.Size = new System.Drawing.Size(74, 22);
            this.ツールTToolStripMenuItem.Text = "ツール(&T)";
            // 
            // ToolStripMenuItem_Capture
            // 
            this.ToolStripMenuItem_Capture.Name = "ToolStripMenuItem_Capture";
            this.ToolStripMenuItem_Capture.ShortcutKeys = System.Windows.Forms.Keys.F7;
            this.ToolStripMenuItem_Capture.Size = new System.Drawing.Size(236, 22);
            this.ToolStripMenuItem_Capture.Text = "撮影(&C)";
            this.ToolStripMenuItem_Capture.Click += new System.EventHandler(this.ToolStripMenuItem_Capture_Click_1);
            // 
            // ToolStripMenuItem_Reload
            // 
            this.ToolStripMenuItem_Reload.Name = "ToolStripMenuItem_Reload";
            this.ToolStripMenuItem_Reload.ShortcutKeys = System.Windows.Forms.Keys.F5;
            this.ToolStripMenuItem_Reload.Size = new System.Drawing.Size(236, 22);
            this.ToolStripMenuItem_Reload.Text = "撮影画像の再読み込み(&R)";
            this.ToolStripMenuItem_Reload.Click += new System.EventHandler(this.ToolStripMenuItem_Reload_Click);
            // 
            // 表示VToolStripMenuItem
            // 
            this.表示VToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_TestForm,
            this.ToolStripMenuItem_Log,
            this.ToolStripMenuItem_GudeRect});
            this.表示VToolStripMenuItem.Name = "表示VToolStripMenuItem";
            this.表示VToolStripMenuItem.Size = new System.Drawing.Size(66, 22);
            this.表示VToolStripMenuItem.Text = "表示(&V )";
            // 
            // ToolStripMenuItem_TestForm
            // 
            this.ToolStripMenuItem_TestForm.Name = "ToolStripMenuItem_TestForm";
            this.ToolStripMenuItem_TestForm.Size = new System.Drawing.Size(155, 22);
            this.ToolStripMenuItem_TestForm.Text = "撮影設定(&E)";
            this.ToolStripMenuItem_TestForm.Click += new System.EventHandler(this.ToolStripMenuItem_TestForm_Click);
            // 
            // ToolStripMenuItem_Log
            // 
            this.ToolStripMenuItem_Log.Name = "ToolStripMenuItem_Log";
            this.ToolStripMenuItem_Log.Size = new System.Drawing.Size(155, 22);
            this.ToolStripMenuItem_Log.Text = "ログ(&L)";
            this.ToolStripMenuItem_Log.Click += new System.EventHandler(this.ToolStripMenuItem_Log_Click);
            // 
            // ToolStripMenuItem_GudeRect
            // 
            this.ToolStripMenuItem_GudeRect.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_GuideRect_Hide,
            this.ToolStripMenuItem_GuideRect_Size1,
            this.ToolStripMenuItem_GuideRect_Size2,
            this.ToolStripMenuItem_GuideRect_Size3});
            this.ToolStripMenuItem_GudeRect.Name = "ToolStripMenuItem_GudeRect";
            this.ToolStripMenuItem_GudeRect.Size = new System.Drawing.Size(155, 22);
            this.ToolStripMenuItem_GudeRect.Text = "ガイド矩形(&G)";
            // 
            // ToolStripMenuItem_GuideRect_Hide
            // 
            this.ToolStripMenuItem_GuideRect_Hide.Name = "ToolStripMenuItem_GuideRect_Hide";
            this.ToolStripMenuItem_GuideRect_Hide.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.NumPad0)));
            this.ToolStripMenuItem_GuideRect_Hide.Size = new System.Drawing.Size(224, 22);
            this.ToolStripMenuItem_GuideRect_Hide.Text = "非表示(&0)";
            this.ToolStripMenuItem_GuideRect_Hide.Click += new System.EventHandler(this.ToolStripMenuItem_GuideRect_Hide_Click);
            // 
            // ToolStripMenuItem_GuideRect_Size1
            // 
            this.ToolStripMenuItem_GuideRect_Size1.Name = "ToolStripMenuItem_GuideRect_Size1";
            this.ToolStripMenuItem_GuideRect_Size1.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.NumPad1)));
            this.ToolStripMenuItem_GuideRect_Size1.Size = new System.Drawing.Size(224, 22);
            this.ToolStripMenuItem_GuideRect_Size1.Text = "設定1(&1)";
            this.ToolStripMenuItem_GuideRect_Size1.Click += new System.EventHandler(this.ToolStripMenuItem_GuideRect_Size1_Click);
            // 
            // ToolStripMenuItem_GuideRect_Size2
            // 
            this.ToolStripMenuItem_GuideRect_Size2.Name = "ToolStripMenuItem_GuideRect_Size2";
            this.ToolStripMenuItem_GuideRect_Size2.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.NumPad2)));
            this.ToolStripMenuItem_GuideRect_Size2.Size = new System.Drawing.Size(224, 22);
            this.ToolStripMenuItem_GuideRect_Size2.Text = "設定2(&2)";
            this.ToolStripMenuItem_GuideRect_Size2.Click += new System.EventHandler(this.ToolStripMenuItem_GuideRect_Size2_Click);
            // 
            // ToolStripMenuItem_GuideRect_Size3
            // 
            this.ToolStripMenuItem_GuideRect_Size3.Name = "ToolStripMenuItem_GuideRect_Size3";
            this.ToolStripMenuItem_GuideRect_Size3.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.NumPad3)));
            this.ToolStripMenuItem_GuideRect_Size3.Size = new System.Drawing.Size(224, 22);
            this.ToolStripMenuItem_GuideRect_Size3.Text = "設定3(&3)";
            this.ToolStripMenuItem_GuideRect_Size3.Click += new System.EventHandler(this.ToolStripMenuItem_GuideRect_Size3_Click);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Location = new System.Drawing.Point(0, 779);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(1223, 22);
            this.statusStrip1.TabIndex = 1;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // dataGridView_PokeList
            // 
            this.dataGridView_PokeList.AllowUserToAddRows = false;
            this.dataGridView_PokeList.AllowUserToDeleteRows = false;
            this.dataGridView_PokeList.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.AllCellsExceptHeader;
            this.dataGridView_PokeList.AutoSizeRowsMode = System.Windows.Forms.DataGridViewAutoSizeRowsMode.AllCells;
            this.dataGridView_PokeList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView_PokeList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dataGridView_PokeList.Location = new System.Drawing.Point(0, 0);
            this.dataGridView_PokeList.Name = "dataGridView_PokeList";
            this.dataGridView_PokeList.ReadOnly = true;
            this.dataGridView_PokeList.RowTemplate.Height = 21;
            this.dataGridView_PokeList.Size = new System.Drawing.Size(323, 691);
            this.dataGridView_PokeList.TabIndex = 0;
            this.dataGridView_PokeList.CellFormatting += new System.Windows.Forms.DataGridViewCellFormattingEventHandler(this.dataGridView_PokeList_CellFormatting);
            this.dataGridView_PokeList.CurrentCellChanged += new System.EventHandler(this.dataGridView_PokeList_CurrentCellChanged);
            this.dataGridView_PokeList.DoubleClick += new System.EventHandler(this.dataGridView_PokeList_DoubleClick);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(864, 691);
            this.tabControl1.TabIndex = 1;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.button_DeleteImage);
            this.tabPage1.Controls.Add(this.button_Reload);
            this.tabPage1.Controls.Add(this.button_Capture);
            this.tabPage1.Controls.Add(this.uCtr_PokeDexCapture);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(856, 665);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "図鑑";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // button_DeleteImage
            // 
            this.button_DeleteImage.Location = new System.Drawing.Point(765, 607);
            this.button_DeleteImage.Name = "button_DeleteImage";
            this.button_DeleteImage.Size = new System.Drawing.Size(75, 23);
            this.button_DeleteImage.TabIndex = 4;
            this.button_DeleteImage.Text = "Delete";
            this.button_DeleteImage.UseVisualStyleBackColor = true;
            this.button_DeleteImage.Click += new System.EventHandler(this.button_DeleteImage_Click);
            // 
            // button_Reload
            // 
            this.button_Reload.Location = new System.Drawing.Point(87, 607);
            this.button_Reload.Name = "button_Reload";
            this.button_Reload.Size = new System.Drawing.Size(75, 23);
            this.button_Reload.TabIndex = 3;
            this.button_Reload.Text = "Reload";
            this.button_Reload.UseVisualStyleBackColor = true;
            this.button_Reload.Click += new System.EventHandler(this.button_Reload_Click);
            // 
            // button_Capture
            // 
            this.button_Capture.Location = new System.Drawing.Point(6, 607);
            this.button_Capture.Name = "button_Capture";
            this.button_Capture.Size = new System.Drawing.Size(75, 23);
            this.button_Capture.TabIndex = 2;
            this.button_Capture.Text = "Capture";
            this.button_Capture.UseVisualStyleBackColor = true;
            this.button_Capture.Click += new System.EventHandler(this.button_Capture_Click);
            // 
            // tabPage2
            // 
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(856, 665);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "tabPage2";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // splitContainer_Main
            // 
            this.splitContainer_Main.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer_Main.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.splitContainer_Main.Location = new System.Drawing.Point(12, 81);
            this.splitContainer_Main.Name = "splitContainer_Main";
            // 
            // splitContainer_Main.Panel1
            // 
            this.splitContainer_Main.Panel1.Controls.Add(this.dataGridView_PokeList);
            // 
            // splitContainer_Main.Panel2
            // 
            this.splitContainer_Main.Panel2.Controls.Add(this.tabControl1);
            this.splitContainer_Main.Size = new System.Drawing.Size(1199, 695);
            this.splitContainer_Main.SplitterDistance = 327;
            this.splitContainer_Main.TabIndex = 3;
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.uCtr_HIODevice);
            this.groupBox1.Location = new System.Drawing.Point(12, 29);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(1199, 46);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "device";
            // 
            // uCtr_HIODevice
            // 
            this.uCtr_HIODevice.Dock = System.Windows.Forms.DockStyle.Fill;
            this.uCtr_HIODevice.Location = new System.Drawing.Point(3, 15);
            this.uCtr_HIODevice.LogForm = null;
            this.uCtr_HIODevice.Name = "uCtr_HIODevice";
            this.uCtr_HIODevice.Size = new System.Drawing.Size(1193, 28);
            this.uCtr_HIODevice.TabIndex = 0;
            // 
            // uCtr_PokeDexCapture
            // 
            this.uCtr_PokeDexCapture.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.uCtr_PokeDexCapture.AutoScroll = true;
            this.uCtr_PokeDexCapture.Caption_Composite = "label1";
            this.uCtr_PokeDexCapture.Caption_InvAlpha = "label1";
            this.uCtr_PokeDexCapture.Caption_SourceAlpha = "label1";
            this.uCtr_PokeDexCapture.Caption_SourceColor = "label1";
            this.uCtr_PokeDexCapture.Image_Composite = null;
            this.uCtr_PokeDexCapture.Image_InvAlpha = null;
            this.uCtr_PokeDexCapture.Image_SourceAlpha = null;
            this.uCtr_PokeDexCapture.Image_SourceColor = null;
            this.uCtr_PokeDexCapture.Location = new System.Drawing.Point(6, 6);
            this.uCtr_PokeDexCapture.Name = "uCtr_PokeDexCapture";
            this.uCtr_PokeDexCapture.OnCompositeBackgroundChanged = null;
            this.uCtr_PokeDexCapture.Size = new System.Drawing.Size(847, 659);
            this.uCtr_PokeDexCapture.TabIndex = 0;
            // 
            // ToolStripMenuItem_Delete
            // 
            this.ToolStripMenuItem_Delete.Name = "ToolStripMenuItem_Delete";
            this.ToolStripMenuItem_Delete.ShortcutKeys = System.Windows.Forms.Keys.Delete;
            this.ToolStripMenuItem_Delete.Size = new System.Drawing.Size(236, 22);
            this.ToolStripMenuItem_Delete.Text = "撮影画像の削除(&D)";
            this.ToolStripMenuItem_Delete.Click += new System.EventHandler(this.ToolStripMenuItem_Delete_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1223, 801);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.menuStrip_FormMenu);
            this.Controls.Add(this.splitContainer_Main);
            this.MainMenuStrip = this.menuStrip_FormMenu;
            this.Name = "MainForm";
            this.Text = "PokeViewerHost";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.menuStrip_FormMenu.ResumeLayout(false);
            this.menuStrip_FormMenu.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_PokeList)).EndInit();
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.splitContainer_Main.Panel1.ResumeLayout(false);
            this.splitContainer_Main.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer_Main)).EndInit();
            this.splitContainer_Main.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip_FormMenu;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.DataGridView dataGridView_PokeList;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.ToolStripMenuItem ファイルToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 読み込みOToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 保存ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 設定CToolStripMenuItem;
        private System.Windows.Forms.SplitContainer splitContainer_Main;
        private Forms.UCtr_PokeDexCapture uCtr_PokeDexCapture;
        private System.Windows.Forms.ToolStripMenuItem 表示VToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_Log;
        private System.Windows.Forms.GroupBox groupBox1;
        private GFHIOHostFormControl.Forms.UCtr_HIODevice uCtr_HIODevice;
        private System.Windows.Forms.ColorDialog colorDialog;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_TestForm;
        private System.Windows.Forms.Button button_Reload;
        private System.Windows.Forms.Button button_Capture;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_Edit;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_Copy;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_Paste;
        private System.Windows.Forms.ToolStripMenuItem ツールTToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_Capture;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_Reload;
        private System.Windows.Forms.Button button_DeleteImage;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_GudeRect;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_GuideRect_Hide;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_GuideRect_Size1;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_GuideRect_Size2;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_GuideRect_Size3;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_Delete;
    }
}

