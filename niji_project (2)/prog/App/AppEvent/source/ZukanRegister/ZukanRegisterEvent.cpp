//==============================================================================
/**
 @file    ZukanRegisterEvent.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.21
 @brief   図鑑登録の呼び出しEvent
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// pmlibのインクルード
#include <pml\include\pmlib.h>

// nijiのインクルード
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/DllProc.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProc.h>

// ポケモンリストのインクルード
#include <App/PokeList/include/PokeListProc.h>
// ポケモンステータスのインクルード
#include <App/Status/include/StatusProc.h>

// 図鑑登録の呼び出しEventのインクルード
#include <App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(event)


void ZukanRegisterEvent::StartEvent(GameSys::GameManager* gmgr, ZukanRegisterEventParam* param)
{
  GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
  ZukanRegisterEvent* event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, ZukanRegisterEvent>(pGameEventManager);
  event->setParam(param);
}


ZukanRegisterEvent::ZukanRegisterEvent(gfl2::heap::HeapBase* heap)
  : GameSys::GameEvent(heap),
    m_param(NULL),
    m_heap(heap),
    m_proc(NULL),
    m_zukanProcParam(),
    m_zukanProc(NULL),
    m_pokeListProcParam(),
    m_pokeListProc(NULL),
    m_pokeStatusProcParam(),
    m_pokeStatusProc(NULL),
    m_newPokePartyForPokeStatusProc(NULL)
{}
ZukanRegisterEvent::~ZukanRegisterEvent()
{}

bool ZukanRegisterEvent::BootChk(GameSys::GameManager* /*gmgr*/)
{
  return true;
}
void ZukanRegisterEvent::InitFunc(GameSys::GameManager* /*gmgr*/)
{}

GameSys::GMEVENT_RESULT ZukanRegisterEvent::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_CALL_ZUKAN_PROC,
    SEQ_CALL_ZUKAN_PROC_AFTER_POKE_LIST,
    SEQ_CALL_ZUKAN_PROC_AFTER_POKE_STATUS,
    SEQ_CALL_POKE_LIST_PROC,
    SEQ_CALL_POKE_STATUS_PROC,
    SEQ_WAIT_PROC_END,
    SEQ_END,
  };

  GameSys::GMEVENT_RESULT res = GameSys::GMEVENT_RES_CONTINUE;

  int seq = this->GetSeqNo();  // 親であるGameSys::GameEventのメンバ関数

  switch(seq)
  {
  case SEQ_CALL_ZUKAN_PROC:
    {
      m_zukanProcParam.caller                  = app::zukan::ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT;
      m_zukanProcParam.zukanRegisterEventParam = m_param;
      m_zukanProc = GameSys::CallZukanProc(&m_zukanProcParam);
      m_proc = m_zukanProc;
      seq = SEQ_WAIT_PROC_END;
      break;
    }
  case SEQ_CALL_ZUKAN_PROC_AFTER_POKE_LIST:
    {
      m_zukanProcParam.caller             = app::zukan::ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_LIST;
      m_zukanProc = GameSys::CallZukanProc(&m_zukanProcParam);
      m_proc = m_zukanProc;
      seq = SEQ_WAIT_PROC_END;
      break;
    }
  case SEQ_CALL_ZUKAN_PROC_AFTER_POKE_STATUS:
    {
      m_zukanProcParam.caller             = app::zukan::ZukanProcParam::CALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_STATUS;
      m_zukanProc = GameSys::CallZukanProc(&m_zukanProcParam);
      m_proc = m_zukanProc;
      seq = SEQ_WAIT_PROC_END;
      break;
    }
  case SEQ_CALL_POKE_LIST_PROC:
    {
      GameSys::GameData* gameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
      m_pokeListProcParam.mode = App::PokeList::MODE_SIMPLE;
      m_pokeListProcParam.my_party.SetParameter(gameData->GetPlayerParty()); 
      m_pokeListProcParam.my_status = gameData->GetPlayerStatus();
      m_pokeListProc = NIJI_PROC_CALL< GameSys::GameProcManager, App::PokeList::Proc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
      m_pokeListProc->SetAppParam(gmgr, &m_pokeListProcParam, NULL, NULL);
      m_proc = m_pokeListProc;
      seq = SEQ_WAIT_PROC_END;
      break;
    }
  case SEQ_CALL_POKE_STATUS_PROC:
    {
      GameSys::GameData* gameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
      
      m_pokeStatusProcParam.mode = App::Status::MODE_PARTY;
      if(m_zukanProcParam.returnVal == app::zukan::ZukanProcParam::RETURN_VAL_NEXT_POKE_STATUS_NEW)
      {
        m_newPokePartyForPokeStatusProc = GFL_NEW(m_heap) pml::PokeParty(m_heap);
        m_newPokePartyForPokeStatusProc->AddMember(*(m_param->pp));
        m_pokeStatusProcParam.party_data.SetParameter(m_newPokePartyForPokeStatusProc);
      }
      else  // m_zukanProcParam.returnVal == app::zukan::ZukanProcParam::RETURN_VAL_NEXT_POKE_STATUS_PARTY
      {
        m_pokeStatusProcParam.party_data.SetParameter(gameData->GetPlayerParty());
      }
      m_pokeStatusProcParam.update_prohibition = true;
      m_pokeStatusProcParam.poke_index = 0;

      m_pokeStatusProc = GameSys::CallStatusProc();
      m_pokeStatusProc->SetAppParam(&m_pokeStatusProcParam);
      m_proc = m_pokeStatusProc;
      seq = SEQ_WAIT_PROC_END;
      break;
    }
  case SEQ_WAIT_PROC_END:
    {
      // このイベントで扱うprocがなくなるのを待つ
      if( gmgr->GetProcManager()->IsRegisterProcess(m_proc) == false )
      {
        if(m_zukanProc)
        {
          m_zukanProc = NULL;
          if(m_zukanProcParam.returnType == app::zukan::ZukanProcParam::RETURN_TYPE_END)
          {
            seq = SEQ_END;
          }
          else  if(m_zukanProcParam.returnType == app::zukan::ZukanProcParam::RETURN_TYPE_NEXT_POKE_LIST)
          {
            seq = SEQ_CALL_POKE_LIST_PROC;
          }
          else  // if(m_zukanProcParam.returnType == app::zukan::ZukanProcParam::RETURN_TYPE_NEXT_POKE_STATUS)
          {
            seq = SEQ_CALL_POKE_STATUS_PROC;
          }
        }
        else if(m_pokeListProc)
        {
          m_pokeListProc = NULL;
          m_zukanProcParam.pokeListSelectPos = pml::PokeParty::MEMBER_INDEX_ERROR;
          if( m_pokeListProcParam.end_mode == App::PokeList::END_MODE_SELECT )
          {
            m_zukanProcParam.pokeListSelectPos = m_pokeListProcParam.poke_index;
          }
          seq = SEQ_CALL_ZUKAN_PROC_AFTER_POKE_LIST;
        }
        else  // if(m_pokeStatusProc)
        {
          m_pokeStatusProc = NULL;
          if(m_zukanProcParam.returnVal == app::zukan::ZukanProcParam::RETURN_VAL_NEXT_POKE_STATUS_NEW)
          {
            GFL_SAFE_DELETE(m_newPokePartyForPokeStatusProc);
          }
          seq = SEQ_CALL_ZUKAN_PROC_AFTER_POKE_STATUS;
        }
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

void ZukanRegisterEvent::EndFunc(GameSys::GameManager* /*gmgr*/)
{}

void ZukanRegisterEvent::setParam(ZukanRegisterEventParam* param)
{
  m_param = param;
}


GFL_NAMESPACE_END(event)
GFL_NAMESPACE_END(app)

