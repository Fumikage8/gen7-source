/**
 *  GAME FREAK inc.
 *
 *  @file   FieldActionEventDataAccessor.cpp
 *  @brief  データアクセッサ
 *  @author miyachi_soichi
 *  @date   2015.05.26
 */

#include "Field/FieldRo/include/PlacementData/accessor/FieldActionEventAccessor.h"

GFL_NAMESPACE_BEGIN( Field )

/**
 *  @brief  コンストラクタ
 */
FieldActionEventAccessor::FieldActionEventAccessor( void )
  : m_pData( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
FieldActionEventAccessor::~FieldActionEventAccessor( void )
{
}

/**
 *  @brief  初期化
 */
void FieldActionEventAccessor::Initialize( const void *data )
{
  m_pData = static_cast<const ActionEventDataHeader*>(data);
}

/**
 *  @brief  最大数
 */
u32 FieldActionEventAccessor::GetDataMax( void ) const
{
  if( !m_pData ){ return 0; }
  return m_pData->num;
}

/**
 *  @brief  情報取得
 */
const FieldActionEventAccessor::ActionEventData *FieldActionEventAccessor::GetData( u32 index ) const
{
  if( m_pData->num > index )
  {
    return &m_pData->data[ index ];
  }

  return NULL;
}

/**
 *  @brief  アクションデータの取得
 */
const void *FieldActionEventAccessor::GetActionData( const FieldActionEventAccessor::ActionEventData &data ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.actionDataOffset;
  return reinterpret_cast<void*>( addr );
}

/**
 *  @brief  コリジョン情報取得
 */
const void *FieldActionEventAccessor::GetCollisionData( const FieldActionEventAccessor::ActionEventData &data ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.collisionOffset;
  return reinterpret_cast<void*>( addr );
}

GFL_NAMESPACE_END( Field )
