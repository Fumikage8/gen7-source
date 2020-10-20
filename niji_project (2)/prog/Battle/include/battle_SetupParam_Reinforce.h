//=============================================================================
/**
 * @file   battle_SetupParam_Reinforce.h
 * @date   2015/12/07
 * @author obata_toshihiro
 * @brief  バトルセットアップパラメータ( 援軍 )
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BATTLE_SETUPPARAM_REINFORCE_H_INCLUDED
#define BATTLE_SETUPPARAM_REINFORCE_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <pml/include/pml_Type.h>
#include <pml/include/pml_PokePara.h>
#include <niji_conv_header/poke_lib/monsno_def.h>
#include "./battle_def.h"


/**
 * @brief ポケモンが場に出る条件
 */
enum ReinforceCondition
{
  REINFORCE_CONDITION_NULL,   // 無効値
  REINFORCE_CONDITION_MUST,   // 必ず出現する
  REINFORCE_CONDITION_PINCH,  // HPが減っている場合に出現する
  REINFORCE_CONDITION_NUM,
};


/**
 * @brief 場に出てくるポケモンの設定
 */
struct ReinforcePokeParam
{
  ReinforceCondition           condition;       // 場に出てくる条件
  s32                          maxAppearCount;  // 場に出てくる最大回数( 負数なら無制限 )
  pml::pokepara::PokemonParam* pokeParam;       // 場に出てくるポケモンのパラメータ
};


/**
 * @brief 増援ポケモンの最大数
 */
static const u32 MAX_REINFORCE_POKE_NUM = 4;


//-----------------------------------------------------------------------------
/**
 * @brief 援軍の設定
 *
 * 「援軍」を有効にすると、毎ターン終了時に、条件を満たした増援ポケモンが乱入します。
 * 「援軍」は、エンカウントしたポケモンに対してのみ適用され、援軍として登場したポケモンは、次の援軍を呼びません。
 */
//-----------------------------------------------------------------------------
struct ReinforceBattleParam
{
  bool               reinforceEnableFlag;                     // 増援可能フラグ
  ReinforcePokeParam reinforcePoke[ MAX_REINFORCE_POKE_NUM ]; // 増援ポケモン毎の設定
};

/**
 * @brief 援軍の設定バッファを確保する
 * @param[out] param  クリア対象のインスタンス
 * @param      heap   バッファの確保に使用するヒープ
 */
extern void REINFORCE_BATTLE_PARAM_Create( ReinforceBattleParam* param, gfl2::heap::HeapBase* heap );

/**
 * @brief 援軍の設定を破棄する( 破棄可能な状態にする )
 * @param[out] param  破棄対象のインスタンス
 */
extern void REINFORCE_BATTLE_PARAM_Delete( ReinforceBattleParam* param );

/**
 * @brief 援軍の設定をクリアする( 無効な値にする )
 * @param[out] param  クリア対象のインスタンス
 */
extern void REINFORCE_BATTLE_PARAM_Clear( ReinforceBattleParam* param );

/**
 * @brief 援軍が有効な設定か？
 * @param param  チェック対象の設定
 * @retval true   有効
 * @retval false  有効でない
 */
extern bool REINFORCE_BATTLE_PARAM_IsEnable( const ReinforceBattleParam& param );




#endif // BATTLE_SETUPPARAM_REINFORCE_H_INCLUDED