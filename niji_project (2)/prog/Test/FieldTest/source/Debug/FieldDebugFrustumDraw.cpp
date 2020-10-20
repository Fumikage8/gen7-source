//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldDebugFrustumDraw.cpp
 *	@brief  視錐台の表示
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "test/FieldTest/include/debug/FieldDebugFrustumDraw.h"

// リソース
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

// デバッグ

#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

#if PM_DEBUG
#include "test/FieldTest/include/debug/FieldDebugTypes.h"
#endif


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
// コンストラクタ/デストラクタ
DebugFrustumDraw::DebugFrustumDraw() : 
  m_pResourceRootNode(NULL),
  m_pModelInstanceNode(NULL),
  m_pPipeLine(NULL),
  m_IsVisible(false)
{
}

DebugFrustumDraw::~DebugFrustumDraw()
{
  Terminate();
}

// 初期化
void DebugFrustumDraw::Initialize( gfl2::heap::HeapBase* pParentHeap, gfl2::renderingengine::scenegraph::resource::ResourceNode * pResource, Test::FieldTest::MyRenderingPipeLine* pPipeLine )
{
  m_pResourceRootNode = pResource; 
  m_pPipeLine = pPipeLine;
  // インスタンス生成
  {
    m_pModelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( pResource );
    m_pModelInstanceNode->SetVisible( true );
    m_pPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode );
    m_IsVisible = true;
  }
}

// 破棄
void DebugFrustumDraw::Terminate( void )
{
  if( m_pModelInstanceNode )
  {
    m_pModelInstanceNode->SetVisible(false);
    m_pPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode );
    GFL_DELETE( m_pModelInstanceNode );
    m_pModelInstanceNode = NULL;
  }
}

// 更新処理
void DebugFrustumDraw::Update( const gfl2::math::Matrix34& view, const f32 far_len )
{
  // ON/OFF設定
  SetVisible( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_DEBUG_FRUSTUM_DRAW ] );

  if( !m_IsVisible ){ return ; }


  gfl2::math::Matrix34 invView = view.Inverse();

  gfl2::math::SRT localSRT = m_pModelInstanceNode->GetLocalSRT();
  
  localSRT.scale.Set( gfl2::math::Vector3( far_len / 10.0f, far_len / 10.0f, far_len / 10.0f ) );
  localSRT.rotation.MatrixToQuaternion(invView);
  localSRT.translate.Set( invView.GetElemPosition() );

  m_pModelInstanceNode->SetLocalSRT( localSRT );
}

// 描画ON・OFF
void DebugFrustumDraw::SetVisible( bool flag )
{
  if( m_IsVisible != flag ){
    m_pModelInstanceNode->SetVisible(flag);
    if( flag ){
      m_pPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode );
      gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pModelInstanceNode );

    }else{
      m_pPipeLine->RemoveEdgeRenderingTarget( m_pModelInstanceNode );
      gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild( m_pModelInstanceNode );

    }

    m_IsVisible = flag;
  }
}

GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )
