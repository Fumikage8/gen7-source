//==============================================================================
/**
 * @file   QRReaderEvent.cpp
 * @date   2016/01/14 Thu. 19:53:41
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

#include  "System/include/HeapDefine.h"
#include  "GameSys/include/GameEventManager.h"
#include  "GameSys/include/GameProcManager.h"
#include  "GameSys/include/DllProc.h"
#include  "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"

/*  zukanRegister  */
//#include  "Savedata/include/ZukanRegisterChecker.h"


/*   QRReader */
#include  "NetApp/QRReader/include/QRReaderProcParam.h"
#include  "NetApp/QRReader/include/QRReaderProc.h"
#include  "NetApp/QRReader/source/System/QRReaderBuilder.h"
#include  "NetApp/QRReader/source/QRReaderFrameDef.h"

#include  "../include/QRReaderEvent.h"




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
    SEQ_ProcRunning_QRReader,

    SEQ_Exit,

    SEQ_MAX,
  };


  static NetEvent::QRReader::QRReaderEventParam  eventParamForField;
}


namespace NetEvent  {
namespace QRReader {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
void QRReaderEvent::StartEventFromField(GameSys::GameManager* pGameManager)
{
  StartEvent(pGameManager, eventParamForField);
}


//------------------------------------------------------------------
/**
  *  @func     StartEvent
  *  @brief    QRReaderEventの開始
  */
//------------------------------------------------------------------
void QRReaderEvent::StartEvent(GameSys::GameManager* pGameManager, QRReaderEventParam& rEventParam)
{
  QRReaderEvent*   pEvent  = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, QRReaderEvent>( pGameManager->GetGameEventManager() );

  pEvent->m_pEventParam  = &rEventParam;
}


//------------------------------------------------------------------
/**
  *  @func     QRReaderEvent
  *  @brief    ctor
  */
//------------------------------------------------------------------
QRReaderEvent::QRReaderEvent( gfl2::heap::HeapBase* pHeap )
  : GameSys::GameEvent(pHeap)
  , m_pEventParam(NULL)
  , m_ProcParam()
  , m_pProc(NULL)
{
}



//------------------------------------------------------------------
/**
  *  @func     ~QRReaderEvent
  *  @brief    dtor
  */
//------------------------------------------------------------------
QRReaderEvent::~QRReaderEvent()
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
bool  QRReaderEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @func    InitFunc
  *  @brief   初期化
  */
//------------------------------------------------------------------
void  QRReaderEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  /*  setup appparam  */
  {
    switch(m_pEventParam->mode)
    {
      /**  ライブ大会          */
      case  QRReaderEventParam::MODE_Live:
        m_pEventParam->result       = RESULT_Live_NotDecoded;
        m_ProcParam.m_BootMode      = NetApp::QRReader::APP_PARAM::BOOTMODE_LiveMatch;
        m_ProcParam.pDst_Regulation = m_pEventParam->pDst_Regulation;
        break;

      /**  なかま大会          */
      case  QRReaderEventParam::MODE_FriendMatch:
        m_pEventParam->result       = RESULT_FriendMatch_NotDecoded;
        m_ProcParam.m_BootMode      = NetApp::QRReader::APP_PARAM::BOOTMODE_FriendMatch;
        m_ProcParam.pDst_Regulation = m_pEventParam->pDst_Regulation;
        break;

      /**  チーム選択          */
      case  QRReaderEventParam::MODE_Team:
        m_pEventParam->result       = RESULT_Team_NotDecoded;
        m_ProcParam.m_BootMode      = NetApp::QRReader::APP_PARAM::BOOTMODE_BattleTeam;
        m_ProcParam.pDst_PokeParty  = m_pEventParam->pDst_PokeParty;
        break;

      case  QRReaderEventParam::MODE_QRScan:        /**<  fieldからのQRScan   */
      default:
        m_ProcParam.m_BootMode  = NetApp::QRReader::APP_PARAM::BOOTMODE_QRScan;
        break;
    }
  }

  /*  BGM  */
//  Sound::StartBGMReq(STRM_BGM_SILENCE);

  /*  bootProc  */
  m_pProc = GameSys::CallQRReaderProc( &m_ProcParam );
  SetSeqNo(SEQ_ProcRunning_QRReader);

  /*  clearsetting  */
  PushClearColor();
}


//------------------------------------------------------------------
/**
  *  @func    EndFunc
  *  @brief   終了
  */
//------------------------------------------------------------------
void  QRReaderEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  PopClearColor();
}


//------------------------------------------------------------------
/**
  *  @func    MainFunc
  *  @brief   更新
  */
//------------------------------------------------------------------
GameSys::GMEVENT_RESULT QRReaderEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result  = GameSys::GMEVENT_RES_CONTINUE;

  switch(GetSeqNo())
  {
    case  SEQ_ProcRunning_QRReader:
      if( !pGameManager->GetProcManager()->IsRegisterProcess(m_pProc) )
      {
        m_pProc = NULL;
        ResultHandler();
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


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  QRReaderEvent::ResultHandler(void)
{
  switch(m_pEventParam->mode)
  {
    /**  ライブ大会          */
    case  QRReaderEventParam::MODE_Live:
      if(m_ProcParam.m_QRSimpleReadDecoded)
      {
        m_pEventParam->result = RESULT_Live_Decoded;
      }
      break;

    /**  なかま大会          */
    case  QRReaderEventParam::MODE_FriendMatch:
      if(m_ProcParam.m_QRSimpleReadDecoded)
      {
        m_pEventParam->result = RESULT_FriendMatch_Decoded;
      }
      break;

    /**  チーム選択          */
    case  QRReaderEventParam::MODE_Team:
      if(m_ProcParam.m_QRSimpleReadDecoded)
      {
        m_pEventParam->result = RESULT_Team_Decoded;
      }
      break;

    case  QRReaderEventParam::MODE_QRScan:        /**<  fieldからのQRScan   */
    default:
      break;
  }
}



} /*  namespace Evolution    */
} /*  namespace NetApp  */
