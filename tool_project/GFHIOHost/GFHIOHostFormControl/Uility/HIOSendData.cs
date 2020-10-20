using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GFHIOHostFormControl.Uility
{
    public class HIOSendDataHeader
    {
        /*   properties   */
        public uint Magic { get { return magic; } }
        public int Bytes { get; set; }      /*   後続データバイト数   */
        public string Description { get { return description; } set { description = value; } }  /*   説明   */
        public Encoding DescriptionEncoding { get { return descriptionEncoding; } set { descriptionEncoding = value; } }    /*   説明エンコーディング   */
        public int[] UserData { get { return userData; } }  /*   任意データ   */
        public static int MyBytes{get{return 256;}}

        /*   core   */
        private uint magic = 0x64616568u;
        private string description = @"";
        private Encoding descriptionEncoding = Encoding.Unicode;
        private int[] userData = new int[32];
        private const int descriptionMaxBytes = 120;


        public HIOSendDataHeader(int _bytes) { Bytes = _bytes; }
        public byte[] ToArray()
        {
            byte[] descBytes = DescriptionEncoding.GetBytes(Description);

            Array.Resize(ref descBytes, descriptionMaxBytes);
            /*
             * NUL-termination保証
             * 配列末尾までコードで埋まっていたら一文字減らす
             * */
            if (descBytes[descBytes.Length - 1] != 0)
            {
                string subDesc = description.Substring(0, DescriptionEncoding.GetCharCount(descBytes) - 1);
                descBytes = DescriptionEncoding.GetBytes(subDesc);
                Array.Resize(ref descBytes, descriptionMaxBytes);
            }


            HIOSendDataByteBuilder b = new HIOSendDataByteBuilder();

            b.Add(magic)
                .Add(Bytes)
                .Add(descBytes);

            foreach (int datum in userData)
            {
                b.Add(datum);
            }

            return b.ToArray();
        }
    }


    public class HIOSendDataByteBuilder
    {
        protected List<byte> DataList { get; set; }

        public HIOSendDataByteBuilder()
        {
            DataList = new List<byte>();
        }

        public byte[] ToArray()
        {
            return DataList.ToArray();
        }

        void Clear()
        {
            DataList.Clear();
        }

        public HIOSendDataByteBuilder Add(Boolean v)    { return Add(BitConverter.GetBytes(v)); }
        public HIOSendDataByteBuilder Add(Char v)       { return Add(BitConverter.GetBytes(v)); }
        public HIOSendDataByteBuilder Add(Double v)     { return Add(BitConverter.GetBytes(v)); }
        public HIOSendDataByteBuilder Add(Int16 v)      { return Add(BitConverter.GetBytes(v)); }
        public HIOSendDataByteBuilder Add(Int32 v)      { return Add(BitConverter.GetBytes(v)); }
        public HIOSendDataByteBuilder Add(Int64 v)      { return Add(BitConverter.GetBytes(v)); }
        public HIOSendDataByteBuilder Add(Single v)     { return Add(BitConverter.GetBytes(v)); }
        public HIOSendDataByteBuilder Add(UInt16 v)     { return Add(BitConverter.GetBytes(v)); }
        public HIOSendDataByteBuilder Add(UInt32 v)     { return Add(BitConverter.GetBytes(v)); }
        public HIOSendDataByteBuilder Add(UInt64 v)     { return Add(BitConverter.GetBytes(v)); }
        public HIOSendDataByteBuilder Add(string v)     { return Add(v, Encoding.UTF8); }
        public HIOSendDataByteBuilder Add(string v, Encoding encoding) { Add(encoding.GetBytes(v)); return Add('\0'); }
        public HIOSendDataByteBuilder Add(byte[] v)     { DataList.AddRange(v); return this; }
        public HIOSendDataByteBuilder Add(FX20_12 v)    { return Add(v.Row); }
    }


    public class HIOSendData
    {
        /*
         * 
         * */
        public HIOSendDataHeader Header { get { return mHeader; } }
        public int Ch { get; set; }
        public byte[] Bytes { get; set; }

        /*
         * 
         * */
        protected HIOSendDataHeader mHeader = new HIOSendDataHeader(0);


        /*
         * 
         * */

        public HIOSendData(int sioChannel)
        {
            this.Ch = sioChannel;
        }


        public virtual byte[] ToArray()
        {
            return Bytes;
        }
    }


    public class FX20_12
    {
        public static int Multiplier { get { return (0x01 << 12); } }
        public int Row { get; set; }
        public double Decimal { get { return ((double)Row / Multiplier); } set { Row = new FX20_12(value).Row; } }

        public FX20_12() { Row = 0; }
        public FX20_12(int v)  { Row = v * Multiplier; }
        public FX20_12(double v)
        {
            Row = new FX20_12((int)v).Row;
            Row += (int)(v * Multiplier - (double)Row);
        }
        public FX20_12(float v) { Row = new FX20_12((double)v).Row; }

        public static FX20_12 ToFx20_12(int v)  { return new FX20_12(v); }
        public static FX20_12 ToFx20_12(double v) { return new FX20_12(v); }
    }
}
