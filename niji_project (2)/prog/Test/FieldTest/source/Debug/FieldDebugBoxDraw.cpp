//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldDebugBoxDraw.cpp
 *	@brief  AABBボックスの表示
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "test/FieldTest/include/debug/FieldDebugBoxDraw.h"

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
DebugBoxDraw::DebugBoxDraw() : 
  m_pResourceRootNode(NULL),
  m_pModelInstanceNode(NULL),
  m_pPipeLine(NULL),
  m_IsVisible(false)
{
}

DebugBoxDraw::~DebugBoxDraw()
{
  Terminate();
}

// 初期化
void DebugBoxDraw::Initialize( gfl2::heap::HeapBase* pParentHeap, gfl2::renderingengine::scenegraph::resource::ResourceNode * pResource, Test::FieldTest::MyRenderingPipeLine* pPipeLine )
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
void DebugBoxDraw::Terminate( void )
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
void DebugBoxDraw::Update( const gfl2::math::AABB& aabb )
{
  // ON/OFF設定
  SetVisible( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_DEBUG_AABB_DRAW ] );


  if( !m_IsVisible ){ return ; }

  gfl2::math::Vector diff = aabb.GetDifference();
  gfl2::math::Vector center = aabb.GetCenter();

  gfl2::math::SRT localSRT = m_pModelInstanceNode->GetLocalSRT();
  
  localSRT.scale.Set( gfl2::math::Vector3( diff.x / 100.0f, diff.y / 100.0f, diff.z / 100.0f ) );
  localSRT.translate.Set( center.GetX(), center.GetY(), center.GetZ() );

  m_pModelInstanceNode->SetLocalSRT( localSRT );
}

// 描画ON・OFF
void DebugBoxDraw::SetVisible( bool flag )
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
