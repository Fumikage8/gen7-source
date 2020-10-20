namespace ModelViewer
{
	partial class CameraControlForm
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
			this.nearFarSelectPanel = new System.Windows.Forms.Panel();
			this.farRadioButton = new System.Windows.Forms.RadioButton();
			this.nearRadioButton = new System.Windows.Forms.RadioButton();
			this.panel1 = new System.Windows.Forms.Panel();
			this.frontSideSizeLRadioButton = new System.Windows.Forms.RadioButton();
			this.frontSideSizeMRadioButton = new System.Windows.Forms.RadioButton();
			this.frontSideSizeSRadioButton = new System.Windows.Forms.RadioButton();
			this.panel2 = new System.Windows.Forms.Panel();
			this.backSideSizeLRadioButton = new System.Windows.Forms.RadioButton();
			this.backSideSizeMRadioButton = new System.Windows.Forms.RadioButton();
			this.backSideSizeSRadioButton = new System.Windows.Forms.RadioButton();
			this.cameraTypeChangePanel = new System.Windows.Forms.Panel();
			this.kwCameraRadioButton = new System.Windows.Forms.RadioButton();
			this.statusCameraRadioButton = new System.Windows.Forms.RadioButton();
			this.battleCameraRadioButton = new System.Windows.Forms.RadioButton();
			this.freeCameraRadioButton = new System.Windows.Forms.RadioButton();
			this.battleCameraExplainLabel = new System.Windows.Forms.Label();
			this.fieldOfViewLabel = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.nearFarSelectPanel.SuspendLayout();
			this.panel1.SuspendLayout();
			this.panel2.SuspendLayout();
			this.cameraTypeChangePanel.SuspendLayout();
			this.SuspendLayout();
			// 
			// nearFarSelectPanel
			// 
			this.nearFarSelectPanel.Controls.Add(this.farRadioButton);
			this.nearFarSelectPanel.Controls.Add(this.nearRadioButton);
			this.nearFarSelectPanel.Location = new System.Drawing.Point(93, 63);
			this.nearFarSelectPanel.Name = "nearFarSelectPanel";
			this.nearFarSelectPanel.Size = new System.Drawing.Size(83, 48);
			this.nearFarSelectPanel.TabIndex = 0;
			// 
			// farRadioButton
			// 
			this.farRadioButton.AutoSize = true;
			this.farRadioButton.Location = new System.Drawing.Point(4, 28);
			this.farRadioButton.Name = "farRadioButton";
			this.farRadioButton.Size = new System.Drawing.Size(71, 16);
			this.farRadioButton.TabIndex = 1;
			this.farRadioButton.TabStop = true;
			this.farRadioButton.Text = "相手視点";
			this.farRadioButton.UseVisualStyleBackColor = true;
			this.farRadioButton.CheckedChanged += new System.EventHandler(this.farRadioButton_CheckedChanged);
			// 
			// nearRadioButton
			// 
			this.nearRadioButton.AutoSize = true;
			this.nearRadioButton.Checked = true;
			this.nearRadioButton.Location = new System.Drawing.Point(4, 4);
			this.nearRadioButton.Name = "nearRadioButton";
			this.nearRadioButton.Size = new System.Drawing.Size(71, 16);
			this.nearRadioButton.TabIndex = 0;
			this.nearRadioButton.TabStop = true;
			this.nearRadioButton.Text = "自分視点";
			this.nearRadioButton.UseVisualStyleBackColor = true;
			this.nearRadioButton.CheckedChanged += new System.EventHandler(this.nearRadioButton_CheckedChanged);
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.frontSideSizeLRadioButton);
			this.panel1.Controls.Add(this.frontSideSizeMRadioButton);
			this.panel1.Controls.Add(this.frontSideSizeSRadioButton);
			this.panel1.Location = new System.Drawing.Point(182, 63);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(115, 24);
			this.panel1.TabIndex = 1;
			// 
			// frontSideSizeLRadioButton
			// 
			this.frontSideSizeLRadioButton.AutoSize = true;
			this.frontSideSizeLRadioButton.Location = new System.Drawing.Point(82, 4);
			this.frontSideSizeLRadioButton.Name = "frontSideSizeLRadioButton";
			this.frontSideSizeLRadioButton.Size = new System.Drawing.Size(29, 16);
			this.frontSideSizeLRadioButton.TabIndex = 2;
			this.frontSideSizeLRadioButton.TabStop = true;
			this.frontSideSizeLRadioButton.Text = "L";
			this.frontSideSizeLRadioButton.UseVisualStyleBackColor = true;
			this.frontSideSizeLRadioButton.CheckedChanged += new System.EventHandler(this.frontSideSizeLRadioButton_CheckedChanged);
			// 
			// frontSideSizeMRadioButton
			// 
			this.frontSideSizeMRadioButton.AutoSize = true;
			this.frontSideSizeMRadioButton.Location = new System.Drawing.Point(42, 4);
			this.frontSideSizeMRadioButton.Name = "frontSideSizeMRadioButton";
			this.frontSideSizeMRadioButton.Size = new System.Drawing.Size(32, 16);
			this.frontSideSizeMRadioButton.TabIndex = 1;
			this.frontSideSizeMRadioButton.TabStop = true;
			this.frontSideSizeMRadioButton.Text = "M";
			this.frontSideSizeMRadioButton.UseVisualStyleBackColor = true;
			this.frontSideSizeMRadioButton.CheckedChanged += new System.EventHandler(this.frontSideSizeMRadioButton_CheckedChanged);
			// 
			// frontSideSizeSRadioButton
			// 
			this.frontSideSizeSRadioButton.AutoSize = true;
			this.frontSideSizeSRadioButton.Checked = true;
			this.frontSideSizeSRadioButton.Location = new System.Drawing.Point(4, 4);
			this.frontSideSizeSRadioButton.Name = "frontSideSizeSRadioButton";
			this.frontSideSizeSRadioButton.Size = new System.Drawing.Size(30, 16);
			this.frontSideSizeSRadioButton.TabIndex = 0;
			this.frontSideSizeSRadioButton.TabStop = true;
			this.frontSideSizeSRadioButton.Text = "S";
			this.frontSideSizeSRadioButton.UseVisualStyleBackColor = true;
			this.frontSideSizeSRadioButton.CheckedChanged += new System.EventHandler(this.frontSideSizeSRadioButton_CheckedChanged);
			// 
			// panel2
			// 
			this.panel2.Controls.Add(this.backSideSizeLRadioButton);
			this.panel2.Controls.Add(this.backSideSizeMRadioButton);
			this.panel2.Controls.Add(this.backSideSizeSRadioButton);
			this.panel2.Location = new System.Drawing.Point(182, 87);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(115, 24);
			this.panel2.TabIndex = 3;
			// 
			// backSideSizeLRadioButton
			// 
			this.backSideSizeLRadioButton.AutoSize = true;
			this.backSideSizeLRadioButton.Location = new System.Drawing.Point(82, 4);
			this.backSideSizeLRadioButton.Name = "backSideSizeLRadioButton";
			this.backSideSizeLRadioButton.Size = new System.Drawing.Size(29, 16);
			this.backSideSizeLRadioButton.TabIndex = 2;
			this.backSideSizeLRadioButton.TabStop = true;
			this.backSideSizeLRadioButton.Text = "L";
			this.backSideSizeLRadioButton.UseVisualStyleBackColor = true;
			this.backSideSizeLRadioButton.CheckedChanged += new System.EventHandler(this.backSideSizeLRadioButton_CheckedChanged);
			// 
			// backSideSizeMRadioButton
			// 
			this.backSideSizeMRadioButton.AutoSize = true;
			this.backSideSizeMRadioButton.Location = new System.Drawing.Point(42, 4);
			this.backSideSizeMRadioButton.Name = "backSideSizeMRadioButton";
			this.backSideSizeMRadioButton.Size = new System.Drawing.Size(32, 16);
			this.backSideSizeMRadioButton.TabIndex = 1;
			this.backSideSizeMRadioButton.TabStop = true;
			this.backSideSizeMRadioButton.Text = "M";
			this.backSideSizeMRadioButton.UseVisualStyleBackColor = true;
			this.backSideSizeMRadioButton.CheckedChanged += new System.EventHandler(this.backSideSizeMRadioButton_CheckedChanged);
			// 
			// backSideSizeSRadioButton
			// 
			this.backSideSizeSRadioButton.AutoSize = true;
			this.backSideSizeSRadioButton.Checked = true;
			this.backSideSizeSRadioButton.Location = new System.Drawing.Point(4, 4);
			this.backSideSizeSRadioButton.Name = "backSideSizeSRadioButton";
			this.backSideSizeSRadioButton.Size = new System.Drawing.Size(30, 16);
			this.backSideSizeSRadioButton.TabIndex = 0;
			this.backSideSizeSRadioButton.TabStop = true;
			this.backSideSizeSRadioButton.Text = "S";
			this.backSideSizeSRadioButton.UseVisualStyleBackColor = true;
			this.backSideSizeSRadioButton.CheckedChanged += new System.EventHandler(this.backSideSizeSRadioButton_CheckedChanged);
			// 
			// cameraTypeChangePanel
			// 
			this.cameraTypeChangePanel.Controls.Add(this.kwCameraRadioButton);
			this.cameraTypeChangePanel.Controls.Add(this.statusCameraRadioButton);
			this.cameraTypeChangePanel.Controls.Add(this.battleCameraRadioButton);
			this.cameraTypeChangePanel.Controls.Add(this.freeCameraRadioButton);
			this.cameraTypeChangePanel.Location = new System.Drawing.Point(13, 13);
			this.cameraTypeChangePanel.Name = "cameraTypeChangePanel";
			this.cameraTypeChangePanel.Size = new System.Drawing.Size(285, 23);
			this.cameraTypeChangePanel.TabIndex = 5;
			// 
			// kwCameraRadioButton
			// 
			this.kwCameraRadioButton.AutoSize = true;
			this.kwCameraRadioButton.Location = new System.Drawing.Point(147, 4);
			this.kwCameraRadioButton.Name = "kwCameraRadioButton";
			this.kwCameraRadioButton.Size = new System.Drawing.Size(71, 16);
			this.kwCameraRadioButton.TabIndex = 3;
			this.kwCameraRadioButton.TabStop = true;
			this.kwCameraRadioButton.Text = "かわいがり";
			this.kwCameraRadioButton.UseVisualStyleBackColor = true;
			this.kwCameraRadioButton.CheckedChanged += new System.EventHandler(this.kwCameraRadioButton_CheckedChanged);
			// 
			// statusCameraRadioButton
			// 
			this.statusCameraRadioButton.AutoSize = true;
			this.statusCameraRadioButton.Location = new System.Drawing.Point(73, 4);
			this.statusCameraRadioButton.Name = "statusCameraRadioButton";
			this.statusCameraRadioButton.Size = new System.Drawing.Size(68, 16);
			this.statusCameraRadioButton.TabIndex = 2;
			this.statusCameraRadioButton.TabStop = true;
			this.statusCameraRadioButton.Text = "ステータス";
			this.statusCameraRadioButton.UseVisualStyleBackColor = true;
			this.statusCameraRadioButton.CheckedChanged += new System.EventHandler(this.statusCameraRadioButton_CheckedChanged);
			// 
			// battleCameraRadioButton
			// 
			this.battleCameraRadioButton.AutoSize = true;
			this.battleCameraRadioButton.Location = new System.Drawing.Point(224, 4);
			this.battleCameraRadioButton.Name = "battleCameraRadioButton";
			this.battleCameraRadioButton.Size = new System.Drawing.Size(51, 16);
			this.battleCameraRadioButton.TabIndex = 1;
			this.battleCameraRadioButton.Text = "バトル";
			this.battleCameraRadioButton.UseVisualStyleBackColor = true;
			this.battleCameraRadioButton.CheckedChanged += new System.EventHandler(this.battleCameraRadioButton_CheckedChanged);
			// 
			// freeCameraRadioButton
			// 
			this.freeCameraRadioButton.AutoSize = true;
			this.freeCameraRadioButton.Location = new System.Drawing.Point(4, 4);
			this.freeCameraRadioButton.Name = "freeCameraRadioButton";
			this.freeCameraRadioButton.Size = new System.Drawing.Size(67, 16);
			this.freeCameraRadioButton.TabIndex = 0;
			this.freeCameraRadioButton.Text = "デフォルト";
			this.freeCameraRadioButton.UseVisualStyleBackColor = true;
			this.freeCameraRadioButton.CheckedChanged += new System.EventHandler(this.freeCameraRadioButton_CheckedChanged);
			// 
			// battleCameraExplainLabel
			// 
			this.battleCameraExplainLabel.AutoSize = true;
			this.battleCameraExplainLabel.Location = new System.Drawing.Point(100, 43);
			this.battleCameraExplainLabel.Name = "battleCameraExplainLabel";
			this.battleCameraExplainLabel.Size = new System.Drawing.Size(54, 12);
			this.battleCameraExplainLabel.TabIndex = 6;
			this.battleCameraExplainLabel.Text = "カメラ画角";
			// 
			// fieldOfViewLabel
			// 
			this.fieldOfViewLabel.AutoSize = true;
			this.fieldOfViewLabel.Location = new System.Drawing.Point(160, 43);
			this.fieldOfViewLabel.Name = "fieldOfViewLabel";
			this.fieldOfViewLabel.Size = new System.Drawing.Size(11, 12);
			this.fieldOfViewLabel.TabIndex = 7;
			this.fieldOfViewLabel.Text = "0";
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(9, 81);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(82, 12);
			this.label1.TabIndex = 8;
			this.label1.Text = "バトルカメラ設定";
			// 
			// CameraControlForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(338, 145);
			this.ControlBox = false;
			this.Controls.Add(this.label1);
			this.Controls.Add(this.fieldOfViewLabel);
			this.Controls.Add(this.battleCameraExplainLabel);
			this.Controls.Add(this.cameraTypeChangePanel);
			this.Controls.Add(this.panel2);
			this.Controls.Add(this.panel1);
			this.Controls.Add(this.nearFarSelectPanel);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.Name = "CameraControlForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.Text = "カメラ設定";
			this.Load += new System.EventHandler(this.CameraControlForm_Load);
			this.nearFarSelectPanel.ResumeLayout(false);
			this.nearFarSelectPanel.PerformLayout();
			this.panel1.ResumeLayout(false);
			this.panel1.PerformLayout();
			this.panel2.ResumeLayout(false);
			this.panel2.PerformLayout();
			this.cameraTypeChangePanel.ResumeLayout(false);
			this.cameraTypeChangePanel.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Panel nearFarSelectPanel;
		private System.Windows.Forms.RadioButton farRadioButton;
		private System.Windows.Forms.RadioButton nearRadioButton;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.RadioButton frontSideSizeLRadioButton;
		private System.Windows.Forms.RadioButton frontSideSizeMRadioButton;
		private System.Windows.Forms.RadioButton frontSideSizeSRadioButton;
		private System.Windows.Forms.Panel panel2;
		private System.Windows.Forms.RadioButton backSideSizeLRadioButton;
		private System.Windows.Forms.RadioButton backSideSizeMRadioButton;
		private System.Windows.Forms.RadioButton backSideSizeSRadioButton;
		private System.Windows.Forms.Panel cameraTypeChangePanel;
		private System.Windows.Forms.RadioButton battleCameraRadioButton;
		private System.Windows.Forms.RadioButton freeCameraRadioButton;
		private System.Windows.Forms.Label battleCameraExplainLabel;
		private System.Windows.Forms.RadioButton statusCameraRadioButton;
		private System.Windows.Forms.RadioButton kwCameraRadioButton;
		private System.Windows.Forms.Label fieldOfViewLabel;
		private System.Windows.Forms.Label label1;
	}
}