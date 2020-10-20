//======================================================================
/**
 * @file FieldDropItemLottery.h
 * @date 2015/08/31 18:09:25
 * @author saita_kazuki
 * @brief ドロップアイテム抽選
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_DROP_ITEM_LOTTERY_H_INCLUDED__
#define __FIELD_DROP_ITEM_LOTTERY_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include "Field/FieldRo/include/DropItem/FieldDropItemDataAccessor.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( DropItem )

/**
 * @class ドロップアイテム抽選
 */
class Lottery
{
  GFL_FORBID_COPY_AND_ASSIGN( Lottery );

public:

  /**
   * @brief コンストラクタ
   * @param pData ドロップアイテムバイナリデータ。FieldResidentから取得
   */
  Lottery();
  Lottery( void* pDropItemData);

  /**
   * @brief デストラクタ
   */
  virtual ~Lottery();

  /**
   * @brief データセット
   * @param pData ドロップアイテムバイナリデータ。FieldResidentから取得
   */
  void SetData( void* pDropItemData);

  /**
   * @brief アイテムIDの抽選
   * @param id ドロップアイテムテーブルID
   * @return アイテム番号。niji_conv_header/poke_lib/item/itemsym.hで定義されている
   * @note 抽選から外れた場合、不正なIDを指定した場合は0を返す(ITEM_DUMMY_DATA)
   */
  u16 LotteryItemID( u32 id) const ;

private:

  DataAccessor    m_accessor;

};

GFL_NAMESPACE_END( DropItem )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_DROP_ITEM_LOTTERY_H_INCLUDED__
