namespace ModelViewer
{
  partial class CheckMaterialAttributeForm
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
      this.selectMaterialTreeView = new System.Windows.Forms.TreeView();
      this.materialAttributeDataGridView = new System.Windows.Forms.DataGridView();
      this.MaterialAttributeKindColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.MaterialAttributeColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
      ((System.ComponentModel.ISupportInitialize)(this.materialAttributeDataGridView)).BeginInit();
      this.SuspendLayout();
      // 
      // selectMaterialTreeView
      // 
      this.selectMaterialTreeView.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
      this.selectMaterialTreeView.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.selectMaterialTreeView.Location = new System.Drawing.Point(12, 12);
      this.selectMaterialTreeView.Name = "selectMaterialTreeView";
      this.selectMaterialTreeView.Size = new System.Drawing.Size(132, 242);
      this.selectMaterialTreeView.TabIndex = 0;
      this.selectMaterialTreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.selectMaterialTreeView_AfterSelect);
      // 
      // materialAttributeDataGridView
      // 
      this.materialAttributeDataGridView.AllowUserToAddRows = false;
      this.materialAttributeDataGridView.AllowUserToDeleteRows = false;
      this.materialAttributeDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.materialAttributeDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
      this.materialAttributeDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.MaterialAttributeKindColumn,
            this.MaterialAttributeColumn});
      this.materialAttributeDataGridView.Location = new System.Drawing.Point(160, 12);
      this.materialAttributeDataGridView.Name = "materialAttributeDataGridView";
      this.materialAttributeDataGridView.RowTemplate.Height = 21;
      this.materialAttributeDataGridView.Size = new System.Drawing.Size(343, 242);
      this.materialAttributeDataGridView.TabIndex = 1;
      // 
      // MaterialAttributeKindColumn
      // 
      this.MaterialAttributeKindColumn.HeaderText = "アトリビュート";
      this.MaterialAttributeKindColumn.Name = "MaterialAttributeKindColumn";
      this.MaterialAttributeKindColumn.ReadOnly = true;
      // 
      // MaterialAttributeColumn
      // 
      this.MaterialAttributeColumn.HeaderText = "設定値";
      this.MaterialAttributeColumn.Name = "MaterialAttributeColumn";
      this.MaterialAttributeColumn.ReadOnly = true;
      this.MaterialAttributeColumn.Width = 200;
      // 
      // CheckMaterialAttributeForm
      // 
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
      this.ClientSize = new System.Drawing.Size(515, 266);
      this.ControlBox = false;
      this.Controls.Add(this.materialAttributeDataGridView);
      this.Controls.Add(this.selectMaterialTreeView);
      this.Name = "CheckMaterialAttributeForm";
      this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
      this.Text = "マテリアル情報確認";
      ((System.ComponentModel.ISupportInitialize)(this.materialAttributeDataGridView)).EndInit();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.TreeView selectMaterialTreeView;
    private System.Windows.Forms.DataGridView materialAttributeDataGridView;
    private System.Windows.Forms.DataGridViewTextBoxColumn MaterialAttributeKindColumn;
    private System.Windows.Forms.DataGridViewTextBoxColumn MaterialAttributeColumn;
  }
}