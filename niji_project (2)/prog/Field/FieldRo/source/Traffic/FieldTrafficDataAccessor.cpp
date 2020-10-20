//======================================================================
/**
 * @file FieldTrafficDataAccessor.cpp
 * @date 2015/07/17 11:08:56
 * @author miyachi_soichi
 * @brief フィールド往来用データアクセッサ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// Field
#include "Field/FieldRo/include/Traffic/FieldTrafficDataAccessor.h"

GFL_NAMESPACE_BEGIN(Field)


/**
 * @class TrafficDataAccessor
 * @brief 往来データアクセッサ
 */
/**
 *  @brief  コンストラクタ
 */
TrafficDataAccessor::TrafficDataAccessor( void )
: m_pData( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
TrafficDataAccessor::~TrafficDataAccessor( void )
{
}

/**
 *  @brief  初期化
 */
void TrafficDataAccessor::Initialize( const void *data )
{
  m_pData = reinterpret_cast<const Traffic::TrafficPathDataHeader*>( data );
}

/**
 *  @brief  パスデータの最大数の取得
 */
u32 TrafficDataAccessor::GetDataMax( void ) const
{
  if( !m_pData ){ return 0; }

  return m_pData->num;
}

/**
 *  @brief  パスデータを取得する
 */
const Traffic::TrafficPathData *TrafficDataAccessor::GetTrafficPathData( u32 idx ) const
{
  if( !m_pData ){ return NULL; }

  if( m_pData->num > idx )
  {
    return &( m_pData->data[ idx ] );
  }
  return NULL;
}

/**
 *  @brief  パスコアデータを取得する
 */
const Traffic::PathCoreData *TrafficDataAccessor::GetPathCoreData( const Traffic::TrafficPathData &data ) const
{
  if( !m_pData ){ return NULL; }

  uptr addr = reinterpret_cast<uptr>( m_pData );
  addr = addr + data.pathOffset;
  return reinterpret_cast<Traffic::PathCoreData*>( addr );
}

/**
 *  @brief  NPCテーブルを取得する
 */
const void *TrafficDataAccessor::GetNpcCoreData( const Traffic::TrafficPathData &data ) const
{
  if( !m_pData ){ return NULL; }

  uptr addr = reinterpret_cast<uptr>( m_pData );
  addr = addr + data.npcDataOffset;
  return reinterpret_cast<void*>( addr );
}

/**
 *  @class  TrafficNpcDataAccessor
 *  @brief  往来NPCデータアクセッサ
 */
/**
 *  @brief  初期化
 */
void TrafficNpcDataAccessor::Initialize( const void *data )
{
  m_pData = reinterpret_cast<const Traffic::NpcCoreDataHeader*>( data );
}

/**
 *  @brief  パスデータの最大数の取得
 */
u32 TrafficNpcDataAccessor::GetDataMax( void ) const
{
  if( !m_pData ){ return 0; }

  return m_pData->num;
}

/**
 *  @brief  パスデータを取得する
 */
const Traffic::NpcCoreData *TrafficNpcDataAccessor::GetNpcCoreData( u32 idx ) const
{
  if( !m_pData ){ return NULL; }

  if( m_pData->num > idx )
  {
    return &( m_pData->data[ idx ] );
  }
  return NULL;
}

GFL_NAMESPACE_END(Field)
