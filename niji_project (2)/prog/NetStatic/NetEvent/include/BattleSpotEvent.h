#if !defined(BATTLESPOTEVENT_H_INCLUDED)
#define BATTLESPOTEVENT_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotEvent.h
 * @date   2015/10/02 Fri. 19:46:22
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  "GameSys/include/GameManager.h"
#include  "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase
#include  "NetApp/BattleSpot/include/BattleSpotProcParam.h"

#include "NetStatic/NetAppLib/include/UI/NetAppRegulationDrawInfo.h"
// バトルチーム選択アプリ
#include "NetApp/TeamSelect/include/TeamSelectEventAppParam.h"
#include "NetStatic/NetEvent/include/TeamSelectEvent.h"
// 通信バトルイベント
#include "NetStatic/NetEvent/include/NetworkBattleEvent.h"
#include "NetStatic/NetEvent/include/QRReaderEvent.h"

// バトルビデオ録画アプリのパラメータ
#include "NetApp/BattleVideoRecording/include/BattleVideoRecordingEventAppParam.h"

/*
  forward declare
*/
namespace NetAppLib  {
namespace System {
  class ApplicationSystemBase;
}
}
namespace NetApp      {
namespace BattleSpot  {
  class BattleSpotProc;
}
}

namespace NetEvent    {
namespace BattleSpot  {
// =============================================================================
/**/
//==============================================================================
/**
  Event起動パラメータ
*/
typedef struct  BattleSpotEventParam
{
  u32 dummy;


  /*    */
  BattleSpotEventParam(void)
    : dummy(0)
  {}
}BattleSpotEventParam;



//==============================================================================
/**
  *  @class  BattleSpotEvent
  */
//==============================================================================
class BattleSpotEvent
  : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleSpotEvent );

private:

  enum
  {
    RECORD_SINGLE = 0,
    RECORD_DOUBLE = 1,
    RECORD_ROYAL = 2,
    RECORD_FREE = 0,
    RECORD_RATE = 1,
    RECORD_CUP = 2,
    RECORD_FRIEND = 3
  };

public:
  /*  イベント起動  */
  static void StartEvent(GameSys::GameManager* pGameManager, BattleSpotEventParam& rEventParam);

  static bool IsEnterBattleSpot( void );

  static void SetSessionLeftFlag( bool b );
  
  static bool IsSessionLeftFlag( void );

  BattleSpotEvent( gfl2::heap::HeapBase* pHeap );
  virtual ~BattleSpotEvent();

  /*
      GameEvent I/F
  */
  virtual bool  BootChk( GameSys::GameManager* pGameManager );
  virtual void  InitFunc( GameSys::GameManager* pGameManager );
  virtual void  EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );
  /*
      End GameEvent I/F
  */

  void SetupNetworkBattleParam( void );

  bool IsSessionLeft( void );

private:

  /*
      members
  */
  BattleSpotEventParam                   m_EventParam;
  NetApp::BattleSpot::APP_PARAM          m_ProcParam;
  NetApp::BattleSpot::BattleSpotProc*    m_pProc;
  //  QRのイベントアプリ
  NetEvent::QRReader::QRReaderEventParam  m_qrReaderEventParam;
  NetApp::TeamSelect::EVENT_APP_PARAM           m_teamSelectEventParam;   //! バトルチーム選択
  NetEvent::NetworkBattle::EVENT_PARAM          m_networkBattleParam;     //! 通信対戦イベント
  NetAppLib::UI::RegulationDrawInfo             m_regulationDrawInfo;     //! レギュレーション描画情報
  NetEvent::NetworkBattle::PersonalData         m_personalData[ BTL_CLIENT_NUM ];  //! 参加者データ 

  bool m_isRecRecord;
  u16 m_recParam1;
  u16 m_recParam2;
  u16 m_recParam3;

  ExtSavedata::BattleRecorderSaveData* m_pBattleRecorderSavedata;
  BattleVideoRecording::EVENT_APP_PARAM         m_battleVideoRecordingParam;  //! バトルビデオ録画アプリのパラメータ 
};  /*  class BattleSpotEvent  */


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTEVENT_H_INCLUDED)  */
