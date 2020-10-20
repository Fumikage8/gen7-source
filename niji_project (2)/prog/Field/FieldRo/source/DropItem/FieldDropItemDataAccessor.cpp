//======================================================================
/**
 * @file FieldDropItemDataAccessor.cpp
 * @date 2015/08/31 18:09:04
 * @author saita_kazuki
 * @brief ドロップアイテムデータアクセサー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/DropItem/FieldDropItemDataAccessor.h"
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( DropItem )

/**
 * @brief コンストラクタ
 *  @param pData ドロップアイテムバイナリデータ。FieldResidentから取得
 */
DataAccessor::DataAccessor()
  : m_pData( NULL)
{
}

DataAccessor::DataAccessor( void* pData)
  : m_pData( reinterpret_cast<AllData*>( pData))
{
}

/**
 * @brief デストラクタ
 */
DataAccessor::~DataAccessor()
{
}

/**
 *  @brief データセット
 *  @param pData ドロップアイテムバイナリデータ。FieldResidentから取得
 */
void DataAccessor::SetData( void* pData)
{
  m_pData = reinterpret_cast<AllData*>( pData);
}

/**
 * @brief データ取得
 * @param id テーブルID
 * @return 指定したIDのテーブルデータ
 * @note 不正なID指定をした場合はNULL
 */
DataAccessor::TableData* DataAccessor::GetData( u32 id) const
{
  GFL_ASSERT( m_pData);
  if( id == DROP_ITEM_INVALID_ID)
  {
    return NULL;
  }

  if( id < m_pData->header.num)
  {
    return &m_pData->data[id];
  }
  GFL_ASSERT( 0);
  return NULL;
}

GFL_NAMESPACE_END( DropItem )
GFL_NAMESPACE_END( Field )
