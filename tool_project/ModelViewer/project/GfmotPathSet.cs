using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace ModelViewer
{
  public class GfmotPathSet
  {
    //!< @brief モーションの種類
    public enum MotionType
    {
      BATTLE,
      KW,
      FIELD,
      Numberof
    }

    public static readonly Dictionary<MotionType, string> MotionTypeString =
      new Dictionary<MotionType,string>
      {
        { MotionType.BATTLE, "ba" },
        { MotionType.KW, "kw" },
        { MotionType.FIELD, "fi" }
      };

    private string[] m_GfmotFilePaths;
    public string[] GfmotFilePaths
    {
      get { return m_GfmotFilePaths; }
    }

    public GfmotPathSet()
    {
      m_GfmotFilePaths = new string[0];
    }

    public GfmotPathSet(string[] gfmotFilePaths)
    {
      m_GfmotFilePaths = gfmotFilePaths;
    }

    public Int32 Length
    {
      get { return m_GfmotFilePaths.Length; }
    }

    public Int32 CalculateBinarySize()
    {
      Int32 binarySize = 0;
      foreach (var path in m_GfmotFilePaths)
      {
        FileInfo fileInfo = new FileInfo(path);
        binarySize += (Int32)fileInfo.Length;
      }
      return binarySize;
    }

    public Int32 CalculateBinarySize(MotionType type)
    {
      Int32 binarySize = 0;
      string stringPregMatch = "_" + MotionTypeString[type] + "[0-9]{2}_";
      foreach (var path in m_GfmotFilePaths)
      {
        if (System.Text.RegularExpressions.Regex.IsMatch(path, stringPregMatch))
        {
          FileInfo fileInfo = new FileInfo(path);
          binarySize += (Int32)fileInfo.Length;
        }
      }
      return binarySize;
    }
  }
}
