//======================================================================
/**
 * @file FieldPokeFinderAccessor.cpp
 * @date 2015/10/01 17:26:11
 * @author miyachi_soichi
 * @brief データアクセッサ：ポケファインダー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/PlacementData/accessor/FieldPokeFinderAccessor.h"

GFL_NAMESPACE_BEGIN(Field)

/**
 * @class  FieldPokeFinderAccessor
 * @brief  データアクセッサ：ポケファインダー
 */
/**
 *  @brief  コンストラクタ
 */
FieldPokeFinderAccessor::FieldPokeFinderAccessor( void )
: m_pData( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
FieldPokeFinderAccessor::~FieldPokeFinderAccessor( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void FieldPokeFinderAccessor::Initialize( const void *data )
{
  m_pData = reinterpret_cast< const PokeFinderDataHeader*>( data );
}

/**
 *  @brief  最大数
 */
u32 FieldPokeFinderAccessor::GetDataMax( void ) const
{
  if( !m_pData ){ return 0; }

  return m_pData->num;
}

/**
 *  @brief  情報取得
 */
const FieldPokeFinderAccessor::PokeFinderData *FieldPokeFinderAccessor::GetData( u32 index ) const
{
  if( !m_pData ){ return NULL; }
  if( GetDataMax() <= index ){ return NULL; }

  return &( m_pData->data[ index ] );
}

/**
 *  @brief  コリジョン情報
 */
const void *FieldPokeFinderAccessor::GetCollisionData( const FieldPokeFinderAccessor::PokeFinderData &data ) const
{
  uptr addr = reinterpret_cast<uptr>( m_pData );
  addr = addr + data.collisionOffset;
  return reinterpret_cast<void*>( addr );
}

/**
 *  @brief  巡回グラフA情報の取得
 */
const void *FieldPokeFinderAccessor::GetMoveAData( const PokeFinderData &data ) const
{
  if( !m_pData ){ return NULL; }
  if( data.moveAOffset == 0 ){ return NULL; }

  uptr addr = reinterpret_cast<uptr>( m_pData );
  addr = addr + data.moveAOffset;
  return reinterpret_cast<void*>( addr );
}

/**
 *  @brief  巡回グラフB情報の取得
 */
const void *FieldPokeFinderAccessor::GetMoveBData( const PokeFinderData &data ) const
{
  if( !m_pData ){ return NULL; }
  if( data.moveBOffset == 0 ){ return NULL; }

  uptr addr = reinterpret_cast<uptr>( m_pData );
  addr = addr + data.moveBOffset;
  return reinterpret_cast<void*>( addr );
}

/**
 *  @brief  待機位置情報の取得
 */
const void *FieldPokeFinderAccessor::GetWaitData( const PokeFinderData &data ) const
{
  if( !m_pData ){ return NULL; }
  if( data.waitOffset == 0 ){ return NULL; }

  uptr addr = reinterpret_cast<uptr>( m_pData );
  addr = addr + data.waitOffset;
  return reinterpret_cast<void*>( addr );
}

/**
 *  @brief  一方通行パスの取得
 */
const void *FieldPokeFinderAccessor::GetOnewayData( const PokeFinderData &data ) const
{
  if( !m_pData ){ return NULL; }
  if( data.onewayOffset == 0 ){ return NULL; }

  uptr addr = reinterpret_cast<uptr>( m_pData );
  addr = addr + data.onewayOffset;
  return reinterpret_cast<void*>( addr );
}

GFL_NAMESPACE_END(Field)
