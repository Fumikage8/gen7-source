#include "ViewerProc/include/FieldRo/MyRenderingPipeLine.h"

#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <RenderingEngine/include/Renderer/gfl2_MeshDrawTag.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>

#include <Heap/include/gfl2_Heap.h>

#if defined(GF_PLATFORM_CTR)
#include <nn/fs.h>
#include <nn/hio.h>
#endif


// niji system
#include "System/include/RenderPath/EdgeMapRenderPath.h"
#include "System/include/RenderPath/OutLinePostRenderPath.h"
#include "System/include/nijiAllocator.h"

// field
#include "ViewerProc/include/FieldStatic/FieldNode.h"

// field debug
///#include "Field/FieldRo/include/Debug/FieldDebugTypes.h"

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

GFL_NAMESPACE_BEGIN( Field );

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
#if FIELD_CUSTOM

SkyBoxRenderPath::SkyBoxRenderPath()
  : m_node( NULL )
{
	m_DrawableNodeContainer.CreateBuffer( 8 );
}

SkyBoxRenderPath::~SkyBoxRenderPath()
{
}

void SkyBoxRenderPath::AddRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_DrawableNodeContainer.push_back( pNode );
  if( m_node )
  {
    m_node->AddChild( pNode );
  }
}

void SkyBoxRenderPath::RemoveRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
  if( m_node )
  {
    m_node->RemoveChild( pNode );
  }
	m_DrawableNodeContainer.remove( pNode );
}

const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* SkyBoxRenderPath::DrawableNodeOverride()
{
	return &m_DrawableNodeContainer;
}

const gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* SkyBoxRenderPath::CameraOverride( void )
{
  return &m_camera; 
}

void SkyBoxRenderPath::CreateNode( gfl2::gfx::IGLAllocator *pAllocator )
{
  gfl2::renderingengine::scenegraph::instance::TransformNode *node = gfl2::renderingengine::scenegraph::instance::TransformNode::Create( pAllocator );
  node->SetWorldMatrix( gfl2::math::Matrix34::GetIdentity() );
  node->SetLocalScale( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );

  m_node = node;
}

void SkyBoxRenderPath::DeleteNode()
{
  GFL_SAFE_DELETE( m_node );
}

void SkyBoxRenderPath::Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetCamera(rDrawContext);
	SetDrawableNode();
	m_pDrawManager->Update();
}

void SkyBoxRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast( m_node);

  SetupDraw( rDrawContext, &GetCameraParamAddress()->m_Projection, &GetCameraParamAddress()->m_View );
  DrawManagerDraw(false);
}

void SkyBoxRenderPath::CallDisplayList( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast( m_node);

	SetCamera(rDrawContext);
  SetupDraw( rDrawContext );
  ReuseCommand();
}

#endif

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
#if FIELD_CUSTOM

OrthoRenderPath::OrthoRenderPath()
{
	m_DrawableNodeContainer.CreateBuffer( 4 );
}

OrthoRenderPath::~OrthoRenderPath()
{
}

void OrthoRenderPath::AddRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_DrawableNodeContainer.push_back( pNode );
}

void OrthoRenderPath::RemoveRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_DrawableNodeContainer.remove( pNode );
}

const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* OrthoRenderPath::DrawableNodeOverride()
{
	return &m_DrawableNodeContainer;
}

const gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* OrthoRenderPath::CameraOverride( void )
{
  return &m_camera; 
}

void OrthoRenderPath::Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetCamera(rDrawContext);
	SetDrawableNode();
	m_pDrawManager->Update();
}

void OrthoRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetRenderTargetOverride( rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil );

	GFGL::BeginScene();

	if ( GFGL::GetRenderTarget() )
	{
		if ( rDrawContext.m_DisplayWidth && rDrawContext.m_DisplayHeight )
		{
			GFGL::SetViewport(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
			GFGL::SetScissor(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
		}
		else
		{
			const gfx::Surface::SurfaceDesc			&rDesc = GFGL::GetRenderTarget()->GetSurfaceDesc();
			GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
			GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
		}

		//GFGL::ClearRenderTarget( rDrawContext.m_pRenderTarget, Color( 0.4f, 0.4f, 0.4f) );
		//GFGL::ClearDepthStencil( rDrawContext.m_pDepthStencil, 1.0f, 0xFF );
	}
	
	const IShaderDriver		*pShader = ShaderOverride();
	m_pDrawManager->SetOverrideShader( pShader );

#if defined(GF_PLATFORM_CTR)

	nn::gr::CommandBufferJumpHelper		*pCommandBuffer = GFGL::GetJumpHelper();
	bit32		*cmd = pCommandBuffer->GetCurrentPtr();

	{//proj
		nn::math::MTX44						*pMtx44;
		u32												tempBuffer[1024];
		u32												*pSeek = tempBuffer;

    pMtx44 = (nn::math::MTX44*)&GetCameraParamAddress()->m_Projection;
		cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 86, *pMtx44 );
	}

	{//view
		nn::math::MTX34						*pMtx34;
		u32												tempBuffer[1024];
		u32												*pSeek = tempBuffer;

		pMtx34 = (nn::math::MTX34*)&GetCameraParamAddress()->m_View;

		cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, 90, *pMtx34 );
	}

	pCommandBuffer->Put(cmd);
#endif

	m_pDrawManager->Draw( false );

	GFGL::EndScene();
}

#endif

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
MainRenderPath::MainRenderPath()
{
	m_DrawableNodeContainer.CreateBuffer( 312 );
#if FIELD_CUSTOM
	m_DrawEnvNodeContainerForFog.CreateBuffer( 4 );
#endif
}

MainRenderPath::~MainRenderPath()
{
#if FIELD_CUSTOM
	m_DrawEnvNodeContainerForFog.clear();
#endif
}

void MainRenderPath::AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_DrawableNodeContainer.push_back( pNode );
}

void MainRenderPath::RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_DrawableNodeContainer.remove( pNode );
}

#if FIELD_CUSTOM
const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* MainRenderPath::DrawableNodeOverride()
{
	return &m_DrawableNodeContainer;
}
#endif

#if FIELD_CUSTOM
void MainRenderPath::ApplyFogParam(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode)
{
  if( !pModelInstanceNode )
  {
    return;
  }

  u32 materialCount = pModelInstanceNode->GetMaterialNum();
  for(u32 materialIndex=0; materialIndex<materialCount; ++materialIndex)
  {
    gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode*                 materialInstanceNode               = pModelInstanceNode->GetMaterialInstanceNode(materialIndex);
    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode*                 materialResourceNode               = const_cast<gfl2::renderingengine::scenegraph::resource::MaterialResourceNode*>(materialInstanceNode->GetMaterialResourceNode());
    gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* materialInstanceNodeAttributeParam = materialInstanceNode->GetAttributeParam();
    materialInstanceNodeAttributeParam->m_FogEnable = true;
  }
  
  for( u32 envNo = 0; envNo < m_DrawEnvNodeContainerForFog.size(); ++envNo )
  {
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pEnv = m_DrawEnvNodeContainerForFog[ envNo ];
    if( pEnv->UpdateMaterialFog(pModelInstanceNode) )
    {
      break;
    }
  }
}
#endif

void MainRenderPath::ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  // クリアしない
}

void MainRenderPath::Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
#if FIELD_CUSTOM
#if defined(GF_PLATFORM_WIN32)
  for( u32 i=0 ; i< m_DrawableNodeContainer.size() ; i++ )
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstaceNode = static_cast<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*>(m_DrawableNodeContainer[i]);
    ApplyFogParam( pModelInstaceNode );
  }
#endif
#endif

	SetCamera(rDrawContext);
	SetDrawableNode();
	m_pDrawManager->Update();
}

void MainRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  SetupDraw( rDrawContext );
  DrawManagerDraw(false);
}

void MainRenderPath::CallDisplayList( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetCamera(rDrawContext);
  SetupDraw( rDrawContext );
  ReuseCommand();
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
MyRenderingPipeLine::MyRenderingPipeLine(
  gfl2::heap::HeapBase* pHeap, 
  System::nijiAllocator*  pNijiAllocatorForEdgeMapTexture, 
  void*                   pEdgeMapResBuf,
  void*                   pOutLineResBuf,
  f32 screanWidth, 
  f32 screanHeight 
  ) :
  m_pEdgeMapSceneRenderPath( NULL )
  ,m_pOutLinePostSceneRenderPath( NULL )
{
	SceneRenderPath::InitDescription desc;
	m_SkyBoxRenderPath.Initialize( desc );
	m_OrthoRenderPath.Initialize( desc );
	m_MainRenderPath.Initialize( desc );


	desc.m_DrawManagerDesc.m_NodeStackSize = 120;
	desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;
  
  
  // アロケーターを差し替える
  // @todo 引数渡しを行えるようにして貰う
  gfl2::gfx::IGLAllocator* pOldAllocator = gfl2::gfx::GLMemory::GetAllocator();;
  gfl2::gfx::GLMemory::SetAllocator( pNijiAllocatorForEdgeMapTexture ); 

  //
  // EdgeMapPath生成
  //
  m_pEdgeMapSceneRenderPath = GFL_NEW( pHeap ) System::NijiEdgeMapSceneRenderPath(pNijiAllocatorForEdgeMapTexture, 256 );
  m_pEdgeMapSceneRenderPath->SetResource( pEdgeMapResBuf );
  m_pEdgeMapSceneRenderPath->CreateEdgeMapTexture( screanWidth, screanHeight );
  m_pEdgeMapSceneRenderPath->Initialize( desc );

  //
  // OutLinePath生成
  //
  m_pOutLinePostSceneRenderPath = GFL_NEW( pHeap ) System::NijiOutLinePostSceneRenderPath(pNijiAllocatorForEdgeMapTexture);
  m_pOutLinePostSceneRenderPath->SetResource( pOutLineResBuf );

  //
  // エッジマップテクスチャをOutLinePathに登録
  //
  for( int i = 0; i < poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Max; i++ )
  {
    const gfl2::gfx::Texture* pTexture = m_pEdgeMapSceneRenderPath->GetEdgeMapTexture( i );
    m_pOutLinePostSceneRenderPath->SetEdgeMapTexture( i, pTexture );
  }

  // フィールドで使用するのは基本IDエッジのみ。IDエッジのみであればエッジマップ自体無くていいが、nijiではポケファインダーで法線エッジを使用する
  m_pEdgeMapSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal, false );
  m_pOutLinePostSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal, false );


  gfl2::gfx::GLMemory::SetAllocator( pOldAllocator ); 

  // Skybox
  m_SkyBoxRenderPath.CreateNode( pNijiAllocatorForEdgeMapTexture );
}

MyRenderingPipeLine::~MyRenderingPipeLine()
{
  m_SkyBoxRenderPath.DeleteNode();
  GFL_SAFE_DELETE( m_pOutLinePostSceneRenderPath );
  GFL_SAFE_DELETE( m_pEdgeMapSceneRenderPath );
}

b32 MyRenderingPipeLine::StartRenderPath()
{
  // @todo
#if PM_DEBUG
  ///m_pEdgeMapSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal, FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_NORMAL_EDGE ) );
  ///m_pOutLinePostSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal, FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_NORMAL_EDGE ));
  ///m_pEdgeMapSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_ID_EDGE ) );
  ///m_pOutLinePostSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_ID_EDGE ));
#endif //#if PM_DEBUG


	m_RenderPathCnt = 0;
	return true;
}

RenderingPipeLine::IRenderPath* MyRenderingPipeLine::GetRenderPath()
{
	//前のパスの結果を踏まえて、パスをスキップすることも可能。

	switch( m_RenderPathCnt ){
#if FIELD_CUSTOM
  case 0:
		return &m_SkyBoxRenderPath;
		break;
#endif
	case 1:
		return m_pEdgeMapSceneRenderPath;
		break;
	case 2:
		return &m_MainRenderPath;
		break;
	case 3:
    return m_pOutLinePostSceneRenderPath;
    break;
#if FIELD_CUSTOM
  case 4:
    return &m_UtilTextRenderPath;
    break;
  case 5:
    return &m_BloomRenderPath;
    break;
  case 6:
    return &m_OrthoRenderPath;
    break;
#endif
	}
	return NULL;
}

b32 MyRenderingPipeLine::NextRenderPath()
{
	m_RenderPathCnt++;

#if FIELD_CUSTOM
  if ( m_RenderPathCnt == 7 )
#else
	if ( m_RenderPathCnt == 3 )
#endif
		return false;
	return true;
}

void MyRenderingPipeLine::AddSkyBoxRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_SkyBoxRenderPath.AddRenderingTarget( pNode );
}

void MyRenderingPipeLine::RemoveSkyBoxRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_SkyBoxRenderPath.RemoveRenderingTarget( pNode );
}

void MyRenderingPipeLine::AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
  m_pEdgeMapSceneRenderPath->AddEdgeRenderingTarget( pNode );
	m_MainRenderPath.AddEdgeRenderingTarget( pNode );
}

void MyRenderingPipeLine::RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_pEdgeMapSceneRenderPath->RemoveEdgeRenderingTarget( pNode );
	m_MainRenderPath.RemoveEdgeRenderingTarget( pNode );
}

void MyRenderingPipeLine::SetDepthTexture( gfl2::gfx::Texture* pTexture )
{
#if defined(GF_PLATFORM_CTR)
	m_pOutLinePostSceneRenderPath->SetEdgeMapTexture( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, pTexture );	
#endif
}

GFL_NAMESPACE_END( Field );
