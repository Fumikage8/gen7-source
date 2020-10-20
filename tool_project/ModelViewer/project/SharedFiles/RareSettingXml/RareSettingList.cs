using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;

using gfl2.clr.poke_3d_lib;

namespace RareSettingXml
{
	public class RareSettingList : ExcelXml.IExcelXmlSerializer
	{
		public XmlDocument XmlDocument { get; set; }
		public string[,] Cell2d { get { return this.cells; } }

		public void DeserializeExcel(string[,] cells)
		{
			this.cells = cells;
			this.rareColorConstantSettingInfoList = new List<PokemonModel.RareColorConstantSettingInfo>();

			for (var i = 0; i < cells.GetLength(0); ++i)
			{
				if (!IsRowAvailable(cells, i))
				{
					continue;
				}

				bool succeeded = false;
				string materialName = cells[i, 1];
				UInt32 index;
				float r, g, b, a;

				succeeded = UInt32.TryParse(cells[i, 2], out index);
				if (!succeeded)
				{
					continue;
				}

				succeeded = float.TryParse(cells[i, 3], out r);
				if (!succeeded)
				{
					continue;
				}

				succeeded = float.TryParse(cells[i, 4], out g);
				if (!succeeded)
				{
					continue;
				}

				succeeded = float.TryParse(cells[i, 5], out b);
				if (!succeeded)
				{
					continue;
				}

				succeeded = float.TryParse(cells[i, 6], out a);
				if (!succeeded)
				{
					continue;
				}

				PokemonModel.RareColorConstantSettingInfo info =
					new PokemonModel.RareColorConstantSettingInfo(materialName, index, r, g, b, a);
				rareColorConstantSettingInfoList.Add(info);
			}
		}

		private bool IsRowAvailable(string[,] cells, Int32 row)
		{
			// 先頭に何もなければfalse
			if (string.IsNullOrEmpty(cells[row, 0]))
			{
				return false;
			}

			// 定数カラーがひとつでも欠けていたらfalse
			for (var i = 1; i <= VALUE_NUM; ++i)
			{
				if (string.IsNullOrEmpty(cells[row, i]))
				{
					return false;
				}
			}

			return true;
		}

		public string[,] cells = null;
		private static readonly Int32 VALUE_NUM = 6;		// 読み取る要素数
		public List<PokemonModel.RareColorConstantSettingInfo> rareColorConstantSettingInfoList;
	}
}
