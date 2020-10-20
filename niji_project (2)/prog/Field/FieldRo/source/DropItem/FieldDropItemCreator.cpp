//======================================================================
/**
 * @file FieldDropItemCreator.cpp
 * @date 2015/09/17 18:09:25
 * @author saita_kazuki
 * @brief ドロップアイテム生成
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/DropItem/FieldDropItemCreator.h"

#include <debug/include/gfl2_Assert.h>
#include <niji_conv_header/poke_lib/item/itemsym.h>

#include "Field/FieldRo/include/DropItem/FieldDropItemLottery.h"
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( DropItem )

/**
 * @brief コンストラクタ
 * @param pData ドロップアイテムバイナリデータ。FieldResidentから取得
 */
Creator::Creator()
  : m_pPlacementDataManager( NULL)
  , m_lottery()
{
}

/**
* @brief デストラクタ
*/
Creator::~Creator()
{
  this->Terminate();
}

/**
 * @brief 初期化
 * @param desc 初期化情報
 */
void Creator::Initialize( const InitializeDescription& desc)
{
  m_lottery.SetData( desc.pDropItemData);
  m_pPlacementDataManager = desc.pPlacementDataManager;
}

/**
 * @brief 終了
 */
void Creator::Terminate()
{
  m_pPlacementDataManager = NULL;
}

/**
 * @brief 抽選したドロップアイテムの生成
 * @param id ドロップアイテムテーブルID
 * @param pos 配置位置
 * @param type タイプ
 * @retval true アイテム抽選が当選し、ドロップアイテムが生成できた
 * @retval false アイテム抽選が落選した or ドロップアイテムが生成できなかった
 */
bool Creator::CreateLotteryDropItem( u32 id, const gfl2::math::Vector3& pos, FieldItemManager::Type type)
{
  u16 itemID = this->LotteryDropItem( id);

  if( itemID == ITEM_DUMMY_DATA)
  {
    // アイテム抽選に落選
    return false;
  }

  // trueでアイテム生成成功、falseでアイテム生成失敗
  return m_pPlacementDataManager->RegistDropItem( itemID, 1, pos, type);
}

/**
 * @brief ドロップアイテムの抽選のみを行う
 * @param id ドロップアイテムテーブルID
 * @return アイテムID(キズぐすり、とかのIDのこと)
 */
u16 Creator::LotteryDropItem( u32 id)
{
  return m_lottery.LotteryItemID( id);
}

GFL_NAMESPACE_END( DropItem )
GFL_NAMESPACE_END( Field )
