using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Text.RegularExpressions;

namespace GFPokemonFile
{
  /// <summary>
  /// Gfmotの表情データ読み取りクラス
  /// </summary>
  public class GfmotFaceDataReader
  {
    /// <summary>
    /// 表情のパーツの種類
    /// </summary>
    private enum FacePattern
    {
      Eye,
      Mouth
    }

    /// <summary>
    /// スケルタルの表情のデータ
    /// </summary>
    private struct SkeltalFaceData
    {
      public uint frame;      //!< @brief フレーム数
      public decimal value;   //!< @brief 設定値
      public decimal slope;   //!< @brief 傾斜角
    }

    /// <summary>
    /// eye01, eye02,...表情データに関連する正規表現
    /// </summary>
    private readonly Dictionary<FacePattern, Regex> m_FacePatternRegexDictionary;

    /// <summary>
    /// コンストラクタ
    /// </summary>
    public GfmotFaceDataReader()
    {
      // 使用する正規表現を初期化
      m_FacePatternRegexDictionary = new Dictionary<FacePattern, Regex>
      {
        { FacePattern.Eye, new Regex(@"pm\d\d\d\d_\d\d_eye0\d.gfmot") },
        { FacePattern.Mouth, new Regex(@"pm\d\d\d\d_\d\d_mouth0\d.gfmot") },
      };
    }

    /// <summary>
    /// 表情のgfmot(eye01.gfmot, mouth01.gfmotなど)からGfmotFaceDataを作成
    /// </summary>
    /// <param name="gfmotFilePath">ファイルパス</param>
    /// <returns>GfmotFaceData GfmotFaceDataオブジェクト. 生成できなかった場合はnull.</returns>
    public GfmotFaceData CreateFaceDataFromGfmot(string gfmotFilePath)
    {
      if (!m_FacePatternRegexDictionary.Any(pair => pair.Value.IsMatch(gfmotFilePath)))
      {// 命名規則に則っているものがなければnull
        return null;
      }

      // gfmotファイルからGfmotFaceDataに必要なデータを抽出
      GfmotFaceData.AnimationType animationType = GfmotFaceData.AnimationType.None;
      uint frameSize = 0;
      XmlDocument gfmotDocument = new XmlDocument();
      try
      {
        gfmotDocument.Load(gfmotFilePath);
      }
      catch (Exception e)
      {
        Console.WriteLine(e.StackTrace);
        return null;
      }

      // ジョイントが存在しているのかどうかをSkeltalAnimation/Nodeを手繰って確認
      // (pm0584_00_baivanillaのeye01.gfmotのようにSkeltalAnimationタグはあるのに空というケースもある)
      var skeltalAnimationNodes = gfmotDocument.SelectNodes("/GfMotion/SkeltalAnimation/Node");
      animationType |= (skeltalAnimationNodes.Count > 0) ? GfmotFaceData.AnimationType.Skeltal : GfmotFaceData.AnimationType.None;

      // UVが使われているかどうかを判別して, あったらフレーム数を取得
      var uvAnimationNodes = gfmotDocument.SelectNodes("/GfMotion/UvAnimation");
      animationType |= (uvAnimationNodes.Count > 0) ? GfmotFaceData.AnimationType.Uv : GfmotFaceData.AnimationType.None;

      if (animationType == GfmotFaceData.AnimationType.None)
      {// ジョイントもUVもなければこの時点でオブジェクトを生成して終了
        return new GfmotFaceData(animationType, frameSize);
      }

      if (!animationType.HasFlag(GfmotFaceData.AnimationType.Skeltal))
      {// ジョイントだけがないのであればUVのフレームからオブジェクトを生成して終了
        var gfmotNode = gfmotDocument.SelectSingleNode("/GfMotion");
        frameSize = uint.Parse(gfmotNode.Attributes["FrameSize"].Value);
        return new GfmotFaceData(animationType, frameSize);
      }

      // ジョイントが存在しているのでSkeltalAnimationノードからフレームを抽出
      foreach (XmlNode skeltalAnimationNode in skeltalAnimationNodes)
      {
        foreach (XmlNode childNode in skeltalAnimationNode.ChildNodes)
        {// <RoteteX>, <RotateY>などの回転量を扱っているタグ
          SkeltalFaceData skeltalFaceData = new SkeltalFaceData();
          foreach (XmlNode hermiteKeyNode in childNode.ChildNodes)
          {
            if (hermiteKeyNode.Name != "HermiteKey")
            {// 名前が"OneFrameKey"などのケースもあるのでその場合は無視
              continue;
            }
            SkeltalFaceData data = new SkeltalFaceData();
            // 指数表記(-1.92E-09のような表記)の可能性もあるので指数表記を許容する
            data.frame = uint.Parse(hermiteKeyNode.Attributes["Frame"].Value, NumberStyles.Number | NumberStyles.AllowExponent);
            data.value = decimal.Parse(hermiteKeyNode.Attributes["Value"].Value, NumberStyles.Number | NumberStyles.AllowExponent);
            data.slope = decimal.Parse(hermiteKeyNode.Attributes["Slope"].Value, NumberStyles.Number | NumberStyles.AllowExponent);
            if ((data.value != skeltalFaceData.value) || (data.slope != skeltalFaceData.slope))
            {// 格納されているデータが前回と変わっていたら更新
              skeltalFaceData = data;
            }
          }
          // SkelDataに格納されているフレーム数と現在のフレーム数を比較し, 大きいほうを設定
          frameSize = Math.Max(frameSize, skeltalFaceData.frame);
        }
      }

      // SkeltalAnimationから抽出したframeSizeからオブジェクトを生成
      return new GfmotFaceData(animationType, frameSize);
    }
  }
}
