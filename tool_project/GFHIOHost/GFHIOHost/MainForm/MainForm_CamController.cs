using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using GFHIOHostFormControl;
using GFHIOHostFormControl.Forms;
using GFHIOHostFormControl.Uility;

namespace GFHIOHost
{
    public partial class Form1 : Form
    {
        public static int MoveAroundGazePoint_MouseRatio {get {return 10;}}

        protected static double ApplyMouseRatio(double v)
        {
            return (v * MoveAroundGazePoint_MouseRatio / 100);
        }


        /*
         * camera
         * 
         * */
        private void panel_MouseController_OnMouseClick(object sender, MouseEventArgs e)
        {
        }

        private void panel_MouseController_MouseUp(object sender, MouseEventArgs e)
        {
            Console.WriteLine(@"released");
        }

        private void panel_MouseController_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                CamData.SetLastPos(e.Location);
                ((Control)sender).Capture = true;
                Console.WriteLine(@"captured");
            }

        }

        private void panel_MouseController_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                Point delta = Point.Subtract(e.Location, new Size(CamData.LastPos));

                if ((delta.X == 0) && (delta.Y == 0)) return;

//                Log(@"cam:{0},{1}", delta.X, delta.Y);

                CamData.SetLastPos(e.Location);
                CamData.MoveAroundGazePointX += ApplyMouseRatio(delta.X);
                CamData.MoveAroundGazePointY += ApplyMouseRatio(delta.Y);
                CamData.IsDirty = true;
            }
        }

        private void OnTick_CamControlTimer(object sender, EventArgs e)
        {
            if (!CamData.IsDirty) return;

            HIO.SendData(CamData);
            CamData.Init();
        }

        
        int wheelRatio = 250 * 120;
        private void MouseWheelHandler(object sender, MouseEventArgs e)
        {
            if (!m_IsMouseOnCameraControlPanel) return;

            const int wheelUnit = 120;

            if (e.Delta != 0)
            {
                CamData.MoveEyeDirection += -e.Delta * wheelRatio / wheelUnit;
                CamData.IsDirty = true;
            }
        }

        private void panel_MouseController_OnKeyPress(object sender, KeyPressEventArgs e)
        {
            if (!m_IsMouseOnCameraControlPanel) return;

            int unitH = 2000;
            int unitV = unitH * 5 / 3;


            switch (e.KeyChar)
            {
                case 'w': CamData.MoveGazePointY += unitV; CamData.IsDirty = true; break;
                case 'a': CamData.MoveGazePointX += unitH; CamData.IsDirty = true; break;
                case 's': CamData.MoveGazePointY -= unitV; CamData.IsDirty = true; break;
                case 'd': CamData.MoveGazePointX -= unitH; CamData.IsDirty = true; break;
            }
        }

        private void panel_MouseController_MouseEnter(object sender, EventArgs e)
        {
            (sender as Control).Select();
            m_IsMouseOnCameraControlPanel = true;
        }

        private void panel_MouseController_MouseLeave(object sender, EventArgs e)
        {
            m_IsMouseOnCameraControlPanel = false;
        }

    };
}
