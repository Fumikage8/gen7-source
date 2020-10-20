//=============================================================================
/**
 * @file   battle_SetupParam_Intrude.h
 * @date   2015/10/16 17:27:46
 * @author obata_toshihiro
 * @brief  バトルセットアップパラメータ( 乱入 )
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BATTLE_SETUPPARAM_INTRUDE_H_INCLUDED
#define BATTLE_SETUPPARAM_INTRUDE_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <pml/include/pml_Type.h>
#include <pml/include/pml_PokePara.h>
#include <niji_conv_header/poke_lib/monsno_def.h>
#include "./battle_def.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Encount )
class PokeSet;
GFL_NAMESPACE_END( Encount )
GFL_NAMESPACE_END( Field )



//-----------------------------------------------------------------------------
/**
 * @brief 乱入の設定
 */
//-----------------------------------------------------------------------------
struct IntrudeBattleParam
{
  bool                     intrudeEnableFlag;         // 乱入可能フラグ
  Field::Encount::PokeSet* intrudePokeParamGenerator; // 乱入ポケモンのパラメータを生成するオブジェクト

#if PM_DEBUG
  pml::pokepara::PokemonParam* intrudePokeParam;
#endif
};

/**
 * @brief 乱入の設定をクリアする( 無効な値にする )
 * @param[out] param  クリア対象のインスタンス
 */
extern void INTRUDE_BATTLE_PARAM_Clear( IntrudeBattleParam* param );

/**
 * @brief 乱入が有効な設定か？
 * @param param  チェック対象の設定
 * @retval true   有効
 * @retval false  有効でない
 */
extern bool INTRUDE_BATTLE_PARAM_IsEnable( const IntrudeBattleParam& param );





#endif // BATTLE_SETUPPARAM_INTRUDE_H_INCLUDED