// ============================================================================
/*
 * @file JoinFestaPlayerListSelectEvent.h
 * @brief プレイヤー単体選択アプリのイベント
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTEVENT_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/JoinFestaPlayerListSelect/include/JoinFestaPlayerListSelectAppParam.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
class JoinFestaPlayerListSelectProc;
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)


class JoinFestaPlayerListSelectEvent : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager, const NetApp::JoinFestaPlayerListSelect::APP_PARAM& appParam, NetApp::JoinFestaPlayerListSelect::APP_RESULT* pAppResult );

  JoinFestaPlayerListSelectEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaPlayerListSelectEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  void SetAppParam( const NetApp::JoinFestaPlayerListSelect::APP_PARAM& appParam ){ m_AppParam = appParam; }
  void SetAppResult( NetApp::JoinFestaPlayerListSelect::APP_RESULT* pAppResult ){ m_pAppResult = pAppResult; }

private:

  NetApp::JoinFestaPlayerListSelect::APP_PARAM                             m_AppParam;
  NetApp::JoinFestaPlayerListSelect::APP_RESULT*                           m_pAppResult;
  NetApp::JoinFestaPlayerListSelect::JoinFestaPlayerListSelectProc*        m_pJoinFestaPlayerListSelectProc;

};


GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetEvent)

#endif // JOINFESTAPLAYERLISTSELECTEVENT_H_INCLUDE
