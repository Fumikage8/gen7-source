/**
 *  GAME FREAK inc.
 *
 *  @file   FieldBgEventAccessor.cpp
 *  @brief  Bgイベント用アクセッサ
 *  @author miyachi_soichi
 *  @date   2015.06.01
 */

#include "Field/FieldRo/include/PlacementData/accessor/FieldBgEventAccessor.h"

GFL_NAMESPACE_BEGIN(Field)

/**
 *  @brief  コンストラクタ
 */
FieldBgEventAccessor::FieldBgEventAccessor( void )
  : m_pData( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
FieldBgEventAccessor::~FieldBgEventAccessor( void )
{
}

/**
 *  @brief  初期化
 */
void FieldBgEventAccessor::Initialize( const void *data )
{
  m_pData = reinterpret_cast<const BgEventDataHeader*>( data );
}

/**
 *  @brief  最大数
 */
u32 FieldBgEventAccessor::GetDataMax( void ) const
{
  if( !m_pData ){ return 0; }
  return m_pData->num;
}

/**
 *  @brief  情報取得
 */
const FieldBgEventAccessor::BgEventData *FieldBgEventAccessor::GetData( u32 index ) const
{
  if( m_pData->num > index )
  {
    return &m_pData->data[ index ];
  }
  return NULL;
}

/**
 *  @brief  コリジョン情報取得
 */
const void *FieldBgEventAccessor::GetCollisionData( const FieldBgEventAccessor::BgEventData &data ) const
{
  uptr addr = reinterpret_cast<uptr>( m_pData );
  addr = addr + data.collisionOffset;
  return reinterpret_cast<void*>( addr );
}

GFL_NAMESPACE_END(Field)
