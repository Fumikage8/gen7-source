namespace ModelViewer
{
	partial class LightControlForm
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.rotateYLabel = new System.Windows.Forms.Label();
			this.rotateXLabel = new System.Windows.Forms.Label();
			this.rotateYTextBox = new System.Windows.Forms.TextBox();
			this.rotateXTextBox = new System.Windows.Forms.TextBox();
			this.rotateMinusXButton = new System.Windows.Forms.Button();
			this.rotateMinusYButton = new System.Windows.Forms.Button();
			this.rotatePlusXButton = new System.Windows.Forms.Button();
			this.rotatePlusYButton = new System.Windows.Forms.Button();
			this.rotateXTrackBar = new System.Windows.Forms.TrackBar();
			this.rotateYTrackBar = new System.Windows.Forms.TrackBar();
			this.leftTopButton = new System.Windows.Forms.Button();
			this.topButton = new System.Windows.Forms.Button();
			this.rightTopButton = new System.Windows.Forms.Button();
			this.rightButton = new System.Windows.Forms.Button();
			this.centerButton = new System.Windows.Forms.Button();
			this.leftButton = new System.Windows.Forms.Button();
			this.rightBottomButton = new System.Windows.Forms.Button();
			this.bottomButton = new System.Windows.Forms.Button();
			this.leftBottomButton = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.rotateXTrackBar)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.rotateYTrackBar)).BeginInit();
			this.SuspendLayout();
			// 
			// rotateYLabel
			// 
			this.rotateYLabel.AutoSize = true;
			this.rotateYLabel.Location = new System.Drawing.Point(96, 25);
			this.rotateYLabel.Name = "rotateYLabel";
			this.rotateYLabel.Size = new System.Drawing.Size(41, 12);
			this.rotateYLabel.TabIndex = 18;
			this.rotateYLabel.Text = "縦回転";
			// 
			// rotateXLabel
			// 
			this.rotateXLabel.AutoSize = true;
			this.rotateXLabel.Location = new System.Drawing.Point(96, 56);
			this.rotateXLabel.Name = "rotateXLabel";
			this.rotateXLabel.Size = new System.Drawing.Size(41, 12);
			this.rotateXLabel.TabIndex = 17;
			this.rotateXLabel.Text = "横回転";
			// 
			// rotateYTextBox
			// 
			this.rotateYTextBox.Enabled = false;
			this.rotateYTextBox.Location = new System.Drawing.Point(176, 22);
			this.rotateYTextBox.Name = "rotateYTextBox";
			this.rotateYTextBox.Size = new System.Drawing.Size(39, 19);
			this.rotateYTextBox.TabIndex = 16;
			this.rotateYTextBox.TextChanged += new System.EventHandler(this.rotateYTextBox_TextChanged);
			this.rotateYTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.rotateYTextBox_KeyDown);
			this.rotateYTextBox.Validating += new System.ComponentModel.CancelEventHandler(this.rotateYTextBox_Validating);
			// 
			// rotateXTextBox
			// 
			this.rotateXTextBox.Enabled = false;
			this.rotateXTextBox.Location = new System.Drawing.Point(176, 53);
			this.rotateXTextBox.Name = "rotateXTextBox";
			this.rotateXTextBox.Size = new System.Drawing.Size(39, 19);
			this.rotateXTextBox.TabIndex = 15;
			this.rotateXTextBox.TextChanged += new System.EventHandler(this.rotateXTextBox_TextChanged);
			this.rotateXTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.rotateXTextBox_KeyDown);
			this.rotateXTextBox.Validating += new System.ComponentModel.CancelEventHandler(this.rotateXTextBox_Validating);
			// 
			// rotateMinusXButton
			// 
			this.rotateMinusXButton.Enabled = false;
			this.rotateMinusXButton.Location = new System.Drawing.Point(146, 53);
			this.rotateMinusXButton.Name = "rotateMinusXButton";
			this.rotateMinusXButton.Size = new System.Drawing.Size(22, 19);
			this.rotateMinusXButton.TabIndex = 20;
			this.rotateMinusXButton.Text = "<";
			this.rotateMinusXButton.UseVisualStyleBackColor = true;
			this.rotateMinusXButton.Click += new System.EventHandler(this.rotateMinusXButton_Click);
			// 
			// rotateMinusYButton
			// 
			this.rotateMinusYButton.Enabled = false;
			this.rotateMinusYButton.Location = new System.Drawing.Point(146, 22);
			this.rotateMinusYButton.Name = "rotateMinusYButton";
			this.rotateMinusYButton.Size = new System.Drawing.Size(22, 19);
			this.rotateMinusYButton.TabIndex = 21;
			this.rotateMinusYButton.Text = "<";
			this.rotateMinusYButton.UseVisualStyleBackColor = true;
			this.rotateMinusYButton.Click += new System.EventHandler(this.rotateMinusYButton_Click);
			// 
			// rotatePlusXButton
			// 
			this.rotatePlusXButton.Enabled = false;
			this.rotatePlusXButton.Location = new System.Drawing.Point(221, 53);
			this.rotatePlusXButton.Name = "rotatePlusXButton";
			this.rotatePlusXButton.Size = new System.Drawing.Size(22, 19);
			this.rotatePlusXButton.TabIndex = 22;
			this.rotatePlusXButton.Text = ">";
			this.rotatePlusXButton.UseVisualStyleBackColor = true;
			this.rotatePlusXButton.Click += new System.EventHandler(this.rotatePlusXButton_Click);
			// 
			// rotatePlusYButton
			// 
			this.rotatePlusYButton.Enabled = false;
			this.rotatePlusYButton.Location = new System.Drawing.Point(221, 22);
			this.rotatePlusYButton.Name = "rotatePlusYButton";
			this.rotatePlusYButton.Size = new System.Drawing.Size(22, 19);
			this.rotatePlusYButton.TabIndex = 23;
			this.rotatePlusYButton.Text = ">";
			this.rotatePlusYButton.UseVisualStyleBackColor = true;
			this.rotatePlusYButton.Click += new System.EventHandler(this.rotatePlusYButton_Click);
			// 
			// rotateXTrackBar
			// 
			this.rotateXTrackBar.AutoSize = false;
			this.rotateXTrackBar.Enabled = false;
			this.rotateXTrackBar.Location = new System.Drawing.Point(249, 53);
			this.rotateXTrackBar.Maximum = 720;
			this.rotateXTrackBar.Name = "rotateXTrackBar";
			this.rotateXTrackBar.Size = new System.Drawing.Size(145, 19);
			this.rotateXTrackBar.TabIndex = 29;
			this.rotateXTrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
			this.rotateXTrackBar.Scroll += new System.EventHandler(this.rotateXTrackBar_Scroll);
			// 
			// rotateYTrackBar
			// 
			this.rotateYTrackBar.AutoSize = false;
			this.rotateYTrackBar.Enabled = false;
			this.rotateYTrackBar.Location = new System.Drawing.Point(249, 22);
			this.rotateYTrackBar.Maximum = 720;
			this.rotateYTrackBar.Name = "rotateYTrackBar";
			this.rotateYTrackBar.Size = new System.Drawing.Size(145, 19);
			this.rotateYTrackBar.TabIndex = 30;
			this.rotateYTrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
			this.rotateYTrackBar.Scroll += new System.EventHandler(this.rotateYTrackBar_Scroll);
			// 
			// leftTopButton
			// 
			this.leftTopButton.Enabled = false;
			this.leftTopButton.Location = new System.Drawing.Point(12, 12);
			this.leftTopButton.Name = "leftTopButton";
			this.leftTopButton.Size = new System.Drawing.Size(22, 19);
			this.leftTopButton.TabIndex = 31;
			this.leftTopButton.Text = "↖";
			this.leftTopButton.UseVisualStyleBackColor = true;
			this.leftTopButton.Click += new System.EventHandler(this.leftTopButton_Click);
			// 
			// topButton
			// 
			this.topButton.Enabled = false;
			this.topButton.Location = new System.Drawing.Point(40, 12);
			this.topButton.Name = "topButton";
			this.topButton.Size = new System.Drawing.Size(22, 19);
			this.topButton.TabIndex = 32;
			this.topButton.Text = "↑";
			this.topButton.UseVisualStyleBackColor = true;
			this.topButton.Click += new System.EventHandler(this.topButton_Click);
			// 
			// rightTopButton
			// 
			this.rightTopButton.Enabled = false;
			this.rightTopButton.Location = new System.Drawing.Point(68, 12);
			this.rightTopButton.Name = "rightTopButton";
			this.rightTopButton.Size = new System.Drawing.Size(22, 19);
			this.rightTopButton.TabIndex = 33;
			this.rightTopButton.Text = "↗";
			this.rightTopButton.UseVisualStyleBackColor = true;
			this.rightTopButton.Click += new System.EventHandler(this.rightTopButton_Click);
			// 
			// rightButton
			// 
			this.rightButton.Enabled = false;
			this.rightButton.Location = new System.Drawing.Point(68, 37);
			this.rightButton.Name = "rightButton";
			this.rightButton.Size = new System.Drawing.Size(22, 19);
			this.rightButton.TabIndex = 36;
			this.rightButton.Text = "→";
			this.rightButton.UseVisualStyleBackColor = true;
			this.rightButton.Click += new System.EventHandler(this.rightButton_Click);
			// 
			// centerButton
			// 
			this.centerButton.Enabled = false;
			this.centerButton.Location = new System.Drawing.Point(40, 37);
			this.centerButton.Name = "centerButton";
			this.centerButton.Size = new System.Drawing.Size(22, 19);
			this.centerButton.TabIndex = 35;
			this.centerButton.Text = "●";
			this.centerButton.UseVisualStyleBackColor = true;
			this.centerButton.Click += new System.EventHandler(this.centerButton_Click);
			// 
			// leftButton
			// 
			this.leftButton.Enabled = false;
			this.leftButton.Location = new System.Drawing.Point(12, 37);
			this.leftButton.Name = "leftButton";
			this.leftButton.Size = new System.Drawing.Size(22, 19);
			this.leftButton.TabIndex = 34;
			this.leftButton.Text = "←";
			this.leftButton.UseVisualStyleBackColor = true;
			this.leftButton.Click += new System.EventHandler(this.leftButton_Click);
			// 
			// rightBottomButton
			// 
			this.rightBottomButton.Enabled = false;
			this.rightBottomButton.Location = new System.Drawing.Point(68, 62);
			this.rightBottomButton.Name = "rightBottomButton";
			this.rightBottomButton.Size = new System.Drawing.Size(22, 19);
			this.rightBottomButton.TabIndex = 39;
			this.rightBottomButton.Text = "↘";
			this.rightBottomButton.UseVisualStyleBackColor = true;
			this.rightBottomButton.Click += new System.EventHandler(this.rightBottomButton_Click);
			// 
			// bottomButton
			// 
			this.bottomButton.Enabled = false;
			this.bottomButton.Location = new System.Drawing.Point(40, 62);
			this.bottomButton.Name = "bottomButton";
			this.bottomButton.Size = new System.Drawing.Size(22, 19);
			this.bottomButton.TabIndex = 38;
			this.bottomButton.Text = "↓";
			this.bottomButton.UseVisualStyleBackColor = true;
			this.bottomButton.Click += new System.EventHandler(this.bottomButton_Click);
			// 
			// leftBottomButton
			// 
			this.leftBottomButton.Enabled = false;
			this.leftBottomButton.Location = new System.Drawing.Point(12, 62);
			this.leftBottomButton.Name = "leftBottomButton";
			this.leftBottomButton.Size = new System.Drawing.Size(22, 19);
			this.leftBottomButton.TabIndex = 37;
			this.leftBottomButton.Text = "↙";
			this.leftBottomButton.UseVisualStyleBackColor = true;
			this.leftBottomButton.Click += new System.EventHandler(this.leftBottomButton_Click);
			// 
			// LightControlForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(432, 127);
			this.ControlBox = false;
			this.Controls.Add(this.rightBottomButton);
			this.Controls.Add(this.bottomButton);
			this.Controls.Add(this.leftBottomButton);
			this.Controls.Add(this.rightButton);
			this.Controls.Add(this.centerButton);
			this.Controls.Add(this.leftButton);
			this.Controls.Add(this.rightTopButton);
			this.Controls.Add(this.topButton);
			this.Controls.Add(this.leftTopButton);
			this.Controls.Add(this.rotateYTrackBar);
			this.Controls.Add(this.rotateXTrackBar);
			this.Controls.Add(this.rotatePlusYButton);
			this.Controls.Add(this.rotatePlusXButton);
			this.Controls.Add(this.rotateMinusYButton);
			this.Controls.Add(this.rotateMinusXButton);
			this.Controls.Add(this.rotateYLabel);
			this.Controls.Add(this.rotateXLabel);
			this.Controls.Add(this.rotateYTextBox);
			this.Controls.Add(this.rotateXTextBox);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.Name = "LightControlForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.Text = "ライト設定";
			this.Load += new System.EventHandler(this.LightControlForm_Load);
			((System.ComponentModel.ISupportInitialize)(this.rotateXTrackBar)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.rotateYTrackBar)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label rotateYLabel;
		private System.Windows.Forms.Label rotateXLabel;
		private System.Windows.Forms.TextBox rotateYTextBox;
		private System.Windows.Forms.TextBox rotateXTextBox;
		private System.Windows.Forms.Button rotateMinusXButton;
		private System.Windows.Forms.Button rotateMinusYButton;
		private System.Windows.Forms.Button rotatePlusXButton;
		private System.Windows.Forms.Button rotatePlusYButton;
		private System.Windows.Forms.TrackBar rotateXTrackBar;
		private System.Windows.Forms.TrackBar rotateYTrackBar;
		private System.Windows.Forms.Button leftTopButton;
		private System.Windows.Forms.Button topButton;
		private System.Windows.Forms.Button rightTopButton;
		private System.Windows.Forms.Button rightButton;
		private System.Windows.Forms.Button centerButton;
		private System.Windows.Forms.Button leftButton;
		private System.Windows.Forms.Button rightBottomButton;
		private System.Windows.Forms.Button bottomButton;
		private System.Windows.Forms.Button leftBottomButton;
	}
}