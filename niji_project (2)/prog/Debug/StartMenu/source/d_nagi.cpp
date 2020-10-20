#if PM_DEBUG
//======================================================================
/**
 * @file d_nagi.cpp
 * @date 2016/02/26 18:20:12
 * @author nagihashi_toru
 * @brief なぎはしスタートメニュー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <Debug/StartMenu/include/d_nagi.h>
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_nagi.h>
#include <Debug/StartMenu/include/StartLauncher.h>
#include <GameSys/include/GameProcManager.h>
#include "GameSys/include/GameData.h"
#include <GameSys/include/DllProc.h>
#include <Sound/include/Sound.h>

#include <System/include/HeapDefine.h>
#include <GameSys/include/GameManager.h>
#include <App/DendouDemo/include/DendouDemoProc.h>
#include <pml/include/pml_PokeParty.h>
#include <App/AppEvent/include/Congrats/CongratsEvent.h>

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

namespace
{
  static app::dendou_demo::APP_PARAM dendouDemoAppParam;

  static const u32 ItemList[] = {
    D_NAGI_00,
    D_NAGI_01,
    D_NAGI_02,
    D_NAGI_03,
  };

  static const u32 NumItemList = GFL_NELEMS(ItemList);
}

NagiTestLauncher::NagiTestLauncher()
{
  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
  GameSys::GameData *gameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  dendouDemoAppParam.pPokeParty = gameData->GetPlayerParty();
  dendouDemoAppParam.pMyStatus = gameData->GetPlayerStatus();
  dendouDemoAppParam.isFirstClear = gameData->GetEventWork()->CheckEventFlag( SYS_FLAG_GAME_CLEAR );
  dendouDemoAppParam.defensedCount = gameData->GetRecordSaveData()->Get(Savedata::Record::RECID_DENDOU_CNT);

  Sound::StartBGMReq(STRM_BGM_VS_TRAINER_NJ);
}

NagiTestLauncher::~NagiTestLauncher()
{
  Sound::EndBGMReq();
}

gfl2::fs::ArcFile::ARCDATID NagiTestLauncher::GetArcDataID()
{
  return GARC_debug_message_d_nagi_DAT;
}

const u32* NagiTestLauncher::GetListTable()
{
  return ItemList;
}

u32 NagiTestLauncher::GetListTableSize()
{
  return NumItemList;
}

launcher::ListData::RequestID NagiTestLauncher::SelectListener( u32 pos, launcher::LauncherProc* launcher)
{
  // キャンセル
  if( pos == debug::launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<debug::startmenu::StartLauncher>();
    return debug::launcher::ListData::REQID_RESET;
  }

  switch (ItemList[pos])
  {
  case D_NAGI_00://B初回
    dendouDemoAppParam.isFirstClear = true;
    GameSys::CallDendouDemoProc( &dendouDemoAppParam );
    return debug::launcher::ListData::REQID_CALL;

  case D_NAGI_01://
    dendouDemoAppParam.isFirstClear = false;
    GameSys::CallDendouDemoProc( &dendouDemoAppParam );
    return debug::launcher::ListData::REQID_NONE;

  case D_NAGI_02://AB初回
    App::Event::CongratsEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager),false);
    return debug::launcher::ListData::REQID_NONE;

  case D_NAGI_03://AB防衛
    App::Event::CongratsEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager),true);
    return debug::launcher::ListData::REQID_NONE;

  default: 
    GFL_ASSERT(0);
    launcher->CreateListData<debug::startmenu::StartLauncher>();
    return debug::launcher::ListData::REQID_RESET;
  }

  return debug::launcher::ListData::REQID_NONE;
}

GFL_NAMESPACE_END(starmenu)
GFL_NAMESPACE_END(debug)
#endif //PM_DEBUG
