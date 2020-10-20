using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;

namespace Setting
{
	public class SettingList : ExcelXml.IExcelXmlSerializer
	{
		public XmlDocument XmlDocument { get; set; }
		public string[,] Cell2d { get { return this.cells; } }

		public void DeserializeExcel(string[,] cells)
		{
			this.cells = cells;
			this.dictionary = new Dictionary<string, decimal>();

			for (var i = 0; i < cells.GetLength(0); ++i)
			{
				string key = cells[i, 0];
				string strValue = cells[i, 1];
				decimal value = 0;

				bool succeeded = ConvertToDecimal(key, strValue, out value);
				if (!succeeded)
				{
					continue;
				}

				dictionary.Add(key, value);
			}
		}

		private bool ConvertToDecimal(string key, string strValue, out decimal value)
		{
			value = 0;

			if (string.IsNullOrEmpty(key) || string.IsNullOrEmpty(strValue))
			{
				return false;
			}

			if (key == "Size")
			{
				value = (decimal)ModelViewer.PokemonModelDataManager.PokemonSizeLabel[strValue];
				return true;
			}

      double doubleValue = 0.0;
			bool succeeded = double.TryParse(strValue, out doubleValue);

      if (succeeded)
      {
        value = (decimal)doubleValue;
      }

			return succeeded;
		}

		public string[,] cells = null;
		public Dictionary<string, decimal> dictionary = null;
	}
}
