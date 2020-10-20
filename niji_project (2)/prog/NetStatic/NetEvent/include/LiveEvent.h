#if !defined(LIVEEVENT_H_INCLUDED)
#define LIVEEVENT_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveEvent.h
 * @date   2015/07/09 Thu. 14:28:07
 * @author muto_yu
 * @brief  Live大会Event
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// ============================================================================
/**/
// ============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  "GameSys/include/GameManager.h"
#include  "PokeTool/include/PokeToolBattleParty.h"

#include  "NetStatic/NetEvent/include/NetEventBase.h"
#include  "NetStatic/NetAppLib/include/UI/NetAppRegulationDrawInfo.h"
#include  "NetStatic/NetAppLib/include/Util/NetAppBattleParty.h"
#include  "NetStatic/NetAppLib/include/Live/LiveNet.h"


#include  "NetStatic/NetEvent/include/TeamSelectEvent.h"          /*  teamselect  */
#include  "NetStatic/NetEvent/include/NetworkBattleEvent.h"       /*  netbattle   */
#include  "NetStatic/NetEvent/include/QRReaderEvent.h"            /*  qrreader    */


#include  "NetApp/Live/include/LiveAppParam.h"


/*
  forward declare
*/
namespace NetAppLib  {
namespace System {
  class ApplicationSystemBase;
}
}

namespace NetApp  {
namespace Live    {
  struct  APP_PARAM;
  class   LiveProc;
}
}

namespace NetEvent  {
namespace Live {
// ============================================================================
/**/
// ============================================================================


//==============================================================================
/**
  *  @class  LiveEvent
  *  @date   2015.05.12
  */
//==============================================================================
class LiveEvent  : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( LiveEvent );

public:
  //------------------------------------------------------------------
  /**
    *  @func     StartEvent
    *  @brief    LiveEventの開始
    */
  //------------------------------------------------------------------
  static void StartEvent(GameSys::GameManager* pGameManager);

  //------------------------------------------------------------------
  /**
    *  @func     LiveEvent
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  LiveEvent( gfl2::heap::HeapBase* pHeap );

  //------------------------------------------------------------------
  /**
    *  @func     ~LiveEvent
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~LiveEvent();


  /*
      GameEvent I/F
  */
  //------------------------------------------------------------------
  /**
    *  @func    BootChk
    *  @brief   起動判定
    */
  //------------------------------------------------------------------
  virtual bool  BootChk( GameSys::GameManager* pGameManager );

  //------------------------------------------------------------------
  /**
    *  @func    InitFunc
    *  @brief   初期化
    */
  //------------------------------------------------------------------
  virtual void  InitFunc( GameSys::GameManager* pGameManager );

  //------------------------------------------------------------------
  /**
    *  @func    EndFunc
    *  @brief   終了
    */
  //------------------------------------------------------------------
  virtual void  EndFunc( GameSys::GameManager* pGameManager );

  //------------------------------------------------------------------
  /**
    *  @func    MainFunc
    *  @brief   更新
    */
  //------------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  /*
      End GameEvent I/F
  */

private:
  void  ResultHandler(void);


  /*
      members
  */
//  LiveEventParam            m_EventParam;
  gfl2::heap::HeapBase*                         m_pHeapBase;            /**<  起動時に渡されるHeap          */
  gfl2::heap::HeapBase*                         m_pEventLocalHeapBase;  /**<  Event用LocalHeap              */
  NetApp::Live::LiveNet*                        m_pLiveNet;
  NetApp::Live::APP_PARAM                       m_ProcParam;            /**<  アプリケーションパラメータ    */
  NetApp::Live::LiveProc*                       m_pProc;
  NetAppLib::Util::NetAppBattleParty*           m_pMyBattleParty;
  

  /*
    others
  */
  /*  regulation*/
  Regulation*                                   m_pSavedRegulation;     /**<  coreDataアクセス用インスタンス（savedata）  */
  Regulation*                                   m_pReceivedRegulation;  /**<  Ir/QR経由で受信したRegulationData           */
  
  /*  teamselect  */
  NetAppLib::UI::RegulationDrawInfo*            m_pRegulationDrawInfo;  /**<  Regulation      */
  u8                                            m_RegulationCoredata[Regulation::REGULATION_MAX_DATA_SIZE_LIVE];    /*  ※ライブ大会専用サイズ  */
  NetApp::TeamSelect::EVENT_APP_PARAM           m_TeamSelectParam;      /**<  チーム選択      */
  /*  battle  */
  NetEvent::NetworkBattle::EVENT_PARAM          m_NetworkBattleParam;   /**<  networkbattle   */
  BATTLE_PGL_RECORD_SET                         m_PGLRecordSet;
  /*  battleReplay  */
  ExtSavedata::BattleRecorderSaveData*          m_pBattleRecorderData;
  BATTLE_SETUP_PARAM                            m_BattleSetupParam;
  /*  QRReader  */
  NetEvent::QRReader::QRReaderEventParam        m_QRReaderParam;
};  /*  class LiveEvent  */


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/12/09 Wed. 11:26:51
  */
//==============================================================================
#if PM_DEBUG
class LiveEvent_ForDebugMenu
  : public  LiveEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(LiveEvent_ForDebugMenu);
public:
  LiveEvent_ForDebugMenu( gfl2::heap::HeapBase* pHeap )
    : LiveEvent(pHeap)
  {}
  virtual ~LiveEvent_ForDebugMenu()  {}

  static void StartEvent(GameSys::GameManager* pGameManager)
  {
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, LiveEvent_ForDebugMenu>(pGameManager->GetGameEventManager());
  }
};
#endif


} /*  namespace Live  */
} /*  namespace NetApp       */



#endif  /*  #if !defined(LIVEEVENT_H_INCLUDED)  */
