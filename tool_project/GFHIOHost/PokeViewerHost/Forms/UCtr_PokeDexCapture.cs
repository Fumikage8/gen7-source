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
    public partial class UCtr_PokeDexCapture : UserControl
    {
        public Image Image_SourceColor { get { return uCtr_PokeDexCapture_Image_SourceColor.Image; } set { uCtr_PokeDexCapture_Image_SourceColor.Image = value; } }
        public Image Image_SourceAlpha { get { return uCtr_PokeDexCapture_Image_SourceAlpha.Image; } set { uCtr_PokeDexCapture_Image_SourceAlpha.Image = value; } }
        public Image Image_InvAlpha { get { return uCtr_PokeDexCapture_Image_InvAlpha.Image; } set { uCtr_PokeDexCapture_Image_InvAlpha.Image = value; } }
        public Image Image_Composite { get { return uCtr_PokeDexCapture_Image_Composite.Image; } set { uCtr_PokeDexCapture_Image_Composite.Image = value; } }
        
        public Image Image_CompositeBackground
        {
            get
            {
                return uCtr_PokeDexCapture_Image_Composite.PictureBackgroundImage;
            }
            private set
            {
                uCtr_PokeDexCapture_Image_Composite.PictureBackgroundImage = (value == null) ? Properties.Resources.Background : value;
            }
        }

        public string Caption_SourceColor { get { return uCtr_PokeDexCapture_Image_SourceColor.Caption; } set { uCtr_PokeDexCapture_Image_SourceColor.Caption = value; } }
        public string Caption_SourceAlpha { get { return uCtr_PokeDexCapture_Image_SourceAlpha.Caption; } set { uCtr_PokeDexCapture_Image_SourceAlpha.Caption = value; } }
        public string Caption_InvAlpha { get { return uCtr_PokeDexCapture_Image_InvAlpha.Caption; } set { uCtr_PokeDexCapture_Image_InvAlpha.Caption = value; } }
        public string Caption_Composite { get { return uCtr_PokeDexCapture_Image_Composite.Caption; } set { uCtr_PokeDexCapture_Image_Composite.Caption = value; } }


        public CompositeBackgroundChanged OnCompositeBackgroundChanged { get; set; }


        public void SetCompositeBackground(string path)
        {
            try
            {
                Image_CompositeBackground = new Bitmap(path);
                if (OnCompositeBackgroundChanged != null)
                {
                    OnCompositeBackgroundChanged(this, new CompositeBackgroundChangedEventArgs(path, Image_CompositeBackground));
                }
            }
            catch (System.Exception ex)
            {
                Image_CompositeBackground = null;
                if (OnCompositeBackgroundChanged != null)
                {
                    OnCompositeBackgroundChanged(this, new CompositeBackgroundChangedEventArgs(null, null));
                }
            }
        }



        public delegate void CompositeBackgroundChanged(UCtr_PokeDexCapture sender, CompositeBackgroundChangedEventArgs args);
        public class CompositeBackgroundChangedEventArgs:EventArgs
        {
            public string Path { get; set; }
            public Image Image { get; set; }

            public CompositeBackgroundChangedEventArgs()
            { }

            public CompositeBackgroundChangedEventArgs(string path, Image image)
            {
                Path = path;
                Image = image;
            }
        }





        public UCtr_PokeDexCapture()
        {
            InitializeComponent();
        }


        public void DisposeAllImage()
        {
            if (Image_SourceColor != null) Image_SourceColor.Dispose();
            if (Image_SourceAlpha != null) Image_SourceAlpha.Dispose();
            if (Image_InvAlpha != null) Image_InvAlpha.Dispose();
            if (Image_Composite != null) Image_Composite.Dispose();


            Image_SourceColor =
            Image_SourceAlpha =
            Image_InvAlpha =
            Image_Composite = null;
        }

        public void ClearAllCaption()
        {
            Caption_SourceColor =
            Caption_SourceAlpha =
            Caption_InvAlpha =
            Caption_Composite = @"";
        }

        private void ToolStripMenuItem_LoadBackgroundImage_Click(object sender, EventArgs e)
        {
            OpenFileDialog dialog = new OpenFileDialog();

            dialog.Title = (sender as ToolStripMenuItem).Text;
            dialog.Filter = 
                @"image files (*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.tif;*.tiff)|*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.tif;*.tiff"
                + @"|all files (*.*)|*.*"
                ;
            if (dialog.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    SetCompositeBackground(dialog.FileName);
                }
                catch (System.Exception ex)
                {
                	/*   nop   */
                }
            }
        }

        private void ToolStripMenuItem_ClearBackgroundImage_Click(object sender, EventArgs e)
        {
            SetCompositeBackground(null);
        }
    }
}
