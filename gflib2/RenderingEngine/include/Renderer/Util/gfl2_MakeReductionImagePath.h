#if !defined(GFL2_RENDERINGENGINE_RENDERER_UTIL_MAKEREDUCTIONIMAGEPATH_H_INCLUDED)
#define GFL2_RENDERINGENGINE_RENDERER_UTIL_MAKEREDUCTIONIMAGEPATH_H_INCLUDED
#pragma once

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <gfx/include/gfl2_DrawUtil.h>

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

//-----------------------------------------------------------
//! @brief 縮小画像描画パスクラス
//-----------------------------------------------------------
class MakeReductionImagePath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
  /**
   * @brief 縮小画像作成パスの初期化情報
   */
  struct InitDescription
  {
    u32 m_ScreenWidth;                            //!< @brief スクリーンの横幅
    u32 m_ScreenHeight;                           //!< @brief スクリーンの縦幅
    const gfl2::gfx::Texture* m_pSourceTexture;   //!< @brief リサイズ対象のテクスチャ
    InitDescription() : m_ScreenWidth(0), m_ScreenHeight(0), m_pSourceTexture(NULL){}
  };

  MakeReductionImagePath();
  virtual ~MakeReductionImagePath();

  void Execute( const RenderingPipeLine::DrawContext& rDrawContext );

  void Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc);

  /**
   * @brief 縮小対象のテクスチャを設定
   * @param pSourceTexture 縮小対象のテクスチャ
   */
  void SetSourceTexture(const gfl2::gfx::Texture* pSourceTexture);

  /**
   * @brief 縮小後のテクスチャを取得
   * @return const gfl2::gfx::Texture* 縮小後のテクスチャ
   */
  const gfl2::gfx::Texture* GetDestinationTexture() const;

private:
  u32 m_ScreenWidth;                                //!< @brief スクリーンの横幅
  u32 m_ScreenHeight;                               //!< @brief スクリーンの縦幅
  u32 m_FrameBufferWidth;                           //!< @brief フレームバッファの横幅
  u32 m_FrameBufferHeight;                          //!< @brief フレームバッファの縦幅
  static const u32 VERTEX_DATA_ARRAY_LENGTH = 6;    //!< @brief 矩形を描画するための頂点数
  gfl2::gfx::DrawUtil::VertexFormat m_VertexDataArray[VERTEX_DATA_ARRAY_LENGTH];  // !< @brief 矩形を描画するための頂点配列
  const gfl2::gfx::Texture* m_pSourceTexture;       //!< @brief 描画対象となるテクスチャ
  const gfl2::gfx::Texture* m_pDestinationTexture;  //!< @brief 画面をコピーする用のテクスチャ
};

}}}}

#endif
