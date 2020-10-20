// ============================================================================
/*
 * @file JoinFestaAttractionListSelectEvent.h
 * @brief アトラクション開催参加アプリのイベント
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONLISTSELECTEVENT_H_INCLUDE )
#define JOINFESTAATTRACTIONLISTSELECTEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/JoinFestaAttractionListSelect/include/JoinFestaAttractionListSelectAppParam.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
class JoinFestaAttractionListSelectProc;
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)


class JoinFestaAttractionListSelectEvent : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionListSelectEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager, NetApp::JoinFestaAttractionListSelect::APP_RESULT* pAppResult );

  JoinFestaAttractionListSelectEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaAttractionListSelectEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  void SetAppResult( NetApp::JoinFestaAttractionListSelect::APP_RESULT* pAppResult ){ m_pAppResult = pAppResult; }

private:

  NetApp::JoinFestaAttractionListSelect::APP_PARAM                                   m_AppParam;
  NetApp::JoinFestaAttractionListSelect::APP_RESULT*                                 m_pAppResult;
  NetApp::JoinFestaAttractionListSelect::JoinFestaAttractionListSelectProc*          m_pJoinFestaAttractionListSelectProc;

};


GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetEvent)

#endif // JOINFESTAATTRACTIONLISTSELECTEVENT_H_INCLUDE
