// ============================================================================
/*
 * @file JoinFestaGlobalAttractionEvent.h
 * @brief グローバルアトラクションアプリのイベント
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTIONEVENT_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTIONEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/JoinFestaGlobalAttraction/include/JoinFestaGlobalAttractionAppParam.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
class JoinFestaGlobalAttractionProc;
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)


class JoinFestaGlobalAttractionEvent : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaGlobalAttractionEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager );

  JoinFestaGlobalAttractionEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaGlobalAttractionEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

private:

  NetApp::JoinFestaGlobalAttraction::APP_PARAM                        m_AppParam;
  NetApp::JoinFestaGlobalAttraction::JoinFestaGlobalAttractionProc*   m_pJoinFestaGlobalAttractionProc;

};


GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetEvent)

#endif // JOINFESTAGLOBALATTRACTIONEVENT_H_INCLUDE
