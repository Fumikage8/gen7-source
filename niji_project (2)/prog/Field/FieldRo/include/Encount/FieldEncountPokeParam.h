//=============================================================================
/**
 *	@file		FieldEncountPokeParam.h
 *	@brief  エンカウント用ポケモンパラメータ
 *	@author saita_kazuki
 *	@data		2015.05.26
 */
//==============================================================================

#if !defined( FIELD_ENCOUNT_POKE_PARAM_H_INCLUDED )
#define FIELD_ENCOUNT_POKE_PARAM_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <pml/include/pokepara/pml_PokemonParamType.h>
#include "niji_conv_header/field/encount/FieldEncountDataTypes.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Encount );


class EncountPokeParam
{
public:
  u16 monsNo;
  u16 item;       // アイテム制御 ITEM_REQ_DEFAULT：パーソナル依存 ITEM_REQ_NONE: アイテムなし
  u8  level;
  u8  form;
  u8  rare;       // レア制御    RARE_REQ_DEFAULT：制御しない RARE_REQ_MUST：レア抽選　RARE_REQ_NOT：レア禁止
  u8  spabi;      // 特性制御    SPABI_REQ_DEFAULT：制御しない SPABI_REQ_01：特性１ SPABI_REQ_02：特性２ SPABI_REQ_03：特性３
  u16 waza[4];    // waza[0]が0なら指定なし、指定する場合4つとも指定する
  u8  sex;        // 性別制御    SEX_REQ_DEFAULT：制御しない SEX_REQ_MALE：オス固定　SEX_REQ_FEMALE：メス固定
  u8  talentVNum; // 個体値V抽選数 0：制御せずランダム 1〜5：Vにする個体値数
  u16 seikaku;    // 性格指定（シンクロぶっち） 0xFFFF=pml::pokepara::INIT_SPEC_SEIKAKU_RANDOMでランダム指定

  u16 talent[pml::pokepara::POWER_NUM];  // 個体値指定 0xFFFF=pml::pokepara::INIT_SPEC_TALENT_RANDOM
  u8  effort[pml::pokepara::POWER_NUM];  // 努力値指定
  
  u8  encount_mode; //エンカウントモード Encount::WildEncMode
  u8  aura_mode;	//オーラ効果 WildEncAulaMode

  u8 aura_color_r;	//オーラの色 R(0-255)
  u8 aura_color_g;	//オーラの色 G(0-255)
  u8 aura_color_b;	//オーラの色 B(0-255)
  u8 aura_color_a;	//オーラの色 A(0-255)

  EncountPokeParam();
  void Clear();
};

class ReinforcePokeParam
{
public:
  int               reinforce_count;  //抽選回数制限
  ReinforcePokeType reinforce_type;   //抽選タイプ ReinforcePokeType
  EncountPokeParam  poke;   //パラメータ

  ReinforcePokeParam();
  void Clear();
}; 

GFL_NAMESPACE_END( Encount );
GFL_NAMESPACE_END( Field );

#endif // FIELD_ENCOUNT_POKE_PARAM_H_INCLUDED 
