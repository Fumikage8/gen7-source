// ============================================================================
/*
 * @file JoinFestaPlayerDetailEvent.h
 * @brief 詳細アプリのイベント
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERDETAILEVENT_H_INCLUDE )
#define JOINFESTAPLAYERDETAILEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/JoinFestaPlayerDetail/include/JoinFestaPlayerDetailAppParam.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
class JoinFestaPlayerDetailProc;
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)


class JoinFestaPlayerDetailEvent : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerDetailEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager, NetApp::JoinFestaPlayerDetail::APP_PARAM& appParam );

  JoinFestaPlayerDetailEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaPlayerDetailEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  void SetAppParam( const NetApp::JoinFestaPlayerDetail::APP_PARAM& appParam ){ m_AppParam = appParam; }

private:

  NetApp::JoinFestaPlayerDetail::APP_PARAM                         m_AppParam;
  NetApp::JoinFestaPlayerDetail::JoinFestaPlayerDetailProc*        m_pJoinFestaPlayerDetailProc;

};


GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetEvent)

#endif // JOINFESTAPLAYERDETAILEVENT_H_INCLUDE
