//======================================================================
/**
 * @file FieldDropItemCreator.h
 * @date 2015/09/17 18:09:25
 * @author saita_kazuki
 * @brief ドロップアイテム生成
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_DROP_ITEM_CREATOR_H_INCLUDED__
#define __FIELD_DROP_ITEM_CREATOR_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_math.h>

#include "Field/FieldRo/include/DropItem/FieldDropItemLottery.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoaderTypes.h"
#include "Field/FieldStatic/include/Item/FieldItemManager.h"


// 前方参照
namespace Field {
  class PlacementDataManager;
}


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( DropItem )

/**
 * @class ドロップアイテム生成
 */
class Creator
{
  GFL_FORBID_COPY_AND_ASSIGN( Creator );

public:

  struct InitializeDescription
  {
    void*                     pDropItemData;
    PlacementDataManager*     pPlacementDataManager;
  };

public:

  /**
   * @brief コンストラクタ
   */
  Creator();

  /**
   * @brief デストラクタ
   */
  virtual ~Creator();

  /**
   * @brief 初期化
   * @param desc 初期化情報
   */
  void Initialize( const InitializeDescription& desc);

  /**
   * @brief 終了
   */
  void Terminate();

  /**
   * @brief 抽選したドロップアイテムの生成
   * @param id ドロップアイテムテーブルID
   * @param pos 配置位置
   * @retval true アイテム抽選が当選し、ドロップアイテムが生成できた
   * @retval false アイテム抽選が落選した or ドロップアイテムが生成できなかった
   */
  bool CreateLotteryDropItem( u32 id, const gfl2::math::Vector3& pos, FieldItemManager::Type type);

  /**
   * @brief ドロップアイテムの抽選のみを行う
   * @param id ドロップアイテムテーブルID
   * @return アイテムID(キズぐすり、とかのIDのこと)
   */
  u16 LotteryDropItem( u32 id);

private:

  Field::PlacementDataManager*    m_pPlacementDataManager;
  Lottery                         m_lottery;

};

GFL_NAMESPACE_END( DropItem )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_DROP_ITEM_CREATOR_H_INCLUDED__
