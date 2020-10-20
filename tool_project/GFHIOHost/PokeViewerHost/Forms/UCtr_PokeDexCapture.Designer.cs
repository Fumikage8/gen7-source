namespace PokeViewerHost.Forms
{
    partial class UCtr_PokeDexCapture
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

        #region コンポーネント デザイナーで生成されたコード

        /// <summary> 
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を 
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.uCtr_PokeDexCapture_Image_SourceColor = new PokeViewerHost.Forms.UCtr_PokeDexCapture_Image();
            this.uCtr_PokeDexCapture_Image_SourceAlpha = new PokeViewerHost.Forms.UCtr_PokeDexCapture_Image();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.uCtr_PokeDexCapture_Image_InvAlpha = new PokeViewerHost.Forms.UCtr_PokeDexCapture_Image();
            this.uCtr_PokeDexCapture_Image_Composite = new PokeViewerHost.Forms.UCtr_PokeDexCapture_Image();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.contextMenuStrip_ForCompositePane = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.ToolStripMenuItem_LoadBackgroundImage = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_ClearBackgroundImage = new System.Windows.Forms.ToolStripMenuItem();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.contextMenuStrip_ForCompositePane.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.uCtr_PokeDexCapture_Image_SourceColor);
            this.groupBox1.Location = new System.Drawing.Point(3, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(413, 292);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "a)SourceColor";
            // 
            // uCtr_PokeDexCapture_Image_SourceColor
            // 
            this.uCtr_PokeDexCapture_Image_SourceColor.Caption = "label1";
            this.uCtr_PokeDexCapture_Image_SourceColor.Dock = System.Windows.Forms.DockStyle.Fill;
            this.uCtr_PokeDexCapture_Image_SourceColor.Image = null;
            this.uCtr_PokeDexCapture_Image_SourceColor.Location = new System.Drawing.Point(3, 15);
            this.uCtr_PokeDexCapture_Image_SourceColor.Name = "uCtr_PokeDexCapture_Image_SourceColor";
            this.uCtr_PokeDexCapture_Image_SourceColor.Size = new System.Drawing.Size(407, 274);
            this.uCtr_PokeDexCapture_Image_SourceColor.TabIndex = 0;
            // 
            // uCtr_PokeDexCapture_Image_SourceAlpha
            // 
            this.uCtr_PokeDexCapture_Image_SourceAlpha.Caption = "label1";
            this.uCtr_PokeDexCapture_Image_SourceAlpha.Dock = System.Windows.Forms.DockStyle.Fill;
            this.uCtr_PokeDexCapture_Image_SourceAlpha.Image = null;
            this.uCtr_PokeDexCapture_Image_SourceAlpha.Location = new System.Drawing.Point(3, 15);
            this.uCtr_PokeDexCapture_Image_SourceAlpha.Name = "uCtr_PokeDexCapture_Image_SourceAlpha";
            this.uCtr_PokeDexCapture_Image_SourceAlpha.Size = new System.Drawing.Size(407, 274);
            this.uCtr_PokeDexCapture_Image_SourceAlpha.TabIndex = 0;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.uCtr_PokeDexCapture_Image_SourceAlpha);
            this.groupBox2.Location = new System.Drawing.Point(422, 3);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(413, 292);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "b)SourceAlpha";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.uCtr_PokeDexCapture_Image_InvAlpha);
            this.groupBox3.Location = new System.Drawing.Point(3, 301);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(413, 292);
            this.groupBox3.TabIndex = 2;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "c)InvAlpha(-b)";
            // 
            // uCtr_PokeDexCapture_Image_InvAlpha
            // 
            this.uCtr_PokeDexCapture_Image_InvAlpha.Caption = "label1";
            this.uCtr_PokeDexCapture_Image_InvAlpha.Dock = System.Windows.Forms.DockStyle.Fill;
            this.uCtr_PokeDexCapture_Image_InvAlpha.Image = null;
            this.uCtr_PokeDexCapture_Image_InvAlpha.Location = new System.Drawing.Point(3, 15);
            this.uCtr_PokeDexCapture_Image_InvAlpha.Name = "uCtr_PokeDexCapture_Image_InvAlpha";
            this.uCtr_PokeDexCapture_Image_InvAlpha.Size = new System.Drawing.Size(407, 274);
            this.uCtr_PokeDexCapture_Image_InvAlpha.TabIndex = 0;
            // 
            // uCtr_PokeDexCapture_Image_Composite
            // 
            this.uCtr_PokeDexCapture_Image_Composite.Caption = "label1";
            this.uCtr_PokeDexCapture_Image_Composite.ContextMenuStrip = this.contextMenuStrip_ForCompositePane;
            this.uCtr_PokeDexCapture_Image_Composite.Dock = System.Windows.Forms.DockStyle.Fill;
            this.uCtr_PokeDexCapture_Image_Composite.Image = null;
            this.uCtr_PokeDexCapture_Image_Composite.Location = new System.Drawing.Point(3, 15);
            this.uCtr_PokeDexCapture_Image_Composite.Name = "uCtr_PokeDexCapture_Image_Composite";
            this.uCtr_PokeDexCapture_Image_Composite.Size = new System.Drawing.Size(407, 274);
            this.uCtr_PokeDexCapture_Image_Composite.TabIndex = 0;
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.uCtr_PokeDexCapture_Image_Composite);
            this.groupBox4.Location = new System.Drawing.Point(422, 301);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(413, 292);
            this.groupBox4.TabIndex = 3;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "d)Composite(a+c)";
            // 
            // contextMenuStrip_ForCompositePane
            // 
            this.contextMenuStrip_ForCompositePane.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_LoadBackgroundImage,
            this.ToolStripMenuItem_ClearBackgroundImage});
            this.contextMenuStrip_ForCompositePane.Name = "contextMenuStrip_ForCompositePane";
            this.contextMenuStrip_ForCompositePane.Size = new System.Drawing.Size(192, 70);
            // 
            // ToolStripMenuItem_LoadBackgroundImage
            // 
            this.ToolStripMenuItem_LoadBackgroundImage.Name = "ToolStripMenuItem_LoadBackgroundImage";
            this.ToolStripMenuItem_LoadBackgroundImage.Size = new System.Drawing.Size(191, 22);
            this.ToolStripMenuItem_LoadBackgroundImage.Text = "背景画像の変更(&B)...";
            this.ToolStripMenuItem_LoadBackgroundImage.Click += new System.EventHandler(this.ToolStripMenuItem_LoadBackgroundImage_Click);
            // 
            // ToolStripMenuItem_ClearBackgroundImage
            // 
            this.ToolStripMenuItem_ClearBackgroundImage.Name = "ToolStripMenuItem_ClearBackgroundImage";
            this.ToolStripMenuItem_ClearBackgroundImage.Size = new System.Drawing.Size(191, 22);
            this.ToolStripMenuItem_ClearBackgroundImage.Text = "背景画像のクリア(&D)";
            this.ToolStripMenuItem_ClearBackgroundImage.Click += new System.EventHandler(this.ToolStripMenuItem_ClearBackgroundImage_Click);
            // 
            // UCtr_PokeDexCapture
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox1);
            this.Name = "UCtr_PokeDexCapture";
            this.Size = new System.Drawing.Size(857, 856);
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.contextMenuStrip_ForCompositePane.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.GroupBox groupBox4;
        public UCtr_PokeDexCapture_Image uCtr_PokeDexCapture_Image_SourceColor;
        public UCtr_PokeDexCapture_Image uCtr_PokeDexCapture_Image_Composite;
        public UCtr_PokeDexCapture_Image uCtr_PokeDexCapture_Image_SourceAlpha;
        public UCtr_PokeDexCapture_Image uCtr_PokeDexCapture_Image_InvAlpha;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip_ForCompositePane;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_LoadBackgroundImage;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_ClearBackgroundImage;
    }
}
