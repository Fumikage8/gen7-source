using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections;

namespace ModelViewer
{
	static public class Util
	{
		// 定数バッファのデータ定義①　変更しないデータ
		public class cbNeverChanges
		{
			public gfl2.clr.math.Matrix Projection = new gfl2.clr.math.Matrix();   // 透視変換行列
		};

		// 定数バッファのデータ定義②　変更頻度の低いデータ
		public class cbChangesEveryFrame
		{
			public gfl2.clr.math.Matrix View = new gfl2.clr.math.Matrix();  // ビュー変換行列
		};

		// 定数バッファのデータ定義③　変更頻度の高いデータ
		public class cbChangesEveryObject
		{
			public gfl2.clr.math.Matrix World = new gfl2.clr.math.Matrix();      // ワールド変換行列
		};

		public struct VertexFormat
		{
			public gfl2.clr.math.Vector pos;
			public gfl2.clr.math.Vector color;

			public VertexFormat(gfl2.clr.math.Vector pos_, gfl2.clr.math.Vector color_)
			{
				pos = pos_;
				color = color_;
			}
		};

		public class MyBitConverter
		{
			static public byte[] ConvertVertexArray(VertexFormat[] vertex)
			{
				MyBitConverter cov = new MyBitConverter(vertex);
				return cov.GetBytes();
			}

			static public byte[] ConvertUInt16Array(UInt16[] data)
			{
				MyBitConverter cov = new MyBitConverter(data);
				return cov.GetBytes();
			}

			static public byte[] ConvertcbNeverChanges(cbNeverChanges data)
			{
				MyBitConverter cov = new MyBitConverter(data.Projection);
				return cov.GetBytes();
			}

			static public byte[] ConvertcbChangesEveryFrame(cbChangesEveryFrame data)
			{
				MyBitConverter cov = new MyBitConverter(data.View);
				return cov.GetBytes();
			}

			static public byte[] ConvertcbChangesEveryObject(cbChangesEveryObject data)
			{
				MyBitConverter cov = new MyBitConverter(data.World);
				return cov.GetBytes();
			}

			//-------------------------------------------

			public MyBitConverter()
			{
				m_List = new ArrayList();
			}

			public MyBitConverter(VertexFormat[] vertex)
			{
				m_List = new ArrayList();
				Add(vertex);
			}

			public MyBitConverter(UInt16[] data)
			{
				m_List = new ArrayList();
				Add(data);
			}

			public MyBitConverter(gfl2.clr.math.Matrix data)
			{
				m_List = new ArrayList();
				Add(data);
			}

			~MyBitConverter()
			{

			}

			public byte[] GetBytes()
			{
				return (byte[])m_List.ToArray(typeof(byte));
			}

			public void Add(UInt16 value)
			{
				byte[] ByteArrayI = BitConverter.GetBytes(value);

				for (int i = 0; i < ByteArrayI.Length; ++i)
					m_List.Add(ByteArrayI[i]);
			}

			public void Add(uint value)
			{
				byte[] ByteArrayI = BitConverter.GetBytes(value);
				for (int i = 0; i < ByteArrayI.Length; ++i)
					m_List.Add(ByteArrayI[i]);
			}

			public void Add(float value)
			{
				byte[] ByteArrayF = BitConverter.GetBytes(value);
				for (int i = 0; i < ByteArrayF.Length; ++i)
					m_List.Add(ByteArrayF[i]);
			}

			public void Add(gfl2.clr.math.Vector vec)
			{
				byte[] ByteArrayX = BitConverter.GetBytes(vec.x);
				byte[] ByteArrayY = BitConverter.GetBytes(vec.y);
				byte[] ByteArrayZ = BitConverter.GetBytes(vec.z);
				byte[] ByteArrayW = BitConverter.GetBytes(vec.w);

				for (int i = 0; i < ByteArrayX.Length; ++i)
				{
					m_List.Add(ByteArrayX[i]);
				}

				for (int i = 0; i < ByteArrayX.Length; ++i)
				{
					m_List.Add(ByteArrayY[i]);
				}

				for (int i = 0; i < ByteArrayX.Length; ++i)
				{
					m_List.Add(ByteArrayZ[i]);
				}

				for (int i = 0; i < ByteArrayX.Length; ++i)
				{
					m_List.Add(ByteArrayW[i]);
				}
			}

			public void Add(gfl2.clr.math.Matrix mat)
			{
				for (int col = 0; col < 4; ++col)
				{
					for (int row = 0; row < 4; ++row)
					{
						Add(mat.GetElem(col, row));
					}
				}
			}

			public void Add(VertexFormat vertex)
			{
				Add(vertex.pos);
				Add(vertex.color);
			}

			public void Add(VertexFormat[] vertex)
			{
				for (int i = 0; i < vertex.Length; ++i)
				{
					Add(vertex[i]);
				}
			}

			public void Add(UInt16[] data)
			{
				for (int i = 0; i < data.Length; ++i)
				{
					Add(data[i]);
				}
			}

			private ArrayList m_List;
		}

    /**
     * @brief ディレクトリのコピー
     * @param sourceDirName コピー元ディレクトリ
     * @param sourceDirName コピー先ディレクトリ
     */
    public static void CopyDirectory(string sourceDirName, string destDirName, bool overwrite)
    {
      //コピー先のディレクトリがないときは作る
      if (!System.IO.Directory.Exists(destDirName))
      {
        System.IO.Directory.CreateDirectory(destDirName);
      }

      //コピー先のディレクトリ名の末尾に"\"をつける
      if (destDirName[destDirName.Length - 1] != System.IO.Path.AltDirectorySeparatorChar)
      {
        destDirName = destDirName + System.IO.Path.AltDirectorySeparatorChar;
      }

      //コピー元のディレクトリにあるファイルをコピー
      string[] files = System.IO.Directory.GetFiles(sourceDirName);
      Array.ForEach(files, path => path = path.Replace("\\", "/"));
      foreach (string file in files)
      {
        byte[] readBytes = System.IO.File.ReadAllBytes(file);
        System.IO.File.WriteAllBytes(destDirName + System.IO.Path.GetFileName(file), readBytes);
      }

      //コピー元のディレクトリにあるディレクトリについて、再帰的に呼び出す
      string[] dirs = System.IO.Directory.GetDirectories(sourceDirName);
      Array.ForEach(dirs, path => path = path.Replace("\\", "/"));
      foreach (string dir in dirs)
      {
        CopyDirectory(dir, destDirName + System.IO.Path.GetFileName(dir), overwrite);
      }
    }

    public static void DeleteDirectory(string dir)
    {
      //DirectoryInfoオブジェクトの作成
      System.IO.DirectoryInfo di = new System.IO.DirectoryInfo(dir);

      //フォルダ以下のすべてのファイル、フォルダの属性を削除
      RemoveReadonlyAttribute(di);

      //フォルダを根こそぎ削除
      di.Delete(true);
    }

    public static void RemoveReadonlyAttribute(System.IO.DirectoryInfo dirInfo)
    {
      //基のフォルダの属性を変更
      if ((dirInfo.Attributes & System.IO.FileAttributes.ReadOnly) == System.IO.FileAttributes.ReadOnly)
      {
        dirInfo.Attributes = System.IO.FileAttributes.Normal;
      }

      //フォルダ内のすべてのファイルの属性を変更
      foreach (System.IO.FileInfo fi in dirInfo.GetFiles())
      {
        if ((fi.Attributes & System.IO.FileAttributes.ReadOnly) == System.IO.FileAttributes.ReadOnly)
        {
          fi.Attributes = System.IO.FileAttributes.Normal;
        }
      }

      //サブフォルダの属性を回帰的に変更
      foreach (System.IO.DirectoryInfo di in dirInfo.GetDirectories())
      {
        RemoveReadonlyAttribute(di);
      }
    }
	}
}
