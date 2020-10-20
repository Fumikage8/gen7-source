using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;

using System.IO;
using System.Xml;
using System.Xml.Serialization;


using GFHIOHostFormControl;
using GFHIOHostFormControl.Forms;
using GFHIOHostFormControl.Uility;


namespace PokeViewerHost.Utility
{
    public class BluePrint : ICloneable
    {
        /*   背景色   */
        public byte ClearColorR { get { return ClearColor.R; } set { ClearColor = Color.FromArgb(value,         ClearColor.G,   ClearColor.B); } }
        public byte ClearColorG { get { return ClearColor.G; } set { ClearColor = Color.FromArgb(ClearColor.R,  value,          ClearColor.B); } }
        public byte ClearColorB { get { return ClearColor.B; } set { ClearColor = Color.FromArgb(ClearColor.R,  ClearColor.G,   value); } }

        /*   カメラ   */
        public int CameraMode { get; set; }
        public float CameraX { get; set; }
        public float CameraY { get; set; }
        public float CameraZ { get; set; }
        public float CameraLookAtX { get; set; }
        public float CameraLookAtY { get; set; }
        public float CameraLookAtZ { get; set; }

        public float CameraLongitude { get; set; }        /*   経度   */
        public float CameraLatitude { get; set; }         /*   緯度   */
        public float CameraPolarCoordRadius { get; set; }
            
        public int CameraFOV { get; set; }

        /*   ライト   */
        public float LightLongitude { get; set; }        /*   経度   */
        public float LightLatitude { get; set; }         /*   緯度   */

        /*   影   */
        public bool EnableShadow { get; set; }

        /*   モーション   */
        public int MotionIndex { get; set; }
        public int MotionFrame { get; set; }

        /*
         * 保存対象外
         */
        [XmlIgnore]
        public string SerializePath { get; set; }

        [XmlIgnore]
        public Color ClearColor { get; set; }

        
        
        /// <summary>
        ///   ctor
        /// </summary>
        public BluePrint()
        {
            Reset();
        }

        /// <summary>
        ///   初期化
        /// </summary>
        public void Reset()
        {
            ClearColor = Color.FromArgb(Color.DimGray.ToArgb());

            CameraMode = 0;

            CameraX = 0;
            CameraY = 0;
            CameraZ = 500;

            CameraLookAtX = 0;
            CameraLookAtY = 10;
            CameraLookAtZ = 0;

            CameraLongitude = 0;
            CameraLatitude = 0;

            CameraPolarCoordRadius = 200;
            CameraFOV = 20;

            LightLongitude = 30;
            LightLatitude = 30;


            EnableShadow = false;

            MotionIndex = 0;
            MotionFrame = 1;
        }

        /// <summary>
        ///   HIO送信データの構築
        /// </summary>
        public HIOSendData CreateHIOSendData(int sioCh)
        {
            HIOSendData data = new HIOSendData(sioCh);
            int index = 0;
            int[] userData = data.Header.UserData;

            userData[index++] = (ClearColor.B << 16) | (ClearColor.G << 8) | (ClearColor.R << 0);


            userData[index++] = CameraMode;

            userData[index++] = FX20_12.ToFx20_12(CameraX).Row;
            userData[index++] = FX20_12.ToFx20_12(CameraY).Row;
            userData[index++] = FX20_12.ToFx20_12(CameraZ).Row;

            userData[index++] = FX20_12.ToFx20_12(CameraLookAtX).Row;
            userData[index++] = FX20_12.ToFx20_12(CameraLookAtY).Row;
            userData[index++] = FX20_12.ToFx20_12(CameraLookAtZ).Row;

            userData[index++] = FX20_12.ToFx20_12(CameraLongitude).Row;
            userData[index++] = FX20_12.ToFx20_12(CameraLatitude).Row;
            userData[index++] = FX20_12.ToFx20_12(CameraPolarCoordRadius).Row;

            userData[index++] = CameraFOV;

            userData[index++] = FX20_12.ToFx20_12(180+LightLongitude).Row;
            userData[index++] = FX20_12.ToFx20_12(-LightLatitude).Row;

            userData[index++] = EnableShadow ? 1 : 0;

            userData[index++] = MotionIndex;
            userData[index++] = MotionFrame;


            return data;
        }

        /// <summary>
        ///   ファイルへ保存(SerializePathへ)
        /// </summary>
        public void Save()
        {
            Save(SerializePath);
        }

        /// <summary>
        ///   ファイルへ保存（パス指定）
        /// </summary>
        public void Save(string path)
        {
            XmlSerializer xml = new XmlSerializer(typeof(BluePrint));

            System.IO.Directory.CreateDirectory(System.IO.Path.GetDirectoryName(path));
            using (StreamWriter stream = new StreamWriter(path))
            {
                xml.Serialize(stream, this);
            }
        }

        /// <summary>
        ///   ファイルから読み込み（パス指定）
        /// </summary>
        public static BluePrint Load(string path)
        {
            BluePrint bp = null;
            XmlSerializer xml = new XmlSerializer(typeof(BluePrint));
            using (StreamReader stream = new StreamReader(path))
            {
                bp = (BluePrint)xml.Deserialize(stream);
            }

            return bp;
        }

        /// <summary>
        ///   ファイルから読み込み（失敗する場合は初期状態で返す）
        /// </summary>
        public static BluePrint SafeLoad(string path)
        {
            BluePrint bp;

            try
            {
                bp = Load(path);
            }
            catch (Exception e)
            {
                bp = new BluePrint();
            }

            bp.SerializePath = path;
            return bp;
        }

        /*
         * inplement ICloneable
         * */
        public virtual object Clone()
        {
            BluePrint bp = (BluePrint)MemberwiseClone();

            return bp;
        }
    }
}
