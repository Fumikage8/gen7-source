//======================================================================
/**
 * @file FieldGimmickEncountLottery.cpp
 * @date 2015/09/04 16:59:34
 * @author saita_kazuki
 * @brief ギミックエンカウント抽選処理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountLottery.h"

// system
#include "System/include/GflUse.h"

// savedata
#include "Savedata/include/EventWork.h"

// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( GimmickEncount )

/**
 * @brief ギミックフラグ抽選
 * @param prob 確率 0～100
 * @param gimmickFlagNo 操作するフラグNo
 * @param pEventWork 抽選後のフラグセットに使用
 * @retval true 当選
 * @retval false 落選
 */
bool Lottery::LotteryGimmickFlag( u8 prob, u16 gimmickFlagNo, EventWork* pEventWork)
{
  u32 rnd = System::GflUse::GetPublicRand( 100);

#if PM_DEBUG
  // デバッグ処理。スイッチが0なら必ず落選、1なら必ず当選
  switch( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::GIMMICK_ENCOUNT_LOTTERY ) )
  {
  case 0:
    rnd = 100;
    break;
  case 2:
    rnd = 0;
    break;
  }
#endif // PM_DEBUG

  if( rnd < static_cast<u32>( prob))
  {
    // バニッシュフラグのように扱うフラグ。0…生成可、1…生成不可
    pEventWork->ResetEventFlag( gimmickFlagNo);
    return true;
  }
  pEventWork->SetEventFlag( gimmickFlagNo);
  return false;
}


GFL_NAMESPACE_END( GimmickEncount )
GFL_NAMESPACE_END( Field )

