namespace ModelViewer
{
	partial class EffectLocatorControlForm
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
			this.effectLocatorGridView = new System.Windows.Forms.DataGridView();
			this.EffectLocatorName = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.EffectLocatorDisplay = new System.Windows.Forms.DataGridViewCheckBoxColumn();
			this.changeAllVisibilityCheckBox = new System.Windows.Forms.CheckBox();
			((System.ComponentModel.ISupportInitialize)(this.effectLocatorGridView)).BeginInit();
			this.SuspendLayout();
			// 
			// effectLocatorGridView
			// 
			this.effectLocatorGridView.AllowUserToAddRows = false;
			this.effectLocatorGridView.AllowUserToDeleteRows = false;
			this.effectLocatorGridView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.effectLocatorGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.effectLocatorGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.EffectLocatorName,
            this.EffectLocatorDisplay});
			this.effectLocatorGridView.Location = new System.Drawing.Point(12, 34);
			this.effectLocatorGridView.Name = "effectLocatorGridView";
			this.effectLocatorGridView.RowTemplate.Height = 21;
			this.effectLocatorGridView.Size = new System.Drawing.Size(244, 188);
			this.effectLocatorGridView.TabIndex = 0;
			this.effectLocatorGridView.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.effectLocatorGridView_CellValueChanged);
			this.effectLocatorGridView.CurrentCellDirtyStateChanged += new System.EventHandler(this.effectLocatorGridView_CurrentCellDirtyStateChanged);
			// 
			// EffectLocatorName
			// 
			this.EffectLocatorName.HeaderText = "ロケータ名";
			this.EffectLocatorName.Name = "EffectLocatorName";
			this.EffectLocatorName.ReadOnly = true;
			this.EffectLocatorName.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			this.EffectLocatorName.Width = 150;
			// 
			// EffectLocatorDisplay
			// 
			this.EffectLocatorDisplay.HeaderText = "表示";
			this.EffectLocatorDisplay.Name = "EffectLocatorDisplay";
			this.EffectLocatorDisplay.Width = 50;
			// 
			// changeAllVisibilityCheckBox
			// 
			this.changeAllVisibilityCheckBox.AutoSize = true;
			this.changeAllVisibilityCheckBox.Enabled = false;
			this.changeAllVisibilityCheckBox.Location = new System.Drawing.Point(12, 12);
			this.changeAllVisibilityCheckBox.Name = "changeAllVisibilityCheckBox";
			this.changeAllVisibilityCheckBox.Size = new System.Drawing.Size(96, 16);
			this.changeAllVisibilityCheckBox.TabIndex = 1;
			this.changeAllVisibilityCheckBox.Text = "全ロケータ表示";
			this.changeAllVisibilityCheckBox.UseVisualStyleBackColor = true;
			this.changeAllVisibilityCheckBox.CheckedChanged += new System.EventHandler(this.changeAllVisibilityCheckBox_CheckedChanged);
			// 
			// EffectLocatorControlForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(272, 234);
			this.ControlBox = false;
			this.Controls.Add(this.changeAllVisibilityCheckBox);
			this.Controls.Add(this.effectLocatorGridView);
			this.Name = "EffectLocatorControlForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.Text = "エフェクトロケータ表示切替";
			this.Load += new System.EventHandler(this.EffectLocatorControlForm_Load);
			((System.ComponentModel.ISupportInitialize)(this.effectLocatorGridView)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.DataGridView effectLocatorGridView;
		private System.Windows.Forms.DataGridViewTextBoxColumn EffectLocatorName;
		private System.Windows.Forms.DataGridViewCheckBoxColumn EffectLocatorDisplay;
		private System.Windows.Forms.CheckBox changeAllVisibilityCheckBox;
	}
}