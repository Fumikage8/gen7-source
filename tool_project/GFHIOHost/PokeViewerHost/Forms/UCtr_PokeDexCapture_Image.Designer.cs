namespace PokeViewerHost.Forms
{
    partial class UCtr_PokeDexCapture_Image
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
            this.pictureBox_Image = new System.Windows.Forms.PictureBox();
            this.toolTip_Label = new System.Windows.Forms.ToolTip(this.components);
            this.textBox_Caption = new System.Windows.Forms.TextBox();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_Image)).BeginInit();
            this.SuspendLayout();
            // 
            // pictureBox_Image
            // 
            this.pictureBox_Image.BackgroundImage = global::PokeViewerHost.Properties.Resources.Background;
            this.pictureBox_Image.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictureBox_Image.Location = new System.Drawing.Point(3, 3);
            this.pictureBox_Image.Name = "pictureBox_Image";
            this.pictureBox_Image.Size = new System.Drawing.Size(400, 240);
            this.pictureBox_Image.TabIndex = 0;
            this.pictureBox_Image.TabStop = false;
            // 
            // toolTip_Label
            // 
            this.toolTip_Label.AutoPopDelay = 10000;
            this.toolTip_Label.InitialDelay = 500;
            this.toolTip_Label.ReshowDelay = 100;
            // 
            // textBox_Caption
            // 
            this.textBox_Caption.Location = new System.Drawing.Point(3, 249);
            this.textBox_Caption.Name = "textBox_Caption";
            this.textBox_Caption.ReadOnly = true;
            this.textBox_Caption.Size = new System.Drawing.Size(400, 19);
            this.textBox_Caption.TabIndex = 2;
            // 
            // UCtr_PokeDexCapture_Image
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.textBox_Caption);
            this.Controls.Add(this.pictureBox_Image);
            this.Name = "UCtr_PokeDexCapture_Image";
            this.Size = new System.Drawing.Size(636, 531);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox_Image)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.PictureBox pictureBox_Image;
        private System.Windows.Forms.ToolTip toolTip_Label;
        private System.Windows.Forms.TextBox textBox_Caption;
    }
}
