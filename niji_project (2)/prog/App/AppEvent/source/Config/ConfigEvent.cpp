//==============================================================================
/**
 @file    ConfigEvent.cpp
 @author  Yuto Uchida
 @date    2015.06.11
 */
//==============================================================================
#include "App/AppEvent/include/Config/ConfigEvent.h"

// gfl2のインクルード
#include <fs/include/gfl2_Fs.h>

// nijiのインクルード
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/DllProc.h>

#include <debug/Launcher/include/LauncherProc.h>
#include <debug/StartMenu/include/StartLauncher.h>

// コンフィグのインクルード
#include "App/Config/include/ConfigProc.h"
#include "GameSys/include/DllProc.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(event)

  void ConfigEvent::StartEvent(GameSys::GameManager* gmgr)
  {
    GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, ConfigEvent>(pGameEventManager);
  }


  ConfigEvent::ConfigEvent(gfl2::heap::HeapBase* heap)
    : GameSys::GameEvent( heap )
    , m_proc(NULL)
  {}

  ConfigEvent::~ConfigEvent()
  {}

  bool ConfigEvent::BootChk(GameSys::GameManager* /*gmgr*/)
  {
    return true;
  }

  void ConfigEvent::InitFunc(GameSys::GameManager* /*gmgr*/)
  {}

  GameSys::GMEVENT_RESULT ConfigEvent::MainFunc(GameSys::GameManager* gmgr)
  {
    GameSys::GMEVENT_RESULT res = GameSys::GMEVENT_RES_CONTINUE;

    GameSys::GameProcManager* pGameProcManager = gmgr->GetProcManager();
  
    enum
    {
      SEQ_START_CONFIG_PROC       = 0,
      SEQ_WAIT_CONFIG_PROC,
      SEQ_NONE
    };

    int seq = GetSeqNo();

    if( seq == SEQ_START_CONFIG_PROC )
    {
      // このイベントで扱うproc作成
      m_proc = GameSys::CallConfigProc();
      ++seq;
    }

    if( seq == SEQ_WAIT_CONFIG_PROC )
    {
      if( pGameProcManager->IsRegisterProcess( m_proc ) == false )
      {
        // このイベントで扱うprocが動作終了
        ++seq;
      }
    }

    if( seq == SEQ_NONE )
    {
      res = GameSys::GMEVENT_RES_FINISH;
    }

    GameEvent::SetSeqNo( seq );

    return res;
  }
  void ConfigEvent::EndFunc(GameSys::GameManager* /*gmgr*/)
  {}

GFL_NAMESPACE_END(event)
GFL_NAMESPACE_END(app)
