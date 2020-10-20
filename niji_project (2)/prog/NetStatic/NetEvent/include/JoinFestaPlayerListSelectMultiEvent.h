// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiEvent.h
 * @brief プレイヤー複数選択アプリのイベント
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTMULTIEVENT_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTMULTIEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/include/JoinFestaPlayerListSelectMultiAppParam.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
class JoinFestaPlayerListSelectMultiProc;
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)


class JoinFestaPlayerListSelectMultiEvent : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectMultiEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager, const NetApp::JoinFestaPlayerListSelectMulti::APP_PARAM& appParam, NetApp::JoinFestaPlayerListSelectMulti::APP_RESULT* pAppResult );

  JoinFestaPlayerListSelectMultiEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaPlayerListSelectMultiEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  void SetAppParam( const NetApp::JoinFestaPlayerListSelectMulti::APP_PARAM& appParam ){ m_AppParam = appParam; }
  void SetAppResult( NetApp::JoinFestaPlayerListSelectMulti::APP_RESULT* pAppResult ){ m_pAppResult = pAppResult; }

private:

  NetApp::JoinFestaPlayerListSelectMulti::APP_PARAM                                   m_AppParam;
  NetApp::JoinFestaPlayerListSelectMulti::APP_RESULT*                                 m_pAppResult;
  NetApp::JoinFestaPlayerListSelectMulti::JoinFestaPlayerListSelectMultiProc*         m_pJoinFestaPlayerListSelectMultiProc;

};


GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetEvent)

#endif // JOINFESTAPLAYERLISTSELECTMULTIEVENT_H_INCLUDE
