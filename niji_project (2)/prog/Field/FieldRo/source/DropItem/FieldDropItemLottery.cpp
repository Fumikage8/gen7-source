//======================================================================
/**
 * @file FieldDropItemLottery.cpp
 * @date 2015/08/31 18:09:25
 * @author saita_kazuki
 * @brief ドロップアイテム抽選
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/DropItem/FieldDropItemLottery.h"

#include <debug/include/gfl2_Assert.h>
#include <niji_conv_header/poke_lib/item/itemsym.h>

#include "System/include/GflUse.h"

#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( DropItem )

/**
 * @brief コンストラクタ
 * @param pData ドロップアイテムバイナリデータ。FieldResidentから取得
 */
Lottery::Lottery()
  :  m_accessor()
{
}

Lottery::Lottery( void* pDropItemData)
  :  m_accessor( pDropItemData)
{
}

/**
* @brief デストラクタ
*/
Lottery::~Lottery()
{
}

/**
* @brief データセット
* @param pData ドロップアイテムバイナリデータ。FieldResidentから取得
*/
void Lottery::SetData( void* pDropItemData)
{
  m_accessor.SetData( pDropItemData);
}

/**
* @brief アイテムIDの抽選
* @param id ドロップアイテムテーブルID
*/
u16 Lottery::LotteryItemID( u32 id) const
{
  DataAccessor::TableData* pData = m_accessor.GetData( id);
  if( pData)
  {
    u32 rnd = System::GflUse::GetPublicRand( 100);

#if PM_DEBUG
    if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::ENABLE_DROPITEM_RND_NUM ) > 0 )
    {
      rnd = FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::ENABLE_DROPITEM_RND_NUM );
    }
#endif
    u32 prob = 0;
    for( u32 i = 0; i < DataAccessor::TABLE_ITEM_NUM; ++i)
    {
      prob += pData->data[i].prob;
      if( rnd < prob)
      {
        return pData->data[i].itemID;
      }
    }
  }
  // 抽選漏れもありえる
  return ITEM_DUMMY_DATA;
}

GFL_NAMESPACE_END( DropItem )
GFL_NAMESPACE_END( Field )
