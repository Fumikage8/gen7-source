//======================================================================
/**
 * @file FieldGimmickEncountLottery.h
 * @date 2015/09/04 16:59:09
 * @author saita_kazuki
 * @brief ギミックエンカウント抽選処理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_GIMMICK_ENCOUNT_LOTTERY_H_INCLUDED__
#define __FIELD_GIMMICK_ENCOUNT_LOTTERY_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

namespace Field {
  class EventWork;
}

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( GimmickEncount )

/**
 * @class ギミックエンカウント抽選
 */
class Lottery
{
  GFL_FORBID_COPY_AND_ASSIGN( Lottery );

public:

  /**
   * @brief コンストラクタ
   */
  Lottery(){};

  /**
   * @brief デストラクタ
   */
  virtual ~Lottery(){};

  /**
   * @brief ギミックフラグ抽選
   * @param prob 確率 0～100
   * @param gimmickFlagNo 操作するフラグNo
   * @param pEventWork 抽選後のフラグセットに使用
   * @retval true 当選
   * @retval false 落選
   */
  bool LotteryGimmickFlag( u8 prob, u16 gimmickFlagNo, EventWork* pEventWork);

};

GFL_NAMESPACE_END( GimmickEncount )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_GIMMICK_ENCOUNT_LOTTERY_H_INCLUDED__

