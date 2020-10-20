using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;
using System.Xml;
using System.Xml.Serialization;

namespace ExcelXml
{
  public class ExcelXml
  {
    public static Type Deserialize<Type>(string filepath) where Type : class, IExcelXmlSerializer, new()
    {
      Type result = null;
      try
      {
        // 余計な情報が多いのでXmlSerializerではなくXmlDocumentにする
        var xmlDocument = new XmlDocument();
        xmlDocument.Load(filepath);

        // 名前空間登録
        var nm = new XmlNamespaceManager(xmlDocument.NameTable);
        nm.AddNamespace("ns", xmlDocument.DocumentElement.NamespaceURI);

        // １枚目のワークシートのTable/Rowを取得
        var rowList = xmlDocument.SelectNodes("/ns:Workbook/ns:Worksheet[1]/ns:Table/ns:Row", nm);

        // 取得できなければ無視
        if (rowList.Count == 0)
        {
          return null;
        }

        var tmpCells = new List<string>[rowList.Count];
        var maxCols = -1;

        for (var rowIndex = 0; rowIndex < rowList.Count; ++rowIndex)
        {
          var row = rowList[rowIndex];

          // Cell取得
          var cellList = row.SelectNodes("ns:Cell", nm);

          var cells = new List<string>();
          maxCols = Math.Max(maxCols, cellList.Count);

          for (var cellIndex = 0; cellIndex < cellList.Count; ++cellIndex)
          {
            var cell = cellList[cellIndex];

            // Data取得
            var data = cell.SelectSingleNode("ns:Data", nm);

            cells.Add((data == null) ? null : data.FirstChild.Value);
          }

          tmpCells[rowIndex] = cells;
        }

        var cell2d = new string[rowList.Count, maxCols];
        for (var row = 0; row < tmpCells.Length; ++row)
        {
          for (var col = 0; col < tmpCells[row].Count; ++col)
          {
            cell2d[row, col] = tmpCells[row][col];
          }
        }

        result = new Type();
        result.XmlDocument = xmlDocument;
        result.DeserializeExcel(cell2d);
      }
      catch
      {
        // 失敗するかもしれない
      }

      return result;
    }

    public static bool Serialize<Type>(string filepath, Type obj) where Type : class, IExcelXmlSerializer, new()
    {
      if (obj.XmlDocument == null)
      {
        return false;
      }

      try
      {
        // 余計な情報が多いのでXmlSerializerではなくXmlDocumentにする
        var xmlDocument = obj.XmlDocument;
        if (xmlDocument == null)
        {
          return false;
        }

        var cell2d = obj.Cell2d;
        if (cell2d == null)
        {
          return false;
        }

        // 名前空間登録
        var nm = new XmlNamespaceManager(xmlDocument.NameTable);
        nm.AddNamespace("ns", xmlDocument.DocumentElement.NamespaceURI);

        // １枚目のワークシートのTableを取得
        var table = xmlDocument.SelectSingleNode("/ns:Workbook/ns:Worksheet[1]/ns:Table", nm);

        // １枚目のワークシートのTable/Rowを取得
        var rowList = xmlDocument.SelectNodes("/ns:Workbook/ns:Worksheet[1]/ns:Table/ns:Row", nm);

        // 全Rowを削除
        for (var rowIndex = 0; rowIndex < rowList.Count; ++rowIndex)
        {
          table.RemoveChild(rowList[rowIndex]);
        }

        // 行、列のサイズを計算
        var expandedRowCount = table.Attributes["ss:ExpandedRowCount"];
        var expandedColumnCount = table.Attributes["ss:ExpandedColumnCount"];
        expandedRowCount.Value = (1 + cell2d.GetLength(0)).ToString();
        expandedColumnCount.Value = (1 + cell2d.GetLength(1)).ToString();

        // 新しいRowを追加
        var ss = "urn:schemas-microsoft-com:office:spreadsheet";
        for (var rowIndex = 0; rowIndex < cell2d.GetLength(0); ++rowIndex)
        {
          var row = xmlDocument.CreateElement("Row", ss);

          var row_autoFit = xmlDocument.CreateAttribute("AutoFitHeight", ss);
          row_autoFit.Value = "0";
          row.Attributes.Append(row_autoFit);

          for (var colIndex = 0; colIndex < cell2d.GetLength(1); ++colIndex)
          {
            var cell = xmlDocument.CreateElement("Cell", ss);

            var cell_styleId = xmlDocument.CreateAttribute("StyleID", ss);
            cell_styleId.Value = (rowIndex == 0)
              ? "s62"
              : "s63";
            cell.Attributes.Append(cell_styleId);

            var cell_value = cell2d[rowIndex, colIndex];
            if (!string.IsNullOrEmpty(cell_value))
            {
              var data = xmlDocument.CreateElement("Data", ss);
              int result = 0;
              var isNumber = int.TryParse(cell_value, out result);
              var data_type = xmlDocument.CreateAttribute("Type", ss);
              data_type.Value = (isNumber)
                ? "Number"
                : "String";
              data.Attributes.Append(data_type);

              data.AppendChild(xmlDocument.CreateTextNode(cell_value));

              cell.AppendChild(data);
            }

            row.AppendChild(cell);
          }

          table.AppendChild(row);
        }

        xmlDocument.Save(filepath);
      }
      catch
      {
        // 失敗するかもしれない
      }

      return true;
    }
  }

  public interface IExcelXmlSerializer
  {
    XmlDocument XmlDocument { get; set; }
    string[,] Cell2d { get; }
    void DeserializeExcel(string[,] cells);
  }
}
