//==============================================================================
/**
@file    RomTrainerPokeCheckEvent.h
@author  munakata_kai
@date    2017.03.15
@brief   RomTrainerPokeCheckTest
*/
//==============================================================================


#if PM_DEBUG
#include <Test/TestEvent/include/RomTrainerPokeCheckTest/RomTrainerPokeCheckEvent.h>

// gfl2のインクルード
#include <fs/include/gfl2_Fs.h>

#include <arc_def_index/arc_def.h>

// バトル施設ポケモン読み込み用
#include "Field/FieldStatic/include/BattleInst/BattleInstTool.h"
#include "Field/FieldStatic/include/BattleInst/BattleInstData.h"

// シナリオトレーナー読み込み用
#include <Trainer/Trainer/include/tr_tool.h>

// HEAP
#include "System/include/HeapDefine.h"
// 通信
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include <NetStatic/NetLib/include/Wifi/WifiConnectRunner.h>
#include <gflnet2/include/nex/gflnet2_NexManager.h>

#include <PokeTool/include/PokeMemo.h>
#include <Savedata/include/MyStatus.h>
#include "GameSys/include/GameData.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(RomTrainerPokeCheckTest)


void RomTrainerPokeCheckEvent::StartEvent(GameSys::GameManager* gmgr)
{
  GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, RomTrainerPokeCheckEvent>(pGameEventManager);
}

RomTrainerPokeCheckEvent::RomTrainerPokeCheckEvent(gfl2::heap::HeapBase* heap)
  : GameSys::GameEvent(heap)
  , m_pHeap(heap)
#if defined( GF_PLATFORM_CTR )
  , m_PokemonValidationUtility(heap)
#endif // defined( GF_PLATFORM_CTR )
  , m_waitTime(0)
  , m_nextSeq(0)
{
#if defined( GF_PLATFORM_CTR )
  m_PokemonValidationUtility.SetServerErrorListener(&m_ServerErrorListener);
#endif // defined( GF_PLATFORM_CTR )
}

RomTrainerPokeCheckEvent::~RomTrainerPokeCheckEvent()
{
#if defined( GF_PLATFORM_CTR )
  m_PokemonValidationUtility.SetServerErrorListener(NULL);//念のため
#endif // defined( GF_PLATFORM_CTR )
}

bool RomTrainerPokeCheckEvent::BootChk(GameSys::GameManager* /*gmgr*/)
{
  return true;
}
void RomTrainerPokeCheckEvent::InitFunc(GameSys::GameManager* gmgr)
{}

GameSys::GMEVENT_RESULT RomTrainerPokeCheckEvent::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_WIFI_INITIALIZE,
    SEQ_BATTLE_TREE_INITIALIZE,
    SEQ_BATTLE_TREE_CHECK,
    SEQ_BATTLE_TREE_UPDATE,
    SEQ_BATTLE_ROYAL_INITIALIZE,
    SEQ_BATTLE_ROYAL_CHECK,
    SEQ_BATTLE_ROYAL_UPDATE,
    SEQ_BATTLE_SCENARIO_INITIALIZE,
    SEQ_BATTLE_SCENARIO_CHECK,
    SEQ_BATTLE_SCENARIO_UPDATE,
    SEQ_WIFI_TERMINATE,
    SEQ_WAIT_TIME,
    SEQ_RESULT_OUTPUT,
    SEQ_END,
  };

  GameSys::GMEVENT_RESULT res = GameSys::GMEVENT_RES_CONTINUE;
  
  int seq = this->GetSeqNo();  // 親であるGameSys::GameEventのメンバ関数
#if defined( GF_PLATFORM_CTR )
  switch(seq)
  {
  case SEQ_WIFI_INITIALIZE:
    {
      gfl2::heap::HeapBase*  heapDev = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      NetLib::NijiNetworkSystem::InitializeWifiNetwork();
      NetLib::Wifi::WifiConnectRunner::E_EXEC_MODE mode = NetLib::Wifi::WifiConnectRunner::E_MODE_CONNECT_AND_CREATE_NEXID;
      NetLib::Wifi::WifiConnectRunner* pWifiConnect = GFL_NEW(heapDev) NetLib::Wifi::WifiConnectRunner(mode);

      while (true)
      {
        NetLib::Wifi::WifiConnectRunner::E_RET_CODE ret = pWifiConnect->Update();
        if (ret != NetLib::Wifi::WifiConnectRunner::E_RET_CONTINUE)
        {
          break;
        }
        GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
        gfl2::thread::ThreadStatic::CurrentSleep(1);
      }
      GFL_SAFE_DELETE(pWifiConnect);
      seq = SEQ_BATTLE_TREE_INITIALIZE;
      break;
    }

  case SEQ_BATTLE_TREE_INITIALIZE:
    {
      // SEQ_BATTLE_TREE_CHECK準備
      m_pokeIndex = 0;
      m_memberIndex = 0;
      m_pParty = GFL_NEW(m_pHeap) pml::PokeParty(m_pHeap);
      while (m_memberIndex < pml::PokeParty::MAX_MEMBERS)
      {
        BattleInst::BINST_POKEMON_ROM_DATA  pd;
        BattleInstTool::GetPokemonRomData(m_pHeap, gmgr->GetAsyncFileManager(), ARCID_BATTLE_TREE_POKE, m_pokeIndex, &pd);
        GFL_ASSERT(pd.mons_no != MONSNO_NULL);
        for (u32 tokuseiIndex(0); tokuseiIndex <= pml::MAX_TOKUSEI_INDEX; ++tokuseiIndex)
        {
          pml::pokepara::PokemonParam* pp = BattleInstTool::MakePokemonParam(&pd, 100, 0x1f, m_pHeap, tokuseiIndex);// 性別指定しない
          // ゲーム中にはやらないが不正チェックにいるため
          PokeTool::PokeMemo::SetFromCapture(pp, gmgr->GetGameData()->GetPlayerStatusConst(), 6, m_pHeap->GetLowerHandle());
          pp->SetLangId(System::GetLang());
          pp->SetGetBall(item::BALLID_MONSUTAABOORU); // 捕獲ボール
          SetPlaceID(pp);
          m_pParty->AddMember(*pp);
          GFL_SAFE_DELETE(pp);
          ++m_memberIndex;
        }
        ++m_pokeIndex;
      }
      seq = SEQ_BATTLE_TREE_CHECK;
      // breakスルー
    }

  case SEQ_BATTLE_TREE_CHECK:
    {
      // チェック
      if (m_PokemonValidationUtility.Verify(NetLib::Validation::PokemonValidation::E_GAME_MODE_BATTLE_M, m_pParty))
      {
        int nextSeq = SEQ_BATTLE_TREE_UPDATE;
        for (u32 index(0); index < m_memberIndex; ++index)
        {
          NetAppLib::Util::NetAppPokemonValidationUtility::VerifyResult eVerifyResult = m_PokemonValidationUtility.GetVerifyResult(index);
          switch (eVerifyResult)
          {
          case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_OK:
            //GFL_PRINT("VERIFY_RESULT_OK : BattleTreePoke MonsNo = %d ID = %d tokusei = %d\n", m_pp->GetMonsNo(), m_pokeIndex, m_tokuseiIndex);
            break;

          case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NG:
            {
              // ６体ずつ調べるため、m_pokeIndexから実際のIDを求める計算
              u32 array_index = (m_pokeIndex - ((m_memberIndex - 1 - index) / (pml::MAX_TOKUSEI_INDEX + 1)));
              m_treeResult[array_index].id = array_index;
              m_treeResult[array_index].mons_no = m_pParty->GetMemberPointer(index)->GetMonsNo();
              m_treeResult[array_index].tokusei_id = m_pParty->GetMemberPointer(index)->GetTokuseiIndex();
              m_treeResult[array_index].result_code = m_PokemonValidationUtility.GetResultCode(index);
              //GFL_PRINT("VERIFY_RESULT_NG : BattleTreePoke MonsNo = %d ID = %d tokusei = %d\n", , (m_pokeIndex - ((m_memberIndex - 1 - index) / 3)), m_pParty->GetMemberPointer(index)->GetTokuseiIndex());
              break;
            }

          case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NET_ERROR:
            {
              nextSeq = SEQ_BATTLE_TREE_CHECK;
              break;
            }
          }

          if (nextSeq == SEQ_BATTLE_TREE_CHECK)
          {
            break;
          }
        }
        seq = nextSeq;
      }
      break;
    }

  case SEQ_BATTLE_TREE_UPDATE:
    {
      // チェック終了
      if (m_pokeIndex >= GARC_battle_tree_poke_DATA_NUM)
      {
        // 開放
        GFL_SAFE_DELETE(m_pParty);
        seq = SEQ_BATTLE_ROYAL_INITIALIZE;
      }
      // 次のチェックへ
      else
      {
        // 開放
        GFL_SAFE_DELETE(m_pParty);
        seq = SEQ_BATTLE_TREE_CHECK;
        m_memberIndex = 0;
        m_pParty = GFL_NEW(m_pHeap) pml::PokeParty(m_pHeap);
        while (m_memberIndex < pml::PokeParty::MAX_MEMBERS && m_pokeIndex < GARC_battle_tree_poke_DATA_NUM)
        {
          BattleInst::BINST_POKEMON_ROM_DATA  pd;
          BattleInstTool::GetPokemonRomData(m_pHeap, gmgr->GetAsyncFileManager(), ARCID_BATTLE_TREE_POKE, m_pokeIndex, &pd);
          GFL_ASSERT(pd.mons_no != MONSNO_NULL);
          for (u32 tokuseiIndex(0); tokuseiIndex <= pml::MAX_TOKUSEI_INDEX; ++tokuseiIndex)
          {
            pml::pokepara::PokemonParam* pp = BattleInstTool::MakePokemonParam(&pd, 100, 0x1f, m_pHeap, tokuseiIndex);// 性別指定しない
            // ゲーム中にはやらないが不正チェックにいるため
            PokeTool::PokeMemo::SetFromCapture(pp, gmgr->GetGameData()->GetPlayerStatusConst(), 6, m_pHeap->GetLowerHandle());
            pp->SetLangId(System::GetLang());
            pp->SetGetBall(item::BALLID_MONSUTAABOORU); // 捕獲ボール
            SetPlaceID(pp);
            m_pParty->AddMember(*pp);
            GFL_SAFE_DELETE(pp);
            ++m_memberIndex;
          }
          // ポケモン更新
          ++m_pokeIndex;
        }
      }
      m_waitTime = 0;
      m_nextSeq = seq;
      seq = SEQ_WAIT_TIME;
      break;
    }

  case SEQ_BATTLE_ROYAL_INITIALIZE:
    {
      // SEQ_BATTLE_ROYAL_CHECK準備
      m_pokeIndex = 0;
      m_memberIndex = 0;
      m_pParty = GFL_NEW(m_pHeap) pml::PokeParty(m_pHeap);
      while (m_memberIndex < pml::PokeParty::MAX_MEMBERS && m_pokeIndex < GARC_battle_royal_poke_DATA_NUM)
      {
        BattleInst::BINST_POKEMON_ROM_DATA  pd;
        BattleInstTool::GetPokemonRomData(m_pHeap, gmgr->GetAsyncFileManager(), ARCID_BATTLE_ROYAL_POKE, m_pokeIndex, &pd);
        GFL_ASSERT(pd.mons_no != MONSNO_NULL);
        for (u32 tokuseiIndex(0); tokuseiIndex <= pml::MAX_TOKUSEI_INDEX; ++tokuseiIndex)
        {
          pml::pokepara::PokemonParam* pp = BattleInstTool::MakePokemonParam(&pd, 100, 0x1f, m_pHeap, tokuseiIndex);// 性別指定しない
          // ゲーム中にはやらないが不正チェックにいるため
          PokeTool::PokeMemo::SetFromCapture(pp, gmgr->GetGameData()->GetPlayerStatusConst(), 6, m_pHeap->GetLowerHandle());
          pp->SetLangId(System::GetLang());
          pp->SetGetBall(item::BALLID_MONSUTAABOORU); // 捕獲ボール
          SetPlaceID(pp);
          m_pParty->AddMember(*pp);
          GFL_SAFE_DELETE(pp);
          ++m_memberIndex;
        }
        ++m_pokeIndex;
      }
      seq = SEQ_BATTLE_ROYAL_CHECK;
      // breakスルー
    }

  case SEQ_BATTLE_ROYAL_CHECK:
    {
      // チェック
      if (m_PokemonValidationUtility.Verify(NetLib::Validation::PokemonValidation::E_GAME_MODE_BATTLE_M, m_pParty))
      {
        int nextSeq = SEQ_BATTLE_ROYAL_UPDATE;
        for (u32 index(0); index < m_memberIndex; ++index)
        {
          NetAppLib::Util::NetAppPokemonValidationUtility::VerifyResult eVerifyResult = m_PokemonValidationUtility.GetVerifyResult(index);
          switch (eVerifyResult)
          {
          case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_OK:
            //GFL_PRINT("VERIFY_RESULT_OK : BattleRoyalPoke MonsNo = %d ID = %d tokusei = %d\n", m_pp->GetMonsNo(), m_pokeIndex, m_tokuseiIndex);
            break;

          case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NG:
            {
              // ６体ずつ調べるため、m_pokeIndexから実際のIDを求める計算
              u32 array_index = (m_pokeIndex - ((m_memberIndex - 1 - index) / (pml::MAX_TOKUSEI_INDEX + 1)));
              m_royalResult[array_index].id = array_index;
              m_royalResult[array_index].mons_no = m_pParty->GetMemberPointer(index)->GetMonsNo();
              m_royalResult[array_index].tokusei_id = m_pParty->GetMemberPointer(index)->GetTokuseiIndex();
              m_royalResult[array_index].result_code = m_PokemonValidationUtility.GetResultCode(index);
              //GFL_PRINT("VERIFY_RESULT_NG : BattleRoyalPoke MonsNo = %d ID = %d tokusei = %d\n", m_pParty->GetMemberPointer(index)->GetMonsNo(), (m_pokeIndex - ((m_memberIndex - 1 - index) / 3)), m_pParty->GetMemberPointer(index)->GetTokuseiIndex());
              break;
            }
          case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NET_ERROR:
            {
              nextSeq = SEQ_BATTLE_ROYAL_CHECK;
              break;
            }
          }

          if (nextSeq == SEQ_BATTLE_ROYAL_CHECK)
          {
            break;
          }
        }
        seq = nextSeq;
      }
      break;
    }

  case SEQ_BATTLE_ROYAL_UPDATE:
    {
      // チェック終了
      if (m_pokeIndex >= GARC_battle_royal_poke_DATA_NUM)
      {
        // 開放
        GFL_SAFE_DELETE(m_pParty);
        seq = SEQ_BATTLE_SCENARIO_INITIALIZE;
      }
      // 次のチェックへ
      else
      {
        seq = SEQ_BATTLE_ROYAL_CHECK;
        // 開放
        GFL_SAFE_DELETE(m_pParty);
        m_pParty = GFL_NEW(m_pHeap) pml::PokeParty(m_pHeap);
        m_memberIndex = 0;
        while (m_memberIndex < pml::PokeParty::MAX_MEMBERS && m_pokeIndex < GARC_battle_royal_poke_DATA_NUM)
        {
          BattleInst::BINST_POKEMON_ROM_DATA  pd;
          BattleInstTool::GetPokemonRomData(m_pHeap, gmgr->GetAsyncFileManager(), ARCID_BATTLE_ROYAL_POKE, m_pokeIndex, &pd);
          // バトルロイヤルのポケモンは2017/3/15時点では0～497まで登録されているがそれ以降は空
          if (pd.mons_no != MONSNO_NULL)
          {
            for (u32 tokuseiIndex(0); tokuseiIndex <= pml::MAX_TOKUSEI_INDEX; ++tokuseiIndex)
            {
              pml::pokepara::PokemonParam* pp = BattleInstTool::MakePokemonParam(&pd, 100, 0x1f, m_pHeap, tokuseiIndex);// 性別指定しない
              // ゲーム中にはやらないが不正チェックにいるため
              PokeTool::PokeMemo::SetFromCapture(pp, gmgr->GetGameData()->GetPlayerStatusConst(), 6, m_pHeap->GetLowerHandle());
              pp->SetLangId(System::GetLang());
              pp->SetGetBall(item::BALLID_MONSUTAABOORU); // 捕獲ボール
              SetPlaceID(pp);
              m_pParty->AddMember(*pp);
              GFL_SAFE_DELETE(pp);
              ++m_memberIndex;
            }
          }
          // ポケモン更新
          ++m_pokeIndex;
        }
      }
      m_waitTime = 0;
      m_nextSeq = seq;
      seq = SEQ_WAIT_TIME;
      break;
    }

  case SEQ_BATTLE_SCENARIO_INITIALIZE:
    {
      // SEQ_BATTLE_SCENARIO_CHECK準備
      m_trainerIndex = trainer::TRID_MINI_01;
      m_pParty = GFL_NEW(m_pHeap) pml::PokeParty(m_pHeap);
      trainer::EncountTrainerPokeDataMake(static_cast<trainer::TrainerID>(m_trainerIndex), m_pParty, m_pHeap);
      m_memberIndex = m_pParty->GetMemberCount();
      for (u32 index(0); index < m_memberIndex; ++index)
      {
        pml::pokepara::PokemonParam* pp = m_pParty->GetMemberPointer(index);
        // ゲーム中にはやらないが不正チェックにいるため
        PokeTool::PokeMemo::SetFromCapture(pp, gmgr->GetGameData()->GetPlayerStatusConst(), 6, m_pHeap->GetLowerHandle());
        pp->SetLangId(System::GetLang());
        pp->SetGetBall(item::BALLID_MONSUTAABOORU); // 捕獲ボール
        SetPlaceID(pp);
      }
      seq = SEQ_BATTLE_SCENARIO_CHECK;
      // breakスルー
    }

  case SEQ_BATTLE_SCENARIO_CHECK:
    {
      // チェック
      if (m_PokemonValidationUtility.Verify(NetLib::Validation::PokemonValidation::E_GAME_MODE_BATTLE_M, m_pParty))
      {
        int nextSeq = SEQ_BATTLE_SCENARIO_UPDATE;
        for (u32 index(0); index < m_memberIndex; ++index)
        {
          NetAppLib::Util::NetAppPokemonValidationUtility::VerifyResult eVerifyResult = m_PokemonValidationUtility.GetVerifyResult(index);
          switch (eVerifyResult)
          {
          case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_OK:
            //GFL_PRINT("VERIFY_RESULT_OK : BattleRoyalPoke MonsNo = %d ID = %d tokusei = %d\n", m_pp->GetMonsNo(), m_pokeIndex, m_tokuseiIndex);
            break;

          case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NG:
            {
              u32 array_index = m_trainerIndex * 6 + index;
              m_scenarioResult[array_index].tr_id = m_trainerIndex;
              m_scenarioResult[array_index].member_id = index;
              m_scenarioResult[array_index].mons_no = m_pParty->GetMemberPointer(index)->GetMonsNo();
              m_scenarioResult[array_index].result_code = m_PokemonValidationUtility.GetResultCode(index);
              //GFL_PRINT("VERIFY_RESULT_NG : Trainer ID = %d Member ID = %d MonsNo = %d\n", m_trainerIndex, index, m_pParty->GetMemberPointer(index)->GetMonsNo());
              break;
            }
          case NetAppLib::Util::NetAppPokemonValidationUtility::VERIFY_RESULT_NET_ERROR:
            {
              nextSeq = SEQ_BATTLE_SCENARIO_CHECK;
              break;
            }
          }

          if (nextSeq == SEQ_BATTLE_SCENARIO_CHECK)
          {
            break;
          }
        }
        seq = nextSeq;
      }
      break;
    }

  case SEQ_BATTLE_SCENARIO_UPDATE:
    {
      // チェック終了
      if (m_trainerIndex >= trainer::TRID_MAX - 1)
      {
        // 開放
        GFL_SAFE_DELETE(m_pParty);
        seq = SEQ_WIFI_TERMINATE;
      }
      // 次のチェックへ
      else
      {
        seq = SEQ_BATTLE_SCENARIO_CHECK;
        // 開放
        GFL_SAFE_DELETE(m_pParty);
        ++m_trainerIndex;
        m_pParty = GFL_NEW(m_pHeap) pml::PokeParty(m_pHeap);
        trainer::EncountTrainerPokeDataMake(static_cast<trainer::TrainerID>(m_trainerIndex), m_pParty, m_pHeap);
        m_memberIndex = m_pParty->GetMemberCount();
        for (u32 index(0); index < m_memberIndex; ++index)
        {
          pml::pokepara::PokemonParam* pp = m_pParty->GetMemberPointer(index);
          // ゲーム中にはやらないが不正チェックにいるため
          PokeTool::PokeMemo::SetFromCapture(pp, gmgr->GetGameData()->GetPlayerStatusConst(), 6, m_pHeap->GetLowerHandle());
          pp->SetLangId(System::GetLang());
          pp->SetGetBall(item::BALLID_MONSUTAABOORU); // 捕獲ボール
          SetPlaceID(pp);
        }
        m_waitTime = 0;
        m_nextSeq = seq;
        seq = SEQ_WAIT_TIME;
      }
      break;
    }

  case SEQ_WIFI_TERMINATE:
    {
      gfl2::heap::HeapBase*  heapDev = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      NetLib::Wifi::WifiConnectRunner::E_EXEC_MODE mode = NetLib::Wifi::WifiConnectRunner::E_MODE_DISCONNECT_SYNC;
      NetLib::Wifi::WifiConnectRunner* pWifiConnect = GFL_NEW(heapDev) NetLib::Wifi::WifiConnectRunner(mode);

      while (true)
      {
        NetLib::Wifi::WifiConnectRunner::E_RET_CODE ret = pWifiConnect->Update();
        if (ret != NetLib::Wifi::WifiConnectRunner::E_RET_CONTINUE)
        {
          break;
        }
        GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
        gfl2::thread::ThreadStatic::CurrentSleep(1);
      }
      GFL_SAFE_DELETE(pWifiConnect);
      seq = SEQ_RESULT_OUTPUT;
      break;
    }

  case SEQ_WAIT_TIME:
    {
      ++m_waitTime;
      if (m_waitTime >= 10)
      {
        m_waitTime = 0;
        seq = m_nextSeq;
        m_nextSeq = 0;
      }
      break;
    }

  case SEQ_RESULT_OUTPUT:
    {
      GFL_PRINT("START_RESULT_OUTPUT\n");
      for (u32 index(0); index < GARC_battle_tree_poke_DATA_NUM; ++index)
      {
        if (m_treeResult[index].id > 0)
        {
          GFL_PRINT("VERIFY_RESULT_NG[%d] : BattleTreePoke ID = %d MonsNo = %d tokusei = %d\n", m_treeResult[index].result_code, m_treeResult[index].id, m_treeResult[index].mons_no, m_treeResult[index].tokusei_id);
        }
      }
      for (u32 index(0); index < GARC_battle_royal_poke_DATA_NUM; ++index)
      {
        if (m_royalResult[index].id > 0)
        {
          GFL_PRINT("VERIFY_RESULT_NG[%d] : BattleRoyalPoke ID = %d MonsNo = %d tokusei = %d\n", m_royalResult[index].result_code, m_royalResult[index].id, m_royalResult[index].mons_no, m_royalResult[index].tokusei_id);
        }
      }
      for (u32 index(0); index < pml::PokeParty::MAX_MEMBERS * trainer::TRID_MAX; ++index)
      {
        if (m_scenarioResult[index].tr_id > 0)
        {
          GFL_PRINT("VERIFY_RESULT_NG[%d] : Trainer ID = %d Member ID = %d MonsNo = %d\n", m_scenarioResult[index].result_code, m_scenarioResult[index].tr_id, m_scenarioResult[index].member_id, m_scenarioResult[index].mons_no);
        }
      }
      GFL_PRINT("END_RESULT_OUTPUT\n");
      seq = SEQ_END;
      break;
    }

  case SEQ_END:
    {
      if (m_PokemonValidationUtility.CanDestroy())
      {
        // 破棄できる
        res = GameSys::GMEVENT_RES_FINISH;
      }
      break;
    }
  }
#else
  seq = SEQ_END;
  res = GameSys::GMEVENT_RES_FINISH;
#endif
  this->SetSeqNo(seq);  // 親であるGameSys::GameEventのメンバ関数

  return res;
}

void RomTrainerPokeCheckEvent::EndFunc(GameSys::GameManager* /*gmgr*/)
{}

// 不正コード210&212に対応するため（捕まえた場所＆地名が不正）
void RomTrainerPokeCheckEvent::SetPlaceID(pml::pokepara::PokemonParam* pp)
{
  switch (pp->GetMonsNo())
  {
  case 132:
  case 144:
  case 145:
  case 146:
  case 150:
  case 151:
  case 243:
  case 244:
  case 245:
  case 249:
  case 250:
  case 251:
  case 377:
  case 378:
  case 379:
  case 380:
  case 381:
  case 382:
  case 383:
  case 384:
  case 385:
  case 386:
  case 480:
  case 481:
  case 482:
  case 483:
  case 484:
  case 485:
  case 486:
  case 487:
  case 488:
  case 490:
  case 491:
  case 492:
  case 493:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 30001);
      break;
    }

  case 494:
  case 647:
  case 648:
  case 649:
  case 716:
  case 717:
  case 718:
  case 719:
  case 720:
  case 721:
  case 785:
  case 801:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 40001);
      break;
    }

  case 638:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 74);
      break;
    }
  case 639:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 73);
      break;
    }
  case 640:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 55);
      break;
    }

  case 643:
  case 644:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 40069);
      break;
    }

  case 645:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 40046);
      break;
    }

  case 646:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 40006);
      break;
    }

  case 772:
  case 773:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 188);
      break;
    }

  case 786:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 92);
      break;
    }

  case 787:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 140);
      break;
    }

  case 788:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 180);
      break;
    }

  case 789:
  case 790:
  case 791:
  case 792:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 144);
      break;
    }

  case 793:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 100);
      break;
    }

  case 794:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 40);
      break;
    }

  case 795:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 46);
      break;
    }

  case 796:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 90);
      break;
    }

  case 797:
  case 798:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 134);
      break;
    }

  case 799:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 182);
      break;
    }

  case 800:
    {
      pp->SetMemories(pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, 36);
      break;
    }
  }
}

GFL_NAMESPACE_END(RomTrainerPokeCheckTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

