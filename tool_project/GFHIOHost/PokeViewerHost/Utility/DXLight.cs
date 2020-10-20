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
    public class DXLight
    {
        public Color AmbientColor { get; set; }
        public Color DiffuseColor { get; set; }
        public Vector3 Direction { get; set; }

        public Device DXDevice {get; set;}

        private float PrimitiveSize { get { return 100.0f; } }
        private int VertexNum { get { return 2; } }

        public DXLight(Device dxDevice)
        {
            DXDevice = dxDevice;

            AmbientColor = Color.FromArgb(0xff, 32, 32, 32);
            DiffuseColor = Color.White; ;
            Direction = new Vector3(1, -1, 1);
        }


        public void SetupToDevice(int lightNum)
        {
            LightsCollection lights = DXDevice.Lights;
            Light light = lights[lightNum];

            Direction.Normalize();

            light.Type = LightType.Directional;
            light.Direction = Direction;
            light.Diffuse = DiffuseColor;
            light.Ambient = AmbientColor;
            light.Enabled = true;
            light.Update();


            DXDevice.RenderState.Ambient = AmbientColor;
            DXDevice.RenderState.NormalizeNormals = true;
        }


        public void Render()
        {
            VertexBuffer vb = new VertexBuffer(typeof(CustomVertex.PositionColored), VertexNum, DXDevice, Usage.None, CustomVertex.PositionColored.Format, Pool.Managed);
            using (GraphicsStream gs = vb.Lock(0, 0, LockFlags.None))
            {
                Direction.Normalize();
                Vector3 vec = -Direction * PrimitiveSize;

                gs.Write(new CustomVertex.PositionColored(0, 0, 0, DiffuseColor.ToArgb()));
                gs.Write(new CustomVertex.PositionColored(vec, DiffuseColor.ToArgb()));

                vb.Unlock();
            }


            DXDevice.SetStreamSource(0, vb, 0);
            DXDevice.SetTexture(0, null);
            DXDevice.VertexFormat = CustomVertex.PositionColored.Format;
            DXDevice.DrawPrimitives(PrimitiveType.LineList, 0, 1);
        }
    }
}
