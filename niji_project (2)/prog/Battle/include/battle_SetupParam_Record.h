//=============================================================================
/**
 * @file   battle_SetupParam_Record.h
 * @date   2015/12/14 15:09:42
 * @author obata_toshihiro
 * @brief  バトルセットアップパラメータ( 録画データ関連 )
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BATTLE_SETUPPARAM_RECORD_H_INCLUDED
#define BATTLE_SETUPPARAM_RECORD_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>


/**
 * @brief 対戦結果１
 */
enum BtlRecordResult1
{
  BTL_RECORD_RESULT_1_WIN,  // 勝利
  BTL_RECORD_RESULT_1_LOSE, // 敗北
  BTL_RECORD_RESULT_1_DRAW, // 引き分け
};


/**
 * @brief 対戦結果２
 */
enum BtlRecordResult2
{
  BTL_RECORD_RESULT_2_DEAD,       // 全てのポケモンがひんし
  BTL_RECORD_RESULT_2_TIMEOVER,   // 制限時間切れ
  BTL_RECORD_RESULT_2_SURRENDER,  // 降参
};



#endif // BATTLE_SETUPPARAM_RECORD_H_INCLUDED