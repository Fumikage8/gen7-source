using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Text.RegularExpressions;

namespace GFPokemonFile
{
  /// <summary>
  /// 全シリーズのディレクトリを収集するクラス
  /// </summary>
  public class SeriesDirectoryCollector
  {
    /// <summary>
    /// ポケモンのディレクトリを見分ける正規表現
    /// </summary>
    private readonly Regex m_PokemonDirectoryRegex;

    /// <summary>
    /// シリーズ全部のポケモンのディレクトリ
    /// </summary>
    private readonly string[] m_AllPokemonDirectories;

    /// <summary>
    /// シリーズ全部のポケモンのディレクトリのプロパティ
    /// </summary>
    public string[] AllPokemonDirectories
    {
      get { return m_AllPokemonDirectories; }
    }

    /// <summary>
    /// 開発番号順に並べるための並び替えクラス
    /// </summary>
    private class DevelopmentNoComparer : IComparer
    {
      /// <summary>
      /// ポケモンのディレクトリを見分ける正規表現
      /// </summary>
      private readonly Regex m_PokemonDirectoryRegex;

      /// <summary>
      /// コンストラクタ
      /// </summary>
      /// <param name="pokemonDirectoryRegex">ポケモンのディレクトリを見分ける正規表現</param>
      public DevelopmentNoComparer(Regex pokemonDirectoryRegex)
      {
        m_PokemonDirectoryRegex = pokemonDirectoryRegex;
      }

      /// <summary>
      /// 比較関数. 開発IDとフォルムIDを比較して小さい番号を返す
      /// </summary>
      /// <remarks>
      /// "pmNNNN_FF"が入っているかどうかを判定し, あれば番号に分解して比較
      /// </remarks>
      /// <param name="x">左辺値(文字列)</param>
      /// <param name="y">右辺値(文字列)</param>
      public int Compare(object x, object y)
      {
        //nullが最も小さいとする
        if (x == null && y == null)
        {
          return 0;
        }
        if (x == null)
        {
          return -1;
        }
        if (y == null)
        {
          return 1;
        }

        //String型以外の比較はエラー
        if (!(x is string))
        {
          throw new ArgumentException("string型でなければなりません。", "x");
        }
        else if (!(y is string))
        {
          throw new ArgumentException("string型でなければなりません。", "y");
        }

        // 文字列に変換し, ディレクトリの正規表現に則っているかをチェック
        string lValue = x as string;
        string rValue = y as string;

        bool isLeftMatch = m_PokemonDirectoryRegex.IsMatch(lValue);
        bool isRightMatch = m_PokemonDirectoryRegex.IsMatch(rValue);

        if (!isLeftMatch && !isRightMatch)
        {// どちらも一致していなければ0
          return 0;
        }
        if (!isLeftMatch)
        {
          return -1;
        }
        if (!isRightMatch)
        {
          return 1;
        }

        string leftPokemonNo = m_PokemonDirectoryRegex.Match(lValue).Value;
        string rightPokemonNo = m_PokemonDirectoryRegex.Match(rValue).Value;

        // "pm"の2文字を削除
        leftPokemonNo = leftPokemonNo.Replace("pm", "");
        rightPokemonNo = rightPokemonNo.Replace("pm", "");

        // '_'で前後を分割
        var leftNo = leftPokemonNo.Split('_');
        var rightNo = rightPokemonNo.Split('_');

        // 開発Noを比較
        int leftDevelopmentNo = int.Parse(leftNo[0]);
        int rightDevelopmentNo = int.Parse(rightNo[0]);
        int returnValue = leftDevelopmentNo - rightDevelopmentNo;
        if (returnValue != 0)
        {
          return returnValue;
        }

        // 開発Noが同じなら, フォルムNoを比較
        int leftFormNo = int.Parse(leftNo[1]);
        int rightFormNo = int.Parse(rightNo[1]);
        return leftFormNo - rightFormNo;
      }
    }

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="rootDirectory">ルートディレクトリ</param>
    public SeriesDirectoryCollector(string rootDirectory)
    {
      // 正規表現の初期化
      m_PokemonDirectoryRegex = new Regex(@"pm\d\d\d\d_\d\d");

      // まずrootディレクトリ以下のディレクトリを取得
      string[] seriesDirectories = Directory.GetDirectories(rootDirectory);

      if (seriesDirectories.Length == 0)
      {// 見つからなければ終了
        throw new DirectoryNotFoundException("指定したディレクトリ以下にpmNNNN_FFの命名規則に則ったディレクトリが見つかりませんでした");
      }

      // rootディレクトリ以下から, 正規表現に則ったディレクトリを抽出
      m_AllPokemonDirectories = new string[0];
      foreach (string seriesDirectory in seriesDirectories)
      {
        string[] directories = Directory.GetDirectories(seriesDirectory);
        directories = Array.FindAll(directories, path => m_PokemonDirectoryRegex.IsMatch(path));
        m_AllPokemonDirectories = m_AllPokemonDirectories.Concat(directories).ToArray();
      }

      // 番号順にソート
      Array.Sort(m_AllPokemonDirectories, new DevelopmentNoComparer(m_PokemonDirectoryRegex));

      // dummyの削除対象の"pm0000_00_d"を外す
      var dummyDirectoryRegex = new Regex(@"pm0000_00_d");
      m_AllPokemonDirectories = Array.FindAll(m_AllPokemonDirectories, path => !dummyDirectoryRegex.IsMatch(path));
    }

    /// <summary>
    /// 指定したファイル名に該当したファイルパスを収集する
    /// </summary>
    /// <param name="fileName">ファイル名</param>
    /// <param name="searchDirectory">検索対象のディレクトリ(pmNNNN_FF_name以下のディレクトリ名)</param>
    /// <returns>string[] 一致したファイルパス群</returns>
    public string[] CollectFilePathsWithFileName(string fileName, string searchDirectory)
    {
      string[] allPokemonDirectories = Array.ConvertAll(m_AllPokemonDirectories, directory => Path.Combine(directory, searchDirectory));
      string[] filePaths = new string[0];
      foreach (string pokemonDirectory in allPokemonDirectories)
      {
        string[] paths = Directory.GetFiles(pokemonDirectory, fileName);
        filePaths = filePaths.Concat(paths).ToArray();
      }
      return filePaths;
    }

    /// <summary>
    /// 指定した正規表現に該当したファイルパスを収集する
    /// </summary>
    /// <param name="regex">正規表現</param>
    /// <param name="searchDirectory">検索対象のディレクトリ(pmNNNN_FF_name以下のディレクトリ名)</param>
    /// <returns>string[] 正規表現に一致したファイルパス群</returns>
    public string[] CollectFilePathsWithRegex(Regex regex, string searchDirectory)
    {
      string[] allPokemonDirectories = Array.ConvertAll(m_AllPokemonDirectories, directory => Path.Combine(directory, searchDirectory));
      string[] filePaths = new string[0];
      foreach (string pokemonDirectory in allPokemonDirectories)
      {
        string[] paths = Directory.GetFiles(pokemonDirectory);
        paths = Array.FindAll(paths, path => regex.IsMatch(path));
        filePaths = filePaths.Concat(paths).ToArray();
      }
      return filePaths;
    }
  }
}
