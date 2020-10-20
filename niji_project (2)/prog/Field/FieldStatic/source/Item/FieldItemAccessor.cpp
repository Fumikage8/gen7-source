//======================================================================
/**
 * @file FieldItemAccessor.cpp
 * @date 2015/09/04 15:43:47
 * @author ikawa_hiroki
 * @brief 配置アイテム用アクセサ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldStatic/include/Item/FieldItemAccessor.h"

GFL_NAMESPACE_BEGIN(Field)

/**
 *  @brief  コンストラクタ
 */
FieldItemAccessor::FieldItemAccessor( void )
  : m_pData( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
FieldItemAccessor::~FieldItemAccessor( void )
{
}

/**
 *  @brief  初期化
 */
void FieldItemAccessor::Initialize( const void *data )
{
  m_pData = reinterpret_cast<const FieldItemDataHeader*>( data );
}

/**
 *  @brief  最大数
 */
u32 FieldItemAccessor::GetDataMax( void ) const
{
  return m_pData->num;
}

/**
 *  @brief  情報取得
 */
const FieldItemAccessor::FieldItemData *FieldItemAccessor::GetData( u32 index ) const
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
const void *FieldItemAccessor::GetCollisionData( const FieldItemAccessor::FieldItemData &data ) const
{
  uptr addr = reinterpret_cast<uptr>( m_pData );
  addr = addr + data.collisionOffset;
  return reinterpret_cast<void*>( addr );
}

GFL_NAMESPACE_END(Field)
