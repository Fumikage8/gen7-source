/**
 *  GAME FREAK inc.
 *
 *  @file   FieldPlacementDataTrainerAccessor.cpp
 *  @brief  トレーナー配置データアクセサ
 *  @author saita_kazuki
 *  @date   2015.06.04
 */

#include "Field/FieldRo/include/PlacementData/accessor/FieldTrainerAccessor.h"

GFL_NAMESPACE_BEGIN( Field );

/**
 *  @brief  コンストラクタ
 */
FieldTrainerAccessor::FieldTrainerAccessor( void )
  : m_pData( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
FieldTrainerAccessor::~FieldTrainerAccessor( void )
{
}

/**
 *  @brief  初期化
 */
void FieldTrainerAccessor::Initialize( const void *data )
{
  m_pData = static_cast<const TrainerDataHeader*>(data);
}

/**
 *  @brief  最大数
 */
u32 FieldTrainerAccessor::GetDataMax( void ) const
{
  if( !m_pData ){ return 0; }
  return m_pData->num;
}


/**
 *  @brief  エイリアスデータ最大数
 */
u32 FieldTrainerAccessor::GetAliasDataMax( void ) const
{
  u32 count = 0;

  for( int i=0; i<m_pData->num; ++i )
  {
    if( m_pData->data[i].alias )
    {
      count ++;
    }
  }

  return count;
}

/**
 *  @brief  エイリアスではないデータ最大数
 */
u32 FieldTrainerAccessor::GetNotAliasDataMax( void ) const
{
  u32 alias = GetAliasDataMax();

  return GetDataMax() - alias;
}


/**
 *  @brief  情報取得
 */
const FieldTrainerAccessor::TrainerData *FieldTrainerAccessor::GetData( u32 index ) const
{
  if( m_pData->num > index )
  {
    return &m_pData->data[ index ];
  }

  return NULL;
}

/**
 *  @brief  動作データ取得
 */
const FieldTrainerAccessor::MoveData *FieldTrainerAccessor::GetMoveData( const FieldTrainerAccessor::TrainerData &data ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.moveDataOffset;
  return reinterpret_cast<MoveData*>( addr );
}

/**
 *  @brief  話しかけコリジョン情報の取得
 */
const void *FieldTrainerAccessor::GetTalkCollisionData( const FieldTrainerAccessor::TrainerData &data ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.talkCollisionOffset;
  return reinterpret_cast<void*>( addr );
}

/**
 *  @brief  コリジョン情報取得
 */
const void *FieldTrainerAccessor::GetCollisionData( const FieldTrainerAccessor::TrainerData &data ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.collisionOffset;
  return reinterpret_cast<void*>( addr );
}

/**
 *  @brief  シグナル情報取得
 */
const void *FieldTrainerAccessor::GetSignalData( const FieldTrainerAccessor::TrainerData &data ) const
{
  if( data.signalOffset == 0 ){ return NULL; }

  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.signalOffset;
  return reinterpret_cast<void*>( addr );
}

/**
 *  @brief  パスデータの取得
 */
const Traffic::PathCoreData *FieldTrainerAccessor::GetPathCoreData( const FieldTrainerAccessor::TrainerData &data ) const
{
  if( data.pathCoreOffset == 0 ){ return NULL; }

  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.pathCoreOffset;
  return reinterpret_cast<Traffic::PathCoreData*>( addr );
}

/**
 *  @brief  中継点演出データの取得
 */
const Traffic::PathPointDataHeader *FieldTrainerAccessor::GetPathPointData( const FieldTrainerAccessor::TrainerData &data ) const
{
  if( data.pathPointOffset == 0 ){ return NULL; }

  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.pathPointOffset;
  return reinterpret_cast<Traffic::PathPointDataHeader*>( addr );
}

GFL_NAMESPACE_END( Field );
