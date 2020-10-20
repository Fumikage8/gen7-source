#ifndef RENDERING_PIPELINE_H_INCLUDED
#define RENDERING_PIPELINE_H_INCLUDED

#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <renderingengine/include/renderer/Util/gfl2_UtilJointRenderPath.h>
#include <renderingengine/include/renderer/Util/gfl2_UtilMcnfRenderPath.h>
#include <RenderingEngine/include/Renderer/gfl2_MeshDrawTag.h>
#include <util/include/gfl2_FixedSizeContainer.h>

#include <FontRenderer/include/gfl2_FontRenderer.h>

// #include <framework/graphics/OutLinePostRenderPath.h>
// #include <framework/graphics/EdgeMapSceneRenderPath.h>

class IDRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath
{
  class IDShaderDriver : public gfl2::gfx::GLMemory, public gfl2::renderingengine::renderer::IShaderDriver
	{
	public:
		IDShaderDriver()
    {
    	m_pPixelShader = gfl2::gfx::GFGL::CreatePixelShaderFromFile( "../data/IdMap.gffsh", "PsMain", NULL, gfl2::gfx::FileFormat::ShaderCode );
    }
		virtual ~IDShaderDriver()
    {
			delete m_pPixelShader;
    }

		virtual b32 Apply( gfl2::renderingengine::renderer::RenderState& rRenderState, const gfl2::renderingengine::renderer::MeshDrawTag* pMeshDrawTag = NULL ) const
    {
      pMeshDrawTag->GetShaderDriver()->Apply( rRenderState );

      gfl2::gfx::GFGL::SetPixelShader( m_pPixelShader );

      return true;
    }

		gfl2::gfx::Shader			*m_pPixelShader;    // ピクセル・シェーダ
	};

public:
  IDRenderPath() : SceneRenderPath()
  {
    m_DrawableNodeContainer.CreateBuffer(128);

    m_pIdTexture = gfl2::gfx::GFGL::CreateTexture(1280, 720, 1, gfl2::gfx::Usage::RenderTarget, gfl2::gfx::Format::A8R8G8B8, gfl2::gfx::Pool::Default);
  }
  virtual ~IDRenderPath()
  {
		delete m_pIdTexture;
  }

#if 0
  virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
  {
    //gfl2::fontrenderer::FontRenderer::Update();

    SetCamera(rDrawContext);
    SetDrawableNode();

    SetRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );

    gfl2::gfx::GFGL::BeginScene();

    {
      const gfl2::gfx::Surface::SurfaceDesc			&rDesc = gfl2::gfx::GFGL::GetRenderTarget()->GetSurfaceDesc();
      gfl2::gfx::GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
    }

    ClearRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );

    const gfl2::renderingengine::renderer::IShaderDriver* pShader = ShaderOverride();
    m_pDrawManager->SetOverrideShader( pShader );
    m_pDrawManager->Draw();

    gfl2::gfx::GFGL::EndScene();
  }
#endif

  void AddDrawableNode(gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode)
  {
    m_DrawableNodeContainer.push_back(pNode);
  }

  gfl2::gfx::Texture* GetIdTexture()
  {
    return m_pIdTexture;
  }

private:
	virtual const gfl2::renderingengine::renderer::IShaderDriver* ShaderOverride()
  {
    return &m_IdShaderDriver;
  }

  virtual const gfl2::util::FixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* DrawableNodeOverride()
  {
    return &m_DrawableNodeContainer;
  }

  virtual void SetRenderTargetOverride(const gfl2::gfx::Surface* pRenderTarget, const gfl2::gfx::Surface* pDepthStencil)
  {
    gfl2::gfx::GFGL::SetRenderTarget(m_pIdTexture->GetSurface());
    gfl2::gfx::GFGL::SetDepthStencil(pDepthStencil);
  }

  virtual void ClearRenderTargetOverride(const gfl2::gfx::Surface* pRenderTarget, const gfl2::gfx::Surface* pDepthStencil)
  {
    gfl2::gfx::GFGL::ClearRenderTarget(m_pIdTexture->GetSurface(), gfl2::gfx::Color(0.0f, 0.0f, 0.0f));
    gfl2::gfx::GFGL::ClearDepthStencil(pDepthStencil, 1.0f, 255);
  }

  gfl2::util::FixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>			m_DrawableNodeContainer;
  gfl2::gfx::Texture																																							*m_pIdTexture;
  IDShaderDriver                                                                                  m_IdShaderDriver;
};

class KawaigariRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
public:
	KawaigariRenderingPipeLine();
	virtual ~KawaigariRenderingPipeLine();

	void AddIDObject( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode );
	void AddDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode );

  void DebugSwitchJointRender()
  {
    m_JointRenderEnabled ^= 1;
  }

  gfl2::gfx::Texture* GetIdTexture()
  {
    return m_IDRenderPath.GetIdTexture();
  }

private:
	virtual b32 StartRenderPath();
	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath();
	virtual b32 NextRenderPath();

	u32																																		m_RenderPathCnt;
	
  IDRenderPath                                                          m_IDRenderPath;
	gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath		m_SceneRenderPath;
	gfl2::renderingengine::renderer::RenderingPipeLine::PresentPath				m_PresentPath;

  gfl2::renderingengine::renderer::util::JointRenderPath                m_JointRenderPath;
  gfl2::renderingengine::renderer::util::McnfRenderPath                 m_McnfRenderPath;
  bool                                                                  m_JointRenderEnabled;
};


#endif
