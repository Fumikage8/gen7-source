using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ModelViewer
{
  public class PokemonUtility
  {
    private static readonly string[] s_SeriesNames = new string[] 
    {
      "rg",
      "gs",
      "rs",
      "dp",
      "bw",
      "kujira",
      "sango",
      "niji"
    };

    public static string[] SortBySeriesName(string[] filePaths)
    {
      List<string> sortedNameList = new List<string>();
      foreach (string seriesName in s_SeriesNames)
      {
        var matchedSeriesNames = Array.FindAll(filePaths, path => path.Contains(seriesName) || path.Contains(seriesName.ToUpper()));
        Array.Sort(matchedSeriesNames);
        Array.ForEach(matchedSeriesNames, path => sortedNameList.Add(path));
      }

      return sortedNameList.ToArray();
    }
  }
}
