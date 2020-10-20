/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEncountAccessor.cpp
 *  @brief  エンカウント配置データアクセサ
 *  @author saita_kazuki
 *  @date   2015.05.31
 */

#include "Field/FieldRo/include/PlacementData/accessor/FieldEncountAccessor.h"

GFL_NAMESPACE_BEGIN( Field );

/**
 *  @brief  コンストラクタ
 */
FieldEncountAccessor::FieldEncountAccessor( void )
  : m_pData( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
FieldEncountAccessor::~FieldEncountAccessor( void )
{
}

/**
 *  @brief  初期化
 */
void FieldEncountAccessor::Initialize( const void *data )
{
  m_pData = static_cast<const EncountDataHeader*>(data);
}

/**
 *  @brief  最大数
 */
u32 FieldEncountAccessor::GetDataMax( void ) const
{
  if( !m_pData ){ return 0; }
  return m_pData->num;
}

/**
 *  @brief  情報取得
 */
const FieldEncountAccessor::EncountData *FieldEncountAccessor::GetData( u32 index ) const
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
const void *FieldEncountAccessor::GetCollisionData( const FieldEncountAccessor::EncountData &data ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData);
  addr = addr + data.collisionOffset;
  return reinterpret_cast<void*>( addr );
}

GFL_NAMESPACE_END( Field );
