namespace ModelViewer
{
  partial class MotionCompForm
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
      System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
      this.saveFileDialog = new System.Windows.Forms.SaveFileDialog();
      this.dataGridView1 = new System.Windows.Forms.DataGridView();
      this.motionName = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.fileSize = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.compLevel = new System.Windows.Forms.DataGridViewComboBoxColumn();
      this.totalSizeLabel = new System.Windows.Forms.Label();
      this.nameComboBox = new System.Windows.Forms.ComboBox();
      this.saveButton = new System.Windows.Forms.Button();
      ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
      this.SuspendLayout();
      // 
      // saveFileDialog
      // 
      this.saveFileDialog.DefaultExt = "xml";
      this.saveFileDialog.FileName = "EachDae.xml";
      this.saveFileDialog.Filter = "圧縮レベルリスト|EachDae.xml";
      // 
      // dataGridView1
      // 
      this.dataGridView1.AllowUserToAddRows = false;
      this.dataGridView1.AllowUserToDeleteRows = false;
      this.dataGridView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
      this.dataGridView1.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.motionName,
            this.fileSize,
            this.compLevel});
      this.dataGridView1.Location = new System.Drawing.Point(12, 38);
      this.dataGridView1.Name = "dataGridView1";
      this.dataGridView1.RowHeadersVisible = false;
      this.dataGridView1.RowTemplate.Height = 21;
      this.dataGridView1.Size = new System.Drawing.Size(386, 201);
      this.dataGridView1.TabIndex = 0;
      this.dataGridView1.CellEnter += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridView1_CellEnter);
      this.dataGridView1.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridView1_CellValueChanged);
      this.dataGridView1.CurrentCellDirtyStateChanged += new System.EventHandler(this.dataGridView1_CurrentCellDirtyStateChanged);
      // 
      // motionName
      // 
      this.motionName.HeaderText = "モーション名";
      this.motionName.Name = "motionName";
      this.motionName.ReadOnly = true;
      this.motionName.Width = 200;
      // 
      // fileSize
      // 
      dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleRight;
      this.fileSize.DefaultCellStyle = dataGridViewCellStyle1;
      this.fileSize.HeaderText = "ファイルサイズ";
      this.fileSize.Name = "fileSize";
      this.fileSize.ReadOnly = true;
      this.fileSize.Width = 70;
      // 
      // compLevel
      // 
      this.compLevel.HeaderText = "圧縮レベル";
      this.compLevel.Items.AddRange(new object[] {
            "NO_ERROR",
            "PRECISE",
            "DEFAULT",
            "ABOUT",
            "MOST_ABOUT"});
      this.compLevel.Name = "compLevel";
      this.compLevel.Width = 110;
      // 
      // totalSizeLabel
      // 
      this.totalSizeLabel.AutoSize = true;
      this.totalSizeLabel.Location = new System.Drawing.Point(272, 15);
      this.totalSizeLabel.Name = "totalSizeLabel";
      this.totalSizeLabel.Size = new System.Drawing.Size(35, 12);
      this.totalSizeLabel.TabIndex = 1;
      this.totalSizeLabel.Text = "合計：";
      // 
      // nameComboBox
      // 
      this.nameComboBox.DropDownWidth = 173;
      this.nameComboBox.FormattingEnabled = true;
      this.nameComboBox.Location = new System.Drawing.Point(12, 12);
      this.nameComboBox.Name = "nameComboBox";
      this.nameComboBox.Size = new System.Drawing.Size(173, 20);
      this.nameComboBox.TabIndex = 2;
      // 
      // saveButton
      // 
      this.saveButton.Location = new System.Drawing.Point(191, 10);
      this.saveButton.Name = "saveButton";
      this.saveButton.Size = new System.Drawing.Size(75, 23);
      this.saveButton.TabIndex = 3;
      this.saveButton.Text = "リスト保存";
      this.saveButton.UseVisualStyleBackColor = true;
      this.saveButton.Click += new System.EventHandler(this.saveButton_Click);
      // 
      // MotionCompForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(411, 251);
      this.ControlBox = false;
      this.Controls.Add(this.saveButton);
      this.Controls.Add(this.nameComboBox);
      this.Controls.Add(this.totalSizeLabel);
      this.Controls.Add(this.dataGridView1);
      this.Name = "MotionCompForm";
      this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
      this.Text = "モーション圧縮設定";
      ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.SaveFileDialog saveFileDialog;
    private System.Windows.Forms.DataGridView dataGridView1;
    private System.Windows.Forms.DataGridViewTextBoxColumn motionName;
    private System.Windows.Forms.DataGridViewTextBoxColumn fileSize;
    private System.Windows.Forms.DataGridViewComboBoxColumn compLevel;
    private System.Windows.Forms.Label totalSizeLabel;
    private System.Windows.Forms.ComboBox nameComboBox;
    private System.Windows.Forms.Button saveButton;
  }
}