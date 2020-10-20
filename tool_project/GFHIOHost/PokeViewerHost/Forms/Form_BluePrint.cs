using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;

namespace PokeViewerHost.Forms
{
    public partial class Form_BluePrint : Form
    {
        public delegate void OnPropertyChangedDelegate(Form_BluePrint sender);

        /*   dx   */
        protected Device DXDevice { get; private set; }
        protected Microsoft.DirectX.DirectInput.Device DIDevice { get; private set; }
        protected Control DXTargetControl { get { return splitContainer.Panel1; } }
        protected Microsoft.DirectX.Direct3D.Font DXFont { get; set; }
        protected Timer RefreshTimer { get; private set; }
        protected int DXFontHeight { get { return 16; } }

        /*   core   */
        public Vector3 Position { get; protected set; }
        public Vector3 LookAt { get; protected set; }
        public float Longitude { get; set; }        /*   経度   */
        public float Latitude { get; set; }         /*   緯度   */
        public float PolarCoordRadius { get; set; }

        public float DLightLongitude { get; set; }
        public float DLightLatitude { get; set; }

        /*  */
        protected UCtr_BluePrint UCtrBluePrint { get { return uCtr_BluePrint; } }


        /*      */
        protected Utility.BluePrint BluePrint { get; set; }
        public Utility.BluePrint ClippedBP { get; set; }

        public OnPropertyChangedDelegate OnPropertyChanged { get; set; }
        public int CameraFOVY { get; set; }

        /*   dx   */
        protected Utility.DXGrid DXGrid { get; set; }
        protected Mesh DXTeapot { get; private set; }
        protected Utility.DXLight DXLight { get; set; }
        protected bool IsLightControlMode { get; set; }

        protected float GazePointMoveSpeed { get { return GazePointMoveFast ? GazePointMoveSpeed_Fast : GazePointMoveSpeed_Normal; } }
        protected float GazePointMoveSpeed_Normal { get { return 0.1f; } }
        protected float GazePointMoveSpeed_Fast { get { return 1.0f; } }
        protected bool GazePointMoveFast { get; set; }

        private bool CloseRequest { get; set; }

        public GFHIOHostFormControl.Forms.Form_Log LogForm { get; set; }


        public Form_BluePrint()
        {
            RefreshTimer = new Timer();
            RefreshTimer.Interval = 1000 / 60;
            RefreshTimer.Tick += new EventHandler(OnRefresh);

            InitializeComponent();
            UCtrBluePrint.OnPropertyChanged = OnUCtrBluePrintPropertyChanged;
        }

        protected void OnUCtrBluePrintPropertyChanged(UCtr_BluePrint sender)
        {
            CameraFOVY = sender.BluePrint.CameraFOV;
            NotifyPropertyChanged();
        }

        public void Reset()
        {
            /*
            Position = new Vector3(0, 0, -100);
            LookAt  = new Vector3(0,0,0);
            Longitude =
            Latitude = 0;
            PolarCoordRadius = 100;
            DLightLatitude = 0;
            DLightLatitude = 45;

             * */
            //UCtrBluePrint.Reset();

            BluePrint.Reset();
            SetupFromBluePrint(BluePrint);
        }

        public void SetupFromBluePrint(Utility.BluePrint bp)
        {
            Position = new Vector3(bp.CameraX, bp.CameraY, bp.CameraZ);
            LookAt = new Vector3(bp.CameraLookAtX, bp.CameraLookAtY, bp.CameraLookAtZ);
            
            Longitude = bp.CameraLongitude;
            Latitude = bp.CameraLatitude;
            PolarCoordRadius = bp.CameraPolarCoordRadius;

            DLightLongitude = bp.LightLongitude;
            DLightLatitude = bp.LightLatitude;
            CameraFOVY = bp.CameraFOV;

            BluePrint = bp;
            UCtrBluePrint.Bind(bp);

            NotifyPropertyChanged();
        }



        public void ApplyToBluePrint(ref Utility.BluePrint bp)
        {
            bp.CameraX = Position.X;
            bp.CameraY = Position.Y;
            bp.CameraZ = Position.Z;

            bp.CameraLookAtX = LookAt.X;
            bp.CameraLookAtY = LookAt.Y;
            bp.CameraLookAtZ = LookAt.Z;

            bp.CameraLongitude = Longitude;
            bp.CameraLatitude = Latitude;
            bp.CameraPolarCoordRadius = PolarCoordRadius;

            bp.LightLongitude = DLightLongitude;
            bp.LightLatitude = DLightLatitude;


            UCtrBluePrint.OnBluePrintPropertyChanged();
        }


        protected void OnRefresh(Object sender, EventArgs args)
        {

            {
                if (DIDevice != null)
                {
                    Microsoft.DirectX.DirectInput.Key[] keys = null;
                    try
                    {
                        keys = DIDevice.GetPressedKeys();
                    }
                    catch(DirectXException)
                    {
                        try
                        {
                            DIDevice.SetCooperativeLevel(null, Microsoft.DirectX.DirectInput.CooperativeLevelFlags.Background | Microsoft.DirectX.DirectInput.CooperativeLevelFlags.NonExclusive);
                            DIDevice.Acquire();
                        }
                        catch(Microsoft.DirectX.DirectInput.InputException e)
                        {
                            Log(e.ToString());
                        }
                        catch (Exception e)
                        {
                            Log(e.ToString());
                        }
                    }

                    /*  reset */
                    IsLightControlMode = false;
                    GazePointMoveFast = false;
                    IsPolarCoordRadiusMoveSpeedRatioFast = false;

                    if(keys != null)
                    {
                        bool    isDirty = false;
                        Vector3 eyeDir = Vector3.Subtract(LookAt, Position);
                        Vector3 orthoEyeDir;

                        orthoEyeDir.X = eyeDir.Z;
                        orthoEyeDir.Y = 0;
                        orthoEyeDir.Z = eyeDir.X;
                        orthoEyeDir.Normalize();

                        foreach(Microsoft.DirectX.DirectInput.Key key in keys)
                        {
                            switch(key)
                            {
                                case Microsoft.DirectX.DirectInput.Key.LeftShift:
                                case Microsoft.DirectX.DirectInput.Key.RightShift:
                                    GazePointMoveFast = true;
                                    IsPolarCoordRadiusMoveSpeedRatioFast = true;
                                    break;

                                case Microsoft.DirectX.DirectInput.Key.LeftAlt:
                                    IsLightControlMode = true;
                                    break;
                            }
                        }



                        foreach(Microsoft.DirectX.DirectInput.Key key in keys)
                        {
                            switch(key)
                            {
                                case Microsoft.DirectX.DirectInput.Key.W:
                                    LookAt = Vector3.Add(LookAt, new Vector3(0, GazePointMoveSpeed, 0));
                                    isDirty = true;
                                    break;

                                case Microsoft.DirectX.DirectInput.Key.A:
                                    LookAt = Vector3.Add(LookAt, orthoEyeDir * GazePointMoveSpeed);
                                    isDirty = true;
                                    break;

                                case Microsoft.DirectX.DirectInput.Key.S:
                                    LookAt = Vector3.Subtract(LookAt, new Vector3(0, GazePointMoveSpeed, 0));
                                    isDirty = true;
                                    break;
                                case Microsoft.DirectX.DirectInput.Key.D:
                                    LookAt = Vector3.Add(LookAt, orthoEyeDir * (-GazePointMoveSpeed));
                                    isDirty = true;
                                    break;
                            }
                        }

                        if (isDirty)
                        {
                            NotifyPropertyChanged();
                        }
                    }
                }

            }

            RenderDXDevice();
        }


        protected float PolarCoordRadiusMoveSpeedRatio { get { return IsPolarCoordRadiusMoveSpeedRatioFast ? PolarCoordRadiusMoveSpeedRatio_Fast : PolarCoordRadiusMoveSpeedRatio_Normal; } }
        protected float PolarCoordRadiusMoveSpeedRatio_Normal { get { return 1.0f / 120.0f; } }
        protected float PolarCoordRadiusMoveSpeedRatio_Fast { get { return 1.0f / 12.0f; } }
        protected bool IsPolarCoordRadiusMoveSpeedRatioFast { get; set; }
        protected float PolarCoordRadius_Min { get { return 10; } }
        protected float PolarCoordRadius_Max { get { return 16384; } }

        /// <summary>
        ///   マウスホイールハンドラ
        /// </summary>
        protected void OnMouseWheel(object sender, MouseEventArgs args)
        {
            PolarCoordRadius += (float)args.Delta * PolarCoordRadiusMoveSpeedRatio;

            PolarCoordRadius = Math.Min(PolarCoordRadius_Max, PolarCoordRadius);
            PolarCoordRadius = Math.Max(PolarCoordRadius_Min, PolarCoordRadius);

            RenderDXDevice();
            NotifyPropertyChanged();
        }



        /// <summary>
        ///   DirectX deviceの生成
        /// </summary>
        protected void CreateDXDevice()
        {
            if (DXDevice != null)
            {
                DisposeDXDevice();
            }


            PresentParameters pp = new PresentParameters();

            pp.Windowed = true;
            pp.SwapEffect = SwapEffect.Discard;

            pp.EnableAutoDepthStencil = true;
            pp.AutoDepthStencilFormat = DepthFormat.D16;
            pp.BackBufferCount = 1;

            /*   dxdevice   */
            DXDevice = new Device(0, DeviceType.Hardware, DXTargetControl.Handle, CreateFlags.HardwareVertexProcessing, pp);

            /*   dinput   */
            DIDevice = new Microsoft.DirectX.DirectInput.Device(Microsoft.DirectX.DirectInput.SystemGuid.Keyboard);
            /*   resources   */
            {
                /*   light   */
                DXLight = new Utility.DXLight(DXDevice);
            }
            {
                /*   grid   */
                DXGrid = new Utility.DXGrid(DXDevice, 10, 50, Color.White.ToArgb(), 20);
            }
            {
                /*   teapot   */
                DXTeapot = Mesh.Teapot(DXDevice);
            }
            {
                /*   font   */
                FontDescription desc = new FontDescription();


                desc.Height = DXFontHeight;
                desc.FaceName = @"メイリオ";
                DXFont = new Microsoft.DirectX.Direct3D.Font(DXDevice, desc);
            }


            RenderDXDevice();
//            NotifyPropertyChanged();
            RefreshTimer.Start();

            Log(@"create dxdevice");
        }


        /// <summary>
        ///   DirectX device のdispose
        /// </summary>
        protected void DisposeDXDevice()
        {
            if ((DXDevice != null) && (!DXDevice.Disposed))
            {
                DXGrid.Dispose();
                DXGrid = null;

                DXFont.Dispose();
                DXFont = null;

                DXTeapot.Dispose();
                DXTeapot = null;


                DIDevice.Unacquire();
                DIDevice.Dispose();
                DIDevice = null;


                DXDevice.Dispose();
                DXDevice = null;

                RefreshTimer.Stop();
            }

            Log(@"dispose dxdevice");
        }

        
        /// <summary>
        ///   DirectX のレンダリング
        /// </summary>
        protected void RenderDXDevice()
        {
            if (DXDevice == null) return;



            ClearFlags cf = 0;

            cf |= ClearFlags.Target;
            cf |= ClearFlags.ZBuffer;

            DXDevice.Clear(cf, Color.FromArgb(0x101020), 1.0f, 0);
            DXDevice.BeginScene();

            DXDevice.RenderState.ZBufferEnable = true;
            DXDevice.RenderState.ZBufferWriteEnable = true;
            {
                Material mat = new Material();

                mat.Diffuse = Color.White;
                mat.Ambient = Color.White;

                DXDevice.Material = mat;
            }



            float near = 1;
            float far = 16384;

            float radi = PolarCoordRadius;
            float holi = Geometry.DegreeToRadian(Longitude);
            float vert = Geometry.DegreeToRadian(90-Latitude);
            Vector3 v = Vector3.Empty;
            Vector3 at = LookAt;
            

            v.X = (float)(radi * Math.Sin(vert) * Math.Sin(holi));
            v.Y = (float)(radi * Math.Cos(vert));
            v.Z = (float)(radi * Math.Sin(vert) * Math.Cos(holi));


            DXDevice.Transform.View = Matrix.LookAtLH(v, at, new Vector3(0, 1, 0));
            DXDevice.Transform.Projection = Matrix.PerspectiveFovLH(Geometry.DegreeToRadian(CameraFOVY), (float)DXTargetControl.Width / (float)DXTargetControl.Height, near, far);


            UpdateDxLight();


            /*      */
            {
                /*   grid   */
                DXDevice.RenderState.Lighting = false;
                DXGrid.Render();

                /*   teapot   */
                DXLight.SetupToDevice(0);
                DXDevice.RenderState.Lighting = true;
                DXDevice.Transform.World = Matrix.RotationY(Geometry.DegreeToRadian(270.0f)) * Matrix.Scaling(10, 10, 10) * Matrix.Translation(0, 10, 0);
                //                DXDevice.Transform.World = Matrix.RotationY(Geometry.DegreeToRadian(270.0f))  * Matrix.Translation(0, 10, 0);
                DXTeapot.DrawSubset(0);
                DXDevice.Transform.World = Matrix.Identity;

                /*   light prim   */
                DXDevice.RenderState.Lighting = false;
                DXLight.Render();


                DXDevice.RenderState.Lighting = false;
                {
                    int y = 0;
                    DXFont.DrawText(null, string.Format(@"pos:{0}, {1}, {2}", Position.X, Position.Y, Position.Z), new Point(0, y++ * DXFontHeight), Color.White);
                    DXFont.DrawText(null, string.Format(@"at:{0}, {1}, {2}", LookAt.X, LookAt.Y, LookAt.Z), new Point(0, y++ * DXFontHeight), Color.White);
                    DXFont.DrawText(null, string.Format(@"longitude:{0}", Longitude), new Point(0, y++ * DXFontHeight), Color.White);
                    DXFont.DrawText(null, string.Format(@"latitude:{0}", Latitude), new Point(0, y++ * DXFontHeight), Color.White);
                    DXFont.DrawText(null, string.Format(@"radius:{0}", PolarCoordRadius), new Point(0, y++ * DXFontHeight), Color.White);
                    DXFont.DrawText(null, string.Format(@"light longitude:{0}", DLightLongitude), new Point(0, y++ * DXFontHeight), Color.White);
                    DXFont.DrawText(null, string.Format(@"light latitude:{0}", DLightLatitude), new Point(0, y++ * DXFontHeight), Color.White);
                }
            }


            DXDevice.EndScene();
            DXDevice.Present();

            {
                Position = v;
                LookAt = at;
            }
        }







        private void Form_CameraController_Load(object sender, EventArgs e)
        {
            CreateDXDevice();
            MouseWheel += new MouseEventHandler(OnMouseWheel);
        }

        private void Form_CameraController_FormClosed(object sender, FormClosedEventArgs e)
        {
            DisposeDXDevice();
        }

        private void Form_CameraController_Paint(object sender, PaintEventArgs e)
        {
            RenderDXDevice();
        }


        protected Point MousePos { get; set; }

        private void Form_CameraController_MouseDown(object sender, MouseEventArgs e)
        {
            if(e.Button == System.Windows.Forms.MouseButtons.Right)
            {
                MousePos = e.Location;
            }

        }

        private void UpdateDxLight()
        {
            float radi = 1.0f;
            float holi = Geometry.DegreeToRadian(DLightLongitude);
            float vert = Geometry.DegreeToRadian(90 - DLightLatitude);
            Vector3 v = Vector3.Empty;


            v.X = (float)(radi * Math.Sin(vert) * Math.Sin(holi));
            v.Y = (float)(radi * Math.Cos(vert));
            v.Z = (float)(radi * Math.Sin(vert) * Math.Cos(holi));

            DXLight.Direction = v;
        }


        private void Form_CameraController_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Right)
            {
                Point delta = Point.Subtract(e.Location, new System.Drawing.Size(MousePos));
                if (IsLightControlMode)
                {
                    /*   light   */

                    DLightLongitude += (float)-delta.X * 0.3f;
                    DLightLatitude += (float)delta.Y * 0.3f;


                    if (DLightLongitude > 180)
                    {
                        DLightLongitude -= 360;
                    }
                    else if (DLightLongitude < -180)
                    {
                        DLightLongitude += 360;
                    }

                    /*   緯度   */
                    if (DLightLatitude > 180.0f)
                    {
                        DLightLatitude -=360;
                    }
                    else if (DLightLatitude < -180.0f)
                    {
                        DLightLatitude +=360;
                    }
                }
                else
                {
                    /*   camera   */

                    Longitude += (float)delta.X * 0.3f;
                    Latitude += (float)delta.Y * 0.3f;


                    if (Longitude > 180)
                    {
                        Longitude -= 360;
                    }
                    else if (Longitude < -180)
                    {
                        Longitude += 360;
                    }

                    /*   緯度   */
                    if (Latitude > 89.5f)
                    {
                        Latitude = 89.5f;
                    }
                    else if (Latitude < -89.5f)
                    {
                        Latitude = -89.5f;
                    }
                }
                RenderDXDevice();
                NotifyPropertyChanged();
                MousePos = e.Location;
            }
        }

        private void Form_CameraController_SizeChanged(object sender, EventArgs e)
        {
            CreateDXDevice();
        }

        private void button_SendData_Click(object sender, EventArgs e)
        {
            NotifyPropertyChanged();
        }

        protected void NotifyPropertyChanged()
        {
            Utility.BluePrint bp = BluePrint;

            ApplyToBluePrint(ref bp);
            if (OnPropertyChanged != null)
            {
                OnPropertyChanged(this);
            }
        }

        protected void Log(string format, params object[] args)
        {
            string str = string.Format(format, args);

            Console.WriteLine(str);
            if(LogForm != null)
            {
                LogForm.WriteLine(str);
            }
        }

        private void button_Copy_Click(object sender, EventArgs e)
        {
            Utility.BluePrint bp = new Utility.BluePrint();

            ApplyToBluePrint(ref bp);
            ClippedBP = bp;
            Log(@"BP copy");
        }

        private void button_Paste_Click(object sender, EventArgs e)
        {
            if (ClippedBP != null)
            {
                SetupFromBluePrint(ClippedBP);
                Log(@"BP paste");
            }
        }

        private void button_CameraReset_Click(object sender, EventArgs e)
        {
            Reset();
        }

        private void button_OK_Click(object sender, EventArgs e)
        {
            Decide();
        }

        private void splitContainer_Panel1_Click(object sender, EventArgs e)
        {
            ActiveControl = (sender as Control);
        }

        private void Form_BluePrint_KeyDown(object sender, KeyEventArgs e)
        {
            switch(e.KeyCode)
            {
                case    Keys.Enter:
                    Decide();
                    e.Handled = true;
                    break;
                case    Keys.Escape:
                    Cancel();
                    e.Handled = true;
                    break;
                case Keys.Delete:
                    Reset();
                    e.Handled = true;
                    break;
            }
        }

        private void Decide()
        {
            DialogResult = DialogResult.OK;
            Hide();
        }

        private void Cancel()
        {
            DialogResult = DialogResult.Cancel;
            Hide();
        }
    }
}
