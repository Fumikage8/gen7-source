namespace ModelViewer
{
	partial class FaceControlForm
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
			this.translateZTrackBar = new System.Windows.Forms.TrackBar();
			this.translateYTrackBar = new System.Windows.Forms.TrackBar();
			this.translateXTrackBar = new System.Windows.Forms.TrackBar();
			this.translateZLabel = new System.Windows.Forms.Label();
			this.translateYLabel = new System.Windows.Forms.Label();
			this.translateXLabel = new System.Windows.Forms.Label();
			this.translateZTextBox = new System.Windows.Forms.TextBox();
			this.translateYTextBox = new System.Windows.Forms.TextBox();
			this.translateXTextBox = new System.Windows.Forms.TextBox();
			this.interestPositionEnableCheckBox = new System.Windows.Forms.CheckBox();
			this.faceLookEnableCheckBox = new System.Windows.Forms.CheckBox();
			this.eyeIndexDecrementButton = new System.Windows.Forms.Button();
			this.eyeIndexLabel = new System.Windows.Forms.Label();
			this.eyeIndexIncrementButton = new System.Windows.Forms.Button();
			this.eyeLabel = new System.Windows.Forms.Label();
			this.mouthLabel = new System.Windows.Forms.Label();
			this.mouthIndexIncrementButton = new System.Windows.Forms.Button();
			this.mouthIndexLabel = new System.Windows.Forms.Label();
			this.mouthIndexDecrementButton = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.translateZTrackBar)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.translateYTrackBar)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.translateXTrackBar)).BeginInit();
			this.SuspendLayout();
			// 
			// translateZTrackBar
			// 
			this.translateZTrackBar.AutoSize = false;
			this.translateZTrackBar.Enabled = false;
			this.translateZTrackBar.Location = new System.Drawing.Point(94, 112);
			this.translateZTrackBar.Maximum = 2000;
			this.translateZTrackBar.Name = "translateZTrackBar";
			this.translateZTrackBar.Size = new System.Drawing.Size(161, 19);
			this.translateZTrackBar.TabIndex = 42;
			this.translateZTrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
			this.translateZTrackBar.Scroll += new System.EventHandler(this.translateZTrackBar_Scroll);
			// 
			// translateYTrackBar
			// 
			this.translateYTrackBar.AutoSize = false;
			this.translateYTrackBar.Enabled = false;
			this.translateYTrackBar.Location = new System.Drawing.Point(94, 78);
			this.translateYTrackBar.Maximum = 2000;
			this.translateYTrackBar.Name = "translateYTrackBar";
			this.translateYTrackBar.Size = new System.Drawing.Size(161, 19);
			this.translateYTrackBar.TabIndex = 41;
			this.translateYTrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
			this.translateYTrackBar.Scroll += new System.EventHandler(this.translateYTrackBar_Scroll);
			// 
			// translateXTrackBar
			// 
			this.translateXTrackBar.AutoSize = false;
			this.translateXTrackBar.Enabled = false;
			this.translateXTrackBar.Location = new System.Drawing.Point(94, 43);
			this.translateXTrackBar.Maximum = 2000;
			this.translateXTrackBar.Name = "translateXTrackBar";
			this.translateXTrackBar.Size = new System.Drawing.Size(161, 19);
			this.translateXTrackBar.TabIndex = 40;
			this.translateXTrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
			this.translateXTrackBar.Scroll += new System.EventHandler(this.translateXTrackBar_Scroll);
			// 
			// translateZLabel
			// 
			this.translateZLabel.AutoSize = true;
			this.translateZLabel.Location = new System.Drawing.Point(18, 115);
			this.translateZLabel.Name = "translateZLabel";
			this.translateZLabel.Size = new System.Drawing.Size(18, 12);
			this.translateZLabel.TabIndex = 39;
			this.translateZLabel.Text = "Z :";
			// 
			// translateYLabel
			// 
			this.translateYLabel.AutoSize = true;
			this.translateYLabel.Location = new System.Drawing.Point(18, 81);
			this.translateYLabel.Name = "translateYLabel";
			this.translateYLabel.Size = new System.Drawing.Size(18, 12);
			this.translateYLabel.TabIndex = 38;
			this.translateYLabel.Text = "Y :";
			// 
			// translateXLabel
			// 
			this.translateXLabel.AutoSize = true;
			this.translateXLabel.Location = new System.Drawing.Point(18, 46);
			this.translateXLabel.Name = "translateXLabel";
			this.translateXLabel.Size = new System.Drawing.Size(18, 12);
			this.translateXLabel.TabIndex = 37;
			this.translateXLabel.Text = "X :";
			// 
			// translateZTextBox
			// 
			this.translateZTextBox.Enabled = false;
			this.translateZTextBox.Location = new System.Drawing.Point(37, 112);
			this.translateZTextBox.Name = "translateZTextBox";
			this.translateZTextBox.Size = new System.Drawing.Size(49, 19);
			this.translateZTextBox.TabIndex = 36;
			this.translateZTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.translateZTextBox_KeyDown);
			this.translateZTextBox.Validating += new System.ComponentModel.CancelEventHandler(this.translateZTextBox_Validating);
			// 
			// translateYTextBox
			// 
			this.translateYTextBox.Enabled = false;
			this.translateYTextBox.Location = new System.Drawing.Point(37, 78);
			this.translateYTextBox.Name = "translateYTextBox";
			this.translateYTextBox.Size = new System.Drawing.Size(49, 19);
			this.translateYTextBox.TabIndex = 35;
			this.translateYTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.translateYTextBox_KeyDown);
			this.translateYTextBox.Validating += new System.ComponentModel.CancelEventHandler(this.translateYTextBox_Validating);
			// 
			// translateXTextBox
			// 
			this.translateXTextBox.Enabled = false;
			this.translateXTextBox.Location = new System.Drawing.Point(37, 43);
			this.translateXTextBox.Name = "translateXTextBox";
			this.translateXTextBox.Size = new System.Drawing.Size(49, 19);
			this.translateXTextBox.TabIndex = 34;
			this.translateXTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.translateXTextBox_KeyDown);
			this.translateXTextBox.Validating += new System.ComponentModel.CancelEventHandler(this.translateXTextBox_Validating);
			// 
			// interestPositionEnableCheckBox
			// 
			this.interestPositionEnableCheckBox.AutoSize = true;
			this.interestPositionEnableCheckBox.Enabled = false;
			this.interestPositionEnableCheckBox.Location = new System.Drawing.Point(13, 13);
			this.interestPositionEnableCheckBox.Name = "interestPositionEnableCheckBox";
			this.interestPositionEnableCheckBox.Size = new System.Drawing.Size(93, 16);
			this.interestPositionEnableCheckBox.TabIndex = 43;
			this.interestPositionEnableCheckBox.Text = "振り向きモード";
			this.interestPositionEnableCheckBox.UseVisualStyleBackColor = true;
			this.interestPositionEnableCheckBox.CheckedChanged += new System.EventHandler(this.interestPositionEnableCheckBox_CheckedChanged);
			// 
			// faceLookEnableCheckBox
			// 
			this.faceLookEnableCheckBox.AutoSize = true;
			this.faceLookEnableCheckBox.Enabled = false;
			this.faceLookEnableCheckBox.Location = new System.Drawing.Point(13, 155);
			this.faceLookEnableCheckBox.Name = "faceLookEnableCheckBox";
			this.faceLookEnableCheckBox.Size = new System.Drawing.Size(79, 16);
			this.faceLookEnableCheckBox.TabIndex = 44;
			this.faceLookEnableCheckBox.Text = "表情チェック";
			this.faceLookEnableCheckBox.UseVisualStyleBackColor = true;
			this.faceLookEnableCheckBox.CheckedChanged += new System.EventHandler(this.faceLookEnableCheckBox_CheckedChanged);
			// 
			// eyeIndexDecrementButton
			// 
			this.eyeIndexDecrementButton.Enabled = false;
			this.eyeIndexDecrementButton.Location = new System.Drawing.Point(94, 179);
			this.eyeIndexDecrementButton.Name = "eyeIndexDecrementButton";
			this.eyeIndexDecrementButton.Size = new System.Drawing.Size(26, 23);
			this.eyeIndexDecrementButton.TabIndex = 45;
			this.eyeIndexDecrementButton.Text = "<";
			this.eyeIndexDecrementButton.UseVisualStyleBackColor = true;
			this.eyeIndexDecrementButton.Click += new System.EventHandler(this.eyeIndexDecrementButton_Click);
			// 
			// eyeIndexLabel
			// 
			this.eyeIndexLabel.AutoSize = true;
			this.eyeIndexLabel.Enabled = false;
			this.eyeIndexLabel.Font = new System.Drawing.Font("MS UI Gothic", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.eyeIndexLabel.Location = new System.Drawing.Point(142, 178);
			this.eyeIndexLabel.Name = "eyeIndexLabel";
			this.eyeIndexLabel.Size = new System.Drawing.Size(22, 24);
			this.eyeIndexLabel.TabIndex = 46;
			this.eyeIndexLabel.Text = "1";
			// 
			// eyeIndexIncrementButton
			// 
			this.eyeIndexIncrementButton.Enabled = false;
			this.eyeIndexIncrementButton.Location = new System.Drawing.Point(183, 179);
			this.eyeIndexIncrementButton.Name = "eyeIndexIncrementButton";
			this.eyeIndexIncrementButton.Size = new System.Drawing.Size(26, 23);
			this.eyeIndexIncrementButton.TabIndex = 47;
			this.eyeIndexIncrementButton.Text = ">";
			this.eyeIndexIncrementButton.UseVisualStyleBackColor = true;
			this.eyeIndexIncrementButton.Click += new System.EventHandler(this.eyeIndexIncrementButton_Click);
			// 
			// eyeLabel
			// 
			this.eyeLabel.AutoSize = true;
			this.eyeLabel.Font = new System.Drawing.Font("MS UI Gothic", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.eyeLabel.Location = new System.Drawing.Point(42, 179);
			this.eyeLabel.Name = "eyeLabel";
			this.eyeLabel.Size = new System.Drawing.Size(34, 24);
			this.eyeLabel.TabIndex = 48;
			this.eyeLabel.Text = "目";
			// 
			// mouthLabel
			// 
			this.mouthLabel.AutoSize = true;
			this.mouthLabel.Font = new System.Drawing.Font("MS UI Gothic", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.mouthLabel.Location = new System.Drawing.Point(42, 212);
			this.mouthLabel.Name = "mouthLabel";
			this.mouthLabel.Size = new System.Drawing.Size(34, 24);
			this.mouthLabel.TabIndex = 52;
			this.mouthLabel.Text = "口";
			// 
			// mouthIndexIncrementButton
			// 
			this.mouthIndexIncrementButton.Enabled = false;
			this.mouthIndexIncrementButton.Location = new System.Drawing.Point(183, 212);
			this.mouthIndexIncrementButton.Name = "mouthIndexIncrementButton";
			this.mouthIndexIncrementButton.Size = new System.Drawing.Size(26, 23);
			this.mouthIndexIncrementButton.TabIndex = 51;
			this.mouthIndexIncrementButton.Text = ">";
			this.mouthIndexIncrementButton.UseVisualStyleBackColor = true;
			this.mouthIndexIncrementButton.Click += new System.EventHandler(this.mouthIndexIncrementButton_Click);
			// 
			// mouthIndexLabel
			// 
			this.mouthIndexLabel.AutoSize = true;
			this.mouthIndexLabel.Enabled = false;
			this.mouthIndexLabel.Font = new System.Drawing.Font("MS UI Gothic", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.mouthIndexLabel.Location = new System.Drawing.Point(142, 211);
			this.mouthIndexLabel.Name = "mouthIndexLabel";
			this.mouthIndexLabel.Size = new System.Drawing.Size(22, 24);
			this.mouthIndexLabel.TabIndex = 50;
			this.mouthIndexLabel.Text = "1";
			// 
			// mouthIndexDecrementButton
			// 
			this.mouthIndexDecrementButton.Enabled = false;
			this.mouthIndexDecrementButton.Location = new System.Drawing.Point(94, 212);
			this.mouthIndexDecrementButton.Name = "mouthIndexDecrementButton";
			this.mouthIndexDecrementButton.Size = new System.Drawing.Size(26, 23);
			this.mouthIndexDecrementButton.TabIndex = 49;
			this.mouthIndexDecrementButton.Text = "<";
			this.mouthIndexDecrementButton.UseVisualStyleBackColor = true;
			this.mouthIndexDecrementButton.Click += new System.EventHandler(this.mouthIndexDecrementButton_Click);
			// 
			// FaceControlForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(273, 270);
			this.ControlBox = false;
			this.Controls.Add(this.mouthLabel);
			this.Controls.Add(this.mouthIndexIncrementButton);
			this.Controls.Add(this.mouthIndexLabel);
			this.Controls.Add(this.mouthIndexDecrementButton);
			this.Controls.Add(this.eyeLabel);
			this.Controls.Add(this.eyeIndexIncrementButton);
			this.Controls.Add(this.eyeIndexLabel);
			this.Controls.Add(this.eyeIndexDecrementButton);
			this.Controls.Add(this.faceLookEnableCheckBox);
			this.Controls.Add(this.interestPositionEnableCheckBox);
			this.Controls.Add(this.translateZTrackBar);
			this.Controls.Add(this.translateYTrackBar);
			this.Controls.Add(this.translateXTrackBar);
			this.Controls.Add(this.translateZLabel);
			this.Controls.Add(this.translateYLabel);
			this.Controls.Add(this.translateXLabel);
			this.Controls.Add(this.translateZTextBox);
			this.Controls.Add(this.translateYTextBox);
			this.Controls.Add(this.translateXTextBox);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.Name = "FaceControlForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.Text = "表情操作";
			this.Load += new System.EventHandler(this.FaceControlForm_Load);
			((System.ComponentModel.ISupportInitialize)(this.translateZTrackBar)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.translateYTrackBar)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.translateXTrackBar)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TrackBar translateZTrackBar;
		private System.Windows.Forms.TrackBar translateYTrackBar;
		private System.Windows.Forms.TrackBar translateXTrackBar;
		private System.Windows.Forms.Label translateZLabel;
		private System.Windows.Forms.Label translateYLabel;
		private System.Windows.Forms.Label translateXLabel;
		private System.Windows.Forms.TextBox translateZTextBox;
		private System.Windows.Forms.TextBox translateYTextBox;
		private System.Windows.Forms.TextBox translateXTextBox;
		private System.Windows.Forms.CheckBox interestPositionEnableCheckBox;
		private System.Windows.Forms.CheckBox faceLookEnableCheckBox;
		private System.Windows.Forms.Button eyeIndexDecrementButton;
		private System.Windows.Forms.Label eyeIndexLabel;
		private System.Windows.Forms.Button eyeIndexIncrementButton;
		private System.Windows.Forms.Label eyeLabel;
		private System.Windows.Forms.Label mouthLabel;
		private System.Windows.Forms.Button mouthIndexIncrementButton;
		private System.Windows.Forms.Label mouthIndexLabel;
		private System.Windows.Forms.Button mouthIndexDecrementButton;
	}
}