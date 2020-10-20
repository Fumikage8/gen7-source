#include <RenderingEngine/include/Renderer/Util/gfl2_ClearRenderPath.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

/**
 * @brief コンストラクタ
 */
ClearRenderPath::ClearRenderPath()
{
  m_clearColor = gfl2::gfx::Color(0.4f, 0.4f, 0.4f, 1.0f);
}

/**
 * @brief デストラクタ
 */
ClearRenderPath::~ClearRenderPath()
{
}

void ClearRenderPath::Execute(const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext)
{
  gfl2::gfx::GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );
  gfl2::gfx::GFGL::SetDepthStencil( rDrawContext.m_pDepthStencil );

  gfl2::gfx::GFGL::BeginScene();

  {
    const gfl2::gfx::Surface::SurfaceDesc			&rDesc = gfl2::gfx::GFGL::GetRenderTarget()->GetSurfaceDesc();
    gfl2::gfx::GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
    gfl2::gfx::GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
  }

  gfl2::gfx::GFGL::ClearRenderTarget( rDrawContext.m_pRenderTarget, m_clearColor );
  gfl2::gfx::GFGL::ClearDepthStencil( rDrawContext.m_pDepthStencil, 1.0f, 255 );
  
  gfl2::gfx::GFGL::EndScene();
}

void ClearRenderPath::SetClearColor(const gfl2::gfx::Color& color)
{
  m_clearColor = color;
}

}}}}