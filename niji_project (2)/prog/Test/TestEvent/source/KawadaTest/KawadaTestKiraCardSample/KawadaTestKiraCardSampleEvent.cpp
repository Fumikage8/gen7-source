//==============================================================================
/**
 @file    KawadaTestKiraCardSampleEvent.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.12.23
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


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

// KawadaTestのインクルード
#include <Test/TestEvent/include/KawadaTest/KawadaTestKiraCardSample/KawadaTestKiraCardSampleEvent.h>
#include <Test/KawadaTest/include/KawadaTestProc.h>


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


void KawadaTestKiraCardSampleEvent::StartEvent(GameSys::GameManager* gmgr)
{
  GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, KawadaTestKiraCardSampleEvent>(pGameEventManager);
}


KawadaTestKiraCardSampleEvent::KawadaTestKiraCardSampleEvent(gfl2::heap::HeapBase* heap)
  : GameSys::GameEvent(heap),
    m_proc(NULL)
{}
KawadaTestKiraCardSampleEvent::~KawadaTestKiraCardSampleEvent()
{}

bool KawadaTestKiraCardSampleEvent::BootChk(GameSys::GameManager* /*gmgr*/)
{
  return true;
}
void KawadaTestKiraCardSampleEvent::InitFunc(GameSys::GameManager* /*gmgr*/)
{}
GameSys::GMEVENT_RESULT KawadaTestKiraCardSampleEvent::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_WAIT_OTHER_PROC_END,
    SEQ_CALL_KIRA_CARD_SAMPLE_PROC,
    SEQ_WAIT_KIRA_CARD_SAMPLE_PROC_START,
    SEQ_WAIT_KIRA_CARD_SAMPLE_PROC_END,
    SEQ_CALL_LAUNCHER,
    SEQ_END,
  };

  GameSys::GMEVENT_RESULT res = GameSys::GMEVENT_RES_CONTINUE;

  int seq = this->GetSeqNo();  // 親であるGameSys::GameEventのメンバ関数

  GameSys::GameProcManager* gameProcManager = gmgr->GetProcManager();
  
  switch(seq)
  {
  case SEQ_WAIT_OTHER_PROC_END:
    {
      // 全てのprocがなくなるのを待つ
      if( gmgr->IsProcessExists() == false )
      {
        seq = SEQ_CALL_KIRA_CARD_SAMPLE_PROC;
      }
      break;
    }
  case SEQ_CALL_KIRA_CARD_SAMPLE_PROC:
    {
      // このイベントで扱うproc作成
      KawadaTestProcParam param;
      param.sampleId = KawadaTestProcParam::SAMPLE_ID_KIRA_CARD_MAIN;
      m_proc = GameSys::CallKawadaTestProc(&param);
      seq = SEQ_WAIT_KIRA_CARD_SAMPLE_PROC_START;
      break;
    }
  case SEQ_WAIT_KIRA_CARD_SAMPLE_PROC_START:
    {
      if( gmgr->IsProcessExists() != false )
      {
        // このイベントで扱うprocが動作中
        seq = SEQ_WAIT_KIRA_CARD_SAMPLE_PROC_END;
      }
      break;
    }
  case SEQ_WAIT_KIRA_CARD_SAMPLE_PROC_END:
    {
      // 全てのprocがなくなるのを待つ
      if( gmgr->IsProcessExists() == false )
      {
        seq = SEQ_CALL_LAUNCHER;
      }
      break;
    }
  case SEQ_CALL_LAUNCHER:
    {
      // このイベントを終了する前に、次のprocを作成しておく
      debug::launcher::LauncherProc::CallParam launcherCallParam;
      debug::launcher::LauncherProc::InitCallParam( &launcherCallParam );
      debug::launcher::LauncherProc* launcher = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( gameProcManager, &launcherCallParam );  // @todo ランチャーがDLLになったら呼び方を変えること。
      launcher->CreateStartListData<debug::startmenu::StartLauncher>();
      seq = SEQ_END;
      res = GameSys::GMEVENT_RES_FINISH;
      break;
    }
  case SEQ_END:
    {
      res = GameSys::GMEVENT_RES_FINISH;
      break;
    }
  }

  this->SetSeqNo(seq);  // 親であるGameSys::GameEventのメンバ関数

  return res;
}
void KawadaTestKiraCardSampleEvent::EndFunc(GameSys::GameManager* /*gmgr*/)
{}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

