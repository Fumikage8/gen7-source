#if !defined(GFL2_RENDERINGENGINE_CLR_RENDERER_UTIL_CLEARRENDERPATH_H_INCLUDED)
#define GFL2_RENDERINGENGINE_CLR_RENDERER_UTIL_CLEARRENDERPATH_H_INCLUDED
#pragma once

#include <renderingengine/include/renderer/gfl2_RenderingPipeLine.h>
#include <util/include/gfl2_FixedSizeContainer.h>

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {
 
//-----------------------------------------------------------
//! @brief 画面クリアパスクラス
//-----------------------------------------------------------
class ClearRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
  ClearRenderPath();
  ~ClearRenderPath();
  void Execute(const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext);
  void SetClearColor(const gfl2::gfx::Color& color);

private:
  gfl2::gfx::Color m_clearColor;
};

}}}}

#endif