//==============================================================================
/**
 * @file   JoinFestaPokeStatusEvent.cpp
 * @date   2016/11/24 Thu. 16:57:03
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <Sound/include/Sound.h>

#include  <System/include/HeapDefine.h>
#include  <GameSys/include/GameEventManager.h>
#include  <GameSys/include/GameProcManager.h>
#include  <GameSys/include/GameData.h>
#include  <GameSys/include/DllProc.h>
#include  <NetStatic/NetAppLib/include/System/ApplicationSystemBase.h>

/*   Status */
#include  <App/Status/include/StatusProc.h>

#include  "../include/JoinFestaPokeStatusEvent.h"




// =============================================================================
/**/
//==============================================================================
/*  local  */
namespace {
  static const u32  EventLocalHeapSize  = 0x00080000;

  /**
    @brief  seqNo
  */
  enum  Sequence
  {
    SEQ_ProcRunning_PokeStatus,

    SEQ_Exit,

    SEQ_MAX,
  };
}


namespace NetEvent  {
namespace JoinFestaPokeStatus {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     StartEvent
  *  @brief    JoinFestaPokeStatusEventの開始
  */
//------------------------------------------------------------------
void JoinFestaPokeStatusEvent::StartEvent(GameSys::GameManager* pGameManager, Param* pParam)
{
  JoinFestaPokeStatusEvent*   pEvent  = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaPokeStatusEvent>( pGameManager->GetGameEventManager() );

  GFL_ASSERT(pParam->partnerInfos[0].pTarget);

  pEvent->m_pEventParam = pParam;
}


//------------------------------------------------------------------
/**
  *  @func     JoinFestaPokeStatusEvent
  *  @brief    ctor
  */
//------------------------------------------------------------------
JoinFestaPokeStatusEvent::JoinFestaPokeStatusEvent( gfl2::heap::HeapBase* pHeap )
  : GameSys::GameEvent(pHeap)
  , m_pEventParam(NULL)
  , m_pEventHeapBase(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ))
  , m_pEventLocalHeapBase(NULL)
  /*  work  */
  , m_pTemporaryParty(NULL)
  , m_ProcParam()
  , m_pProc(NULL)
{
}



//------------------------------------------------------------------
/**
  *  @func     ~JoinFestaPokeStatusEvent
  *  @brief    dtor
  */
//------------------------------------------------------------------
JoinFestaPokeStatusEvent::~JoinFestaPokeStatusEvent()
{
}


/*
    GameEvent I/F
*/

//------------------------------------------------------------------
/**
  *  @func    BootChk
  *  @brief   起動判定
  */
//------------------------------------------------------------------
bool  JoinFestaPokeStatusEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @func    InitFunc
  *  @brief   初期化
  */
//------------------------------------------------------------------
void  JoinFestaPokeStatusEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  gfl2::heap::HeapBase*   pHeapBase = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);


  /*  event local heap  */
  {
    m_pEventLocalHeapBase = GFL_CREATE_LOCAL_HEAP( m_pEventHeapBase, EventLocalHeapSize, gfl2::heap::HEAP_TYPE_EXP, false );
  }

  /*  party  */
  {
    m_pTemporaryParty = GFL_NEW(m_pEventLocalHeapBase) pml::PokeParty(m_pEventLocalHeapBase);

    for(u32 index = 0; index < GFL_NELEMS(m_pEventParam->partnerInfos); ++index)
    {
      pml::pokepara::PokemonParam*  pCoreParam = m_pEventParam->partnerInfos[index].pTarget;

      if(pCoreParam)
      {
        m_pTemporaryParty->AddMember(*pCoreParam);
      }
    }
  }


  /*  bootProc  */
  {
    m_ProcParam.mode        = App::Status::MODE_BFC;
    m_ProcParam.poke_index  = m_pEventParam->focusPokemonIndex;

    for(u32 index = 0; index < GFL_NELEMS(m_ProcParam.battle_fes_trainer_info_ptr); ++index)
    {
      m_ProcParam.battle_fes_trainer_info_ptr[index] = m_pEventParam->partnerInfos[index].pTrainerInfo;
    }

    m_ProcParam.party_data.SetParameter(m_pTemporaryParty);
  }

  m_pProc = GameSys::CallStatusProcEx(m_pEventParam->bLoadRadarChartCRO);
  m_pProc->SetAppParam(&m_ProcParam);
  
  SetSeqNo(SEQ_ProcRunning_PokeStatus);
}


//------------------------------------------------------------------
/**
  *  @func    EndFunc
  *  @brief   終了
  */
//------------------------------------------------------------------
void  JoinFestaPokeStatusEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  /*  event local heap  */
  {
    /*  party  */
    GFL_SAFE_DELETE(m_pTemporaryParty);

    gfl2::heap::Manager::DeleteHeap(m_pEventLocalHeapBase);
    m_pEventLocalHeapBase = NULL;
  }
}


//------------------------------------------------------------------
/**
  *  @func    MainFunc
  *  @brief   更新
  */
//------------------------------------------------------------------
GameSys::GMEVENT_RESULT JoinFestaPokeStatusEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result  = GameSys::GMEVENT_RES_CONTINUE;

  switch(GetSeqNo())
  {
    case  SEQ_ProcRunning_PokeStatus:
      if( !pGameManager->GetProcManager()->IsRegisterProcess(m_pProc) )
      {
        /*  proc終了  */
        m_pProc = NULL;
        /* focusPokemonIndex渡す */
        m_pEventParam->focusPokemonIndex = m_ProcParam.poke_index;
        SetSeqNo(SEQ_Exit);
      }
      break;
      
    case  SEQ_Exit:   /*  fallthrough  */
    default:
      result  = GameSys::GMEVENT_RES_FINISH;
      break;
  }

  return  result;
}



} /*  namespace Evolution    */
} /*  namespace NetApp  */
