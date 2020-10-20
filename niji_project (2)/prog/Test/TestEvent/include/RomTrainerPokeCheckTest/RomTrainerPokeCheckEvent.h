#if !defined(NIJI_PROJECT_TEST_TESTEVENT_ROMTRAINERPOKECHECKEVENT_H_INCLUDED)
#define NIJI_PROJECT_TEST_TESTEVENT_ROMTRAINERPOKECHECKEVENT_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    RomTrainerPokeCheckEvent.h
 @author  munakata_kai
 @date    2017.03.15
 @brief   RomTrainerPokeCheckTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameManager.h>

#include "NetStatic/NetAppLib/include/Util/NetAppPokemonValidationUtility.h"

// バトルツリーポケモンデータ
#include <arc_index/battle_tree_poke.gaix>
// バトルロイヤルポケモンデータ
#include <arc_index/battle_royal_poke.gaix>
// シナリオトレーナーＩＤデータ
#include <niji_conv_header/trainer/trid_def.h>
// poke_libのインクルード
#include <pml/include/pmlib.h>


namespace BattleInst
{
  struct BINST_TRAINER_ROM_DATA;
}

namespace pml{
  class PokeParty;
}


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(RomTrainerPokeCheckTest)


//==============================================================================
/**
 @class     RomTrainerPokeCheckEvent
 @brief     RomTrainerPokeCheckTest
 */
//==============================================================================
class RomTrainerPokeCheckEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(RomTrainerPokeCheckEvent);

public:
  //! @brief  このイベントを起動する関数
  static void StartEvent(GameSys::GameManager* gmgr);

public:
  RomTrainerPokeCheckEvent(gfl2::heap::HeapBase* heap);
  virtual ~RomTrainerPokeCheckEvent();

  virtual bool BootChk(GameSys::GameManager* gmgr);
  virtual void InitFunc(GameSys::GameManager* gmgr);
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);
  virtual void EndFunc(GameSys::GameManager* gmgr);

  void SetPlaceID(pml::pokepara::PokemonParam* pp);

private:
  gfl2::heap::HeapBase*                             m_pHeap;                          ///<
#if defined( GF_PLATFORM_CTR )
  // NetAppPokemonValidationUtilityに必要
  class Dummy : public NetAppLib::Util::ServerErrorListener
  {
    void OnServerError(void) {}
  };
  Dummy                                             m_ServerErrorListener;
  NetAppLib::Util::NetAppPokemonValidationUtility   m_PokemonValidationUtility;
#endif // defined( GF_PLATFORM_CTR )
  u32 m_pokeIndex;
  u32 m_trainerIndex;
  u32 m_memberIndex;
  pml::PokeParty* m_pParty;
  u32 m_waitTime;
  int m_nextSeq;
  struct TREE_ROYAL_RESULT
  {
    int id;
    int mons_no;
    int tokusei_id;
    int result_code;
    TREE_ROYAL_RESULT()
    {
      id = -1;
      mons_no = -1;
      tokusei_id = -1;
      result_code = -1;
    }
  };

  struct SCENARIO_RESULT
  {
    int tr_id;
    int member_id;
    int mons_no;
    int result_code;
    SCENARIO_RESULT()
    {
      tr_id = -1;
      member_id = -1;
      mons_no = -1;
      result_code = -1;
    }
  };

  TREE_ROYAL_RESULT m_treeResult[GARC_battle_tree_poke_DATA_NUM];
  TREE_ROYAL_RESULT m_royalResult[GARC_battle_royal_poke_DATA_NUM];
  SCENARIO_RESULT m_scenarioResult[pml::PokeParty::MAX_MEMBERS * trainer::TRID_MAX];
};


GFL_NAMESPACE_END(RomTrainerPokeCheckTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_TESTEVENT_ROMTRAINERPOKECHECKEVENT_H_INCLUDED

