// ============================================================================
/*
 * @file JoinFestaOpenListEvent.h
 * @brief 開催選択アプリのイベント
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAOPENLISTEVENT_H_INCLUDE )
#define JOINFESTAOPENLISTEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/JoinFestaOpenList/include/JoinFestaOpenListAppParam.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
class JoinFestaOpenListProc;
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)


class JoinFestaOpenListEvent : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaOpenListEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager, NetApp::JoinFestaOpenList::APP_RESULT* pAppResult );

  JoinFestaOpenListEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaOpenListEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  void SetAppResult( NetApp::JoinFestaOpenList::APP_RESULT* pAppResult ){ m_pAppResult = pAppResult; }

private:

  NetApp::JoinFestaOpenList::APP_PARAM                     m_AppParam;
  NetApp::JoinFestaOpenList::APP_RESULT*                   m_pAppResult;
  NetApp::JoinFestaOpenList::JoinFestaOpenListProc*        m_pJoinFestaOpenListProc;

};


GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetEvent)

#endif // JOINFESTAOPENLISTEVENT_H_INCLUDE
