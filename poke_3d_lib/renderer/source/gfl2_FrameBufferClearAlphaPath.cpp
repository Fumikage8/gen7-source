#include <renderer/include/gfl2_FrameBufferClearAlphaPath.h>

#include <gfx/include/gfl2_GFGLUtil.h>
#include <debug/include/gfl2_Assert.h>

#include <RenderingEngine/include/Renderer/gfl2_MeshDrawTag.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>

#if defined(GF_PLATFORM_CTR)
	#include <nn/fs.h>
	#include <nn/hio.h>
#else
	#include <gfx/include/WinOpenGL/gfl2_WinOpenGLStateObject.h>
#endif

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

namespace poke_3d { namespace renderer {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
FrameBufferClearAlphaPath::FrameBufferClearAlphaPath() : RenderingPipeLine::IRenderPath(),
  m_pBinModelRootNode( NULL ),
  m_pModelInstanceNode( NULL )
{
  
}

FrameBufferClearAlphaPath::~FrameBufferClearAlphaPath()
{
  GFL_SAFE_DELETE( m_pBinModelRootNode );
  GFL_SAFE_DELETE( m_pModelInstanceNode );
}

void FrameBufferClearAlphaPath::Initialize( gfl2::gfx::IGLAllocator* pAllocator, c8* gfbmdlBinary, c8* vertexShaderBinary, c8* fragmentShaderBinary )
{
  {
		scenegraph::resource::GfBinaryMdlData							resourceData;

		resourceData.SetModelData( gfbmdlBinary );

		m_pBinModelRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &resourceData );	
	}

	{
    c8*       pShaderBinarys[] = { vertexShaderBinary, fragmentShaderBinary };

    for( s32 i = 0; i < 2; ++i )
		{
			scenegraph::resource::GfBinaryShdData							resourceData;

			resourceData.SetShaderData( pShaderBinarys[i] );
			scenegraph::resource::ResourceNode		*pBinShdNode = scenegraph::resource::ResourceManager::CreateResourceNode( pAllocator, &resourceData );

			m_pBinModelRootNode->AddChild( pBinShdNode );
		}
	}

  {
		m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( pAllocator, m_pBinModelRootNode );

		scenegraph::instance::DrawableNode::UpdateDrawTagInfo				info;
		m_pModelInstanceNode->UpdateDrawTag(info);

		scenegraph::SceneGraphManager::RemoveChild( m_pModelInstanceNode );
	}
}

void FrameBufferClearAlphaPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  GFGL::SetRenderTarget( rDrawContext.m_pRenderTarget );
	GFGL::SetDepthStencil( NULL );

	GFGL::BeginScene();

	const gfx::Surface::SurfaceDesc			&rDesc = GFGL::GetRenderTarget()->GetSurfaceDesc();
	GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);

	{
		RenderState			renderState;
		math::Matrix34		dummyMat34 = math::Matrix34::GetIdentity();
		math::Matrix44		dummyMat44 = math::Matrix44::GetIdentity();
			
		renderState.SetProjectionMatrix( &dummyMat44 );
		renderState.SetViewMatrix( &dummyMat34 );
		renderState.SetWorldMatrix( &dummyMat34 );

    {
			MeshDrawTag			*pTag = m_pModelInstanceNode->GetMeshDrawTag( 0 );
			pTag->Draw( renderState );
		}
  }

  GFGL::EndScene();
	GFGL::SetDepthStencil( rDrawContext.m_pDepthStencil );
}

}}
