//==============================================================================
/**
 * @file	BattleFesPokeLotterySystem.h
 * @brief	フィールドギミック：バトルフェス　ポケモン抽選管理
 * @author	munakata_kai
 * @date	2016/11/18
 */
// =============================================================================

#if !defined(__BATTLE_FES_POKE_LOTTERY_H__)
#define __BATTLE_FES_POKE_LOTTERY_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "AppLib/include/Util/app_util_heap.h"
// pokelib
#include <pml/include/pmlib.h>

#include "Field/FieldStatic/include/BattleFes/BattleFesDefine.h"

namespace gfl2
{
  namespace math
  {
    class SFMTRandom;
  }
}

namespace BattleInst
{
  struct BINST_POKEMON_ROM_DATA;
}

GFL_NAMESPACE_BEGIN(BattleFes)

/// バトルフェス　ポケモン抽選管理
class BattleFesPokeLotterySystem
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN(BattleFesPokeLotterySystem);

public:
  // コンストラクタ
  BattleFesPokeLotterySystem(gfl2::heap::HeapBase* pHeap);
  // デストラクタ
  virtual ~BattleFesPokeLotterySystem();

  //-----------------------------------------------------------------------------
  /**
  * @brief ポケモン抽選（三匹決定）
  * @param[in] randSeed シード値
  * @param[in] pHeap ヒープ
  * @param[in] isShop お店抽選か否か
  */
  //-----------------------------------------------------------------------------
  void PokemonLottery(const u32 randSeed, gfl2::heap::HeapBase* pHeap, const bool isShop);
 
  //-----------------------------------------------------------------------------
  /**
  * @brief 抽選されたポケモンを取得
  * @param[in] index (0~2)
  * @param[out] pml::pokepara::PokemonParam* 
  */
  //-----------------------------------------------------------------------------
  pml::pokepara::PokemonParam* GetPokemon(const u32 index) const;

  //-----------------------------------------------------------------------------
  /**
  * @brief 抽選されたポケモンのバトルツリーデータ取得
  * @param[in] index (0~2)
  * @param[out] const BATTLE_TREE_DATA&
  */
  //-----------------------------------------------------------------------------
  const BATTLE_TREE_DATA& GetBattleTreeData(const u32 index) const;

  //-----------------------------------------------------------------------------
  /**
  * @brief 抽選されたポケモンのパラメータを再計算（レベル更新用）
  * @param[in] pHeap ヒープ
  */
  //-----------------------------------------------------------------------------
  void UpdatePokemonData(gfl2::heap::HeapBase* pHeap);

private:
  //-----------------------------------------------------------------------------
  /**
  * @brief ポケモン作成（被りなし）
  * @param[in] gfl2::heap::HeapBase*
  * @param[in] u32 抽選番号（０～２）
  */
  //-----------------------------------------------------------------------------
  void createPokemon(gfl2::heap::HeapBase* pHeap, const u32 index);

  //-----------------------------------------------------------------------------
  /**
  * @brief お店抽選によるポケモン作成（被りなし）
  * @param[in] gfl2::heap::HeapBase*
  * @param[in] u32 抽選番号（０～２）
  */
  //-----------------------------------------------------------------------------
  void createShopPokemon(gfl2::heap::HeapBase* pHeap, const u32 index);

  //-----------------------------------------------------------------------------
  /**
  * @brief ポケモンの性別取得
  * @param[out] u8
  */
  //-----------------------------------------------------------------------------
  u8 getPokemonRandomSex();

  //-----------------------------------------------------------------------------
  /**
  * @brief ポケモンの特性取得
  * @param[out] u8
  */
  //-----------------------------------------------------------------------------
  u8 getPokemonRandomTokusei();

  //--------------------------------------------------------------
  /**
  * @brief   バトル施設モンスターデータを取得する
  * @param   pHeap    ヒープ
  * @param   data_idx データインデックス
  * @param   pd       格納バッファ
  */
  //--------------------------------------------------------------
  void getPokemonRomData(gfl2::heap::HeapBase* pHeap, int data_idx, BattleInst::BINST_POKEMON_ROM_DATA *pd);

  gfl2::math::SFMTRandom       m_randSys;                                         // 乱数
  pml::pokepara::PokemonParam* m_pPokeParam[BattleFesDefine::POKEMON_MAX];        // 抽選されたポケモン３匹
  BATTLE_TREE_DATA             m_BattleTreeData[BattleFesDefine::POKEMON_MAX];    // 抽選に必要なパラメータ
};

GFL_NAMESPACE_END(BattleFes)

#endif // __BATTLE_FES_POKE_LOTTERY_H__
