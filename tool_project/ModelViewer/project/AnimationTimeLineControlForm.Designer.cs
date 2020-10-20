namespace ModelViewer
{
	partial class AnimationTimeLineControlForm
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AnimationTimeLineControlForm));
      this.animationReverseButton = new System.Windows.Forms.Button();
      this.frameRateComboBox = new System.Windows.Forms.ComboBox();
      this.loopCheckBox = new System.Windows.Forms.CheckBox();
      this.animationStopButton = new System.Windows.Forms.Button();
      this.animationPauseButton = new System.Windows.Forms.Button();
      this.nextFrameButton = new System.Windows.Forms.Button();
      this.animationPlayButton = new System.Windows.Forms.Button();
      this.prevFrameButton = new System.Windows.Forms.Button();
      this.motionSpeedLabel = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this.decrementMotionSpeedButton = new System.Windows.Forms.Button();
      this.incrementMotionSpeedButton = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // animationReverseButton
      // 
      resources.ApplyResources(this.animationReverseButton, "animationReverseButton");
      this.animationReverseButton.Name = "animationReverseButton";
      this.animationReverseButton.UseVisualStyleBackColor = true;
      this.animationReverseButton.Click += new System.EventHandler(this.animationReverseButton_Click);
      // 
      // frameRateComboBox
      // 
      this.frameRateComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
      resources.ApplyResources(this.frameRateComboBox, "frameRateComboBox");
      this.frameRateComboBox.FormattingEnabled = true;
      this.frameRateComboBox.Items.AddRange(new object[] {
            resources.GetString("frameRateComboBox.Items"),
            resources.GetString("frameRateComboBox.Items1")});
      this.frameRateComboBox.Name = "frameRateComboBox";
      this.frameRateComboBox.SelectedIndexChanged += new System.EventHandler(this.frameRateComboBox_SelectedIndexChanged);
      // 
      // loopCheckBox
      // 
      resources.ApplyResources(this.loopCheckBox, "loopCheckBox");
      this.loopCheckBox.Name = "loopCheckBox";
      this.loopCheckBox.UseVisualStyleBackColor = true;
      this.loopCheckBox.CheckedChanged += new System.EventHandler(this.loopCheckBox_CheckedChanged);
      // 
      // animationStopButton
      // 
      resources.ApplyResources(this.animationStopButton, "animationStopButton");
      this.animationStopButton.Name = "animationStopButton";
      this.animationStopButton.UseVisualStyleBackColor = true;
      this.animationStopButton.Click += new System.EventHandler(this.animationStopButton_Click);
      // 
      // animationPauseButton
      // 
      resources.ApplyResources(this.animationPauseButton, "animationPauseButton");
      this.animationPauseButton.Name = "animationPauseButton";
      this.animationPauseButton.UseVisualStyleBackColor = true;
      this.animationPauseButton.Click += new System.EventHandler(this.animationPauseButton_Click);
      // 
      // nextFrameButton
      // 
      resources.ApplyResources(this.nextFrameButton, "nextFrameButton");
      this.nextFrameButton.Name = "nextFrameButton";
      this.nextFrameButton.UseVisualStyleBackColor = true;
      this.nextFrameButton.Click += new System.EventHandler(this.nextFrameButton_Click);
      // 
      // animationPlayButton
      // 
      resources.ApplyResources(this.animationPlayButton, "animationPlayButton");
      this.animationPlayButton.Name = "animationPlayButton";
      this.animationPlayButton.UseVisualStyleBackColor = true;
      this.animationPlayButton.Click += new System.EventHandler(this.animationPlayButton_Click);
      // 
      // prevFrameButton
      // 
      resources.ApplyResources(this.prevFrameButton, "prevFrameButton");
      this.prevFrameButton.Name = "prevFrameButton";
      this.prevFrameButton.UseVisualStyleBackColor = true;
      this.prevFrameButton.Click += new System.EventHandler(this.prevFrameButton_Click);
      // 
      // motionSpeedLabel
      // 
      resources.ApplyResources(this.motionSpeedLabel, "motionSpeedLabel");
      this.motionSpeedLabel.Name = "motionSpeedLabel";
      // 
      // label1
      // 
      resources.ApplyResources(this.label1, "label1");
      this.label1.Name = "label1";
      // 
      // decrementMotionSpeedButton
      // 
      resources.ApplyResources(this.decrementMotionSpeedButton, "decrementMotionSpeedButton");
      this.decrementMotionSpeedButton.Name = "decrementMotionSpeedButton";
      this.decrementMotionSpeedButton.UseVisualStyleBackColor = true;
      this.decrementMotionSpeedButton.Click += new System.EventHandler(this.decrementMotionSpeedButton_Click);
      // 
      // incrementMotionSpeedButton
      // 
      resources.ApplyResources(this.incrementMotionSpeedButton, "incrementMotionSpeedButton");
      this.incrementMotionSpeedButton.Name = "incrementMotionSpeedButton";
      this.incrementMotionSpeedButton.UseVisualStyleBackColor = true;
      this.incrementMotionSpeedButton.Click += new System.EventHandler(this.incrementMotionSpeedButton_Click);
      // 
      // AnimationTimeLineControlForm
      // 
      resources.ApplyResources(this, "$this");
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ControlBox = false;
      this.Controls.Add(this.incrementMotionSpeedButton);
      this.Controls.Add(this.decrementMotionSpeedButton);
      this.Controls.Add(this.label1);
      this.Controls.Add(this.motionSpeedLabel);
      this.Controls.Add(this.animationReverseButton);
      this.Controls.Add(this.frameRateComboBox);
      this.Controls.Add(this.loopCheckBox);
      this.Controls.Add(this.animationStopButton);
      this.Controls.Add(this.animationPauseButton);
      this.Controls.Add(this.nextFrameButton);
      this.Controls.Add(this.animationPlayButton);
      this.Controls.Add(this.prevFrameButton);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
      this.Name = "AnimationTimeLineControlForm";
      this.Load += new System.EventHandler(this.AnimationTimeLineControlForm_Load);
      this.ResumeLayout(false);
      this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button animationReverseButton;
		private System.Windows.Forms.ComboBox frameRateComboBox;
		private System.Windows.Forms.CheckBox loopCheckBox;
		private System.Windows.Forms.Button animationStopButton;
		private System.Windows.Forms.Button animationPauseButton;
		private System.Windows.Forms.Button nextFrameButton;
		private System.Windows.Forms.Button animationPlayButton;
		private System.Windows.Forms.Button prevFrameButton;
		private System.Windows.Forms.Label motionSpeedLabel;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button decrementMotionSpeedButton;
		private System.Windows.Forms.Button incrementMotionSpeedButton;
	}
}