// ============================================================================
/*
 * @file RegulationScriptChecker.h
 * @brief レギュレーションのスクリプトチェックを行います
 * @date 2015.12.11
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATION_SCRIPT_CHECKER_H_INCLUDE )
#define REGULATION_SCRIPT_CHECKER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <pawn/include/gfl2_Pawn.h>
#include "Battle/Regulation/include/Regulation.h"
#include <pml/include/pml_PokeParty.h>


class RegulationScriptChecker
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationScriptChecker );

public:

  static pml::PokeParty* GetCheckPokeParty();

private:

  static pml::PokeParty* s_pCheckPokeParty;

public:

  // 参加チェックの結果
  enum ResultEntry
  {
    RESULT_ENTRY_OK,               // 0 : 参加条件を満たした
    RESULT_ENTRY_NG                // 1 : 参加条件を満たしていない
  };

  // 上限下限のチェック結果
  enum ResultLimit
  {
    RESULT_LIMIT_OK,               // 0 : 上限下限のチェックに引っかからなかった
    RESULT_LIMIT_NG,               // 1 : 上限下限の設定がされておらず、参加条件を満たす事ができなかった
    RESULT_LIMIT_NG_LOWER,         // 2 : 下限の設定がされており、下限の数を満たす事ができなかった
    RESULT_LIMIT_NG_UPPER,         // 3 : 上限の設定がされており、上限の数を超えてしまった
    RESULT_LIMIT_NG_LOWER_UPPER    // 4 : 上限下限の設定がされており、参加条件を満たす事ができなかった(上限下限の範囲外)
  };

  enum ResultPokemon
  {
    RESULT_POKEMON_OK,             // 0 : 指定したポケモンがパーティに存在する
    RESULT_POKEMON_NG              // 1 : 指定したポケモンがパーティに存在しない
  };

  enum ResultItem
  {
    RESULT_ITEM_OK,                // 0 : 指定したポケモンがアイテムを所持している
    RESULT_ITEM_NG                 // 1 : 指定したポケモンがアイテムを所持していない
  };

  enum Mode
  {
    MODE_BATTLE_TEAM,
    MODE_SELECT_POKEMON
  };

private:

  enum
  {
    POKE_PARTY_MAX = 6
  };

public:

  RegulationScriptChecker( gfl2::heap::HeapBase* pHeap );
  virtual~RegulationScriptChecker();

  void CheckStart( Regulation* pRegulation, pml::PokeParty* pCheckPokeParty, Mode eMode );

  //! @retval true : チェック中
  //! @retval false : チェック完了
  bool Update();

  RegulationScriptChecker::ResultEntry GetResultEntry() const { return m_eResultEntry; }
  RegulationScriptChecker::ResultLimit GetResultLimit() const { return m_eResultLimit; }
  RegulationScriptChecker::ResultPokemon GetResultPokemon( int index ) const { return m_aePoke[ index ]; }
  RegulationScriptChecker::ResultItem GetResultItem( int index ) const { return m_aeItem[ index ]; }
  bool IsVerifyNg() const { return m_isVerifyNg; }

private:

  gfl2::heap::HeapBase*                     m_pHeap;
  gfl2::pawn::Pawn*                         m_pPawn;
  u8*                                       m_pAmxBuffer;
  RegulationScriptChecker::ResultEntry      m_eResultEntry;
  RegulationScriptChecker::ResultLimit      m_eResultLimit;
  RegulationScriptChecker::ResultPokemon    m_aePoke[ POKE_PARTY_MAX ];
  RegulationScriptChecker::ResultItem       m_aeItem[ POKE_PARTY_MAX ];
  pml::PokeParty                            m_CheckPokeParty;
  bool                                      m_isVerifyNg; //! 署名NGフラグ

#if PM_DEBUG
public:
  static bool m_debugRegulationInvalidCert;
#endif
};


#endif // REGULATION_SCRIPT_CHECKER_H_INCLUDE
