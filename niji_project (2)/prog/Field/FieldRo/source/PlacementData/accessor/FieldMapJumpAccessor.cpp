//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldMapJumpAccessor.cpp
 *	@brief  マップジャンプイベントのデータアクセサー
 *	@author	tomoya takahashi
 *	@date		2015.05.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <debug/include/gfl2_DebugPrint.h>

#include "Field/FieldRo/include/PlacementData/accessor/FieldMapJumpAccessor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldCollisionAccessor.h"

GFL_NAMESPACE_BEGIN(Field);

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
/**
 *  @biref  コンストラクタ
 */
GFL_DLL_EXPORT FieldMapJumpAccessor::FieldMapJumpAccessor() : 
  m_pData(NULL)
{
}

/**
 *  @brief  デストラクタ
 */
GFL_DLL_EXPORT FieldMapJumpAccessor::~FieldMapJumpAccessor()
{
}

/**
 *  @brief  初期化
 */
GFL_DLL_EXPORT void FieldMapJumpAccessor::Initialize( const void * data )
{
  m_pData = static_cast<const MapJumpDataHeader*>(data);
}

/**
 *  @brief  最大数
 */
GFL_DLL_EXPORT u32 FieldMapJumpAccessor::GetDataMax() const
{
  if( !m_pData ){ return 0; }
  return m_pData->num;
}

/**
 *  @brief 情報取得
 */
GFL_DLL_EXPORT const FieldMapJumpAccessor::MapJumpData * FieldMapJumpAccessor::GetData( u32 idx ) const
{
  if( m_pData->num > idx ){
    return &m_pData->data[ idx ];
  }

  return NULL;
}

/**
 *  @brief 衝突用コリジョン情報の取得
 */
GFL_DLL_EXPORT const void * FieldMapJumpAccessor::GetCollisionData( const FieldMapJumpAccessor::MapJumpData & data ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.collisionOffset;
  return reinterpret_cast<void*>( addr );
}


/**
 *  @brief コリジョン情報　ポジションの幅Percentage取得
 */
f32 FieldMapJumpAccessor::GetCollisionCenterWidthPercentage( const FieldMapJumpAccessor::MapJumpData & data, const gfl2::math::Vector3& pos ) const
{
  f32 retval = 0.0f;

  FieldCollisionAccessor col;
  col.Initialize( this->GetCollisionData( data ) );

  // コリジョンは1個
  if( col.GetDataMax() != 1 )
  {
    return retval;
  }

  FieldCollisionAccessor::CollisionData col_data;

  if( col.GetData( 0, &col_data ) == false )
  {
    return retval;
  }

  // コリジョンはBOXかLINE
  if( !((col_data.type == FieldCollisionAccessor::BOX) || (col_data.type == FieldCollisionAccessor::LINE)) )
  {
    return retval;
  }

  // 中心座標
  // 横軸方向
  gfl2::math::Vector3 center_pos( col_data.box.center[0], col_data.box.center[1], col_data.box.center[2] );
  gfl2::math::Vector3 x;
  f32 width;

  FieldCollisionAccessor::GetParameter( col_data, &center_pos, &x, &width );

  gfl2::math::Vector3 in_pos_v = pos - center_pos;
  if( in_pos_v.LengthSq() <= EPSILON )
  {
    return retval;
  }

  // (in_pos - center_pos) と x軸の内積で距離を求め、widthの幅からパーセントを求める。
  f32 dist = - x.Dot( pos - center_pos ); // 出るときは、接続イベント方向も反転していることから、-をかけて反転する。
  retval = dist / width;

  if( retval > 1.0f )
  {
    TOMOYA_PRINT( "MapJump width control 1.0 over %f\n", retval );
    retval  = 1.0f;
  }
  else if( retval < -1.0f )
  {
    TOMOYA_PRINT( "MapJump width control -1.0 min %f\n", retval );
    retval  = -1.0f;
  }

  TOMOYA_PRINT( "MapJump Percent center_pos(%f,%f,%f) pos(%f,%f,%f) x(%f,%f,%f) scale(%f) percent(%f)\n", 
    center_pos.GetX(), center_pos.GetY(),  center_pos.GetZ(), 
    pos.GetX(), pos.GetY(),  pos.GetZ(), 
    x.GetX(), x.GetY(), x.GetZ(),
    width, retval );

  return retval;
}

/**
 *  @brief コリジョン情報　幅Percentの座標を取得
 */
void FieldMapJumpAccessor::GetCollisionCenterWidthPercentagePosition( const FieldMapJumpAccessor::MapJumpData & data, const f32 parcent, const gfl2::math::Vector3& jump_pos, gfl2::math::Vector3* out_pos ) const
{
  FieldCollisionAccessor col;
  col.Initialize( this->GetCollisionData( data ) );

  // コリジョンは1個
  if( col.GetDataMax() != 1 )
  {
    return ;
  }

  FieldCollisionAccessor::CollisionData col_data;

  if( col.GetData( 0, &col_data ) == false )
  {
    return ;
  }

  // コリジョンはBOXかLINE
  if( !((col_data.type == FieldCollisionAccessor::BOX) || (col_data.type == FieldCollisionAccessor::LINE)) )
  {
    return ;
  }

  // 中心座標
  gfl2::math::Vector3 center_pos;
  gfl2::math::Vector3 x;
  f32 width;

  FieldCollisionAccessor::GetParameter( col_data, &center_pos, &x, &width );
  center_pos = jump_pos;  // JUMPPOSを使う

  // 横軸方向
  *out_pos = center_pos + (x * (width * parcent));

  TOMOYA_PRINT( "MapJump Percent center_pos(%f,%f,%f) pos(%f,%f,%f) x(%f,%f,%f) scale(%f) percent(%f)\n", 
    center_pos.GetX(), center_pos.GetY(),  center_pos.GetZ(), 
    out_pos->GetX(), out_pos->GetY(),  out_pos->GetZ(), 
    x.GetX(), x.GetY(), x.GetZ(),
    width, parcent );
}


GFL_NAMESPACE_END(Field);
