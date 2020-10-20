#if !defined(GFL2_RENDERINGENGINE_RENDERER_UTIL_StretchBltFrameBufferPath_H_INCLUDED)
#define GFL2_RENDERINGENGINE_RENDERER_UTIL_StretchBltFrameBufferPath_H_INCLUDED
#pragma once

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_FixedSizeContainer.h>

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

//-----------------------------------------------------------
//! @brief フレームバッファを引き伸ばしコピーするパス
//-----------------------------------------------------------
class StretchBltFrameBufferPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:

  StretchBltFrameBufferPath();
  virtual ~StretchBltFrameBufferPath();

  //! @brief フレームバッファのコピー先テクスチャオブジェクトを生成
  //! @return なし
  void CreateFrameBufferTexture( gfl2::gfx::IGLAllocator* pAllocator, const gfl2::gfx::Surface *pFrameBuffer );

  //! @brief フレームバッファを引き伸ばしコピーしたテクスチャの取得
  //! @return テクスチャのポインタ
  gfl2::gfx::Texture* GetFrameBufferTexture() { return m_pFrameBufferCopy; }

private:

  virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

  gfl2::gfx::Texture        *m_pFrameBufferTexture;     //! フレームバッファ(テクスチャ)
  gfl2::gfx::Texture        *m_pFrameBufferCopy;        //! フレームバッファのコピー先

  f32                       m_ScaleWidth;
  f32                       m_ScaleHeight;

};

}}}}

#endif