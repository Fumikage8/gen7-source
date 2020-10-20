#include "Field/FieldRo/include/MyRenderingPipeLine.h"

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

#include <model/include/gfl2_PokemonModel.h>

#if defined(GF_PLATFORM_CTR)
#include <nn/fs.h>
#include <nn/hio.h>
#endif


// niji system
#include "System/include/RenderPath/EdgeMapRenderPath.h"
#include "System/include/RenderPath/OutLinePostRenderPath.h"
#include "System/include/nijiAllocator.h"

// field
#include "Field/FieldStatic/include/FieldNode.h"

// field debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#if PM_DEBUG
#include "Field/FieldRo/include/Debug/FieldDebugCollisionRenderPath.h"
#endif  //#if PM_DEBUG

#if defined(GF_PLATFORM_CTR)
#include <RenderingEngine/include/Renderer/gfl2_CombinerCtrShaderDriver.h>
#endif

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

#define USE_TABLE_OVERRIDE 1
GFL_NAMESPACE_BEGIN( Field );

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
#if FIELD_CUSTOM

SkyBoxRenderPath::SkyBoxRenderPath(gfl2::gfx::IGLAllocator * allocator)
  : m_node( NULL )
{
	m_DrawableNodeContainer.CreateBuffer(8, allocator);

#if defined(GF_PLATFORM_CTR)
    m_ViewSpaceDiffMatrix = gfl2::math::Matrix34::GetIdentity();
#endif
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
  // @fix GFMMCat[13]:位置座標が大きい場合に天球の表示がカクつく、対処。ViewSpace描画を天球に適用。mainviewと差分行列が違うので、差し替えコードを追加。
#if defined(GF_PLATFORM_CTR)
  // ViewSpace用差分行列を一時的に上書き
  gfl2::math::Matrix34 prevDiffMatrix;
  prevDiffMatrix = gfl2::renderingengine::renderer::CombinerCtrShaderDriver::GetViewSpaceDiffMatrix();
  gfl2::renderingengine::renderer::CombinerCtrShaderDriver::SetViewSpaceDiffMatrix(m_ViewSpaceDiffMatrix);
#endif

  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFastFromChild( m_node);

  SetupDraw( rDrawContext, &GetCameraParamAddress()->m_Projection, &GetCameraParamAddress()->m_View );
  DrawManagerDraw(false);

#if defined(GF_PLATFORM_CTR)
  gfl2::renderingengine::renderer::CombinerCtrShaderDriver::SetViewSpaceDiffMatrix(prevDiffMatrix);
#endif
}

void SkyBoxRenderPath::CallDisplayList( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  // @fix GFMMCat[13]:位置座標が大きい場合に天球の表示がカクつく、対処。ViewSpace描画を天球に適用。mainviewと差分行列が違うので、差し替えコードを追加。
#if defined(GF_PLATFORM_CTR)
  // ViewSpace用差分行列を一時的に上書き
  gfl2::math::Matrix34 prevDiffMatrix;
  prevDiffMatrix = gfl2::renderingengine::renderer::CombinerCtrShaderDriver::GetViewSpaceDiffMatrix();
  gfl2::renderingengine::renderer::CombinerCtrShaderDriver::SetViewSpaceDiffMatrix(m_ViewSpaceDiffMatrix);
#endif
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFastFromChild( m_node);

	SetCamera(rDrawContext);
  SetupDraw( rDrawContext );
  ReuseCommand();
#if defined(GF_PLATFORM_CTR)
  gfl2::renderingengine::renderer::CombinerCtrShaderDriver::SetViewSpaceDiffMatrix(prevDiffMatrix);
#endif
}



WeatherBoardRenderPath::WeatherBoardRenderPath(gfl2::gfx::IGLAllocator * allocator, int modelNum )
{
  m_DrawableNodeContainer.CreateBuffer( modelNum, allocator );
}

WeatherBoardRenderPath::~WeatherBoardRenderPath()
{
}

void WeatherBoardRenderPath::AddRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
  m_DrawableNodeContainer.push_back( pNode );
}

void WeatherBoardRenderPath::RemoveRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
  m_DrawableNodeContainer.remove( pNode );
}

const gfl2::gfx::GLFixedSizeContainer<gfl2::renderingengine::scenegraph::instance::DrawableNode*>* WeatherBoardRenderPath::DrawableNodeOverride()
{
	return &m_DrawableNodeContainer;
}

const gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::Camera* WeatherBoardRenderPath::CameraOverride( void )
{
  return &m_camera; 
}

void WeatherBoardRenderPath::Update( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	SetCamera(rDrawContext);
	SetDrawableNode();
	m_pDrawManager->Update();
}

void WeatherBoardRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
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

    // デプスのクリアを行なう。
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

	GFGL::EndScene();

  DrawManagerDraw( false);
}

#endif

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
#if FIELD_CUSTOM

OrthoRenderPath::OrthoRenderPath(gfl2::gfx::IGLAllocator* allocator, int modelNum)
{
	m_DrawableNodeContainer.CreateBuffer( modelNum, allocator );
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

    // デプスのクリアを行なう。
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

	GFGL::EndScene();

  DrawManagerDraw( false);
}

#endif

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
MainRenderPath::MainRenderPath(gfl2::gfx::IGLAllocator* allocator, int modelNum)
{
	//m_DrawableNodeContainer.CreateBuffer( 312, allocator );
	m_DrawableNodeContainer.CreateBuffer( modelNum, allocator );
#if FIELD_CUSTOM
	m_DrawEnvNodeContainerForFog.CreateBuffer( 4, allocator );

  m_IsDepthClear = false;

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

void MainRenderPath::SetClearDepthFlag( b32 is_depth )
{
  m_IsDepthClear = is_depth;
}
#endif

void MainRenderPath::ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  // クリアしない
#if FIELD_CUSTOM
  if( m_IsDepthClear )
  {
		GFGL::ClearDepthStencil( pDepthStencil, 1.0f, 0xFF );
  }

#endif // FIELD_CUSTOM
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
  System::nijiAllocator* pAllocator,
  void*                   pEdgeMapResBuf,
  void*                   pOutLineResBuf,
  f32 screanWidth, 
  f32 screanHeight 
  ) :
   m_pEdgeMapSceneRenderPath( NULL )
  ,m_pOutLinePostSceneRenderPath( NULL )
  ,m_MainRenderPath(pAllocator, 312)
  ,m_SkyBoxRenderPath(pAllocator)
  ,m_WeatherBoardRenderPath(pAllocator, 8)
  ,m_OrthoRenderPath(pAllocator, 8)
  ,m_UtilTextRenderPath(pAllocator)
  ,m_pMakeBlurImagePath_Front( NULL )
  ,m_pMakeReductionImagePath_Front( NULL )
  ,m_pDofRenderPath_Front( NULL )
  ,m_pMakeBlurImagePath_Back( NULL )
  ,m_pMakeReductionImagePath_Back( NULL )
  ,m_pDofRenderPath_Back( NULL )
  ,m_EnableDofRender( false )
  ,m_pDofResourceNode( NULL )
  ,m_pDofModelInstanceNode_Front( NULL )
  ,m_pDofModelInstanceNode_Back( NULL )
  ,m_pDofModelData( NULL )
  ,m_pDofVertexShaderData( NULL )
  ,m_pDofFrontPixelShaderData( NULL )
  ,m_pDofBackPixelShaderData( NULL )
  ,m_pDofTextureData( NULL )
{

	SceneRenderPath::InitDescription desc;

	desc.m_DrawManagerDesc.m_NodeStackSize = 16;
	desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;
  desc.m_DrawManagerDesc.m_DrawEnvStackSize = 1;
  m_SkyBoxRenderPath.Initialize( pAllocator, desc );

  desc.m_DrawManagerDesc.m_NodeStackSize = 16;
	desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;
  desc.m_DrawManagerDesc.m_DrawEnvStackSize = 1;
  m_WeatherBoardRenderPath.Initialize( pAllocator, desc );

	desc.m_DrawManagerDesc.m_NodeStackSize = 16;
	desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;
  desc.m_DrawManagerDesc.m_DrawEnvStackSize = 1;
	m_OrthoRenderPath.Initialize( pAllocator, desc );

  // Main EdgeMap Path用Desc設定
	desc.m_DrawManagerDesc.m_NodeStackSize = 128;
	desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;
  desc.m_DrawManagerDesc.m_DrawEnvStackSize = 1;
	m_MainRenderPath.Initialize( pAllocator, desc );
  
  
  //
  // EdgeMapPath生成
  //
  m_pEdgeMapSceneRenderPath = GFL_NEW( pHeap ) System::NijiEdgeMapSceneRenderPath(pAllocator, desc.m_DrawManagerDesc.m_NodeStackSize );
  m_pEdgeMapSceneRenderPath->SetResource( pEdgeMapResBuf );
  m_pEdgeMapSceneRenderPath->CreateEdgeMapTexture(pAllocator, screanWidth, screanHeight );
  m_pEdgeMapSceneRenderPath->Initialize(pAllocator,  desc );

  //
  // OutLinePath生成
  //
  m_pOutLinePostSceneRenderPath = GFL_NEW( pHeap ) System::NijiOutLinePostSceneRenderPath(pAllocator);
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
  m_IsEdgeNormalMap = false;
  m_pEdgeMapSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal, m_IsEdgeNormalMap );
  m_pOutLinePostSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal, m_IsEdgeNormalMap );

  // Skybox
  m_SkyBoxRenderPath.CreateNode( pAllocator );

#if PM_DEBUG
  //
  // DebugCollisionRenderPath生成
  //
  gfl2::DebugCollisionRenderPath::InitDescription collision_desc;
  m_pDebugCollisionRenderPath = GFL_NEW( pHeap ) gfl2::DebugCollisionRenderPath();
  m_pDebugCollisionRenderPath->Initialize( collision_desc, pAllocator );
#endif  //#if PM_DEBUG

#if USE_TABLE_OVERRIDE
  m_MainRenderPath.GetDrawManager()->SetOrderingTableInfoOverride( m_pEdgeMapSceneRenderPath->GetDrawManager()->GetOrderingTableInfo() );
#endif

  m_RenderPathCnt = 0;
}

MyRenderingPipeLine::~MyRenderingPipeLine()
{
  m_SkyBoxRenderPath.DeleteNode();
  GFL_SAFE_DELETE( m_pOutLinePostSceneRenderPath );
  GFL_SAFE_DELETE( m_pEdgeMapSceneRenderPath );
#if PM_DEBUG
  GFL_SAFE_DELETE( m_pDebugCollisionRenderPath );
#endif  //#if PM_DEBUG
}

b32 MyRenderingPipeLine::StartRenderPath()
{
  // 法線エッジ　IDエッジのON・OFF
  bool is_edge_normal = m_IsEdgeNormalMap;

#if PM_DEBUG
  // IDエッジは基本ON　デバッグでのみOFFにする
  m_pEdgeMapSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_ID_EDGE ) );
  m_pOutLinePostSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_ID_EDGE ));

  // 法線エッジは仕様としてONのときのみ、デバッグフラグが有効になる。
  if(is_edge_normal) { is_edge_normal = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_ID_EDGE ); }
#endif //#if PM_DEBUG

  m_pEdgeMapSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal, is_edge_normal );
  m_pOutLinePostSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal, is_edge_normal );


	m_RenderPathCnt = 0;
	return true;
}

RenderingPipeLine::IRenderPath* MyRenderingPipeLine::GetRenderPath()
{
	//前のパスの結果を踏まえて、パスをスキップすることも可能。
#if FIELD_CUSTOM
#endif

	switch( m_RenderPathCnt ){
	case 0:
		return m_pEdgeMapSceneRenderPath;
#if FIELD_CUSTOM
  case 1:
		return &m_SkyBoxRenderPath;
#endif
	case 2:
		return &m_MainRenderPath;
	case 3:
    return m_pOutLinePostSceneRenderPath;
#if FIELD_CUSTOM
  case 4:
    return &m_EffectRenderPath;
  case 5:
    if( !m_EnableDofRender ){ return NULL; }
    return m_pMakeBlurImagePath_Back;
  case  6:
    if( !m_EnableDofRender ){ return NULL; }
    return m_pMakeReductionImagePath_Back;
  case 7:
    if( !m_EnableDofRender ){ return NULL; }
    return m_pDofRenderPath_Back;
  case 8:
    if( !m_EnableDofRender ){ return NULL; }
    return m_pMakeBlurImagePath_Front;
  case 9:
    if( !m_EnableDofRender ){ return NULL; }
    return m_pMakeReductionImagePath_Front;
  case 10:
    if( !m_EnableDofRender ){ return NULL; }
    return m_pDofRenderPath_Front;
  case 11:
    return &m_WeatherBoardRenderPath;
  case 12:
    return &m_BloomRenderPath;
  case 13:
    if( !m_MotionBlurPath.IsInitialized() )
    {
      return NULL;
    }
    return &m_MotionBlurPath;
  case 14:
    return &m_OrthoRenderPath;  // アイテムゲットの面
#if PM_DEBUG
  case 15:
    return m_pDebugCollisionRenderPath;
  case 16:
    return &m_DebugTextBeforeRenderPath;
  case 17:
    return &m_UtilTextRenderPath;
#endif  //#if PM_DEBUG
#endif
	}
	return NULL;
}

b32 MyRenderingPipeLine::NextRenderPath()
{
	m_RenderPathCnt++;

#if FIELD_CUSTOM
#if PM_DEBUG
  if ( m_RenderPathCnt == 18 )
#else
   if ( m_RenderPathCnt == 15 )
#endif //#if PM_DEBUG
#else
	if ( m_RenderPathCnt == 4 )
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

void MyRenderingPipeLine::AddWeatherBoardRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_WeatherBoardRenderPath.AddRenderingTarget( pNode );
}

void MyRenderingPipeLine::RemoveWeatherBoardRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
	m_WeatherBoardRenderPath.RemoveRenderingTarget( pNode );
}

void MyRenderingPipeLine::AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
  if( pNode )
  {
    m_pEdgeMapSceneRenderPath->AddEdgeRenderingTarget( pNode );
#if !USE_TABLE_OVERRIDE
    m_MainRenderPath.AddEdgeRenderingTarget( pNode );
#endif
  }
}

void MyRenderingPipeLine::RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
  if( pNode )
  {
    m_pEdgeMapSceneRenderPath->RemoveEdgeRenderingTarget( pNode );
#if !USE_TABLE_OVERRIDE
    m_MainRenderPath.RemoveEdgeRenderingTarget( pNode );
#endif
  }
}

void MyRenderingPipeLine::SetDepthTexture( gfl2::gfx::Texture* pTexture )
{
#if defined(GF_PLATFORM_CTR)
	m_pOutLinePostSceneRenderPath->SetEdgeMapTexture( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, pTexture );	
#endif
}



#if FIELD_CUSTOM
//----------------------------------------------------------------------------
/**
*  @brief  被写界深度レンダーパスを生成する
*/
//-----------------------------------------------------------------------------
void MyRenderingPipeLine::InitializeDofRenderPath( gfl2::heap::HeapBase *pHeap, const DofRenderPathInitDescription &desc )
{
  {
    gfl2::renderingengine::scenegraph::resource::ResourceNode *pResourceNode = NULL;
    // リソース：モデル
    m_pDofModelData = GFL_NEW( pHeap )gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData();
    m_pDofModelData->SetModelData((c8*)desc.pDofModelData);
    m_pDofResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( desc.pAllocator, m_pDofModelData );
    // リソース：VertexShader
    m_pDofVertexShaderData = GFL_NEW( pHeap )gfl2::renderingengine::scenegraph::resource::GfBinaryShdData();
    m_pDofVertexShaderData->SetShaderData((c8*)desc.pDofVertexShaderData);
    pResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( desc.pAllocator, m_pDofVertexShaderData );
    m_pDofResourceNode->AddChild( pResourceNode );
    // リソース：PixelShader
    m_pDofFrontPixelShaderData = GFL_NEW( pHeap )gfl2::renderingengine::scenegraph::resource::GfBinaryShdData();
    m_pDofFrontPixelShaderData->SetShaderData((c8*)desc.pDofFrontPixelShaderData);
    pResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( desc.pAllocator, m_pDofFrontPixelShaderData );
    m_pDofResourceNode->AddChild( pResourceNode );
    // リソース：PixelShadere
    m_pDofBackPixelShaderData = GFL_NEW( pHeap )gfl2::renderingengine::scenegraph::resource::GfBinaryShdData();
    m_pDofBackPixelShaderData->SetShaderData((c8*)desc.pDofBackPixelShaderData);
    pResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( desc.pAllocator, m_pDofBackPixelShaderData );
    m_pDofResourceNode->AddChild( pResourceNode );
    // リソース：Texture
    m_pDofTextureData = GFL_NEW( pHeap )gfl2::renderingengine::scenegraph::resource::GfBinaryTexData();
    m_pDofTextureData->SetTextureData((c8*)desc.pDofTextureData);
    pResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( desc.pAllocator, m_pDofTextureData );
    m_pDofResourceNode->AddChild( pResourceNode );
    // ModelInstance
    m_pDofModelInstanceNode_Front = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( desc.pAllocator, m_pDofResourceNode );
    gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild( m_pDofModelInstanceNode_Front );
    m_pDofModelInstanceNode_Back = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( desc.pAllocator, m_pDofResourceNode );
    gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild( m_pDofModelInstanceNode_Back );
  }
  if( !m_pMakeBlurImagePath_Front )
  {
    // 生成
    m_pMakeBlurImagePath_Front = GFL_NEW( pHeap )gfl2::renderingengine::renderer::util::MakeBlurImagePath;
    // 初期化
    gfl2::renderingengine::renderer::util::MakeBlurImagePath::InitDescription mbip_desc;
    mbip_desc.m_ScreenWidth  = desc.screen_width;
    mbip_desc.m_ScreenHeight = desc.screen_height;
    m_pMakeBlurImagePath_Front->Initialize( desc.pAllocator, mbip_desc );
    m_pMakeBlurImagePath_Front->ChangeDofType( gfl2::renderingengine::DofType::Front );
    m_pMakeBlurImagePath_Front->SetAvailableLayerNum( 3 );
  }
  if( !m_pMakeBlurImagePath_Back )
  {
    // 生成
    m_pMakeBlurImagePath_Back = GFL_NEW( pHeap )gfl2::renderingengine::renderer::util::MakeBlurImagePath;
    // 初期化
    gfl2::renderingengine::renderer::util::MakeBlurImagePath::InitDescription mbip_desc;
    mbip_desc.m_ScreenWidth  = desc.screen_width;
    mbip_desc.m_ScreenHeight = desc.screen_height;
    m_pMakeBlurImagePath_Back->Initialize( desc.pAllocator, mbip_desc );
    m_pMakeBlurImagePath_Back->ChangeDofType( gfl2::renderingengine::DofType::Back );
    m_pMakeBlurImagePath_Back->SetAvailableLayerNum( 3 );
  }
  if( !m_pMakeReductionImagePath_Front )
  {
    // 生成
    m_pMakeReductionImagePath_Front = GFL_NEW( pHeap )gfl2::renderingengine::renderer::util::MakeReductionImagePath;
    // 初期化
    gfl2::renderingengine::renderer::util::MakeReductionImagePath::InitDescription mrip_desc;
    mrip_desc.m_ScreenWidth     = desc.screen_width  / 2;
    mrip_desc.m_ScreenHeight    = desc.screen_height / 2;
    mrip_desc.m_pSourceTexture  = m_pMakeBlurImagePath_Front->GetDepthWrittenTexture();
    m_pMakeReductionImagePath_Front->Initialize( desc.pAllocator, mrip_desc );
  }
  if( !m_pMakeReductionImagePath_Back )
  {
    // 生成
    m_pMakeReductionImagePath_Back = GFL_NEW( pHeap )gfl2::renderingengine::renderer::util::MakeReductionImagePath;
    // 初期化
    gfl2::renderingengine::renderer::util::MakeReductionImagePath::InitDescription mrip_desc;
    mrip_desc.m_ScreenWidth     = desc.screen_width  / 2;
    mrip_desc.m_ScreenHeight    = desc.screen_height / 2;
    mrip_desc.m_pSourceTexture  = m_pMakeBlurImagePath_Back->GetDepthWrittenTexture();
    m_pMakeReductionImagePath_Back->Initialize( desc.pAllocator, mrip_desc );
  }
  if( !m_pDofRenderPath_Front )
  {
    // 生成
    m_pDofRenderPath_Front = GFL_NEW( pHeap )gfl2::renderingengine::renderer::util::DofRenderPath;
    // 破棄
    gfl2::renderingengine::renderer::util::DofRenderPath::InitDescription drp_desc;
    drp_desc.m_pTargetModelInstanceNode = m_pDofModelInstanceNode_Front;
    drp_desc.m_pSourceTexture           = m_pMakeReductionImagePath_Front->GetDestinationTexture();
    m_pDofRenderPath_Front->Initialize( desc.pAllocator, drp_desc );
    m_pDofRenderPath_Front->ChangeDofType( gfl2::renderingengine::DofType::Front );
    m_pDofRenderPath_Front->SetBlurScale(1.0f);
  }
  if( !m_pDofRenderPath_Back )
  {
    // 生成
    m_pDofRenderPath_Back = GFL_NEW( pHeap )gfl2::renderingengine::renderer::util::DofRenderPath;
    // 破棄
    gfl2::renderingengine::renderer::util::DofRenderPath::InitDescription drp_desc;
    drp_desc.m_pTargetModelInstanceNode = m_pDofModelInstanceNode_Back;
    drp_desc.m_pSourceTexture           = m_pMakeReductionImagePath_Back->GetDestinationTexture();
    m_pDofRenderPath_Back->Initialize( desc.pAllocator, drp_desc );
    m_pDofRenderPath_Back->ChangeDofType( gfl2::renderingengine::DofType::Back );
    m_pDofRenderPath_Back->SetBlurScale(1.0f);
  }
}
//----------------------------------------------------------------------------
/**
*  @brief  被写界深度レンダーパスを破棄する
*/
//-----------------------------------------------------------------------------
void MyRenderingPipeLine::FinalizeDofRenderPath()
{
  GFL_SAFE_DELETE( m_pDofModelInstanceNode_Front );
  GFL_SAFE_DELETE( m_pDofModelInstanceNode_Back );
  GFL_SAFE_DELETE( m_pDofResourceNode );
  GFL_SAFE_DELETE( m_pDofModelData );
  GFL_SAFE_DELETE( m_pDofVertexShaderData );
  GFL_SAFE_DELETE( m_pDofFrontPixelShaderData );
  GFL_SAFE_DELETE( m_pDofBackPixelShaderData );
  GFL_SAFE_DELETE( m_pDofTextureData );
  GFL_SAFE_DELETE( m_pDofRenderPath_Front );
  GFL_SAFE_DELETE( m_pMakeReductionImagePath_Front );
  GFL_SAFE_DELETE( m_pMakeBlurImagePath_Front );
  GFL_SAFE_DELETE( m_pDofRenderPath_Back );
  GFL_SAFE_DELETE( m_pMakeReductionImagePath_Back );
  GFL_SAFE_DELETE( m_pMakeBlurImagePath_Back );
}

//----------------------------------------------------------------------------
/**
*  @brief  DrawManager::スキニング精度アップ機能ON / OFF
*/
//-----------------------------------------------------------------------------
void MyRenderingPipeLine::SetViewSpaceRenderEnable( b32 flag )
{
  if(m_pEdgeMapSceneRenderPath) m_pEdgeMapSceneRenderPath->GetDrawManager()->ViewSpaceRenderEnable(flag);
  //if(m_pOutLinePostSceneRenderPath) m_pOutLinePostSceneRenderPath->GetDrawManager()->ViewSpaceRenderEnable(flag);
  m_MainRenderPath.GetDrawManager()->ViewSpaceRenderEnable(flag);
  // @fix GFMMCat[13]:位置座標が大きい場合に天球の表示がカクつく、対処。ViewSpace描画を天球に適用。
  m_SkyBoxRenderPath.GetDrawManager()->ViewSpaceRenderEnable(flag);
  //m_SkyBoxRenderPath;
  //m_OrthoRenderPath;
  //m_BloomRenderPath;
  //m_UtilTextRenderPath;
  //m_EffectRenderPath;
  //m_MotionBlurPath;

}


//----------------------------------------------------------------------------
/**
*  @brief 法線マップのON/OFFを行なう
*/
//-----------------------------------------------------------------------------
void MyRenderingPipeLine::SetEdgeNormalMapEnable( b32 flag )
{
  m_IsEdgeNormalMap = flag;
}

b32  MyRenderingPipeLine::IsEdgeNormalMapEnable() const
{
  return m_IsEdgeNormalMap;
}


#endif


//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  キャプチャー用
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
CaptureRenderingPipeLine::CaptureRenderingPipeLine(
  gfl2::heap::HeapBase* pHeap, 
  System::nijiAllocator*  pAllocator, 
  void*                   pEdgeMapResBuf,
  void*                   pOutLineResBuf,
  f32 screanWidth, 
  f32 screanHeight 
  ) :
   m_pEdgeMapSceneRenderPath( NULL )
  ,m_pOutLinePostSceneRenderPath( NULL )
  ,m_MainRenderPath(pAllocator, 16 )
  ,m_OrthoRenderPath(pAllocator, 8)
  ,m_pMotionBlurPath(NULL)
{
	SceneRenderPath::InitDescription desc;

	desc.m_DrawManagerDesc.m_NodeStackSize = 8;
	desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;

	m_OrthoRenderPath.Initialize(pAllocator, desc );
	m_MainRenderPath.Initialize(pAllocator, desc );
  m_MainRenderPath.SetClearDepthFlag(true);


  //
  // EdgeMapPath生成
  //
  m_pEdgeMapSceneRenderPath = GFL_NEW( pHeap ) System::NijiEdgeMapSceneRenderPath(pAllocator, desc.m_DrawManagerDesc.m_NodeStackSize );
  m_pEdgeMapSceneRenderPath->SetResource( pEdgeMapResBuf );
  m_pEdgeMapSceneRenderPath->CreateEdgeMapTexture(pAllocator, screanWidth, screanHeight );
  m_pEdgeMapSceneRenderPath->Initialize(pAllocator, desc );

  //
  // OutLinePath生成
  //
  m_pOutLinePostSceneRenderPath = GFL_NEW( pHeap ) System::NijiOutLinePostSceneRenderPath(pAllocator);
  m_pOutLinePostSceneRenderPath->SetResource( pOutLineResBuf );

  //
  // エッジマップテクスチャをOutLinePathに登録
  //
  for( int i = 0; i < poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Max; i++ )
  {
    const gfl2::gfx::Texture* pTexture = m_pEdgeMapSceneRenderPath->GetEdgeMapTexture( i );
    m_pOutLinePostSceneRenderPath->SetEdgeMapTexture( i, pTexture );
  }

  m_pEdgeMapSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal, true );
  m_pOutLinePostSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal, true );


#if FIELD_CUSTOM
  m_PokeShaderRenderPaths.CreateBuffer( pHeap, poke_3d::model::PokemonModel::COLORTEXTURE_MAX * 6);
#endif

  m_RenderPathCnt = 0;
}

CaptureRenderingPipeLine::~CaptureRenderingPipeLine()
{
  GFL_SAFE_DELETE( m_pOutLinePostSceneRenderPath );
  GFL_SAFE_DELETE( m_pEdgeMapSceneRenderPath );
}

b32 CaptureRenderingPipeLine::StartRenderPath()
{
#if PM_DEBUG
  m_pEdgeMapSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal, FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_NORMAL_EDGE ) );
  m_pOutLinePostSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal, FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_NORMAL_EDGE ));
  m_pEdgeMapSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_ID_EDGE ) );
  m_pOutLinePostSceneRenderPath->SetEnable( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_ID_EDGE ));
#endif //#if PM_DEBUG

	m_RenderPathCnt = 0;
	return true;
}

RenderingPipeLine::IRenderPath* CaptureRenderingPipeLine::GetRenderPath()
{
	//前のパスの結果を踏まえて、パスをスキップすることも可能。

#if FIELD_CUSTOM
  if( m_RenderPathCnt < m_PokeShaderRenderPaths.size())
  {
    return m_PokeShaderRenderPaths[m_RenderPathCnt];
  }
#endif

#if FIELD_CUSTOM
	switch( m_RenderPathCnt - m_PokeShaderRenderPaths.size() ){
#else
	switch( m_RenderPathCnt ){
#endif

	case 0:
		return m_pEdgeMapSceneRenderPath;
#if FIELD_CUSTOM
  case 1:
    return &m_OrthoRenderPath;
#endif // FIELD_CUSTOM
	case 2:
		return &m_MainRenderPath;
	case 3:
    return m_pOutLinePostSceneRenderPath;
#if FIELD_CUSTOM
  case 4:
    if( m_pMotionBlurPath && !m_pMotionBlurPath->IsInitialized() )
    {
      return NULL;
    }
    return m_pMotionBlurPath;
  case 5:
    return &m_EffectRenderPath;
#endif // FIELD_CUSTOM

	}
	return NULL;
}

b32 CaptureRenderingPipeLine::NextRenderPath()
{
	m_RenderPathCnt++;

#if FIELD_CUSTOM
  if ( m_RenderPathCnt == 6 + m_PokeShaderRenderPaths.size() )
#else
	if ( m_RenderPathCnt == 6 )
#endif
  {
		return false;
  }
	return true;
}

void CaptureRenderingPipeLine::AddEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
  if( pNode )
  {
    m_pEdgeMapSceneRenderPath->AddEdgeRenderingTarget( pNode );
    m_MainRenderPath.AddEdgeRenderingTarget( pNode );
  }
}

void CaptureRenderingPipeLine::RemoveEdgeRenderingTarget( gfl2::renderingengine::scenegraph::instance::DrawableNode* pNode )
{
  if( pNode )
  {
    m_pEdgeMapSceneRenderPath->RemoveEdgeRenderingTarget( pNode );
    m_MainRenderPath.RemoveEdgeRenderingTarget( pNode );
  }
}

void CaptureRenderingPipeLine::SetDepthTexture( gfl2::gfx::Texture* pTexture )
{
#if defined(GF_PLATFORM_CTR)
	m_pOutLinePostSceneRenderPath->SetEdgeMapTexture( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, pTexture );	
#endif
}

//----------------------------------------------------------------------------
/**
*  @brief  モーションブラーのレンダーパス生成、初期化
*/
//-----------------------------------------------------------------------------
void CaptureRenderingPipeLine::InitializeMotionBlurPath(
  gfl2::gfx::IGLAllocator*  pAllocator,
  gfl2::heap::HeapBase* pHeap,
  gfl2::fs::AsyncFileManager* pAsyncFileManager,
  u32 useTextureTarget
  )
{
  if( m_pMotionBlurPath == NULL)
  {
    m_pMotionBlurPath = GFL_NEW( pHeap) System::MotionBlur();

    // モーションブラーパスの初期化
    m_pMotionBlurPath->InitializeMotionBlur(
      pAllocator,
      pHeap,
      pAsyncFileManager,
      useTextureTarget
      );
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  モーションブラーのレンダーパス初期化待ち
*  @retval true 初期化完了
*  @retval false 初期化実行中 or 生成されていない
*/
//-----------------------------------------------------------------------------
bool CaptureRenderingPipeLine::WaitInitializeMotionBlurPath()
{
  if( m_pMotionBlurPath)
  {
    return m_pMotionBlurPath->InitializeMotionBlurUpdate();
  }
  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief  モーションブラーのレンダーパス破棄
*/
//-----------------------------------------------------------------------------
void CaptureRenderingPipeLine::TerminateMotionBlurPath()
{
  if( m_pMotionBlurPath)
  {
    m_pMotionBlurPath->Terminate();

    GFL_DELETE( m_pMotionBlurPath);
    m_pMotionBlurPath = NULL;
  }
}

GFL_NAMESPACE_END( Field );
