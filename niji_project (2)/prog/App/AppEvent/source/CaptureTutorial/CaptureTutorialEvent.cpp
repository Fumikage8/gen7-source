//==============================================================================
/**
 @file    CaptureTutorialEvent.cpp
 @author  Yuto Uchida
 @date    2015.06.24
 */
//==============================================================================
#include "App/AppEvent/include/CaptureTutorial/CaptureTutorialEvent.h"

// gfl2のインクルード
#include <fs/include/gfl2_Fs.h>

// nijiのインクルード
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/DllProc.h>
#include <Sound/include/Sound.h>

#include <debug/Launcher/include/LauncherProc.h>
#include <debug/StartMenu/include/StartLauncher.h>

// コンフィグのインクルード
#include "App/CaptureTutorial/include/CaptureTutorialProc.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(event)

  void CaptureTutorialEvent::StartEvent(GameSys::GameManager* gmgr, b32 isCapture)
  {
    GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
    CaptureTutorialEvent * pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, CaptureTutorialEvent>(pGameEventManager);
    pEvent->SetCapture( isCapture );
  }

  CaptureTutorialEvent::CaptureTutorialEvent(gfl2::heap::HeapBase* heap)
    : GameSys::GameEvent(heap),
      m_pProc(NULL),
      mIsCapture(true)
  {}

  CaptureTutorialEvent::~CaptureTutorialEvent()
  {}

  bool CaptureTutorialEvent::BootChk(GameSys::GameManager* /*gmgr*/)
  {
    return true;
  }

  void CaptureTutorialEvent::InitFunc(GameSys::GameManager* /*gmgr*/)
  {
    //  野生戦闘用のをBGM再生
    Sound::StartBattleBGMReq( STRM_BGM_MJ_M02 );
  }

  GameSys::GMEVENT_RESULT CaptureTutorialEvent::MainFunc(GameSys::GameManager* gmgr)
  {
    GameSys::GMEVENT_RESULT res = GameSys::GMEVENT_RES_CONTINUE;

    GameSys::GameProcManager* pGameProcManager = gmgr->GetProcManager();
  
    enum
    {
      SEQ_START_PROC       = 0,
      SEQ_WAIT_PROC,
      SEQ_NONE
    };

    int seq = GetSeqNo();

    if( seq == SEQ_START_PROC )
    {
      // このイベントで扱うproc作成
      m_pProc = GameSys::CallCaptureTutorialProc(mIsCapture);
      ++seq;
    }

    if( seq == SEQ_WAIT_PROC )
    {
      if( pGameProcManager->IsRegisterProcess( m_pProc ) == false )
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
  void CaptureTutorialEvent::EndFunc(GameSys::GameManager* /*gmgr*/)
  {
    //  BGMを復帰
    Sound::EndBattleBGMReq();
  }

  void CaptureTutorialEvent::SetCapture( b32 isCapture )
  {
    mIsCapture = isCapture;
  }

GFL_NAMESPACE_END(event)
GFL_NAMESPACE_END(app)
