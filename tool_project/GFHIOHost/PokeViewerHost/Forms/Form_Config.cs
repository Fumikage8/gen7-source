using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PokeViewerHost.Forms
{
    public partial class Form_Config : Form
    {
        public Utility.PokeInfo PokeInfoForPreview { get { return mPokeInfoForPreview; } set { mPokeInfoForPreview = value; UpdateFileNamePreview(); } }



        private Utility.PokeInfo mPokeInfoForPreview;




        public Form_Config()
        {
            InitializeComponent();
            textBox_Macro.Text = Utility.PokeInfo.FormatMacroDescription;
        }


        public static Form_Config CreateFromConfig(Utility.Config config)
        {
            Form_Config form = new Form_Config();

            form.textBox_Workspace.Text = config.WorkspacePath;
            form.textBox_Environment.Text = config.EnvironmentDir;
            form.textBox_ConfigFileName.Text = config.ConfigFileName;

            form.textBox_ColorPath.Text = config.ImageDir_Color;
            form.textBox_AlphaPath.Text = config.ImageDir_Alpha;
            form.textBox_CompositePath.Text = config.ImageDir_Composite;

            form.textBox_BluePrintPath.Text = config.BluePrintDir;

            form.textBox_NamingRule.Text = config.FileNamingRule;

            /*  guidegrid   */
            form.textBox_GuideGridSize1.Text = config.GuideGridSize_1.ToString();
            form.textBox_GuideGridSize2.Text = config.GuideGridSize_2.ToString();
            form.textBox_GuideGridSize3.Text = config.GuideGridSize_3.ToString();

            return form;
        }

        public void ApplyTo(ref Utility.Config config)
        {
            config.WorkspacePath    = textBox_Workspace.Text;
            config.EnvironmentDir   = textBox_Environment.Text;
            config.ConfigFileName = textBox_ConfigFileName.Text;

            config.ImageDir_Color   = textBox_ColorPath.Text;
            config.ImageDir_Alpha   = textBox_AlphaPath.Text;
            config.ImageDir_Composite = textBox_CompositePath.Text;

            config.BluePrintDir = textBox_BluePrintPath.Text;

            config.FileNamingRule  = textBox_NamingRule.Text;

            /*   guidegrid   */
            try { config.GuideGridSize_1 = int.Parse(textBox_GuideGridSize1.Text); }
            catch { textBox_GuideGridSize1.Text = config.GuideGridSize_1.ToString(); }

            try { config.GuideGridSize_2 = int.Parse(textBox_GuideGridSize2.Text); }
            catch { textBox_GuideGridSize2.Text = config.GuideGridSize_2.ToString(); }

            try { config.GuideGridSize_3 = int.Parse(textBox_GuideGridSize3.Text); }
            catch { textBox_GuideGridSize3.Text = config.GuideGridSize_3.ToString(); }
        }

        private void button_OK_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {

            if(keyData == Keys.Escape)
            {
                this.Close();
            }


            return base.ProcessCmdKey(ref msg, keyData);
        }

        /// <summary>
        ///   
        /// </summary>
        private void UpdateFileNamePreview()
        {
            if(PokeInfoForPreview == null) return;

            try
            {
                textBox_RulePreview.Text = PokeInfoForPreview.MakeFileName(textBox_NamingRule.Text);
            }
            catch (System.Exception ex)
            {
                textBox_RulePreview.Text = @"-illegal format-";
            }
        }

        /// <summary>
        ///   
        /// </summary>
        private void textBox_NamingRule_TextChanged(object sender, EventArgs e)
        {
            UpdateFileNamePreview();
        }

        private void tableLayoutPanel2_Paint(object sender, PaintEventArgs e)
        {

        }
    }
}
