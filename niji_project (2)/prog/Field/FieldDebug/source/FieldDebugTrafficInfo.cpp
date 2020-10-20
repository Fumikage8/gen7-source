//======================================================================
/**
 * @file FieldDebugTrafficInfo.cpp
 * @date 2016/03/07 13:00:12
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG
// self
#include "Field/FieldDebug/include/FieldDebugTrafficInfo.h"
// traffic
#include "Field/FieldRo/include/Traffic/FieldTrafficManager.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficPath.h"
// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Traffic)

/**
 *  @class  DebugInfo
 *  @brief  デバッグ用：往来描画
 */
/**
 *  @brief  コンストラクタ
 */
DebugInfo::DebugInfo( const Desc &desc )
: m_bMasterDraw( true )
{
  m_pTrafficManager = desc.pTrafficManager;
  m_VertexNum       = 3;
  m_pVertex         = GFL_NEW(desc.pHeap) gfl2::gfx::DrawUtil::VertexFormat[m_VertexNum];
}

/**
 *  @brief  デストラクタ
 */
DebugInfo::~DebugInfo( void )
{
  GFL_DELETE[] m_pVertex;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  描画
 */
void DebugInfo::Draw( void )
{
  if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::VISIBLE_TRAFFIC_PATH ) == 0 && IsEnableMasterDraw() ){ SetEnableMasterDraw( false ); }
  if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::VISIBLE_TRAFFIC_PATH ) != 0 && !IsEnableMasterDraw() ){ SetEnableMasterDraw(  true ); }

  if( !m_pTrafficManager ){ return; }
  if( !IsEnableMasterDraw() ){ return; }
  
  drawPathAll();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *
 */
void DebugInfo::drawPathAll( void )
{
  for( u32 idx = 0; idx < Field::Traffic::FIELD_TRAFFIC_MAX; ++ idx )
  {
    drawPathSingle( idx );
  }
}
void DebugInfo::drawPathSingle( u32 idx )
{
  if( !m_pTrafficManager ){ return; }
  
  Field::TrafficPath *pPath = m_pTrafficManager->GetTrafficPath( idx );
  if( !pPath ){ return; }
  if( !pPath->CheckFlags( Field::TrafficPath::FLAG_INITIALIZED ) ){ return; }
  
  // 簡易描画のデプステストを無しに設定する
  gfl2::gfx::DrawUtil::SetDepthTestEnable(false);
  
  u32 num = pPath->debug_getPosNum();
  gfl2::math::Vector4 color( 0.0f, 1.0f, 0.0f, 1.0f );
  for( u32 idx = 0; idx < num - 1; ++ idx )
  {
    gfl2::math::Vector3 start_pos = pPath->debug_getPos( idx );
    gfl2::math::Vector3   end_pos = pPath->debug_getPos( idx + 1 );
    gfl2::math::Vector3 start_top = start_pos + gfl2::math::Vector3( 0.0f, 10.0f, 0.0f );
    gfl2::math::Vector3   end_top =   end_pos + gfl2::math::Vector3( 0.0f, 10.0f, 0.0f );
    
    m_pVertex[0].pos   = start_pos;
    m_pVertex[0].color = color;
    m_pVertex[1].pos   = end_pos;
    m_pVertex[1].color = color;
    m_pVertex[2].pos   = start_top;
    m_pVertex[2].color = color;
    
    gfl2::gfx::DrawUtil::DrawTriangles( m_pVertex, 3, false );
    
    m_pVertex[0].pos   = start_top;
    m_pVertex[0].color = color;
    m_pVertex[1].pos   = end_pos;
    m_pVertex[1].color = color;
    m_pVertex[2].pos   = end_top;
    m_pVertex[2].color = color;
    
    gfl2::gfx::DrawUtil::DrawTriangles( m_pVertex, 3, false );
  }
}

GFL_NAMESPACE_END(Traffic)
GFL_NAMESPACE_END(Field)
#endif
