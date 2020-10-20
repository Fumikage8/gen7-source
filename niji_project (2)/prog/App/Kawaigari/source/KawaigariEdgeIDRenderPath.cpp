//======================================================================
/**
 * @file	KawaigariEdgeIDRenderPath.C++
 * @brief	Kawaigari Edge ID Offscreen Buffer Render Code
 * @author	PETE
 * @data	2015.11.10
 */
//======================================================================
#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <debug/include/Gfl2_DebugPrint.h>

#include "KawaigariEdgeIDRenderPath.h"


GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariEdgeIDRenderPath :: KawaigariEdgeIDRenderPath(gfl2::gfx::IGLAllocator *allocator,int modelNum)
:mGlAllocator(allocator)
{
	mNodeContainer.CreateBuffer( modelNum , mGlAllocator);
  mpCamera=NULL;
  mRenderTexture = NULL;
  mOffscreenBuf = NULL;
}

KawaigariEdgeIDRenderPath :: ~KawaigariEdgeIDRenderPath()
{
  GFL_SAFE_DELETE( mRenderTexture );
}

void KawaigariEdgeIDRenderPath :: Dump(void)
{
  /*
  if(mOffscreenBuf==NULL) return;
  
  u8 *buf = (u8*)mOffscreenBuf;

  u8 r,g,b;


#if defined(GF_PLATFORM_CTR)

  u32 off = (512-320)*256*3;
  
  for(int y=0;y<=320;y+=8)
  {
    GFL_PRINT("\n");
    for(int x=0;x<256;x+=8)
    {
      r = buf[off+ ( y*256*3)+( (x)*3)];
      g = buf[off+ ( y*256*3)+((x)*3)+1];
      b = buf[off+ (y*256*3)+((x)*3)+2];

      GFL_PRINT("%02X%02X%02X ",b,g,r);
     // GFL_PRINT("%02X ",r);
    }
  }

#else
  for(int y=240;y>=0;y-=8)
  {
    GFL_PRINT("\n");
    for(int x=0;x<256;x+=8)
    {
      r = buf[ ( y*512*3)+(x*3)];
      g = buf[ ( y*512*3)+(x*3)+1];
      b = buf[(y*512*3)+(x*3)+2];

      GFL_PRINT("%02X%02X%02X ",b,g,r);
    }
  }
#endif

  GFL_PRINT("\n");
  */
}

void KawaigariEdgeIDRenderPath :: SetupRenderTargetTexture(u32 screenWidth,u32 screenHeight)
{
    u32 w=screenWidth,h=screenHeight;

    // Reverse Width & Height for CTR:
#if defined(GF_PLATFORM_CTR)
    h = screenWidth;
    w = screenHeight;
#endif

  mRenderTexture = gfl2::gfx::GFGL::CreateTexture( mGlAllocator,w, h, 1, gfl2::gfx::Usage::RenderTarget, gfl2::gfx::Format::A8R8G8B8, gfl2::gfx::Pool::Default );      
}

void KawaigariEdgeIDRenderPath :: AddNode( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	if( mNodeContainer.free_size() )
  {
	  mNodeContainer.push_back( pNode );
  }
  else
  {
    GFL_ASSERT_MSG(0,"KawaigariEdgeIDRenderPath:モデルの個数がオーバーしました。\n");
  }
}

void KawaigariEdgeIDRenderPath :: RemoveNode( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
  mNodeContainer.remove( pNode );
}


void KawaigariEdgeIDRenderPath :: AddSceneCamera(gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* pCamera)
{
  mpCamera = pCamera;
}

void KawaigariEdgeIDRenderPath :: RemoveSceneCamera(void)
{
  mpCamera = NULL;
}



const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* KawaigariEdgeIDRenderPath :: DrawableNodeOverride()
{
	return &mNodeContainer;
}

void KawaigariEdgeIDRenderPath :: Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetCamera(rDrawContext);
	SetDrawableNode();
	m_pDrawManager->Update();
}

void KawaigariEdgeIDRenderPath :: Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
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

    ClearRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );

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

  gfl2::gfx::GFGL::AddTransferLinearColorBufferCommand( mOffscreenBuf );
  
  

}


void KawaigariEdgeIDRenderPath::SetRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
{
  gfl2::gfx::GFGL::SetRenderTarget( mRenderTexture->GetSurface() );    
  gfl2::gfx::GFGL::SetDepthStencil( pDepthStencil );
}

void KawaigariEdgeIDRenderPath::ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
{
#if defined(GF_PLATFORM_WIN32)
  gfl2::gfx::GFGL::ClearRenderTarget( mRenderTexture->GetSurface(), gfl2::gfx::Color( 1.0f, 0.0f, 0.0f) );  
  gfl2::gfx::GFGL::ClearDepthStencil( pDepthStencil, 1.0f, 255 );
#elif defined(GF_PLATFORM_CTR)
  gfl2::gfx::GFGL::ClearRenderTargetDepthStencil( mRenderTexture->GetSurface(), gfl2::gfx::Color( 1.0f, 0.0f, 0.0f), pDepthStencil, 1.0f, 255 );  
#endif
}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

