using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Drawing;

using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;


namespace PokeViewerHost.Utility
{
    public class DXGrid
    {
        protected Device DXDevice { get; set; }

        protected int Primitives { get; set; }

        public VertexBuffer VertexBuffer { get; protected set; 
}


        public DXGrid(Device dxDevice, float gridSize, int gridNum, int gridColorArgb, float axisSize)
        {
            DXDevice    = dxDevice;

            Primitives = 3 + ((gridNum*2 + 1) * 2);

            VertexBuffer = new VertexBuffer(typeof(CustomVertex.PositionColored), Primitives*2, DXDevice, Usage.None, CustomVertex.PositionColored.Format, Pool.Managed);
            using (GraphicsStream gs = VertexBuffer.Lock(0, 0, LockFlags.None))
            {
                float lineLength = gridSize * (gridNum*2);
                float halfLength = lineLength /2.0f;


                /*   grid   */
                for (int i = 0; i < (gridNum*2 + 1); ++i)
                {
                    float offset = (-halfLength) + gridSize * i;

                    gs.Write(new CustomVertex.PositionColored(-halfLength, 0, offset, gridColorArgb));
                    gs.Write(new CustomVertex.PositionColored(+halfLength, 0, offset, gridColorArgb));

                    gs.Write(new CustomVertex.PositionColored(offset, 0, -halfLength, gridColorArgb));
                    gs.Write(new CustomVertex.PositionColored(offset, 0, +halfLength, gridColorArgb));
                }

                /*   axis   */
                gs.Write(new CustomVertex.PositionColored(0,            0,          0,          Color.Red.ToArgb()));
                gs.Write(new CustomVertex.PositionColored(+axisSize,    0,          0,          Color.Red.ToArgb()));
                gs.Write(new CustomVertex.PositionColored(0,            0,          0,          Color.Blue.ToArgb()));
                gs.Write(new CustomVertex.PositionColored(0,            +axisSize,  0,          Color.Blue.ToArgb()));
                gs.Write(new CustomVertex.PositionColored(0,            0,          0,          Color.Green.ToArgb()));
                gs.Write(new CustomVertex.PositionColored(0,            0,          +axisSize,  Color.Green.ToArgb()));


                VertexBuffer.Unlock();
            }
        }


        public void Render()
        {
            DXDevice.SetStreamSource(0, VertexBuffer, 0);
            DXDevice.SetTexture(0, null);
            DXDevice.VertexFormat = CustomVertex.PositionColored.Format;
            DXDevice.DrawPrimitives(PrimitiveType.LineList, 0, Primitives);
        }

        public void Dispose()
        {
            VertexBuffer.Dispose();
            VertexBuffer = null;
        }
    }
}
