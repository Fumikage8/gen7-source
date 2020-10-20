// ============================================================================
/*
 * @file JoinFestaProfileEvent.h
 * @brief プロフィールアプリのイベント
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILEEVENT_H_INCLUDE )
#define JOINFESTAPROFILEEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/JoinFestaProfile/include/JoinFestaProfileAppParam.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileWork.h"
#include "GameSys/include/GameEvent.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
class JoinFestaProfileProc;
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)


class JoinFestaProfileEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaProfileEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager );

  JoinFestaProfileEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaProfileEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  void bootProfileProc();
  void bootSelectWordEvent( GameSys::GameManager* pGameManager );

private:

  NetApp::JoinFestaProfile::APP_PARAM                      m_AppParam;
  NetApp::JoinFestaProfile::JoinFestaProfileProc*          m_pJoinFestaProfileProc;
  NetApp::JoinFestaProfile::System::JoinFestaProfileWork*  m_pJoinFestaProfileWork;

};


GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetEvent)

#endif // JOINFESTAPROFILEEVENT_H_INCLUDE
