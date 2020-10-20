namespace ModelViewer
{
  partial class MarkingControlForm
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
      this.mark1Label = new System.Windows.Forms.Label();
      this.mark1TextBox = new System.Windows.Forms.TextBox();
      this.mark2TextBox = new System.Windows.Forms.TextBox();
      this.mark2Label = new System.Windows.Forms.Label();
      this.mark4TextBox = new System.Windows.Forms.TextBox();
      this.mark4Label = new System.Windows.Forms.Label();
      this.mark3TextBox = new System.Windows.Forms.TextBox();
      this.mark3Label = new System.Windows.Forms.Label();
      this.setMarkingsButton = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // mark1Label
      // 
      this.mark1Label.AutoSize = true;
      this.mark1Label.Location = new System.Drawing.Point(12, 9);
      this.mark1Label.Name = "mark1Label";
      this.mark1Label.Size = new System.Drawing.Size(86, 12);
      this.mark1Label.TabIndex = 52;
      this.mark1Label.Text = "Mark1(0～255) :";
      // 
      // mark1TextBox
      // 
      this.mark1TextBox.Enabled = false;
      this.mark1TextBox.Location = new System.Drawing.Point(104, 6);
      this.mark1TextBox.Name = "mark1TextBox";
      this.mark1TextBox.Size = new System.Drawing.Size(44, 19);
      this.mark1TextBox.TabIndex = 58;
      this.mark1TextBox.Text = "0";
      this.mark1TextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.mark1TextBox_KeyDown);
      this.mark1TextBox.Validating += new System.ComponentModel.CancelEventHandler(this.mark1TextBox_Validating);
      // 
      // mark2TextBox
      // 
      this.mark2TextBox.Enabled = false;
      this.mark2TextBox.Location = new System.Drawing.Point(104, 32);
      this.mark2TextBox.Name = "mark2TextBox";
      this.mark2TextBox.Size = new System.Drawing.Size(44, 19);
      this.mark2TextBox.TabIndex = 60;
      this.mark2TextBox.Text = "0";
      this.mark2TextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.mark2TextBox_KeyDown);
      this.mark2TextBox.Validating += new System.ComponentModel.CancelEventHandler(this.mark2TextBox_Validating);
      // 
      // mark2Label
      // 
      this.mark2Label.AutoSize = true;
      this.mark2Label.Location = new System.Drawing.Point(12, 35);
      this.mark2Label.Name = "mark2Label";
      this.mark2Label.Size = new System.Drawing.Size(86, 12);
      this.mark2Label.TabIndex = 59;
      this.mark2Label.Text = "Mark2(0～255) :";
      // 
      // mark4TextBox
      // 
      this.mark4TextBox.Enabled = false;
      this.mark4TextBox.Location = new System.Drawing.Point(104, 83);
      this.mark4TextBox.Name = "mark4TextBox";
      this.mark4TextBox.Size = new System.Drawing.Size(44, 19);
      this.mark4TextBox.TabIndex = 64;
      this.mark4TextBox.Text = "0";
      this.mark4TextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.mark4TextBox_KeyDown);
      this.mark4TextBox.Validating += new System.ComponentModel.CancelEventHandler(this.mark4TextBox_Validating);
      // 
      // mark4Label
      // 
      this.mark4Label.AutoSize = true;
      this.mark4Label.Location = new System.Drawing.Point(12, 86);
      this.mark4Label.Name = "mark4Label";
      this.mark4Label.Size = new System.Drawing.Size(86, 12);
      this.mark4Label.TabIndex = 63;
      this.mark4Label.Text = "Mark4(0～255) :";
      // 
      // mark3TextBox
      // 
      this.mark3TextBox.Enabled = false;
      this.mark3TextBox.Location = new System.Drawing.Point(104, 57);
      this.mark3TextBox.Name = "mark3TextBox";
      this.mark3TextBox.Size = new System.Drawing.Size(44, 19);
      this.mark3TextBox.TabIndex = 62;
      this.mark3TextBox.Text = "0";
      this.mark3TextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.mark3TextBox_KeyDown);
      this.mark3TextBox.Validating += new System.ComponentModel.CancelEventHandler(this.mark3TextBox_Validating);
      // 
      // mark3Label
      // 
      this.mark3Label.AutoSize = true;
      this.mark3Label.Location = new System.Drawing.Point(12, 60);
      this.mark3Label.Name = "mark3Label";
      this.mark3Label.Size = new System.Drawing.Size(86, 12);
      this.mark3Label.TabIndex = 61;
      this.mark3Label.Text = "Mark3(0～255) :";
      // 
      // setMarkingsButton
      // 
      this.setMarkingsButton.Enabled = false;
      this.setMarkingsButton.Location = new System.Drawing.Point(49, 108);
      this.setMarkingsButton.Name = "setMarkingsButton";
      this.setMarkingsButton.Size = new System.Drawing.Size(72, 19);
      this.setMarkingsButton.TabIndex = 65;
      this.setMarkingsButton.Text = "設定";
      this.setMarkingsButton.UseVisualStyleBackColor = true;
      this.setMarkingsButton.Click += new System.EventHandler(this.setMarkingsButton_Click);
      // 
      // MarkingControlForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(179, 146);
      this.ControlBox = false;
      this.Controls.Add(this.setMarkingsButton);
      this.Controls.Add(this.mark4TextBox);
      this.Controls.Add(this.mark4Label);
      this.Controls.Add(this.mark3TextBox);
      this.Controls.Add(this.mark3Label);
      this.Controls.Add(this.mark2TextBox);
      this.Controls.Add(this.mark2Label);
      this.Controls.Add(this.mark1TextBox);
      this.Controls.Add(this.mark1Label);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
      this.Name = "MarkingControlForm";
      this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
      this.Text = "模様変更";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Label mark1Label;
    private System.Windows.Forms.TextBox mark1TextBox;
    private System.Windows.Forms.TextBox mark2TextBox;
    private System.Windows.Forms.Label mark2Label;
    private System.Windows.Forms.TextBox mark4TextBox;
    private System.Windows.Forms.Label mark4Label;
    private System.Windows.Forms.TextBox mark3TextBox;
    private System.Windows.Forms.Label mark3Label;
    private System.Windows.Forms.Button setMarkingsButton;
  }
}