//==============================================================================
/**
 @file    ZukanEvent.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   図鑑の呼び出しEvent
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/DllProc.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProc.h>

// 図鑑の呼び出しEventのインクルード
#include <App/AppEvent/include/Zukan/ZukanEvent.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(event)


void ZukanEvent::StartEvent(GameSys::GameManager* gmgr)
{
  GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, ZukanEvent>(pGameEventManager);
}


ZukanEvent::ZukanEvent(gfl2::heap::HeapBase* heap)
  : GameSys::GameEvent(heap),
    m_procParam(),
    m_proc(NULL)
{}
ZukanEvent::~ZukanEvent()
{}

bool ZukanEvent::BootChk(GameSys::GameManager* /*gmgr*/)
{
  return true;
}
void ZukanEvent::InitFunc(GameSys::GameManager* /*gmgr*/)
{}

GameSys::GMEVENT_RESULT ZukanEvent::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_CALL_ZUKAN_PROC        = 0,
    SEQ_WAIT_ZUKAN_PROC_START      ,
    SEQ_WAIT_ZUKAN_PROC_END        ,
    SEQ_END                        ,
  };

  GameSys::GMEVENT_RESULT res = GameSys::GMEVENT_RES_CONTINUE;

  int seq = this->GetSeqNo();  // 親であるGameSys::GameEventのメンバ関数

  switch(seq)
  {
  case SEQ_CALL_ZUKAN_PROC:
    {
      // このイベントで扱うproc作成
      m_proc = GameSys::CallZukanProc(&m_procParam);
      seq = SEQ_WAIT_ZUKAN_PROC_START;
      break;
    }
  case SEQ_WAIT_ZUKAN_PROC_START:
    {
      if( gmgr->IsProcessExists() != false )
      {
        // このイベントで扱うprocが動作中
        seq = SEQ_WAIT_ZUKAN_PROC_END;
      }
      break;
    }
  case SEQ_WAIT_ZUKAN_PROC_END:
    {
      // このイベントで扱うprocがなくなるのを待つ
      if( gmgr->GetProcManager()->IsRegisterProcess(m_proc) == false )
      {
        seq = SEQ_END;
      }
      break;
    }
  case SEQ_END:
  default:
    {
      res = GameSys::GMEVENT_RES_FINISH;
      break;
    }
  }
  
  this->SetSeqNo(seq);  // 親であるGameSys::GameEventのメンバ関数

  return res;
}

void ZukanEvent::EndFunc(GameSys::GameManager* /*gmgr*/)
{}


GFL_NAMESPACE_END(event)
GFL_NAMESPACE_END(app)

