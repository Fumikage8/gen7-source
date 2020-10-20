#if PM_DEBUG
//======================================================================
/**
* @file BattleFesDebug.cpp
* @date 2016/12/14
* @author munakata_kai
* @brief バトルフェスのデバッグ
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================
#include "Field/FieldStatic/include/BattleFes/BattleFesDebug.h"
#include "Field/FieldStatic/include/BattleFes/BattleFesDefine.h"
#include <Savedata/include/BattleFesSave.h>
// システム
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>

#include "Field/FieldStatic/include/BattleFes/BattleFesManager.h"
#include "Field/FieldStatic/include/BattleFes/BattleFes.h"
#include "Field/FieldStatic/include/BattleFes/BattleFesPokeLotterySystem.h"
#include "Savedata/include/RandomGroup.h"
#include "savedata/include/EventWork.h"

#include "System/include/HeapDefine.h"
#include <niji_conv_header/message/debug/msg_d_munakata_kai.h>
#include "debug/DebugWin/include/DebugWinSystem.h"
#include <Print/include/PrintSystem.h>
#include <arc_index/debug_message.gaix>
#include <str/include/gfl2_MsgData.h>
// sound
#include "Sound/include/Sound.h"

#include "Field/FieldStatic/include/BattleFes/BattleFesTool.h"
// ジョインフェスタパーソナルデータ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"

GFL_NAMESPACE_BEGIN(BattleFesDebug)

static gfl2::heap::HeapBase* s_pDebugHeap = NULL;
static gfl2::debug::DebugWinGroup* s_pRootDebugWinGroup = NULL;
static s32 s_winCount = 0;
static s32 s_oldWinCount = 0;
static s32 s_rank = 0;
static s32 s_resetCount = 0;
static s32 s_openFlg = 0;
static s32 s_fixedLevelFlg = 0;
static s32 s_fieldType = -1;
static gfl2::str::MsgData* s_pMsgData = NULL;
static gfl2::str::StrBuf* s_pStrBuf = NULL;
static s32 s_nowFieldType = -1;

// Debug　更新関数
void UpdateWinCount(void* userWork, gfl2::debug::DebugWinItem* item)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  Savedata::BattleFesSave* pBattleFesSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetBattleFesSave();

  // 選択
  if (pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
  {
    s_winCount--;
    if (s_winCount < 0) s_winCount = (pBattleFesSave->GetRankUpBattleSetCount() * BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX) - 1;
  }
  if (pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
  {
    s_winCount++;
    if (s_winCount > ((pBattleFesSave->GetRankUpBattleSetCount() * BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX) - 1)) s_winCount = 0;
  }

  if (pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    pBattleFesSave->SetWinCount(s_winCount);
    s32 SetCount = static_cast<s32>(pBattleFesSave->GetNowBattleSetCount());
    s32 oldSetCount = static_cast<s32>(s_oldWinCount / BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX);
    if (SetCount != oldSetCount)
    {
      // トレーナーデータが変化するので対処
      pBattleFesSave->ClearTrainerData();
    }
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBattleFesManager()->GetBattleFes()->DebugCreateEnemy(s_pDebugHeap);
    s_oldWinCount = s_winCount;
    Sound::PlaySE(SEQ_SE_DECIDE1);
  }
}

void UpdateRank(void* userWork, gfl2::debug::DebugWinItem* item)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  // 選択
  if (pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
  {
    s_rank--;
    if (s_rank < 0) s_rank = BattleFes::BattleFesDefine::BFES_RANK_MAX;
  }
  if (pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
  {
    s_rank++;
    if (s_rank > BattleFes::BattleFesDefine::BFES_RANK_MAX) s_rank = 0;
  }

  if (pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Savedata::BattleFesSave* pBattleFesSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetBattleFesSave();
    pBattleFesSave->SetRank(s_rank);
    // @fix MMCat Contact[151] 【バトルエージェント】デバッグモード使用時の不具合へのご対応要望
    // 抽選ポケモン（レベル）が変化するので対処
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBattleFesManager()->GetBattleFesPokeLotterySystem()->UpdatePokemonData(s_pDebugHeap);
    // 既に借りているポケモンのレベル更新
    if (pBattleFesSave->IsExistRentalPokemonData())
    {
      // メガリング所持の有無取得
      NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();
      NetAppLib::JoinFesta::PacketGameStatus1 val;
      if (pMyData->GetPacketGameStatus1(&val) == false)
      {
        // 情報取得失敗時
        val.has_mega_ring = 0;
        val.has_zenryoku_ring = 1;
      }
      pml::pokepara::PokemonParam* pPokeParam = BattleFesTool::CreateBattleTreePokemon(s_pDebugHeap, pBattleFesSave->GetBattleTreeID(), pBattleFesSave->GetBattleTreeSex(), pBattleFesSave->GetBattleTreeTokusei(), pBattleFesSave->GetRank(), val.has_mega_ring, val.has_zenryoku_ring);
      pBattleFesSave->SetRentalPokemonData(pPokeParam);
      GFL_SAFE_DELETE(pPokeParam);
    }
    // ランクに応じて最大連勝数とトレーナーデータが変わる
    s_winCount = gfl2::math::clamp(s_winCount, static_cast<s32>(0), static_cast<s32>((pBattleFesSave->GetRankUpBattleSetCount() * BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX) - 1));
    pBattleFesSave->SetWinCount(s_winCount);
    s_oldWinCount = s_winCount;
    // トレーナーデータが変化するので対処
    pBattleFesSave->ClearTrainerData();
    // トレーナーデータが変化するので対処
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBattleFesManager()->GetBattleFes()->DebugCreateEnemy(s_pDebugHeap);
    Sound::PlaySE(SEQ_SE_DECIDE1);
  }
}

void UpdateResetCount(void* userWork, gfl2::debug::DebugWinItem* item)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  const u16 max = 0xffff;

  // 選択
  if (pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
  {
    if (pButton->IsRepeat(gfl2::ui::BUTTON_L | gfl2::ui::BUTTON_R))
    {
      s_resetCount -= 10;
    }
    else
    {
      s_resetCount--;
    }
    if (s_resetCount < 0) s_resetCount = max;
  }
  if (pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
  {
    if (pButton->IsRepeat(gfl2::ui::BUTTON_L | gfl2::ui::BUTTON_R))
    {
      s_resetCount += 10;
    }
    else
    {
      s_resetCount++;
    }
    if (s_resetCount > max) s_resetCount = 0;
  }

  if (pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Savedata::BattleFesSave* pBattleFesSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetBattleFesSave();
    pBattleFesSave->SetTotalRandCount(s_resetCount);
    // 抽選ポケモンが変化するので対処
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBattleFesManager()->GetBattleFesPokeLotterySystem()->PokemonLottery(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetRandomGroup()->GetTodayRandSeed(), s_pDebugHeap, false);
    Sound::PlaySE(SEQ_SE_DECIDE1);
  }
}

void UpdateOpenFlg(void* userWork, gfl2::debug::DebugWinItem* item)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  // 選択
  if (pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
  {
    s_openFlg--;
    if (s_openFlg < 0) s_openFlg = 1;
  }
  if (pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
  {
    s_openFlg++;
    if (s_openFlg > 1) s_openFlg = 0;
  }

  if (pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    Field::EventWork* ev = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
    if ((s_openFlg != 0))
    {
      ev->SetEventFlag(FE_BATTLE_FES_OPEN);
    }
    else
    {
      ev->ResetEventFlag(FE_BATTLE_FES_OPEN);
    }
    Sound::PlaySE(SEQ_SE_DECIDE1);
  }
}

void UpdateFixedLevelFlg(void* userWork, gfl2::debug::DebugWinItem* item)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  // 選択
  if (pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
  {
    s_fixedLevelFlg--;
    if (s_fixedLevelFlg < 0) s_fixedLevelFlg = 1;
  }
  if (pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
  {
    s_fixedLevelFlg++;
    if (s_fixedLevelFlg > 1) s_fixedLevelFlg = 0;
  }
}

void UpdateField(void* userWork, gfl2::debug::DebugWinItem* item)
{
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  // 選択
  if (pButton->IsRepeat(gfl2::ui::BUTTON_LEFT))
  {
    s_fieldType--;
    if (s_fieldType < -1) s_fieldType = BATTLE_FES_FIELD_MAX - 1;
  }
  if (pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT))
  {
    s_fieldType++;
    if (s_fieldType > BATTLE_FES_FIELD_MAX - 1) s_fieldType = -1;
  }
  if (pButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    // 設定無し
    s_fieldType = -1;
    Sound::PlaySE(SEQ_SE_DECIDE1);
  }
}

// Debug　描画関数
wchar_t* DrawWinCount(void* userWork, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), s_winCount);
  return workStr;
}

wchar_t* DrawRank(void* userWork, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), s_rank);
  return workStr;
}

wchar_t* DrawResetCount(void* userWork, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), s_resetCount);
  return workStr;
}

wchar_t* DrawOpenFlg(void* userWork, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), s_openFlg);
  return workStr;
}

wchar_t* DrawFixedLevelFlg(void* userWork, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), s_fixedLevelFlg);
  return workStr;
}

wchar_t* DrawField(void* userWork, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  const u32 fieldType[] = {
    BattleFesField_00,
    BattleFesField_01,
    BattleFesField_02,
    BattleFesField_03,
    BattleFesField_04,
    BattleFesField_05,
    BattleFesField_06,
    BattleFesField_07,
    BattleFesField_08,
    BattleFesField_09,
    BattleFesField_10,
    BattleFesField_11,
    BattleFesField_12,
  };
  // 毎フレームアクセス防止
  if (s_nowFieldType != s_fieldType)
  {
    s_pMsgData->GetString(fieldType[(s_fieldType + 1)], *s_pStrBuf);
    s_nowFieldType = s_fieldType;
  }
  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), s_pStrBuf->GetPtr());
  return workStr;
}

void InitializeBattleFesDebug()
{
  s_pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);

  // debug_menuの文字列データ
  s_pMsgData = GFL_NEW(s_pDebugHeap) gfl2::str::MsgData(print::GetMessageArcId_Debug(), GARC_debug_message_d_munakata_kai_DAT, s_pDebugHeap, gfl2::str::MsgData::LOAD_FULL);

  s_pRootDebugWinGroup = gfl2::debug::DebugWin_AddGroup(s_pDebugHeap, s_pMsgData, BattleFesGroupName, NULL);

  Savedata::BattleFesSave* pBattleFesSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetBattleFesSave();
  s_winCount = pBattleFesSave->GetDebugWinCount();
  s_oldWinCount = s_winCount;
  s_rank = pBattleFesSave->GetRank();
  s_resetCount = pBattleFesSave->GetTotalRandCount();
  s_openFlg = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork()->CheckEventFlag(FE_BATTLE_FES_OPEN);
  //s_fixedLevelFlg = 0;
  //s_fieldType = -1;

  s_pStrBuf = GFL_NEW(s_pDebugHeap) gfl2::str::StrBuf(gfl2::debug::DEBUGWIN_NAME_LEN, s_pDebugHeap);
  s_nowFieldType = 0;

  gfl2::debug::DebugWin_AddItemUpdateFunc(s_pRootDebugWinGroup, s_pDebugHeap, s_pMsgData, BattleFesWinCount, NULL, UpdateWinCount, DrawWinCount);
  gfl2::debug::DebugWin_AddItemUpdateFunc(s_pRootDebugWinGroup, s_pDebugHeap, s_pMsgData, BattleFesRank, NULL, UpdateRank, DrawRank);
  gfl2::debug::DebugWin_AddItemUpdateFunc(s_pRootDebugWinGroup, s_pDebugHeap, s_pMsgData, BattleFesResetCount, NULL, UpdateResetCount, DrawResetCount);
  gfl2::debug::DebugWin_AddItemUpdateFunc(s_pRootDebugWinGroup, s_pDebugHeap, s_pMsgData, BattleFesOpen, NULL, UpdateOpenFlg, DrawOpenFlg);
  gfl2::debug::DebugWin_AddItemUpdateFunc(s_pRootDebugWinGroup, s_pDebugHeap, s_pMsgData, BattleFesFixedLevel, NULL, UpdateFixedLevelFlg, DrawFixedLevelFlg);
  gfl2::debug::DebugWin_AddItemUpdateFunc(s_pRootDebugWinGroup, s_pDebugHeap, s_pMsgData, BattleFesField, NULL, UpdateField, DrawField);
}

void TerminateBattleFesDebug()
{
  if (s_pDebugHeap != NULL)
  {
    gfl2::debug::DebugWin_RemoveGroup(s_pRootDebugWinGroup);
    s_pDebugHeap = NULL;
  }

  GFL_SAFE_DELETE(s_pMsgData);
  GFL_SAFE_DELETE(s_pStrBuf);
}

void SetWinCount()
{
  Savedata::BattleFesSave* pBattleFesSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetBattleFesSave();
  s_winCount = pBattleFesSave->GetDebugWinCount();
  s_oldWinCount = s_winCount;
}

void SetRank()
{
  Savedata::BattleFesSave* pBattleFesSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetBattleFesSave();
  s_rank = pBattleFesSave->GetRank();
}

void SetTotalRandCount()
{
  Savedata::BattleFesSave* pBattleFesSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData()->GetBattleFesSave();
  s_resetCount = pBattleFesSave->GetTotalRandCount();
}

bool GetFixedLevelFlg()
{
  return (s_fixedLevelFlg != 0);
}

s32 GetFieldType()
{
  return s_fieldType;
}

GFL_NAMESPACE_END(BattleFesDebug)

#endif // PM_DEBUG
