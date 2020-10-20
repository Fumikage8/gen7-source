using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

namespace GFPokemonFile
{
  /// <summary>
  /// ポケモンのディレクトリ名から必要な情報を抽出するクラス
  /// </summary>
  public class PokemonDirectoryNameSplitter
  {
    /// <summary>
    /// コンストラクタ. 正規表現を初期化
    /// </summary>
    /// <remarks>msgpackでシリアライズしたシリーズ名のファイルを受け取って, rg, gsなどのシリーズ名が取れるようにする</remarks>
    public PokemonDirectoryNameSplitter()
    {
      // pmNNNN_FF_nameを抽出する正規表現で初期化
      m_PokemonDirectoryRegex = PokemonRegularExpression.DirectoryRegex;

      // @todo 2016/03/09時点でのシリーズ名. 今後を考慮しシリーズ名をシリアライズしたデータを受け取れるようにする
      // "egg"は正式シリーズ, 正式モデルとして扱う(実際に使用されているモデルデータ)
      m_SeriesNames = new string[] { "egg", "rg", "gs", "rs", "dp", "bw", "kujira", "sango", "niji" };
    }

    /// <summary>
    /// 受け取ったディレクトリ名を分解してPokemonDirectoryDataオブジェクトを作成
    /// <para>ディレクトリ名から開発番号, フォルム番号, 開発名, シリーズ名を抽出</para>
    /// </summary>
    public PokemonDirectoryData SplitToPokemonDirectoryData(string pokemonDirectory)
    {
      // ディレクトリを区切り文字で分解
      string[] splitPokemonDirectoryNames = pokemonDirectory.Split('\\');

      int index = Array.FindIndex(splitPokemonDirectoryNames, name => m_PokemonDirectoryRegex.IsMatch(name));
      if (index < 0)
      {// 見つからなければ例外を投げる
        throw new ArgumentException("pmNNNN_FF_name の命名規則に則ったディレクトリではありません.", "pokemonDirectory");
      }

      if (index == 0)
      {// 先頭の場合はシリーズのディレクトリ名がないということなので例外を投げる
        throw new ArgumentException("シリーズ名(rg, gsなど)のディレクトリ以下にあるディレクトリではありません.", "pokemonDirectory");
      }

      // 一つ上のシリーズのディレクトリを調査し, シリーズ名と一致しているかを調査
      // pmNNNN_FF_nameの一つ上のディレクトリがシリーズ名なので, シリーズ名に該当しているかを確認
      if (!m_SeriesNames.Any(name => name == splitPokemonDirectoryNames[index - 1]))
      {
        throw new ArgumentException("シリーズ名(rg, gsなど)のディレクトリ以下にあるディレクトリではありません.", "pokemonDirectory");
      }

      // 見つかったら正規表現に則った文字列を切り出し, 要素を抽出
      string pokemonDirectoryString = m_PokemonDirectoryRegex.Match(splitPokemonDirectoryNames[index]).Value;
      pokemonDirectoryString = pokemonDirectoryString.Replace("pm", "");
      string[] pokemonDirectoryDataArray = pokemonDirectoryString.Split('_');
      ushort developmentNo = ushort.Parse(pokemonDirectoryDataArray[0]);
      ushort formNo = ushort.Parse(pokemonDirectoryDataArray[1]);
      string developmentName = pokemonDirectoryDataArray[2];
      string seriesName = splitPokemonDirectoryNames[index - 1];

      return new PokemonDirectoryData(developmentNo, formNo, developmentName, seriesName);
    }

    /// <summary>
    /// ポケモンのディレクトリ用の正規表現
    /// </summary>
    private readonly Regex m_PokemonDirectoryRegex;

    /// <summary>
    /// シリーズのディレクトリ名(あくまで従来のシリーズが存在しているかのチェック用. 今後の追加されたシリーズ名はmsgpackのシリアライズデータを使う)
    /// </summary>
    private readonly string[] m_SeriesNames;
  }
}
