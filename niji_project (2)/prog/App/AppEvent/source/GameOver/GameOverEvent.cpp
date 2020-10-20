//==============================================================================
/**
 * @file        GameOverEvent.cpp
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/07/23(木) 15:22:15
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

#include <App/GameOver/include/GameOverProc.h>
#include "GameSys/include/GameData.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

// Eventのインクルード
#include <App/AppEvent/include/GameOver/GameOverEvent.h>

#include "niji_conv_header/field/warpdata/FieldWarpDataType.h"
#include "niji_conv_header/message/msg_gameover.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(event)


void GameOverAppEvent::StartEvent(GameSys::GameManager* gmgr)
{
  GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, GameOverAppEvent>(pGameEventManager);
  
}


GameOverAppEvent::GameOverAppEvent(gfl2::heap::HeapBase* heap)
  : GameSys::GameEvent(heap),
    m_proc(NULL)
{}

GameOverAppEvent::~GameOverAppEvent()
{}

bool GameOverAppEvent::BootChk(GameSys::GameManager* /*gmgr*/)
{
  return true;
}
void GameOverAppEvent::InitFunc(GameSys::GameManager* /*gmgr*/)
{}

GameSys::GMEVENT_RESULT GameOverAppEvent::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_CALL_GMOVER_PROC        = 0,
    SEQ_WAIT_GMOVER_PROC_START     ,
    SEQ_WAIT_GMOVER_PROC_END       ,
    SEQ_END                        ,
  };

  GameSys::GMEVENT_RESULT res = GameSys::GMEVENT_RES_CONTINUE;

  int seq = this->GetSeqNo();  // 親であるGameSys::GameEventのメンバ関数

  switch(seq)
  {
  case SEQ_CALL_GMOVER_PROC:
    {
/*  R:/home/momiji/exec/conv_header/niji_conv_header/message/msg_gameover.h
      m_param.m_iMsgId = msg_all_dead_01;  //"つかれて　うごけなくなった　ひんしのポケモンたちを　かばいながら[Name:TrainerName Idx=""1"" ]は　いそいでポケモンセンターに　もどるのであった"
      m_param.m_iMsgId = msg_all_dead_02;  //"つかれて　うごけなくなった　ひんしのポケモンたちを　かばいながら[Name:TrainerName Idx=""1"" ]は　いそいでじたくに　もどるのであった"
      m_param.m_iMsgId = msg_all_dead_03;  //"つかれて　うごけなくなった　ひんしのポケモンたちを　かばいながら[Name:TrainerName Idx=""1"" ]は　いそいでポケきゅうきゅうしゃに　もどるのであった"
*/
      int warp_id = gmgr->GetGameData()->GetWarpID()-1;
      Field::WarpDataTools *pWarpData = gmgr->GetGameData()->GetWarpDataTools();
      m_param.m_iMsgId = ( pWarpData ) ? pWarpData->GetRecoverType( warp_id ) : msg_all_dead_01;
      m_param.m_pStatus = gmgr->GetGameData()->GetPlayerStatus();
      
      // このイベントで扱うproc作成
      m_proc = GameSys::CallGameOverProc( &m_param );
      seq = SEQ_WAIT_GMOVER_PROC_START;
      break;
    }
  case SEQ_WAIT_GMOVER_PROC_START:
    {
      if( gmgr->IsProcessExists() != false )
      {
        // このイベントで扱うprocが動作中
        seq = SEQ_WAIT_GMOVER_PROC_END;
      }
      break;
    }
  case SEQ_WAIT_GMOVER_PROC_END:
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

void GameOverAppEvent::EndFunc(GameSys::GameManager* /*gmgr*/)
{}


GFL_NAMESPACE_END(event)
GFL_NAMESPACE_END(app)

