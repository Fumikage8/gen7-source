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
        protected class SendData_Cam
            : HIOSendData
        {
            /*   props   */
            public Point LastPos { get; private set; }
            public bool IsDirty { get; set; }

            /*   core   */
            public double MoveAroundGazePointX { get; set; }
            public double MoveAroundGazePointY { get; set; }
            public int MoveEyeDirection { get; set; }
            public int MoveGazePointX { get; set; }
            public int MoveGazePointY { get; set; }

            /*   members   */


            /*
             * 
             * */
            public SendData_Cam(int sioChannel)
                :   base(sioChannel)
            {
                Init();
                LastPos = new Point();
            }

            public void Init()
            {
                MoveAroundGazePointX = 0;
                MoveAroundGazePointY = 0;
                MoveEyeDirection = 0;
                MoveGazePointX = 0;
                MoveGazePointY = 0;
                
                IsDirty = false;
            }
            
            public void SetLastPos(Point pos) { LastPos = pos; }

            public override byte[] ToArray()
            {
                HIOSendDataByteBuilder b = new HIOSendDataByteBuilder();

                b.Add(FX20_12.ToFx20_12(MoveAroundGazePointX))
                    .Add(FX20_12.ToFx20_12(MoveAroundGazePointY))
                    .Add(MoveEyeDirection)
                    .Add(MoveGazePointX)
                    .Add(MoveGazePointY)
                    ;
                Bytes = b.ToArray();

                return base.ToArray();
            }
        };

    };
}
