//======================================================================
/**
 * @file	ModelRenderPath.cpp
 * @brief	モデル描画パス
 * @author	ariizumi
 * @data	15/05/23
 */
//======================================================================

#include <System/include/RenderPath/ModelRenderPath.h>


GFL_NAMESPACE_BEGIN(System)

ModelRenderPath::ModelRenderPath(gfl2::gfx::IGLAllocator *allocator,int modelNum)
:mGlAllocator(allocator)
{
	mNodeContainer.CreateBuffer( modelNum );
}

ModelRenderPath::~ModelRenderPath()
{

}

void ModelRenderPath::AddNode( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	if( mNodeContainer.free_size() )
  {
	  mNodeContainer.push_back( pNode );
  }
  else
  {
    GFL_ASSERT_MSG(0,"ModelRenderPath:モデルの個数がオーバーしました。\n");
  }
}

void ModelRenderPath::RemoveNode( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
  mNodeContainer.remove( pNode );
}

const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* ModelRenderPath::DrawableNodeOverride()
{
	return &mNodeContainer;
}

void ModelRenderPath::Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetCamera(rDrawContext);
	SetDrawableNode();
	m_pDrawManager->Update();
}

void ModelRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );

	gfl2::gfx::GFGL::BeginScene();

	if ( gfl2::gfx::GFGL::GetRenderTarget() )
	{
		if ( rDrawContext.m_DisplayWidth && rDrawContext.m_DisplayHeight )
		{
			gfl2::gfx::GFGL::SetViewport(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
			gfl2::gfx::GFGL::SetScissor(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
		}
		else
		{
			const gfl2::gfx::Surface::SurfaceDesc			&rDesc = gfl2::gfx::GFGL::GetRenderTarget()->GetSurfaceDesc();
			gfl2::gfx::GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
			gfl2::gfx::GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
		}
	}
	
	const gfl2::renderingengine::renderer::IShaderDriver		*pShader = ShaderOverride();
	m_pDrawManager->SetOverrideShader( pShader );

#if defined(GF_PLATFORM_CTR)

	nn::gr::CommandBufferJumpHelper		*pCommandBuffer = gfl2::gfx::GFGL::GetJumpHelper();
	bit32		*cmd = pCommandBuffer->GetCurrentPtr();

	{//proj
		nn::math::MTX44						*pMtx44;
		//u32												tempBuffer[1024];
		//u32												*pSeek = tempBuffer;

    pMtx44 = (nn::math::MTX44*)&(m_pDrawManager->GetProjectionMatrix());
		cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 86, *pMtx44 );
	}

	{//view
		nn::math::MTX34						*pMtx34;
		//u32												tempBuffer[1024];
		//u32												*pSeek = tempBuffer;

    pMtx34 = (nn::math::MTX34*)&(m_pDrawManager->GetViewMatrix());
		cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 90, *pMtx34 );
	}

	pCommandBuffer->Put(cmd);
#endif

	m_pDrawManager->Draw( false );

	gfl2::gfx::GFGL::EndScene();
}

#if 0
//実験中(使えません

FillRenderPath::FillRenderPath()
{
}

FillRenderPath::~FillRenderPath()
{

}


void FillRenderPath::Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
}

void FillRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );

  gfl2::gfx::GFGL::BeginScene();

	if ( gfl2::gfx::GFGL::GetRenderTarget() )
	{
    gfl2::gfx::Color col( 0.3f, 0.3f, 0.3f, 0.5f );
    gfl2::gfx::GFGL::ClearRenderTarget(col);
	}

	gfl2::gfx::GFGL::EndScene();
}
#endif
GFL_NAMESPACE_END(System)