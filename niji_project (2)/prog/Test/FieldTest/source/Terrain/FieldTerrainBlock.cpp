/**
* @brief  フィールドの地形1ブロック
* @file   FieldTerrainBlock.cpp
* @author ikuta_junya
* @data   2015.02.07
*/

// gfl2
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

// FieldTest
#include "test/FieldTest/include/Terrain/FieldTerrainBlock.h"
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
FieldTerrainBlock::FieldTerrainBlock( void ) : m_LodState( LOD_HIGHT ), m_LodStateDraw( LOD_HIGHT ), m_InvisibleBit( 0 )
{ 
  for( u32 i = 0; i < LOD_MAX; i++ )
  {
    m_pModelInstanceNodeTable[ i ] = NULL;
  }
}

/**
* @brief デストラクタ
*
* @return 無し
*/
FieldTerrainBlock::~FieldTerrainBlock( void )
{

}

/**
* @brief 初期化処理
*
* @return 無し
*/
void FieldTerrainBlock::Initialize( void )
{

}

/**
* @brief 終了処理
*
* @return 無し
*/
void FieldTerrainBlock::Finalize( void )
{

}

/**
* @brief モデルを設定
*
* @param  lod     LOD段階
* @param  pModel  モデル
*
* @return 無し
*/
void FieldTerrainBlock::SetModel( Lod lod, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModel )
{
  m_pModelInstanceNodeTable[ lod ] = pModel;
  m_pModelInstanceNodeTable[ lod ]->SetVisible( lod == m_LodState );
}

/**
* @brief LOD状態を設定
*
* @param  lod     LOD段階
*
* @return 無し
*/
void FieldTerrainBlock::ChangeLod( Lod lod )
{
#if 0
  if( m_LodState == lod )
  {
    return;
  }
#endif

  // 今のをOFF
  if( m_pModelInstanceNodeTable[ m_LodStateDraw ] )
  {
    UpdateVisibleImpl( m_LodStateDraw, false );
  }

  m_LodState = m_LodStateDraw = lod;

  // モデルが見つかるまで落とす
  for( u32 i = m_LodStateDraw; i < LOD_MAX - 1; i++ )
  {
    if( m_pModelInstanceNodeTable[ lod ] )
    {
      break;
    }

    lod = static_cast< Lod >( lod + 1 );
  }

  m_LodStateDraw = lod;
  if( m_pModelInstanceNodeTable[ m_LodStateDraw ] )
  {
    UpdateVisibleImpl( m_LodStateDraw, !m_InvisibleBit );
  }
}

void FieldTerrainBlock::UpdateCulling( const gfl2::math::Matrix44* pCullingMatrix, const gfl2::math::AABB& rAABB )
{
  if( m_pModelInstanceNodeTable[ m_LodStateDraw ] == NULL )
  {
     return;
  }

  // @todo バイナリデータに含まれていない?
  gfl2::math::AABB      aabb = m_pModelInstanceNodeTable[ m_LodStateDraw ]->GetWorldAABB();

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

  // デバッグスイッチ
  b32 debugVisible = true;
  switch( m_LodState )    // ここでみるのは本当のState
  {
  case LOD_HIGHT: debugVisible = Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_TERRAIN_HIGH ]; break;
  case LOD_MIDDLE: debugVisible = Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_TERRAIN_MIDDLE ]; break;
  }

  if( debugVisible )
  {
    m_InvisibleBit = ( m_InvisibleBit & (~INVISIBLE_DEBUG_DRAW_SWITCH) );
  }
  else
  {
    m_InvisibleBit = ( m_InvisibleBit | INVISIBLE_DEBUG_DRAW_SWITCH );
  }


  UpdateVisibleImpl( m_LodStateDraw, !m_InvisibleBit );
}

void FieldTerrainBlock::UpdateVisibleImpl( Lod lod, b32 visible )
{
  if( m_pModelInstanceNodeTable[ lod ] == NULL )
  {
    return;
  }

  if( m_pModelInstanceNodeTable[ lod ]->IsVisible() == visible )
  {
    return;
  }

  m_pModelInstanceNodeTable[ lod ]->SetVisible( visible );

  // @todo 重いかも？
  if( visible )
  {
    gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pModelInstanceNodeTable[ lod ] );
  }
  else
  {
    gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild( m_pModelInstanceNodeTable[ lod ] );
  }

}


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );