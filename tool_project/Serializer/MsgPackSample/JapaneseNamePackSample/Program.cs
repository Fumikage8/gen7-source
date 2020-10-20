using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MsgPack;
using MsgPack.Serialization;
using GFExcelSerializer;
using GFFileIO;

namespace JapaneseNamePackSample
{
  class Program
  {
    public static Dictionary<int, string> CreatePokemonJapaneseNameDictioinary(string seriesDirectory)
    {
      Dictionary<int, string> pokemonJapaneseNameDictionary = new Dictionary<int, string>();
      string[] settingXmlDirectories = Directory.GetDirectories(seriesDirectory);

      foreach (var settingXmlDirectory in settingXmlDirectories)
      {
        string[] settingXmlFilePaths = Directory.GetFiles(settingXmlDirectory, "*_setting.xml");
        foreach (string settingXmlFilePath in settingXmlFilePaths)
        {
          var match_pmXXXX_YY = s_pmXXXX_YY.Match(settingXmlFilePath);
          string pmXXXX_YY = match_pmXXXX_YY.ToString();
          string fileName = Path.GetFileName(settingXmlFilePath);
          if (fileName != pmXXXX_YY + "_setting.xml")
          {
            continue;
          }

          // シリアライズ対象となるデータをexcelから取得
          var settingList = ExcelXml.Deserialize<SettingXmlSerializer>(settingXmlFilePath);
          string pokemonJapaneseName = settingList.Cell2d[2, 1];
          // pmXXXX_YYから番号とフォルム番号を取得
          string XXXX_YY = pmXXXX_YY.Replace("pm", "");
          string[] separateStrings = XXXX_YY.Split('_');
          int pokemonNo = int.Parse(separateStrings[0]);
          pokemonJapaneseNameDictionary[pokemonNo] = pokemonJapaneseName;
        }
      }

      return pokemonJapaneseNameDictionary;
    }

    //!< @todo : 正規表現をまとめたクラスを作ってそちらに移動させる
    //!< @brief pmXXXXを判定する正規表現
    public static readonly System.Text.RegularExpressions.Regex s_pmXXXX = new System.Text.RegularExpressions.Regex(@"pm\d\d\d\d");
    //!< @brief pmXXXX_YYを判定する正規表現
    public static readonly System.Text.RegularExpressions.Regex s_pmXXXX_YY = new System.Text.RegularExpressions.Regex(@"pm\d\d\d\d_\d\d");

    static int Main(string[] args)
    {
      string pokemonDataDirectory = "R:/home/niji/pokemon/data/rg/";
      var pokemonJapaneseNameDictionary = CreatePokemonJapaneseNameDictioinary(pokemonDataDirectory);

      {
        byte[] serializedBytes;
        int serializedSize;
        // 1. Dictionary(phpのmap)を用意して, シリアライズしてファイル出力
        // どのようなデータフォーマットになっているのかを調べる
        var seriarizer = MessagePackSerializer.Get<Dictionary<int, string>>();

        // MsgPackでシリアライズ
        using (var packedStream = new MemoryStream())
        {
          seriarizer.Pack(packedStream, pokemonJapaneseNameDictionary);

          // ストリームからバイト配列を取り出す
          serializedBytes = packedStream.GetBuffer();
          serializedSize = (int)packedStream.Length;
          Console.WriteLine("バイト長 : {0}", serializedBytes.Length);
        }

        // ファイルに出力
        BinaryFile.Write("serialized.msgpck", serializedBytes);

        // MsgPackでデシリアライズして結果を表示
        using (var unpackedStream = new MemoryStream(serializedBytes))
        {
          var unpackedData = seriarizer.Unpack(unpackedStream);

          foreach (var data in unpackedData)
          {
            Console.WriteLine("key : {0}, value : {1}", data.Key, data.Value);
          }
        }
      }

      {
        // 2. ファイル出力したシリアライズ後のデータからデシリアライズ
        // シリアライズしたファイルをデシリアライズしてbyte配列に書き込み
        var seriarizer = MessagePackSerializer.Get<Dictionary<int, string>>();

        // ファイルから読み込み
        var serializedBytes = BinaryFile.Read("serialized.msgpck");
        
        using (var stream = new MemoryStream())
        {
          stream.Write(serializedBytes, 0, serializedBytes.Length);
          stream.Seek(0, SeekOrigin.Begin);
          var unpackedData = seriarizer.Unpack(stream);

          foreach (var data in unpackedData)
          {
            Console.WriteLine("key : {0}, value : {1}", data.Key, data.Value);
          }
        }
      }

      Console.ReadLine();
      return 0;
    }
  }
}
