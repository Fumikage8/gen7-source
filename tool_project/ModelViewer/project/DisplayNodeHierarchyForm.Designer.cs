namespace ModelViewer
{
  partial class DisplayNodeHierarchyForm
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
      this.nodeHierarchyTreeView = new System.Windows.Forms.TreeView();
      this.nodeColorDataGridView = new System.Windows.Forms.DataGridView();
      this.NodeNameColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.NodeColorColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
      ((System.ComponentModel.ISupportInitialize)(this.nodeColorDataGridView)).BeginInit();
      this.SuspendLayout();
      // 
      // nodeHierarchyTreeView
      // 
      this.nodeHierarchyTreeView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.nodeHierarchyTreeView.Location = new System.Drawing.Point(13, 115);
      this.nodeHierarchyTreeView.Name = "nodeHierarchyTreeView";
      this.nodeHierarchyTreeView.Size = new System.Drawing.Size(267, 325);
      this.nodeHierarchyTreeView.TabIndex = 0;
      // 
      // nodeColorDataGridView
      // 
      this.nodeColorDataGridView.AllowUserToAddRows = false;
      this.nodeColorDataGridView.AllowUserToDeleteRows = false;
      this.nodeColorDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
      this.nodeColorDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.NodeNameColumn,
            this.NodeColorColumn});
      this.nodeColorDataGridView.Location = new System.Drawing.Point(27, 12);
      this.nodeColorDataGridView.Name = "nodeColorDataGridView";
      this.nodeColorDataGridView.RowTemplate.Height = 21;
      this.nodeColorDataGridView.Size = new System.Drawing.Size(243, 96);
      this.nodeColorDataGridView.TabIndex = 1;
      // 
      // NodeNameColumn
      // 
      this.NodeNameColumn.HeaderText = "ノードの種類";
      this.NodeNameColumn.Name = "NodeNameColumn";
      this.NodeNameColumn.ReadOnly = true;
      // 
      // NodeColorColumn
      // 
      this.NodeColorColumn.HeaderText = "ノードの色";
      this.NodeColorColumn.Name = "NodeColorColumn";
      this.NodeColorColumn.ReadOnly = true;
      // 
      // DisplayNodeHierarchyForm
      // 
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
      this.ClientSize = new System.Drawing.Size(292, 452);
      this.ControlBox = false;
      this.Controls.Add(this.nodeColorDataGridView);
      this.Controls.Add(this.nodeHierarchyTreeView);
      this.Name = "DisplayNodeHierarchyForm";
      this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
      this.Text = "ノード階層";
      ((System.ComponentModel.ISupportInitialize)(this.nodeColorDataGridView)).EndInit();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.TreeView nodeHierarchyTreeView;
    private System.Windows.Forms.DataGridView nodeColorDataGridView;
    private System.Windows.Forms.DataGridViewTextBoxColumn NodeNameColumn;
    private System.Windows.Forms.DataGridViewTextBoxColumn NodeColorColumn;
  }
}