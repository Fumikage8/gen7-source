namespace ModelViewer
{
	partial class RenderForm
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
      this.eyePositionLabel = new System.Windows.Forms.Label();
      this.focusPositionLabel = new System.Windows.Forms.Label();
      this.upDirectionLabel = new System.Windows.Forms.Label();
      this.performanceTimeLabel = new System.Windows.Forms.Label();
      this.battleBinarySizeLabel = new System.Windows.Forms.Label();
      this.fpsLabel = new System.Windows.Forms.Label();
      this.walkSpeedLabel = new System.Windows.Forms.Label();
      this.walkSpeedLabel2 = new System.Windows.Forms.Label();
      this.kwBinarySizeLabel = new System.Windows.Forms.Label();
      this.fieldBinarySizeLabel = new System.Windows.Forms.Label();
      this.allBinarySizeLabel = new System.Windows.Forms.Label();
      this.polygonCountLabel = new System.Windows.Forms.Label();
      this.materialCountLabel = new System.Windows.Forms.Label();
      this.jointCountLabel = new System.Windows.Forms.Label();
      this.SuspendLayout();
      // 
      // eyePositionLabel
      // 
      this.eyePositionLabel.AutoSize = true;
      this.eyePositionLabel.Location = new System.Drawing.Point(18, 48);
      this.eyePositionLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.eyePositionLabel.Name = "eyePositionLabel";
      this.eyePositionLabel.Size = new System.Drawing.Size(53, 12);
      this.eyePositionLabel.TabIndex = 0;
      this.eyePositionLabel.Text = "視点座標";
      // 
      // focusPositionLabel
      // 
      this.focusPositionLabel.AutoSize = true;
      this.focusPositionLabel.Location = new System.Drawing.Point(18, 67);
      this.focusPositionLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.focusPositionLabel.Name = "focusPositionLabel";
      this.focusPositionLabel.Size = new System.Drawing.Size(65, 12);
      this.focusPositionLabel.TabIndex = 1;
      this.focusPositionLabel.Text = "注視点座標";
      // 
      // upDirectionLabel
      // 
      this.upDirectionLabel.AutoSize = true;
      this.upDirectionLabel.Location = new System.Drawing.Point(18, 85);
      this.upDirectionLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.upDirectionLabel.Name = "upDirectionLabel";
      this.upDirectionLabel.Size = new System.Drawing.Size(125, 12);
      this.upDirectionLabel.TabIndex = 2;
      this.upDirectionLabel.Text = "視点 - 注視点間の距離";
      // 
      // performanceTimeLabel
      // 
      this.performanceTimeLabel.AutoSize = true;
      this.performanceTimeLabel.Location = new System.Drawing.Point(18, 29);
      this.performanceTimeLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.performanceTimeLabel.Name = "performanceTimeLabel";
      this.performanceTimeLabel.Size = new System.Drawing.Size(113, 12);
      this.performanceTimeLabel.TabIndex = 4;
      this.performanceTimeLabel.Text = "CPU負荷 / GPU負荷";
      // 
      // battleBinarySizeLabel
      // 
      this.battleBinarySizeLabel.AutoSize = true;
      this.battleBinarySizeLabel.Location = new System.Drawing.Point(18, 123);
      this.battleBinarySizeLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.battleBinarySizeLabel.Name = "battleBinarySizeLabel";
      this.battleBinarySizeLabel.Size = new System.Drawing.Size(148, 12);
      this.battleBinarySizeLabel.TabIndex = 5;
      this.battleBinarySizeLabel.Text = "バイナリサイズ(バトル)[Byte] : ";
      // 
      // fpsLabel
      // 
      this.fpsLabel.AutoSize = true;
      this.fpsLabel.Location = new System.Drawing.Point(18, 179);
      this.fpsLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.fpsLabel.Name = "fpsLabel";
      this.fpsLabel.Size = new System.Drawing.Size(36, 12);
      this.fpsLabel.TabIndex = 6;
      this.fpsLabel.Text = "FPS : ";
      // 
      // walkSpeedLabel
      // 
      this.walkSpeedLabel.AutoSize = true;
      this.walkSpeedLabel.Location = new System.Drawing.Point(18, 197);
      this.walkSpeedLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.walkSpeedLabel.Name = "walkSpeedLabel";
      this.walkSpeedLabel.Size = new System.Drawing.Size(50, 12);
      this.walkSpeedLabel.TabIndex = 7;
      this.walkSpeedLabel.Text = "歩き速度";
      // 
      // walkSpeedLabel2
      // 
      this.walkSpeedLabel2.AutoSize = true;
      this.walkSpeedLabel2.Location = new System.Drawing.Point(18, 216);
      this.walkSpeedLabel2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.walkSpeedLabel2.Name = "walkSpeedLabel2";
      this.walkSpeedLabel2.Size = new System.Drawing.Size(56, 12);
      this.walkSpeedLabel2.TabIndex = 8;
      this.walkSpeedLabel2.Text = "歩き速度2";
      // 
      // kwBinarySizeLabel
      // 
      this.kwBinarySizeLabel.AutoSize = true;
      this.kwBinarySizeLabel.Location = new System.Drawing.Point(18, 141);
      this.kwBinarySizeLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.kwBinarySizeLabel.Name = "kwBinarySizeLabel";
      this.kwBinarySizeLabel.Size = new System.Drawing.Size(168, 12);
      this.kwBinarySizeLabel.TabIndex = 9;
      this.kwBinarySizeLabel.Text = "バイナリサイズ(かわいがり)[Byte] : ";
      // 
      // fieldBinarySizeLabel
      // 
      this.fieldBinarySizeLabel.AutoSize = true;
      this.fieldBinarySizeLabel.Location = new System.Drawing.Point(18, 160);
      this.fieldBinarySizeLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.fieldBinarySizeLabel.Name = "fieldBinarySizeLabel";
      this.fieldBinarySizeLabel.Size = new System.Drawing.Size(164, 12);
      this.fieldBinarySizeLabel.TabIndex = 10;
      this.fieldBinarySizeLabel.Text = "バイナリサイズ(フィールド)[Byte] : ";
      // 
      // allBinarySizeLabel
      // 
      this.allBinarySizeLabel.AutoSize = true;
      this.allBinarySizeLabel.Location = new System.Drawing.Point(18, 104);
      this.allBinarySizeLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.allBinarySizeLabel.Name = "allBinarySizeLabel";
      this.allBinarySizeLabel.Size = new System.Drawing.Size(144, 12);
      this.allBinarySizeLabel.TabIndex = 11;
      this.allBinarySizeLabel.Text = "バイナリサイズ(全体)[Byte] : ";
      // 
      // polygonCountLabel
      // 
      this.polygonCountLabel.AutoSize = true;
      this.polygonCountLabel.Location = new System.Drawing.Point(18, 237);
      this.polygonCountLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.polygonCountLabel.Name = "polygonCountLabel";
      this.polygonCountLabel.Size = new System.Drawing.Size(58, 12);
      this.polygonCountLabel.TabIndex = 12;
      this.polygonCountLabel.Text = "ポリゴン数 :";
      this.polygonCountLabel.Visible = false;
      // 
      // materialCountLabel
      // 
      this.materialCountLabel.AutoSize = true;
      this.materialCountLabel.Location = new System.Drawing.Point(18, 258);
      this.materialCountLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.materialCountLabel.Name = "materialCountLabel";
      this.materialCountLabel.Size = new System.Drawing.Size(67, 12);
      this.materialCountLabel.TabIndex = 13;
      this.materialCountLabel.Text = "マテリアル数 :";
      this.materialCountLabel.Visible = false;
      // 
      // jointCountLabel
      // 
      this.jointCountLabel.AutoSize = true;
      this.jointCountLabel.Location = new System.Drawing.Point(18, 279);
      this.jointCountLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
      this.jointCountLabel.Name = "jointCountLabel";
      this.jointCountLabel.Size = new System.Drawing.Size(65, 12);
      this.jointCountLabel.TabIndex = 14;
      this.jointCountLabel.Text = "ジョイント数 :";
      this.jointCountLabel.Visible = false;
      // 
      // RenderForm
      // 
      this.AllowDrop = true;
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
      this.ClientSize = new System.Drawing.Size(600, 320);
      this.ControlBox = false;
      this.Controls.Add(this.jointCountLabel);
      this.Controls.Add(this.materialCountLabel);
      this.Controls.Add(this.polygonCountLabel);
      this.Controls.Add(this.allBinarySizeLabel);
      this.Controls.Add(this.fieldBinarySizeLabel);
      this.Controls.Add(this.kwBinarySizeLabel);
      this.Controls.Add(this.walkSpeedLabel2);
      this.Controls.Add(this.walkSpeedLabel);
      this.Controls.Add(this.fpsLabel);
      this.Controls.Add(this.battleBinarySizeLabel);
      this.Controls.Add(this.performanceTimeLabel);
      this.Controls.Add(this.upDirectionLabel);
      this.Controls.Add(this.focusPositionLabel);
      this.Controls.Add(this.eyePositionLabel);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
      this.Margin = new System.Windows.Forms.Padding(4);
      this.Name = "RenderForm";
      this.Text = "ビュー";
      this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.ViewForm_FormClosed);
      this.Load += new System.EventHandler(this.ViewForm_Load);
      this.DragDrop += new System.Windows.Forms.DragEventHandler(this.ViewForm_DragDrop);
      this.DragEnter += new System.Windows.Forms.DragEventHandler(this.ViewForm_DragEnter);
      this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.ViewForm_MouseDown);
      this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.ViewForm_MouseMove);
      this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.ViewForm_MouseUp);
      this.MouseWheel += new System.Windows.Forms.MouseEventHandler(this.ViewForm_MouseWheel);
      this.ResumeLayout(false);
      this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label eyePositionLabel;
		private System.Windows.Forms.Label focusPositionLabel;
		private System.Windows.Forms.Label upDirectionLabel;
		private System.Windows.Forms.Label performanceTimeLabel;
		private System.Windows.Forms.Label battleBinarySizeLabel;
    private System.Windows.Forms.Label fpsLabel;
    private System.Windows.Forms.Label walkSpeedLabel;
    private System.Windows.Forms.Label walkSpeedLabel2;
		private System.Windows.Forms.Label kwBinarySizeLabel;
		private System.Windows.Forms.Label fieldBinarySizeLabel;
		private System.Windows.Forms.Label allBinarySizeLabel;
    private System.Windows.Forms.Label polygonCountLabel;
    private System.Windows.Forms.Label materialCountLabel;
    private System.Windows.Forms.Label jointCountLabel;
	}
}

