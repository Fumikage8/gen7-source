using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PokeViewerHost.Forms
{
    public partial class UCtr_PokeDexCapture_Image : UserControl
    {
        public Image Image { get { return pictureBox_Image.Image; } set { pictureBox_Image.Image = value; } }
        public string Caption { get { return textBox_Caption.Text; } set { textBox_Caption.Text = value; toolTip_Label.SetToolTip(textBox_Caption, value); } }
        public Image PictureBackgroundImage { get { return pictureBox_Image.BackgroundImage; } set { pictureBox_Image.BackgroundImage = value; } }

        public UCtr_PokeDexCapture_Image()
        {
            InitializeComponent();
        }
    }
}
