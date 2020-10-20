﻿/**
* @brief  フィールドの地形1ブロック
* @file   FieldTerrainDistantView.cpp
* @author ikuta_junya
* @data   2015.02.07
*/

// gfl2
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

// FieldTest
#include "test/FieldTest/include/Terrain/FieldTerrainDistantView.h"
#include "test/FieldTest/include/Debug/FieldDebugTypes.h"


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

const static u32 INVISIBLE_CULLING = 0x01;
const static u32 INVISIBLE_DEBUG_DRAW_SWITCH = 0x01 << 1;

/**
* @brief コンストラクタ
*
* @return 無し
*/ 
FieldTerrainDistantView::FieldTerrainDistantView( void ) : m_pModelInstanceNode( NULL ), m_InvisibleBit( 0 )
{ 
}

/**
* @brief デストラクタ
*
* @return 無し
*/
FieldTerrainDistantView::~FieldTerrainDistantView( void )
{

}

/**
* @brief 初期化処理
*
* @return 無し
*/
void FieldTerrainDistantView::Initialize( void )
{

}

/**
* @brief 終了処理
*
* @return 無し
*/
void FieldTerrainDistantView::Finalize( void )
{

}

/**
* @brief モデルを設定
*
* @param  pModel  モデル
*
* @return 無し
*/
void FieldTerrainDistantView::SetModel( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModel )
{
  m_pModelInstanceNode = pModel;
}

void FieldTerrainDistantView::UpdateCulling( const gfl2::math::Matrix44* pCullingMatrix, const gfl2::math::AABB& rAABB )
{
  if( m_pModelInstanceNode == NULL )
  {
     return;
  }

#if 1
  gfl2::math::AABB aabb = m_pModelInstanceNode->GetWorldAABB();

  // ＡＡＢＢをカリング判定する。
  if ( gfl2::math::AABB::Util::IsIntersectFrustum( pCullingMatrix, aabb ) )
  {
    // 内側
    m_InvisibleBit = ( m_InvisibleBit & (~INVISIBLE_CULLING) );
  }
  else
  {
    // 外側
    m_InvisibleBit = ( m_InvisibleBit | INVISIBLE_CULLING );
  }
#endif

  if( Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_TERRAIN_DISTANT_VIEW ] )
  {
    m_InvisibleBit = ( m_InvisibleBit & (~INVISIBLE_DEBUG_DRAW_SWITCH) );
  }
  else
  {
    m_InvisibleBit = ( m_InvisibleBit | INVISIBLE_DEBUG_DRAW_SWITCH );
  }

  UpdateVisibleImpl( !m_InvisibleBit );
}

void FieldTerrainDistantView::UpdateVisibleImpl( b32 visible )
{
  if( m_pModelInstanceNode == NULL )
  {
    return;
  }

  if( m_pModelInstanceNode->IsVisible() == visible )
  {
    return;
  }

  m_pModelInstanceNode->SetVisible( visible );

  // @todo 重いかも？
  if( visible )
  {
    gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pModelInstanceNode );
  }
  else
  {
    gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild( m_pModelInstanceNode );
  }

}


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );