#if !defined(GFL2_RENDERINGENGINE_RENDERER_UTIL_MAKEBLURIMAGEPATH_H_INCLUDED)
#define GFL2_RENDERINGENGINE_RENDERER_UTIL_MAKEBLURIMAGEPATH_H_INCLUDED
#pragma once

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <gfx/include/gfl2_DrawUtil.h>

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

//-----------------------------------------------------------
//! @brief 被写界深度設定用画像描画パスクラス
//-----------------------------------------------------------
class MakeBlurImagePath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
  /**
   * @brief パスの初期化情報
   */
  struct InitDescription
  {
    u32 m_ScreenWidth;                            //!< @brief スクリーンの横幅
    u32 m_ScreenHeight;                           //!< @brief スクリーンの縦幅
    InitDescription() : m_ScreenWidth(0), m_ScreenHeight(0){}
  };

  MakeBlurImagePath();
  virtual ~MakeBlurImagePath();

  void Execute( const RenderingPipeLine::DrawContext& rDrawContext );

  void Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc);

  /**
   * @brief 被写界深度の種類の設定(Front/Backの設定)
   * @param dofType 前面/背面の設定
   */
  void ChangeDofType(DofType dofType);

  /**
   * @brief 使用するレイヤーの枚数の設定
   * @param layerNum 使用するレイヤーの枚数
   */
  void SetAvailableLayerNum(const u32 layerNum);

  /**
   * @brief ぼやけ具合の設定
   * @param layerNo 描画するレイヤー番号
   * @param rangeDistance 影響を受ける範囲の距離
   * @param blurredCoef ぼやけ具合(0.0f～1.0f)
   */
  void SetFocusRange(u32 layerNo, f32 rangeDistance, f32 blurredCoef);

  /**
   * @brief 深度情報をアルファに書き込んだテクスチャ取得
   * @return const gfl2::gfx::Texture* 深度情報を書き込んだテクスチャ
   */
  const gfl2::gfx::Texture* GetDepthWrittenTexture() const;

  /**
   * @brief ビュー行列を設定
   */
  void SetViewMatrix(const gfl2::math::Matrix34& viewMatrix)
  {
    m_ViewMatrix = viewMatrix;
  }

  /**
   * @brief 射影行列を設定
   */
  void SetProjMatrix(const gfl2::math::Matrix44& projMatrix)
  {
    m_ProjMatrix = projMatrix;
  }

private:
  /**
   * @brief ブラーイメージを描画する際に必要となる情報(距離, ぼかし具合など)
   */
  struct BlurLayerInfo
  {
    f32 m_RangeDistance;   //!< @brief 被写界深度の影響範囲
    f32 m_BlurredCoef;     //!< @brief ぼやけ具合(0.0f～1.0f)
  };

  static const gfl2::gfx::CompareFunc s_DofCompareFunc[DofType::NumberOf];  //!< @brief 被写界深度描画時の深度の比較設定
  DofType m_DofType;                                  //!< @brief 被写界深度画像描画時の設定(Front/Back)
  f32 m_FocusLength;                                  //!< @brief 焦点距離
  u32 m_ScreenWidth;                                  //!< @brief スクリーンの横幅
  u32 m_ScreenHeight;                                 //!< @brief スクリーンの縦幅
  u32 m_FrameBufferWidth;                             //!< @brief フレームバッファの横幅
  u32 m_FrameBufferHeight;                            //!< @brief フレームバッファの縦幅
  static const u32 VERTEX_DATA_ARRAY_LENGTH = 6;      //!< @brief 矩形を描画するための頂点数
  gfl2::gfx::DrawUtil::VertexFormat m_VertexDataArray[VERTEX_DATA_ARRAY_LENGTH];  // !< @brief 矩形を描画するための頂点配列

  static const u32 LAYER_MAX = 4;                     //!< @brief レイヤーの枚数
  u32 m_AvailableLayerNum;                            //!< @brief 有効なレイヤーの枚数
  BlurLayerInfo m_BlurLayerInfoArray[LAYER_MAX];      //!< @brief 各レイヤーのブラー情報
  const gfl2::gfx::Texture* m_pDepthWrittenTexture;   //!< @brief 深度テストに合格した点にアルファを書き込むテクスチャ
  const gfl2::gfx::Texture* m_pCopyColorBufferTexture;//!< @brief カラーバッファに描かれている内容をコピーするテクスチャ
  const gfl2::gfx::Texture* m_pScreenCapturedTexture; //!< @brief 画面をコピーする用のテクスチャ

  gfl2::math::Matrix34 m_ViewMatrix;                  //!< @brief ビュー行列
  gfl2::math::Matrix44 m_ProjMatrix;                  //!< @brief 射影行列
};

}}}}

#endif
