// ============================================================================
/*
 * @file GameSyncEvent.h
 * @brief ゲームシンクアプリのイベント
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNCEVENT_H_INCLUDE )
#define GAMESYNCEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/GameSync/include/GameSyncAppParam.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
class GameSyncProc;
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(GameSync)


class GameSyncEvent : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager );

  GameSyncEvent( gfl2::heap::HeapBase* pHeap );
  virtual~GameSyncEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

private:

  NetApp::GameSync::APP_PARAM                        m_AppParam;
  NetApp::GameSync::GameSyncProc*                    m_pGameSyncProc;

};


GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetEvent)

#endif // GAMESYNCEVENT_H_INCLUDE
