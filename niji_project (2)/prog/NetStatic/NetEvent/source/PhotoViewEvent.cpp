//==============================================================================
/**
 * @file   PhotoViewEvent.cpp
 * @date   2017/02/07 Tue. 18:24:39
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
#include  <NetApp/PhotoView/include/PhotoViewProc.h>

#include  "../include/PhotoViewEvent.h"


// =============================================================================
/**/
//==============================================================================
/*  local  */
namespace {
  /**
    @brief  seqNo
  */
  enum  Sequence
  {
    SEQ_ProcRunning_PhotoView,

    SEQ_Exit,

    SEQ_MAX,
  };
}


namespace NetEvent  {
namespace PhotoView {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     StartEvent
  *  @brief    PhotoViewEventの開始
  */
//------------------------------------------------------------------
void PhotoViewEvent::StartEvent(GameSys::GameManager* pGameManager, Param& rParam)
{
  PhotoViewEvent*   pEvent  = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, PhotoViewEvent>( pGameManager->GetGameEventManager() );

  pEvent->m_EventParam = rParam;
}


//------------------------------------------------------------------
/**
  *  @func     PhotoViewEvent
  *  @brief    ctor
  */
//------------------------------------------------------------------
PhotoViewEvent::PhotoViewEvent( gfl2::heap::HeapBase* pHeap )
  : GameSys::GameEvent(pHeap)
  , m_EventParam()
  /*  work  */
  , m_pProc(NULL)
{
}



//------------------------------------------------------------------
/**
  *  @func     ~PhotoViewEvent
  *  @brief    dtor
  */
//------------------------------------------------------------------
PhotoViewEvent::~PhotoViewEvent()
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
bool  PhotoViewEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @func    InitFunc
  *  @brief   初期化
  */
//------------------------------------------------------------------
void  PhotoViewEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  m_pProc = GameSys::CallPhotoViewProc(&(m_EventParam.appParam));

  SetSeqNo(SEQ_ProcRunning_PhotoView);
}


//------------------------------------------------------------------
/**
  *  @func    EndFunc
  *  @brief   終了
  */
//------------------------------------------------------------------
void  PhotoViewEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


//------------------------------------------------------------------
/**
  *  @func    MainFunc
  *  @brief   更新
  */
//------------------------------------------------------------------
GameSys::GMEVENT_RESULT PhotoViewEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result  = GameSys::GMEVENT_RES_CONTINUE;

  switch(GetSeqNo())
  {
    case  SEQ_ProcRunning_PhotoView:
      if( !pGameManager->GetProcManager()->IsRegisterProcess(m_pProc) )
      {
        /*  proc終了  */
        m_pProc = NULL;


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
