using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Xml;
using System.IO;

namespace Mcnf
{
  class EachDae
  {
    public static EachDae Deserialize(string inputPath)
    {
      var result = new EachDae();

      var xmlDoc = new XmlDocument();
      xmlDoc.Load(inputPath);

      var childNodes = xmlDoc.DocumentElement.ChildNodes;

      for (var i = 0; i < childNodes.Count; ++i)
      {
        var childNode = childNodes.Item(i);

        var name = childNode.Name;
        var attr = childNode.Attributes;

        var value = attr.Item(0).Value;

        if (CompSetting.LevelDict.ContainsKey(value))
        {
          result.levelDict.Add(name, CompSetting.LevelDict[value]);
        }
      }

      return result;
    }

    public static void Serialize(string outputPath, EachDae eachDae)
    {
      var xmlDoc = new XmlDocument();
      var declation = xmlDoc.CreateXmlDeclaration("1.0", "UTF-8", null);
      var root = xmlDoc.CreateElement("root");

      foreach (var item in eachDae.levelDict)
      {
        var child = xmlDoc.CreateElement(item.Key);

        var rengeLevel = xmlDoc.CreateAttribute("RangeLevel");
        rengeLevel.Value = item.Value.ToString();

        child.Attributes.Append(rengeLevel);

        root.AppendChild(child);
      }

      xmlDoc.AppendChild(declation);
      xmlDoc.AppendChild(root);
      xmlDoc.Save(outputPath);
    }

    public void ChangeLevel(string name, CompSetting.Levels level)
    {
      if (this.levelDict.ContainsKey(name))
      {
        this.levelDict.Remove(name);
      }

      if (level == CompSetting.Levels.DEFAULT)
      {
        return;
      }

      this.levelDict.Add(name, level);
    }

    public CompSetting.Levels GetLevel(string name)
    {
      if (this.levelDict.ContainsKey(name))
      {
        return this.levelDict[name];
      }

      return CompSetting.Levels.DEFAULT;
    }

    private Dictionary<string, CompSetting.Levels> levelDict = new Dictionary<string, CompSetting.Levels>();
  }

  class CompSetting
  {
    public enum Levels
    {
      NO_ERROR,    // 誤差無し
      PRECISE,    // 正確
      DEFAULT,    // デフォルト
      ABOUT,      // 適当
      MOST_ABOUT,  // 超適当

      Count,
    }
    public static readonly Dictionary<string, Levels> LevelDict = new Dictionary<string, Levels>()
    {
      {"NO_ERROR", Levels.NO_ERROR},
      {"PRECISE", Levels.PRECISE},
      {"DEFAULT", Levels.DEFAULT},
      {"ABOUT", Levels.ABOUT},
      {"MOST_ABOUT", Levels.MOST_ABOUT},
    };

    public CompSetting()
    {
      this.Level = Levels.DEFAULT;
    }

    public Levels Level
    {
      get { return this.level; }
      set { this.level = value; }
    }

    public float GetTranslateRange()
    {
      return rangeTable[(int)this.level] * 0.1f;
    }
    public float GetRotationRange()
    {
      return (float)(rangeTable[(int)this.level] * Math.PI / 180.0);
    }
    public float GetScaleRange()
    {
      return rangeTable[(int)this.level] * 0.1f;
    }

    private static readonly float[] rangeTable = new float[(int)Levels.Count]
    {
      0.01f,  // NoError,    // 誤差無し
      0.2f,  // Precise,    // 正確
      0.3f,  // Default,    // デフォルト
      0.5f,  // About,      // 適当
      0.9f,  // MostAbout,  // 超適当
    };

    private Levels level;
  }
}
