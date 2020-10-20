using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace SimpleViewer
{
  public struct GfmdlResourcePathSet
  {
    public string gfmdlFilePath;      //!< @brief 変換前のgfmdlデータ // いらない
    public string gfbmdlFilePath; // path to converted_data
    public Tuple<string, string, string>[] shaderFilePathTuples; // converted_data
    public Tuple<string, PokemonModelDataManager.GfmdlTextureType>[] textureFilePathTuples; // converted_data
    public Tuple<string, PokemonModelDataManager.GfmdlTextureType>[] textureRareFilePathTuples; //converted_data
  }

  public class GfMdlResourcePathSetDictionary
  {
    private Dictionary<PokemonModelDataManager.ConvertExtensionType, GfmdlResourcePathSet> m_GfmdlResourcePathSets;

    public GfMdlResourcePathSetDictionary()
    {
      m_GfmdlResourcePathSets = new Dictionary<PokemonModelDataManager.ConvertExtensionType, GfmdlResourcePathSet>();
    }

    public GfmdlResourcePathSet this[PokemonModelDataManager.ConvertExtensionType type]
    {
      get { return m_GfmdlResourcePathSets[type]; }
      set { m_GfmdlResourcePathSets[type] = value; }
    }

    public void Add(PokemonModelDataManager.ConvertExtensionType type, GfmdlResourcePathSet pathSet)
    {
      m_GfmdlResourcePathSets.Add(type, pathSet);
    }

    public Tuple<string, string>[] CheckConvertedTextureFileSizeDiff(PokemonModelDataManager.ConvertExtensionType type)
    {
      List<Tuple<string, string>> diffTextureList = new List<Tuple<string, string>>();

      var textureTuples = m_GfmdlResourcePathSets[type].textureFilePathTuples;
      var rareTextureTuples = m_GfmdlResourcePathSets[type].textureRareFilePathTuples;

      if (rareTextureTuples == null)
      {// レアが設定されていなければ処理を打ち止め
        return diffTextureList.ToArray();
      }

      foreach (var textureTuple in textureTuples)
      {
        string textureFileName = Path.GetFileName(textureTuple.Item1);
        var rareTextureTuple = Array.Find(rareTextureTuples, tuple => Path.GetFileName(tuple.Item1) == textureFileName);
        if (rareTextureTuple == null)
        {
          continue;
        }

        FileInfo textureFileInfo = new FileInfo(textureTuple.Item1);
        FileInfo rareTextureFileInfo = new FileInfo(rareTextureTuple.Item1);
        if (textureFileInfo.Length == rareTextureFileInfo.Length)
        {
          continue;
        }

        diffTextureList.Add(new Tuple<string, string>(textureTuple.Item1, rareTextureTuple.Item1));
      }

      return diffTextureList.ToArray();
    }

    public string[] OutputShaderResourcePaths(PokemonModelDataManager.ConvertExtensionType type)
    {
      List<string> shaderResourcePathList = new List<string>();
      var shaderFilePathTuples = m_GfmdlResourcePathSets[type].shaderFilePathTuples;
      foreach (var tuple in shaderFilePathTuples)
      {
        shaderResourcePathList.Add(tuple.Item1);
        // ジオメトリシェーダはない場合があるのでチェック
        if (!string.IsNullOrEmpty(tuple.Item2))
        {
          shaderResourcePathList.Add(tuple.Item2);
        }
        shaderResourcePathList.Add(tuple.Item3);
      }

      return shaderResourcePathList.Distinct().ToArray();
    }

    public string[] OutputTextureResourcePaths(PokemonModelDataManager.ConvertExtensionType type)
    {
      List<string> textureResourcePathList = new List<string>();
      foreach (var tuple in m_GfmdlResourcePathSets[type].textureFilePathTuples)
      {
        textureResourcePathList.Add(tuple.Item1);
      }

      return textureResourcePathList.Distinct().ToArray();
    }

    public string[] OutputRareTextureResourcePaths(PokemonModelDataManager.ConvertExtensionType type)
    {
      List<string> textureResourcePathList = new List<string>();
      foreach (var tuple in m_GfmdlResourcePathSets[type].textureRareFilePathTuples)
      {
        textureResourcePathList.Add(tuple.Item1);
      }

      return textureResourcePathList.Distinct().ToArray();
    }

    public Int32 CalculateGfmdlFileSize(PokemonModelDataManager.ConvertExtensionType type)
    {
      FileInfo fileInfo = new FileInfo(m_GfmdlResourcePathSets[type].gfbmdlFilePath);
      return (Int32)fileInfo.Length;
    }

    public Int32 CalculateShaderFileSize(PokemonModelDataManager.ConvertExtensionType type)
    {
      Int32 fileSize = 0;
      var shaderFilePaths = OutputShaderResourcePaths(type);
      foreach (var path in shaderFilePaths)
      {
        FileInfo fileInfo = new FileInfo(path);
        fileSize += (Int32)fileInfo.Length;
      }
      return fileSize;
    }

    public Int32 CalculateTextureFileSize(PokemonModelDataManager.ConvertExtensionType type)
    {
      Int32 fileSize = 0;
      var textureFilePaths = OutputTextureResourcePaths(type);
      foreach (var path in textureFilePaths)
      {
        FileInfo fileInfo = new FileInfo(path);
        fileSize += (Int32)fileInfo.Length;
      }
      return fileSize;
    }
  }
}
