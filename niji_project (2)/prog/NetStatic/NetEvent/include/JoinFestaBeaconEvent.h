// ============================================================================
/*
 * @file JoinFestaBeaconEvent.h
 * @brief ビーコンアプリのイベント
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( JOINFESTABEACONEVENT_H_INCLUDE )
#define JOINFESTABEACONEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/JoinFestaBeacon/include/JoinFestaBeaconAppParam.h"
#include "NetApp/JoinFestaBeacon/include/JoinFestaBeaconProc.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase
#include "NetStatic/NetEvent/include/NBREvent.h"

#if PM_DEBUG
#include "Debug/Launcher/include/LauncherProc.h"
#include "Debug/StartMenu/include/StartLauncher.h"
#include "Debug/DebugWin/include/DebugWinSystem.h"
#endif // PM_DEBUG


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)


class JoinFestaBeaconEvent : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaBeaconEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager , NetApp::JoinFestaBeacon::APP_PARAM* param );

  JoinFestaBeaconEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaBeaconEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

private:

  void ResetPersonalDataStatus( void );

private:

  static NetApp::JoinFestaBeacon::APP_PARAM*                       m_pAppParam;

  NetApp::JoinFestaBeacon::JoinFestaBeaconProc* m_proc;
  NetEvent::NBR::EVENT_PARAM m_NBREventParam;

  u32 m_eventSeq;

};


GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetEvent)

#endif // JOINFESTABEACONEVENT_H_INCLUDE
