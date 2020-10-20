namespace ModelViewer
{
  partial class PokedecoLocatorControlForm
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
      this.changeAllVisibilityCheckBox = new System.Windows.Forms.CheckBox();
      this.pokedecoLocatorGridView = new System.Windows.Forms.DataGridView();
      this.PokedecoLocatorName = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.PokedecoBoundingBoxName = new System.Windows.Forms.DataGridViewComboBoxColumn();
      this.PokedecoLocatorDisplay = new System.Windows.Forms.DataGridViewCheckBoxColumn();
      ((System.ComponentModel.ISupportInitialize)(this.pokedecoLocatorGridView)).BeginInit();
      this.SuspendLayout();
      // 
      // changeAllVisibilityCheckBox
      // 
      this.changeAllVisibilityCheckBox.AutoSize = true;
      this.changeAllVisibilityCheckBox.Enabled = false;
      this.changeAllVisibilityCheckBox.Location = new System.Drawing.Point(12, 12);
      this.changeAllVisibilityCheckBox.Name = "changeAllVisibilityCheckBox";
      this.changeAllVisibilityCheckBox.Size = new System.Drawing.Size(96, 16);
      this.changeAllVisibilityCheckBox.TabIndex = 2;
      this.changeAllVisibilityCheckBox.Text = "全ロケータ表示";
      this.changeAllVisibilityCheckBox.UseVisualStyleBackColor = true;
      this.changeAllVisibilityCheckBox.CheckedChanged += new System.EventHandler(this.changeAllVisibilityCheckBox_CheckedChanged);
      // 
      // pokedecoLocatorGridView
      // 
      this.pokedecoLocatorGridView.AllowUserToAddRows = false;
      this.pokedecoLocatorGridView.AllowUserToDeleteRows = false;
      this.pokedecoLocatorGridView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.pokedecoLocatorGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
      this.pokedecoLocatorGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.PokedecoLocatorName,
            this.PokedecoBoundingBoxName,
            this.PokedecoLocatorDisplay});
      this.pokedecoLocatorGridView.Location = new System.Drawing.Point(12, 34);
      this.pokedecoLocatorGridView.Name = "pokedecoLocatorGridView";
      this.pokedecoLocatorGridView.RowTemplate.Height = 21;
      this.pokedecoLocatorGridView.Size = new System.Drawing.Size(408, 227);
      this.pokedecoLocatorGridView.TabIndex = 3;
      this.pokedecoLocatorGridView.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.pokedecoLocatorGridView_CellValueChanged);
      this.pokedecoLocatorGridView.CurrentCellDirtyStateChanged += new System.EventHandler(this.pokedecoLocatorGridView_CurrentCellDirtyStateChanged);
      // 
      // PokedecoLocatorName
      // 
      this.PokedecoLocatorName.HeaderText = "ロケータ名";
      this.PokedecoLocatorName.Name = "PokedecoLocatorName";
      this.PokedecoLocatorName.ReadOnly = true;
      this.PokedecoLocatorName.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
      this.PokedecoLocatorName.Width = 150;
      // 
      // PokedecoBoundingBoxName
      // 
      this.PokedecoBoundingBoxName.HeaderText = "バウンディングボックス";
      this.PokedecoBoundingBoxName.Items.AddRange(new object[] {
            "none",
            "bb_hat",
            "bb_ribbon",
            "bb_glasses",
            "bb_badge",
            "all_head_parts",
            "all_chest_parts",
            "all_face_parts"});
      this.PokedecoBoundingBoxName.Name = "PokedecoBoundingBoxName";
      this.PokedecoBoundingBoxName.Width = 120;
      // 
      // PokedecoLocatorDisplay
      // 
      this.PokedecoLocatorDisplay.HeaderText = "表示";
      this.PokedecoLocatorDisplay.Name = "PokedecoLocatorDisplay";
      this.PokedecoLocatorDisplay.Width = 50;
      // 
      // PokedecoLocatorControlForm
      // 
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
      this.ClientSize = new System.Drawing.Size(432, 273);
      this.ControlBox = false;
      this.Controls.Add(this.pokedecoLocatorGridView);
      this.Controls.Add(this.changeAllVisibilityCheckBox);
      this.Name = "PokedecoLocatorControlForm";
      this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
      this.Text = "ポケデコロケータ表示切替";
      this.Load += new System.EventHandler(this.PokedecoLocatorControlForm_Load);
      ((System.ComponentModel.ISupportInitialize)(this.pokedecoLocatorGridView)).EndInit();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.CheckBox changeAllVisibilityCheckBox;
    private System.Windows.Forms.DataGridView pokedecoLocatorGridView;
    private System.Windows.Forms.DataGridViewTextBoxColumn PokedecoLocatorName;
    private System.Windows.Forms.DataGridViewComboBoxColumn PokedecoBoundingBoxName;
    private System.Windows.Forms.DataGridViewCheckBoxColumn PokedecoLocatorDisplay;
  }
}