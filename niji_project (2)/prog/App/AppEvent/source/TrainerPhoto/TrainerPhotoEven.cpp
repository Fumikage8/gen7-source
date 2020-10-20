//==============================================================================
/**
 @file    TrainerPhotoEvent.cpp
 @author  Yuto Uchida
 @date    2015.09.01
 */
//==============================================================================
#include "App/AppEvent/include/TrainerPhoto/TrainerPhotoEvent.h"

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
#include "App/TrainerPhoto/include/TrainerPhotoProc.h"
#include "GameSys/include/DllProc.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(event)

  void TrainerPhotoEvent::StartEvent(GameSys::GameManager* gmgr)
  {
    GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, TrainerPhotoEvent>(pGameEventManager);
  }

  TrainerPhotoEvent::TrainerPhotoEvent(gfl2::heap::HeapBase* heap)
    : GameSys::GameEvent( heap )
    , m_proc(NULL)
  {}

  TrainerPhotoEvent::~TrainerPhotoEvent()
  {}

  bool TrainerPhotoEvent::BootChk(GameSys::GameManager* /*gmgr*/)
  {
    return true;
  }

  void TrainerPhotoEvent::InitFunc(GameSys::GameManager* /*gmgr*/)
  {}

  GameSys::GMEVENT_RESULT TrainerPhotoEvent::MainFunc(GameSys::GameManager* gmgr)
  {
    GameSys::GMEVENT_RESULT res = GameSys::GMEVENT_RES_CONTINUE;

    GameSys::GameProcManager* pGameProcManager = gmgr->GetProcManager();
  
    enum
    {
      SEQ_START_TRAINER_PHOTO_PROC       = 0,
      SEQ_WAIT_TRAINER_PHOTO_PROC,
      SEQ_NONE
    };

    int seq = GetSeqNo();

    if( seq == SEQ_START_TRAINER_PHOTO_PROC )
    {
      // このイベントで扱うproc作成
      m_proc = GameSys::CallTrainerPhotoProc();

      m_proc->Initialize( &m_appParam );
      ++seq;
    }

    if( seq == SEQ_WAIT_TRAINER_PHOTO_PROC )
    {
      if( pGameProcManager->IsRegisterProcess( m_proc ) == false )
      {
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
  void TrainerPhotoEvent::EndFunc(GameSys::GameManager* /*gmgr*/)
  {}

GFL_NAMESPACE_END(event)
GFL_NAMESPACE_END(app)
