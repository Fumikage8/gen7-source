namespace ModelViewer
{
  partial class McnfForm
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
      this.interestGridView = new System.Windows.Forms.DataGridView();
      this.interestJointName = new System.Windows.Forms.DataGridViewComboBoxColumn();
      this.interestType = new System.Windows.Forms.DataGridViewComboBoxColumn();
      this.interestLow1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.interestHigh1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.interestLow2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.interestHigh2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.interestSpeed = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.mcnfTabControl = new System.Windows.Forms.TabControl();
      this.interestPage = new System.Windows.Forms.TabPage();
      this.springPage = new System.Windows.Forms.TabPage();
      this.springGridView = new System.Windows.Forms.DataGridView();
      this.springJointName = new System.Windows.Forms.DataGridViewComboBoxColumn();
      this.springDamping = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.springStiffness = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.springMass = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.springGravity = new System.Windows.Forms.DataGridViewTextBoxColumn();
      this.expressionPage = new System.Windows.Forms.TabPage();
      this.expressionGridView = new System.Windows.Forms.DataGridView();
      this.expressionType = new System.Windows.Forms.DataGridViewComboBoxColumn();
      this.expressionSrc = new System.Windows.Forms.DataGridViewComboBoxColumn();
      this.expressionDst = new System.Windows.Forms.DataGridViewComboBoxColumn();
      this.mcnfSaveButton = new System.Windows.Forms.Button();
      this.mcnfComboBox = new System.Windows.Forms.ComboBox();
      this.mcnfSaveFileDialog = new System.Windows.Forms.SaveFileDialog();
      this.mcnfCreate = new System.Windows.Forms.Button();
      this.checkBoxSpring = new System.Windows.Forms.CheckBox();
      ((System.ComponentModel.ISupportInitialize)(this.interestGridView)).BeginInit();
      this.mcnfTabControl.SuspendLayout();
      this.interestPage.SuspendLayout();
      this.springPage.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.springGridView)).BeginInit();
      this.expressionPage.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.expressionGridView)).BeginInit();
      this.SuspendLayout();
      // 
      // interestGridView
      // 
      this.interestGridView.AllowUserToAddRows = false;
      this.interestGridView.AllowUserToDeleteRows = false;
      this.interestGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
      this.interestGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.interestJointName,
            this.interestType,
            this.interestLow1,
            this.interestHigh1,
            this.interestLow2,
            this.interestHigh2,
            this.interestSpeed});
      this.interestGridView.Dock = System.Windows.Forms.DockStyle.Fill;
      this.interestGridView.Location = new System.Drawing.Point(3, 3);
      this.interestGridView.Name = "interestGridView";
      this.interestGridView.RowTemplate.Height = 21;
      this.interestGridView.Size = new System.Drawing.Size(373, 152);
      this.interestGridView.TabIndex = 1;
      this.interestGridView.CellEndEdit += new System.Windows.Forms.DataGridViewCellEventHandler(this.interestGridView_CellEndEdit);
      this.interestGridView.CellEnter += new System.Windows.Forms.DataGridViewCellEventHandler(this.interestGridView_CellEnter);
      this.interestGridView.CellValidating += new System.Windows.Forms.DataGridViewCellValidatingEventHandler(this.interestGridView_CellValidating);
      this.interestGridView.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.interestGridView_CellValueChanged);
      this.interestGridView.CurrentCellDirtyStateChanged += new System.EventHandler(this.interestGridView_CurrentCellDirtyStateChanged);
      this.interestGridView.SelectionChanged += new System.EventHandler(this.interestGridView_SelectionChanged);
      // 
      // interestJointName
      // 
      this.interestJointName.HeaderText = "ジョイント名";
      this.interestJointName.Name = "interestJointName";
      this.interestJointName.Resizable = System.Windows.Forms.DataGridViewTriState.True;
      this.interestJointName.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.Automatic;
      // 
      // interestType
      // 
      this.interestType.HeaderText = "Type";
      this.interestType.Name = "interestType";
      this.interestType.Resizable = System.Windows.Forms.DataGridViewTriState.True;
      this.interestType.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.Automatic;
      this.interestType.Width = 50;
      // 
      // interestLow1
      // 
      this.interestLow1.HeaderText = "Low";
      this.interestLow1.Name = "interestLow1";
      this.interestLow1.Width = 50;
      // 
      // interestHigh1
      // 
      this.interestHigh1.HeaderText = "High";
      this.interestHigh1.Name = "interestHigh1";
      this.interestHigh1.Width = 50;
      // 
      // interestLow2
      // 
      this.interestLow2.HeaderText = "Low2";
      this.interestLow2.Name = "interestLow2";
      this.interestLow2.Width = 50;
      // 
      // interestHigh2
      // 
      this.interestHigh2.HeaderText = "High2";
      this.interestHigh2.Name = "interestHigh2";
      this.interestHigh2.Width = 50;
      // 
      // interestSpeed
      // 
      this.interestSpeed.HeaderText = "Speed";
      this.interestSpeed.Name = "interestSpeed";
      this.interestSpeed.Width = 50;
      // 
      // mcnfTabControl
      // 
      this.mcnfTabControl.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
      this.mcnfTabControl.Controls.Add(this.interestPage);
      this.mcnfTabControl.Controls.Add(this.springPage);
      this.mcnfTabControl.Controls.Add(this.expressionPage);
      this.mcnfTabControl.Location = new System.Drawing.Point(12, 55);
      this.mcnfTabControl.Name = "mcnfTabControl";
      this.mcnfTabControl.SelectedIndex = 0;
      this.mcnfTabControl.Size = new System.Drawing.Size(387, 184);
      this.mcnfTabControl.TabIndex = 2;
      this.mcnfTabControl.Enter += new System.EventHandler(this.mcnfTabControl_Enter);
      // 
      // interestPage
      // 
      this.interestPage.Controls.Add(this.interestGridView);
      this.interestPage.Location = new System.Drawing.Point(4, 22);
      this.interestPage.Name = "interestPage";
      this.interestPage.Padding = new System.Windows.Forms.Padding(3);
      this.interestPage.Size = new System.Drawing.Size(379, 158);
      this.interestPage.TabIndex = 0;
      this.interestPage.Text = "注視";
      this.interestPage.UseVisualStyleBackColor = true;
      // 
      // springPage
      // 
      this.springPage.Controls.Add(this.springGridView);
      this.springPage.Location = new System.Drawing.Point(4, 22);
      this.springPage.Name = "springPage";
      this.springPage.Padding = new System.Windows.Forms.Padding(3);
      this.springPage.Size = new System.Drawing.Size(379, 158);
      this.springPage.TabIndex = 1;
      this.springPage.Text = "バネ";
      this.springPage.UseVisualStyleBackColor = true;
      // 
      // springGridView
      // 
      this.springGridView.AllowUserToAddRows = false;
      this.springGridView.AllowUserToDeleteRows = false;
      this.springGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
      this.springGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.springJointName,
            this.springDamping,
            this.springStiffness,
            this.springMass,
            this.springGravity});
      this.springGridView.Dock = System.Windows.Forms.DockStyle.Fill;
      this.springGridView.Location = new System.Drawing.Point(3, 3);
      this.springGridView.Name = "springGridView";
      this.springGridView.RowTemplate.Height = 21;
      this.springGridView.Size = new System.Drawing.Size(373, 152);
      this.springGridView.TabIndex = 0;
      this.springGridView.CellValidating += new System.Windows.Forms.DataGridViewCellValidatingEventHandler(this.springGridView_CellValidating);
      // 
      // springJointName
      // 
      this.springJointName.HeaderText = "ジョイント名";
      this.springJointName.Name = "springJointName";
      this.springJointName.Resizable = System.Windows.Forms.DataGridViewTriState.True;
      this.springJointName.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.Automatic;
      // 
      // springDamping
      // 
      this.springDamping.HeaderText = "減衰";
      this.springDamping.Name = "springDamping";
      this.springDamping.Width = 60;
      // 
      // springStiffness
      // 
      this.springStiffness.HeaderText = "バネ定数";
      this.springStiffness.Name = "springStiffness";
      this.springStiffness.Width = 60;
      // 
      // springMass
      // 
      this.springMass.HeaderText = "質量";
      this.springMass.Name = "springMass";
      this.springMass.Width = 60;
      // 
      // springGravity
      // 
      this.springGravity.HeaderText = "重力";
      this.springGravity.Name = "springGravity";
      this.springGravity.Width = 60;
      // 
      // expressionPage
      // 
      this.expressionPage.Controls.Add(this.expressionGridView);
      this.expressionPage.Location = new System.Drawing.Point(4, 22);
      this.expressionPage.Name = "expressionPage";
      this.expressionPage.Padding = new System.Windows.Forms.Padding(3);
      this.expressionPage.Size = new System.Drawing.Size(379, 158);
      this.expressionPage.TabIndex = 2;
      this.expressionPage.Text = "エクスプレッション";
      this.expressionPage.UseVisualStyleBackColor = true;
      // 
      // expressionGridView
      // 
      this.expressionGridView.AllowUserToAddRows = false;
      this.expressionGridView.AllowUserToDeleteRows = false;
      this.expressionGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
      this.expressionGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.expressionType,
            this.expressionSrc,
            this.expressionDst});
      this.expressionGridView.Dock = System.Windows.Forms.DockStyle.Fill;
      this.expressionGridView.Location = new System.Drawing.Point(3, 3);
      this.expressionGridView.Name = "expressionGridView";
      this.expressionGridView.RowTemplate.Height = 21;
      this.expressionGridView.Size = new System.Drawing.Size(373, 152);
      this.expressionGridView.TabIndex = 0;
      this.expressionGridView.CellValidating += new System.Windows.Forms.DataGridViewCellValidatingEventHandler(this.expressionGridView_CellValidating);
      // 
      // expressionType
      // 
      this.expressionType.HeaderText = "タイプ";
      this.expressionType.Name = "expressionType";
      // 
      // expressionSrc
      // 
      this.expressionSrc.HeaderText = "SRC";
      this.expressionSrc.Name = "expressionSrc";
      // 
      // expressionDst
      // 
      this.expressionDst.HeaderText = "DST";
      this.expressionDst.Name = "expressionDst";
      // 
      // mcnfSaveButton
      // 
      this.mcnfSaveButton.Location = new System.Drawing.Point(191, 10);
      this.mcnfSaveButton.Name = "mcnfSaveButton";
      this.mcnfSaveButton.Size = new System.Drawing.Size(75, 23);
      this.mcnfSaveButton.TabIndex = 2;
      this.mcnfSaveButton.Text = "mcnf保存";
      this.mcnfSaveButton.UseVisualStyleBackColor = true;
      this.mcnfSaveButton.Click += new System.EventHandler(this.mcnfSaveButton_Click);
      // 
      // mcnfComboBox
      // 
      this.mcnfComboBox.FormattingEnabled = true;
      this.mcnfComboBox.Location = new System.Drawing.Point(12, 12);
      this.mcnfComboBox.Name = "mcnfComboBox";
      this.mcnfComboBox.Size = new System.Drawing.Size(173, 20);
      this.mcnfComboBox.TabIndex = 3;
      this.mcnfComboBox.SelectedIndexChanged += new System.EventHandler(this.mcnfComboBox_SelectedIndexChanged);
      // 
      // mcnfSaveFileDialog
      // 
      this.mcnfSaveFileDialog.DefaultExt = "mcnf";
      this.mcnfSaveFileDialog.Filter = "モーションコンフィグ|*.mcnf";
      // 
      // mcnfCreate
      // 
      this.mcnfCreate.Location = new System.Drawing.Point(316, 10);
      this.mcnfCreate.Name = "mcnfCreate";
      this.mcnfCreate.Size = new System.Drawing.Size(75, 23);
      this.mcnfCreate.TabIndex = 4;
      this.mcnfCreate.Text = "自動生成";
      this.mcnfCreate.UseVisualStyleBackColor = true;
      this.mcnfCreate.Click += new System.EventHandler(this.mcnfCreate_Click);
      // 
      // checkBoxSpring
      // 
      this.checkBoxSpring.AutoSize = true;
      this.checkBoxSpring.Location = new System.Drawing.Point(323, 39);
      this.checkBoxSpring.Name = "checkBoxSpring";
      this.checkBoxSpring.Size = new System.Drawing.Size(68, 16);
      this.checkBoxSpring.TabIndex = 5;
      this.checkBoxSpring.Text = "バネ有効";
      this.checkBoxSpring.UseVisualStyleBackColor = true;
      this.checkBoxSpring.CheckedChanged += new System.EventHandler(this.checkBoxSpring_CheckedChanged);
      // 
      // McnfForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(411, 251);
      this.ControlBox = false;
      this.Controls.Add(this.checkBoxSpring);
      this.Controls.Add(this.mcnfCreate);
      this.Controls.Add(this.mcnfComboBox);
      this.Controls.Add(this.mcnfTabControl);
      this.Controls.Add(this.mcnfSaveButton);
      this.Name = "McnfForm";
      this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
      this.Text = "モーションコンフィグ";
      ((System.ComponentModel.ISupportInitialize)(this.interestGridView)).EndInit();
      this.mcnfTabControl.ResumeLayout(false);
      this.interestPage.ResumeLayout(false);
      this.springPage.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.springGridView)).EndInit();
      this.expressionPage.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)(this.expressionGridView)).EndInit();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.DataGridView interestGridView;
    private System.Windows.Forms.TabControl mcnfTabControl;
    private System.Windows.Forms.TabPage interestPage;
    private System.Windows.Forms.TabPage springPage;
    private System.Windows.Forms.DataGridView springGridView;
    private System.Windows.Forms.DataGridViewComboBoxColumn springJointName;
    private System.Windows.Forms.DataGridViewTextBoxColumn springDamping;
    private System.Windows.Forms.DataGridViewTextBoxColumn springStiffness;
    private System.Windows.Forms.DataGridViewTextBoxColumn springMass;
    private System.Windows.Forms.DataGridViewTextBoxColumn springGravity;
    private System.Windows.Forms.Button mcnfSaveButton;
    private System.Windows.Forms.ComboBox mcnfComboBox;
    private System.Windows.Forms.SaveFileDialog mcnfSaveFileDialog;
    private System.Windows.Forms.DataGridViewComboBoxColumn interestJointName;
    private System.Windows.Forms.DataGridViewComboBoxColumn interestType;
    private System.Windows.Forms.DataGridViewTextBoxColumn interestLow1;
    private System.Windows.Forms.DataGridViewTextBoxColumn interestHigh1;
    private System.Windows.Forms.DataGridViewTextBoxColumn interestLow2;
    private System.Windows.Forms.DataGridViewTextBoxColumn interestHigh2;
    private System.Windows.Forms.DataGridViewTextBoxColumn interestSpeed;
    private System.Windows.Forms.TabPage expressionPage;
    private System.Windows.Forms.DataGridView expressionGridView;
    private System.Windows.Forms.DataGridViewComboBoxColumn expressionType;
    private System.Windows.Forms.DataGridViewComboBoxColumn expressionSrc;
    private System.Windows.Forms.DataGridViewComboBoxColumn expressionDst;
    private System.Windows.Forms.Button mcnfCreate;
    private System.Windows.Forms.CheckBox checkBoxSpring;
  }
}