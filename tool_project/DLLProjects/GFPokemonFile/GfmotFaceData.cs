using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GFPokemonFile
{
  /// <summary>
  /// 表情のgfmotから抽出したデータ
  /// </summary>
  public class GfmotFaceData
  {
    /// <summary>
    /// 表情のモーションの種類(Skeltal, Uv, Visibility), 名称はgfmotのタグ名(SkeltalAnimation, UvAnimation, VisibilityAnimation)に関連.
    /// </summary>
    [Flags]
    public enum AnimationType
    {
      /// <summary>
      /// 表情がない
      /// </summary>
      None = 0,
      /// <summary>
      /// ジョイントによる表情制御
      /// </summary>
      Skeltal = 1 << 0,
      /// <summary>
      /// テクスチャによる表情制御
      /// </summary>
      Uv = 1 << 1,
      /// <summary>
      /// ビジビリティによる表情制御
      /// </summary>
      Visibility = 1 << 2
    }

    /// <summary>
    /// 表情を制御しているアニメーションの種類
    /// </summary>
    private readonly AnimationType m_AnimationType;
    /// <summary>
    /// フレーム数
    /// </summary>
    private readonly uint m_FrameSize;

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="animationType">表情を制御しているアニメーションの種類(Skeltal, Uv, Visibility)</param>
    /// <param name="frameSize">フレーム数</param>
    public GfmotFaceData(AnimationType animationType, uint frameSize)
    {
      m_AnimationType = animationType;
      m_FrameSize = frameSize;
    }

    /// <summary>
    /// ジョイント制御になっているか
    /// </summary>
    public bool HasJoint { get { return m_AnimationType.HasFlag(AnimationType.Skeltal); } }

    /// <summary>
    /// テクスチャ制御になっているか
    /// </summary>
    public bool HasUv { get { return m_AnimationType.HasFlag(AnimationType.Uv); } }

    /// <summary>
    /// フレーム数
    /// </summary>
    public uint FrameSize { get { return m_FrameSize; } }
  }
}
