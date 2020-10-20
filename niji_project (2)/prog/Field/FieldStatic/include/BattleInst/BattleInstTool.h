//======================================================================
/**
 * @file BattleInstTool.h
 * @date 2015/01/12
 * @author saita_kazuki
 * @brief バトル施設データ作成などの便利関数群 (sangoから移植)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BATTLE_INST_TOOL_H_INCLUDED__
#define __BATTLE_INST_TOOL_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <pml/include/pmlib.h>

namespace BattleInst
{
  struct BINST_POKEMON_ROM_DATA;
  struct BINST_TRAINER_ROM_DATA;
}

GFL_NAMESPACE_BEGIN( BattleInstTool )

static const int PARTNER_POKE_CNT       = 2;      ///<パートナーのポケモン数
static const int ROM_FRIEND_NUM         = 6;      ///<スーパーランク友達数 データ依存

//--------------------------------------------------------------
/**
 * @brief   ポケモン作成　中でメモリアロケーションします
 * @param   pd        作成するポケモンデータポインタ
 * @param   level     レベル
 * @param   pow       パワー乱数
 * @param   pHeap     ヒープ
 * @param   tokusei   特性指定  @fix BTS[1911]
 * @param   sex       特性指定  @fix BTS[2522]
 * @return  ポケモンパラム
 */
//--------------------------------------------------------------  
pml::pokepara::PokemonParam* MakePokemonParam(
        BattleInst::BINST_POKEMON_ROM_DATA *pd,
        int level,
        u8 pow,
        gfl2::heap::HeapBase* pHeap,
        int tokusei = -1,
        int sex = pml::pokepara::INIT_SPEC_SEX_BOTH );

//--------------------------------------------------------------
/**
 * @brief  バトル施設トレーナーデータのサイズを取得する
 * @param   arc_id    アーカイブＩＤ
 * @param   data_idx  データインデックス
 */
//--------------------------------------------------------------
size_t GetTrainerRomDataSize( gfl2::heap::HeapBase* p_heap, gfl2::fs::AsyncFileManager* pAsyncFileManager, int arc_id, int data_idx );

//--------------------------------------------------------------
/**
 * @brief  バトル施設トレーナーデータを取得する
 * @param   arc_id    アーカイブＩＤ
 * @param   data_idx  データインデックス
 * @param   td　格納バッファ
 */
//--------------------------------------------------------------
void GetTrainerRomData( gfl2::heap::HeapBase* p_heap, gfl2::fs::AsyncFileManager* pAsyncFileManager, int arc_id, int data_idx, BattleInst::BINST_TRAINER_ROM_DATA  *td, size_t size );

//--------------------------------------------------------------
/**
 * @brief  バトル施設モンスターデータを取得する
 * @param   arc_id    アーカイブＩＤ
 * @param   data_idx  データインデックス
 * @param   pd　格納バッファ
 */
//--------------------------------------------------------------
void GetPokemonRomData( gfl2::heap::HeapBase* p_heap, gfl2::fs::AsyncFileManager* pAsyncFileManager, int arc_id, int data_idx, BattleInst::BINST_POKEMON_ROM_DATA  *pd );

GFL_NAMESPACE_END( BattleInstTool )

#endif // __BATTLE_INST_TOOL_H_INCLUDED__

