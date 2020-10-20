//======================================================================
/**
 * @file BattleInstData.h
 * @date 2015/01/12
 * @author saita_kazuki (sangoから移植)
 * @brief 
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BATTLE_INST_DATA_H_INCLUDED__
#define __BATTLE_INST_DATA_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN( BattleInst )

/**
 *  @brief バトル施設トレーナーバイナリデータ
 */
struct BINST_TRAINER_ROM_DATA
{
  u16    tr_type;              ///<トレーナータイプ
  u16    use_poke_cnt;          ///<使用可能ポケモン数  
  u16    use_poke_table[1];    ///<使用可能ポケモンINDEXテーブル（可変長）
};

/**
 *  @brief バトル施設ポケモンバイナリデータ
 */
struct BINST_POKEMON_ROM_DATA
{
  u16    mons_no;                     ///<モンスターナンバー
  u16    waza[/*WAZA_TEMOTI_MAX*/4];  ///<所持技
  u8     exp_bit;                     ///<努力値振り分けビット
  u8     chr;                         ///<性格
  u16    item_no;                     ///<装備道具
  u16    form_no;                     ///<フォルムナンバー
};

GFL_NAMESPACE_END( BattleInst )

#endif // __BATTLE_INST_DATA_H_INCLUDED__

