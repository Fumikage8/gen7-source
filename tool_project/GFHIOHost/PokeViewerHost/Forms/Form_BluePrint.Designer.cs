namespace PokeViewerHost.Forms
{
    partial class Form_BluePrint
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
            this.splitContainer = new System.Windows.Forms.SplitContainer();
            this.button_OK = new System.Windows.Forms.Button();
            this.uCtr_BluePrint = new PokeViewerHost.Forms.UCtr_BluePrint();
            this.button_CameraReset = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer)).BeginInit();
            this.splitContainer.Panel2.SuspendLayout();
            this.splitContainer.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer
            // 
            this.splitContainer.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.splitContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer.Location = new System.Drawing.Point(0, 0);
            this.splitContainer.Name = "splitContainer";
            // 
            // splitContainer.Panel1
            // 
            this.splitContainer.Panel1.SizeChanged += new System.EventHandler(this.Form_CameraController_SizeChanged);
            this.splitContainer.Panel1.Click += new System.EventHandler(this.splitContainer_Panel1_Click);
            this.splitContainer.Panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.Form_CameraController_Paint);
            this.splitContainer.Panel1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Form_CameraController_MouseDown);
            this.splitContainer.Panel1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form_CameraController_MouseMove);
            // 
            // splitContainer.Panel2
            // 
            this.splitContainer.Panel2.Controls.Add(this.button_OK);
            this.splitContainer.Panel2.Controls.Add(this.uCtr_BluePrint);
            this.splitContainer.Panel2.Controls.Add(this.button_CameraReset);
            this.splitContainer.Size = new System.Drawing.Size(975, 698);
            this.splitContainer.SplitterDistance = 547;
            this.splitContainer.TabIndex = 0;
            // 
            // button_OK
            // 
            this.button_OK.Location = new System.Drawing.Point(344, 396);
            this.button_OK.Name = "button_OK";
            this.button_OK.Size = new System.Drawing.Size(75, 23);
            this.button_OK.TabIndex = 2;
            this.button_OK.Text = "OK";
            this.button_OK.UseVisualStyleBackColor = true;
            this.button_OK.Click += new System.EventHandler(this.button_OK_Click);
            // 
            // uCtr_BluePrint
            // 
            this.uCtr_BluePrint.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.uCtr_BluePrint.Location = new System.Drawing.Point(3, 3);
            this.uCtr_BluePrint.Name = "uCtr_BluePrint";
            this.uCtr_BluePrint.OnPropertyChanged = null;
            this.uCtr_BluePrint.Size = new System.Drawing.Size(416, 358);
            this.uCtr_BluePrint.TabIndex = 0;
            // 
            // button_CameraReset
            // 
            this.button_CameraReset.Location = new System.Drawing.Point(344, 367);
            this.button_CameraReset.Name = "button_CameraReset";
            this.button_CameraReset.Size = new System.Drawing.Size(75, 23);
            this.button_CameraReset.TabIndex = 1;
            this.button_CameraReset.Text = "Reset";
            this.button_CameraReset.UseVisualStyleBackColor = true;
            this.button_CameraReset.Click += new System.EventHandler(this.button_CameraReset_Click);
            // 
            // Form_BluePrint
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(975, 698);
            this.Controls.Add(this.splitContainer);
            this.KeyPreview = true;
            this.Name = "Form_BluePrint";
            this.Text = "BluePrint";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form_CameraController_FormClosed);
            this.Load += new System.EventHandler(this.Form_CameraController_Load);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Form_BluePrint_KeyDown);
            this.splitContainer.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer)).EndInit();
            this.splitContainer.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer;
        private System.Windows.Forms.Button button_CameraReset;
        public UCtr_BluePrint uCtr_BluePrint;
        private System.Windows.Forms.Button button_OK;
    }
}