namespace ModelViewer
{
  partial class BlendListForm
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
      this.blendListSaveButton = new System.Windows.Forms.Button();
      this.blendListComboBox = new System.Windows.Forms.ComboBox();
      this.blendListSaveFileDialog = new System.Windows.Forms.SaveFileDialog();
      this.blendListDataGridView = new System.Windows.Forms.DataGridView();
      this.ColumnA = new System.Windows.Forms.DataGridViewTextBoxColumn();
      ((System.ComponentModel.ISupportInitialize)(this.blendListDataGridView)).BeginInit();
      this.SuspendLayout();
      // 
      // blendListSaveButton
      // 
      this.blendListSaveButton.Location = new System.Drawing.Point(191, 10);
      this.blendListSaveButton.Name = "blendListSaveButton";
      this.blendListSaveButton.Size = new System.Drawing.Size(84, 23);
      this.blendListSaveButton.TabIndex = 2;
      this.blendListSaveButton.Text = "blendlist保存";
      this.blendListSaveButton.UseVisualStyleBackColor = true;
      this.blendListSaveButton.Click += new System.EventHandler(this.blendListSaveButton_Click);
      // 
      // blendListComboBox
      // 
      this.blendListComboBox.FormattingEnabled = true;
      this.blendListComboBox.Location = new System.Drawing.Point(12, 12);
      this.blendListComboBox.Name = "blendListComboBox";
      this.blendListComboBox.Size = new System.Drawing.Size(173, 20);
      this.blendListComboBox.TabIndex = 3;
      // 
      // blendListSaveFileDialog
      // 
      this.blendListSaveFileDialog.DefaultExt = "xml";
      this.blendListSaveFileDialog.FileName = "blend_list.xml";
      this.blendListSaveFileDialog.Filter = "ブレンドリスト|blend_list.xml";
      // 
      // blendListDataGridView
      // 
      this.blendListDataGridView.AllowUserToOrderColumns = true;
      this.blendListDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.blendListDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
      this.blendListDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.ColumnA});
      this.blendListDataGridView.Location = new System.Drawing.Point(12, 39);
      this.blendListDataGridView.Name = "blendListDataGridView";
      this.blendListDataGridView.RowTemplate.Height = 21;
      this.blendListDataGridView.Size = new System.Drawing.Size(387, 200);
      this.blendListDataGridView.TabIndex = 4;
      this.blendListDataGridView.CellEndEdit += new System.Windows.Forms.DataGridViewCellEventHandler(this.blendListDataGridView_CellEndEdit);
      this.blendListDataGridView.CellPainting += new System.Windows.Forms.DataGridViewCellPaintingEventHandler(this.blendListDataGridView_CellPainting);
      this.blendListDataGridView.CellStateChanged += new System.Windows.Forms.DataGridViewCellStateChangedEventHandler(this.blendListDataGridView_CellStateChanged);
      this.blendListDataGridView.CellValidating += new System.Windows.Forms.DataGridViewCellValidatingEventHandler(this.blendListDataGridView_CellValidating);
      this.blendListDataGridView.SelectionChanged += new System.EventHandler(this.blendListDataGridView_SelectionChanged);
      this.blendListDataGridView.KeyDown += new System.Windows.Forms.KeyEventHandler(this.blendListDataGridView_KeyDown);
      // 
      // ColumnA
      // 
      this.ColumnA.HeaderText = "A";
      this.ColumnA.Name = "ColumnA";
      // 
      // BlendListForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(411, 251);
      this.ControlBox = false;
      this.Controls.Add(this.blendListDataGridView);
      this.Controls.Add(this.blendListComboBox);
      this.Controls.Add(this.blendListSaveButton);
      this.Name = "BlendListForm";
      this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
      this.Text = "ブレンドリスト";
      ((System.ComponentModel.ISupportInitialize)(this.blendListDataGridView)).EndInit();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.Button blendListSaveButton;
    private System.Windows.Forms.ComboBox blendListComboBox;
    private System.Windows.Forms.SaveFileDialog blendListSaveFileDialog;
    private System.Windows.Forms.DataGridView blendListDataGridView;
    private System.Windows.Forms.DataGridViewTextBoxColumn ColumnA;
  }
}