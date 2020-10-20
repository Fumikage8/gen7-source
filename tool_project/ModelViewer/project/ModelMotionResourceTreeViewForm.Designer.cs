namespace ModelViewer
{
	partial class ModelMotionResourceTreeViewForm
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
			this.modelMotionResourceTreeView = new System.Windows.Forms.TreeView();
			this.SuspendLayout();
			// 
			// modelMotionResourceTreeView
			// 
			this.modelMotionResourceTreeView.AllowDrop = true;
			this.modelMotionResourceTreeView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.modelMotionResourceTreeView.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.modelMotionResourceTreeView.Location = new System.Drawing.Point(12, 12);
			this.modelMotionResourceTreeView.Name = "modelMotionResourceTreeView";
			this.modelMotionResourceTreeView.Size = new System.Drawing.Size(260, 338);
			this.modelMotionResourceTreeView.TabIndex = 0;
			this.modelMotionResourceTreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.modelMotionResourceTreeView_AfterSelect);
			this.modelMotionResourceTreeView.DragDrop += new System.Windows.Forms.DragEventHandler(this.modelMotionResourceTreeView_DragDrop);
			this.modelMotionResourceTreeView.DragEnter += new System.Windows.Forms.DragEventHandler(this.modelMotionResourceTreeView_DragEnter);
			this.modelMotionResourceTreeView.KeyDown += new System.Windows.Forms.KeyEventHandler(this.modelMotionResourceTreeView_KeyDown);
			// 
			// ModelMotionResourceTreeViewForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(284, 362);
			this.ControlBox = false;
			this.Controls.Add(this.modelMotionResourceTreeView);
			this.Name = "ModelMotionResourceTreeViewForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.Text = "モデル/モーション";
			this.Load += new System.EventHandler(this.ModelMotionResourceTreeViewForm_Load);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.TreeView modelMotionResourceTreeView;
	}
}