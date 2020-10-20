namespace ModelViewer
{
	partial class ColorShaderControlForm
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
			this.colorShaderEnableCheckBox = new System.Windows.Forms.CheckBox();
			this.interpColorRLabel = new System.Windows.Forms.Label();
			this.interpColorATextBox = new System.Windows.Forms.TextBox();
			this.interpColorGLabel = new System.Windows.Forms.Label();
			this.interpColorATrackBar = new System.Windows.Forms.TrackBar();
			this.interpColorBLabel = new System.Windows.Forms.Label();
			this.interpColorALabel = new System.Windows.Forms.Label();
			this.interpColorRTrackBar = new System.Windows.Forms.TrackBar();
			this.interpColorGTrackBar = new System.Windows.Forms.TrackBar();
			this.interpColorBTrackBar = new System.Windows.Forms.TrackBar();
			this.interpColorRTextBox = new System.Windows.Forms.TextBox();
			this.interpColorGTextBox = new System.Windows.Forms.TextBox();
			this.interpColorBTextBox = new System.Windows.Forms.TextBox();
			this.ambientColorEnableCheckBox = new System.Windows.Forms.CheckBox();
			this.ambientColorTextBox = new System.Windows.Forms.TextBox();
			this.ambientColorTrackBar = new System.Windows.Forms.TrackBar();
			this.ambientColorLabel = new System.Windows.Forms.Label();
			this.rareColorEnableButton = new System.Windows.Forms.Button();
			this.normalColorEnableButton = new System.Windows.Forms.Button();
			this.purpleButton = new System.Windows.Forms.Button();
			this.blueButton = new System.Windows.Forms.Button();
			this.yellowButton = new System.Windows.Forms.Button();
			this.redButton = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.interpColorATrackBar)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.interpColorRTrackBar)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.interpColorGTrackBar)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.interpColorBTrackBar)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.ambientColorTrackBar)).BeginInit();
			this.SuspendLayout();
			// 
			// colorShaderEnableCheckBox
			// 
			this.colorShaderEnableCheckBox.AutoSize = true;
			this.colorShaderEnableCheckBox.Enabled = false;
			this.colorShaderEnableCheckBox.Location = new System.Drawing.Point(12, 43);
			this.colorShaderEnableCheckBox.Name = "colorShaderEnableCheckBox";
			this.colorShaderEnableCheckBox.Size = new System.Drawing.Size(141, 16);
			this.colorShaderEnableCheckBox.TabIndex = 50;
			this.colorShaderEnableCheckBox.Text = "カラーシェーダ有効/無効";
			this.colorShaderEnableCheckBox.UseVisualStyleBackColor = true;
			this.colorShaderEnableCheckBox.CheckedChanged += new System.EventHandler(this.colorShaderEnableCheckBox_CheckedChanged);
			// 
			// interpColorRLabel
			// 
			this.interpColorRLabel.AutoSize = true;
			this.interpColorRLabel.Enabled = false;
			this.interpColorRLabel.Location = new System.Drawing.Point(41, 68);
			this.interpColorRLabel.Name = "interpColorRLabel";
			this.interpColorRLabel.Size = new System.Drawing.Size(19, 12);
			this.interpColorRLabel.TabIndex = 51;
			this.interpColorRLabel.Text = "R :";
			// 
			// interpColorATextBox
			// 
			this.interpColorATextBox.Enabled = false;
			this.interpColorATextBox.Location = new System.Drawing.Point(64, 168);
			this.interpColorATextBox.Name = "interpColorATextBox";
			this.interpColorATextBox.Size = new System.Drawing.Size(43, 19);
			this.interpColorATextBox.TabIndex = 62;
			this.interpColorATextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.interpColorATextBox_KeyDown);
			this.interpColorATextBox.Validating += new System.ComponentModel.CancelEventHandler(this.interpColorATextBox_Validating);
			// 
			// interpColorGLabel
			// 
			this.interpColorGLabel.AutoSize = true;
			this.interpColorGLabel.Enabled = false;
			this.interpColorGLabel.Location = new System.Drawing.Point(41, 103);
			this.interpColorGLabel.Name = "interpColorGLabel";
			this.interpColorGLabel.Size = new System.Drawing.Size(19, 12);
			this.interpColorGLabel.TabIndex = 52;
			this.interpColorGLabel.Text = "G :";
			// 
			// interpColorATrackBar
			// 
			this.interpColorATrackBar.AutoSize = false;
			this.interpColorATrackBar.Enabled = false;
			this.interpColorATrackBar.Location = new System.Drawing.Point(113, 168);
			this.interpColorATrackBar.Name = "interpColorATrackBar";
			this.interpColorATrackBar.Size = new System.Drawing.Size(161, 19);
			this.interpColorATrackBar.TabIndex = 61;
			this.interpColorATrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
			this.interpColorATrackBar.Scroll += new System.EventHandler(this.interpColorATrackBar_Scroll);
			// 
			// interpColorBLabel
			// 
			this.interpColorBLabel.AutoSize = true;
			this.interpColorBLabel.Enabled = false;
			this.interpColorBLabel.Location = new System.Drawing.Point(41, 137);
			this.interpColorBLabel.Name = "interpColorBLabel";
			this.interpColorBLabel.Size = new System.Drawing.Size(19, 12);
			this.interpColorBLabel.TabIndex = 53;
			this.interpColorBLabel.Text = "B :";
			// 
			// interpColorALabel
			// 
			this.interpColorALabel.AutoSize = true;
			this.interpColorALabel.Enabled = false;
			this.interpColorALabel.Location = new System.Drawing.Point(10, 171);
			this.interpColorALabel.Name = "interpColorALabel";
			this.interpColorALabel.Size = new System.Drawing.Size(49, 12);
			this.interpColorALabel.TabIndex = 60;
			this.interpColorALabel.Text = "係り具合";
			// 
			// interpColorRTrackBar
			// 
			this.interpColorRTrackBar.AutoSize = false;
			this.interpColorRTrackBar.Enabled = false;
			this.interpColorRTrackBar.Location = new System.Drawing.Point(113, 65);
			this.interpColorRTrackBar.Name = "interpColorRTrackBar";
			this.interpColorRTrackBar.Size = new System.Drawing.Size(161, 19);
			this.interpColorRTrackBar.TabIndex = 54;
			this.interpColorRTrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
			this.interpColorRTrackBar.Scroll += new System.EventHandler(this.interpColorRTrackBar_Scroll);
			// 
			// interpColorGTrackBar
			// 
			this.interpColorGTrackBar.AutoSize = false;
			this.interpColorGTrackBar.Enabled = false;
			this.interpColorGTrackBar.Location = new System.Drawing.Point(113, 100);
			this.interpColorGTrackBar.Name = "interpColorGTrackBar";
			this.interpColorGTrackBar.Size = new System.Drawing.Size(161, 19);
			this.interpColorGTrackBar.TabIndex = 55;
			this.interpColorGTrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
			this.interpColorGTrackBar.Scroll += new System.EventHandler(this.interpColorGTrackBar_Scroll);
			// 
			// interpColorBTrackBar
			// 
			this.interpColorBTrackBar.AutoSize = false;
			this.interpColorBTrackBar.Enabled = false;
			this.interpColorBTrackBar.Location = new System.Drawing.Point(113, 134);
			this.interpColorBTrackBar.Name = "interpColorBTrackBar";
			this.interpColorBTrackBar.Size = new System.Drawing.Size(161, 19);
			this.interpColorBTrackBar.TabIndex = 56;
			this.interpColorBTrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
			this.interpColorBTrackBar.Scroll += new System.EventHandler(this.interpColorBTrackBar_Scroll);
			// 
			// interpColorRTextBox
			// 
			this.interpColorRTextBox.Enabled = false;
			this.interpColorRTextBox.Location = new System.Drawing.Point(63, 65);
			this.interpColorRTextBox.Name = "interpColorRTextBox";
			this.interpColorRTextBox.Size = new System.Drawing.Size(44, 19);
			this.interpColorRTextBox.TabIndex = 57;
			this.interpColorRTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.interpColorRTextBox_KeyDown);
			this.interpColorRTextBox.Validating += new System.ComponentModel.CancelEventHandler(this.interpColorRTextBox_Validating);
			// 
			// interpColorGTextBox
			// 
			this.interpColorGTextBox.Enabled = false;
			this.interpColorGTextBox.Location = new System.Drawing.Point(62, 100);
			this.interpColorGTextBox.Name = "interpColorGTextBox";
			this.interpColorGTextBox.Size = new System.Drawing.Size(45, 19);
			this.interpColorGTextBox.TabIndex = 58;
			this.interpColorGTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.interpColorGTextBox_KeyDown);
			this.interpColorGTextBox.Validating += new System.ComponentModel.CancelEventHandler(this.interpColorGTextBox_Validating);
			// 
			// interpColorBTextBox
			// 
			this.interpColorBTextBox.Enabled = false;
			this.interpColorBTextBox.Location = new System.Drawing.Point(64, 134);
			this.interpColorBTextBox.Name = "interpColorBTextBox";
			this.interpColorBTextBox.Size = new System.Drawing.Size(43, 19);
			this.interpColorBTextBox.TabIndex = 59;
			this.interpColorBTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.interpColorBTextBox_KeyDown);
			this.interpColorBTextBox.Validating += new System.ComponentModel.CancelEventHandler(this.interpColorBTextBox_Validating);
			// 
			// ambientColorEnableCheckBox
			// 
			this.ambientColorEnableCheckBox.AutoSize = true;
			this.ambientColorEnableCheckBox.Enabled = false;
			this.ambientColorEnableCheckBox.Location = new System.Drawing.Point(12, 247);
			this.ambientColorEnableCheckBox.Name = "ambientColorEnableCheckBox";
			this.ambientColorEnableCheckBox.Size = new System.Drawing.Size(114, 16);
			this.ambientColorEnableCheckBox.TabIndex = 63;
			this.ambientColorEnableCheckBox.Text = "環境光有効/無効";
			this.ambientColorEnableCheckBox.UseVisualStyleBackColor = true;
			this.ambientColorEnableCheckBox.CheckedChanged += new System.EventHandler(this.ambientColorEnableCheckBox_CheckedChanged);
			// 
			// ambientColorTextBox
			// 
			this.ambientColorTextBox.Enabled = false;
			this.ambientColorTextBox.Location = new System.Drawing.Point(64, 269);
			this.ambientColorTextBox.Name = "ambientColorTextBox";
			this.ambientColorTextBox.Size = new System.Drawing.Size(43, 19);
			this.ambientColorTextBox.TabIndex = 66;
			this.ambientColorTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.ambientColorTextBox_KeyDown);
			this.ambientColorTextBox.Validating += new System.ComponentModel.CancelEventHandler(this.ambientColorTextBox_Validating);
			// 
			// ambientColorTrackBar
			// 
			this.ambientColorTrackBar.AutoSize = false;
			this.ambientColorTrackBar.Enabled = false;
			this.ambientColorTrackBar.Location = new System.Drawing.Point(113, 269);
			this.ambientColorTrackBar.Name = "ambientColorTrackBar";
			this.ambientColorTrackBar.Size = new System.Drawing.Size(161, 19);
			this.ambientColorTrackBar.TabIndex = 65;
			this.ambientColorTrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
			this.ambientColorTrackBar.Scroll += new System.EventHandler(this.ambientColorTrackBar_Scroll);
			// 
			// ambientColorLabel
			// 
			this.ambientColorLabel.AutoSize = true;
			this.ambientColorLabel.Enabled = false;
			this.ambientColorLabel.Location = new System.Drawing.Point(13, 272);
			this.ambientColorLabel.Name = "ambientColorLabel";
			this.ambientColorLabel.Size = new System.Drawing.Size(49, 12);
			this.ambientColorLabel.TabIndex = 64;
			this.ambientColorLabel.Text = "係り具合";
			// 
			// rareColorEnableButton
			// 
			this.rareColorEnableButton.Enabled = false;
			this.rareColorEnableButton.Location = new System.Drawing.Point(41, 8);
			this.rareColorEnableButton.Name = "rareColorEnableButton";
			this.rareColorEnableButton.Size = new System.Drawing.Size(99, 23);
			this.rareColorEnableButton.TabIndex = 68;
			this.rareColorEnableButton.Text = "レアカラー読込";
			this.rareColorEnableButton.UseVisualStyleBackColor = true;
			this.rareColorEnableButton.Click += new System.EventHandler(this.rareColorEnableButton_Click);
			// 
			// normalColorEnableButton
			// 
			this.normalColorEnableButton.Enabled = false;
			this.normalColorEnableButton.Location = new System.Drawing.Point(159, 8);
			this.normalColorEnableButton.Name = "normalColorEnableButton";
			this.normalColorEnableButton.Size = new System.Drawing.Size(99, 23);
			this.normalColorEnableButton.TabIndex = 69;
			this.normalColorEnableButton.Text = "通常カラー読込";
			this.normalColorEnableButton.UseVisualStyleBackColor = true;
			this.normalColorEnableButton.Click += new System.EventHandler(this.normalColorEnableButton_Click);
			// 
			// purpleButton
			// 
			this.purpleButton.BackColor = System.Drawing.Color.MediumSlateBlue;
			this.purpleButton.Enabled = false;
			this.purpleButton.Location = new System.Drawing.Point(62, 202);
			this.purpleButton.Name = "purpleButton";
			this.purpleButton.Size = new System.Drawing.Size(27, 23);
			this.purpleButton.TabIndex = 70;
			this.purpleButton.Text = " ";
			this.purpleButton.UseVisualStyleBackColor = false;
			this.purpleButton.Click += new System.EventHandler(this.purpleButton_Click);
			// 
			// blueButton
			// 
			this.blueButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
			this.blueButton.Enabled = false;
			this.blueButton.Location = new System.Drawing.Point(231, 202);
			this.blueButton.Name = "blueButton";
			this.blueButton.Size = new System.Drawing.Size(27, 23);
			this.blueButton.TabIndex = 71;
			this.blueButton.Text = " ";
			this.blueButton.UseVisualStyleBackColor = false;
			this.blueButton.Click += new System.EventHandler(this.blueButton_Click);
			// 
			// yellowButton
			// 
			this.yellowButton.BackColor = System.Drawing.Color.Yellow;
			this.yellowButton.Enabled = false;
			this.yellowButton.Location = new System.Drawing.Point(113, 202);
			this.yellowButton.Name = "yellowButton";
			this.yellowButton.Size = new System.Drawing.Size(27, 23);
			this.yellowButton.TabIndex = 72;
			this.yellowButton.Text = " ";
			this.yellowButton.UseVisualStyleBackColor = false;
			this.yellowButton.Click += new System.EventHandler(this.yellowButton_Click);
			// 
			// redButton
			// 
			this.redButton.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(128)))), ((int)(((byte)(128)))));
			this.redButton.Enabled = false;
			this.redButton.Location = new System.Drawing.Point(170, 202);
			this.redButton.Name = "redButton";
			this.redButton.Size = new System.Drawing.Size(27, 23);
			this.redButton.TabIndex = 73;
			this.redButton.Text = " ";
			this.redButton.UseVisualStyleBackColor = false;
			this.redButton.Click += new System.EventHandler(this.redButton_Click);
			// 
			// ColorShaderControlForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(311, 378);
			this.ControlBox = false;
			this.Controls.Add(this.redButton);
			this.Controls.Add(this.yellowButton);
			this.Controls.Add(this.blueButton);
			this.Controls.Add(this.purpleButton);
			this.Controls.Add(this.normalColorEnableButton);
			this.Controls.Add(this.rareColorEnableButton);
			this.Controls.Add(this.ambientColorTextBox);
			this.Controls.Add(this.ambientColorTrackBar);
			this.Controls.Add(this.ambientColorLabel);
			this.Controls.Add(this.ambientColorEnableCheckBox);
			this.Controls.Add(this.colorShaderEnableCheckBox);
			this.Controls.Add(this.interpColorRLabel);
			this.Controls.Add(this.interpColorATextBox);
			this.Controls.Add(this.interpColorGLabel);
			this.Controls.Add(this.interpColorATrackBar);
			this.Controls.Add(this.interpColorBLabel);
			this.Controls.Add(this.interpColorALabel);
			this.Controls.Add(this.interpColorRTrackBar);
			this.Controls.Add(this.interpColorGTrackBar);
			this.Controls.Add(this.interpColorBTrackBar);
			this.Controls.Add(this.interpColorRTextBox);
			this.Controls.Add(this.interpColorGTextBox);
			this.Controls.Add(this.interpColorBTextBox);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.Name = "ColorShaderControlForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.Text = "カラーシェーダ";
			this.Load += new System.EventHandler(this.ColorShaderControlForm_Load);
			((System.ComponentModel.ISupportInitialize)(this.interpColorATrackBar)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.interpColorRTrackBar)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.interpColorGTrackBar)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.interpColorBTrackBar)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.ambientColorTrackBar)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.CheckBox colorShaderEnableCheckBox;
		private System.Windows.Forms.Label interpColorRLabel;
		private System.Windows.Forms.TextBox interpColorATextBox;
		private System.Windows.Forms.Label interpColorGLabel;
		private System.Windows.Forms.TrackBar interpColorATrackBar;
		private System.Windows.Forms.Label interpColorBLabel;
		private System.Windows.Forms.Label interpColorALabel;
		private System.Windows.Forms.TrackBar interpColorRTrackBar;
		private System.Windows.Forms.TrackBar interpColorGTrackBar;
		private System.Windows.Forms.TrackBar interpColorBTrackBar;
		private System.Windows.Forms.TextBox interpColorRTextBox;
		private System.Windows.Forms.TextBox interpColorGTextBox;
		private System.Windows.Forms.TextBox interpColorBTextBox;
		private System.Windows.Forms.CheckBox ambientColorEnableCheckBox;
		private System.Windows.Forms.TextBox ambientColorTextBox;
		private System.Windows.Forms.TrackBar ambientColorTrackBar;
		private System.Windows.Forms.Label ambientColorLabel;
		private System.Windows.Forms.Button rareColorEnableButton;
		private System.Windows.Forms.Button normalColorEnableButton;
		private System.Windows.Forms.Button purpleButton;
		private System.Windows.Forms.Button blueButton;
		private System.Windows.Forms.Button yellowButton;
		private System.Windows.Forms.Button redButton;
	}
}