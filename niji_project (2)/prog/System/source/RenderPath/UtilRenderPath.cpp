//======================================================================
/**
 * @file UtilRenderPath.cpp
 * @date 2016/03/09 20:52:36
 * @author ariizumi_nobuhiko
 * @brief 汎用レンダーパス群
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>

#include "System/include/RenderPath/UtilRenderPath.h"
GFL_NAMESPACE_BEGIN(System)

//デプスバッファクリアパス
void ClearDepthRenderPath::Execute(const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext)
{
  gfl2::gfx::GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );
  gfl2::gfx::GFGL::SetDepthStencil( rDrawContext.m_pDepthStencil );

  gfl2::gfx::GFGL::BeginScene();

  {
    const gfl2::gfx::Surface::SurfaceDesc			&rDesc = gfl2::gfx::GFGL::GetRenderTarget()->GetSurfaceDesc();
    gfl2::gfx::GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
    gfl2::gfx::GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
  }

  gfl2::gfx::GFGL::ClearDepthStencil( rDrawContext.m_pDepthStencil, 1.0f, 255 );

  gfl2::gfx::GFGL::EndScene();
}


GFL_NAMESPACE_END(System)
