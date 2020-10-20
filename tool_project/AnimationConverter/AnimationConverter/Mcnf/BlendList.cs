using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Serialization;

namespace Mcnf
{
  public class BlendList : ExcelXml.IExcelXmlSerializer
  {
    public XmlDocument XmlDocument { get; set; }
    public string[,] Cell2d { get { return this.cells; } }

    public void DeserializeExcel(string[,] cells)
    {
      // オリジナルのセルを保存
      this.cells = cells;
      this.blendPairs = new BlendPair[cells.GetLength(0) - 1, cells.GetLength(1) - 1];

      for (var row = 1; row < cells.GetLength(0); ++row)
      {
        var from = cells[row, 0];
        for (var col = 1; col < cells.GetLength(1); ++col)
        {
          var to = cells[0, col];
          var blendPair = new BlendPair();
          blendPair.from = from;
          blendPair.to = to;

          if (!int.TryParse(cells[row, col], out blendPair.frame))
          {
            blendPair.frame = -1;
          }

          this.blendPairs[row - 1, col - 1] = blendPair;
        }
      }
    }

    public void ResizeCells(int rowCount, int colCount)
    {
      var newCells = new string[rowCount, colCount];
      for (var row = 0; row < this.cells.GetLength(0); ++row)
      {
        if (row >= rowCount)
        {
          continue;
        }

        for (var col = 0; col < this.cells.GetLength(1); ++col)
        {
          if (col >= colCount)
          {
            continue;
          }

          newCells[row, col] = this.cells[row, col];
        }
      }

      this.cells = newCells;
    }

    public string[,] cells = null;
    public BlendPair[,] blendPairs = null;
  }

  public class BlendPair
  {
    public string from;
    public string to;
    public int frame;
  }
}
