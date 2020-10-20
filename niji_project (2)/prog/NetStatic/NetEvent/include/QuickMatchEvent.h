// ============================================================================
/*
 * @file QuickMatchEvent.h
 * @brief クイックマッチアプリのイベント
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( QUICKMATCHEVENT_H_INCLUDE )
#define QUICKMATCHEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/QuickMatch/include/QuickMatchAppParam.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase

#if PM_DEBUG
#include "Debug/Launcher/include/LauncherProc.h"
#include "Debug/StartMenu/include/StartLauncher.h"
#include "Debug/DebugWin/include/DebugWinSystem.h"
#endif // PM_DEBUG

#include "NetStatic/NetEvent/include/NBREvent.h"

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(QuickMatch)

//バトルツリー用
struct EVENT_PARAM
{
  bool isConnected;//true:接続できた　、false:キャンセルした
};

enum
{
  MATCH_TYPE_NORMAL = 0,
  MATCH_TYPE_TREE_NORMAL
};

class QuickMatchEvent : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( QuickMatchEvent );
private:

  enum EVENT_SEQ
  {
    CALL_NONE = 0,
    CALL_NBR,
    CALL_TRADE,
    RETURN_FIELD,
  };


public:

  static void StartEvent( GameSys::GameManager* pGameManager );
  static void StartEventForBattleTree( GameSys::GameManager* pGameManager , EVENT_PARAM* pEventParam );

  QuickMatchEvent( gfl2::heap::HeapBase* pHeap );
  virtual~QuickMatchEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  void SetEventParam( EVENT_PARAM* pEventParam );

private:

  NetApp::QuickMatch::APP_PARAM*                       m_pAppParam;
  u32 m_eventSeq;
  
  EVENT_PARAM* m_pEventParam;

  NetEvent::NBR::EVENT_PARAM m_NBREventParam;

#if PM_DEBUG
  debug::launcher::LauncherProc::CallParam      m_LauncherParam;
#endif // PM_DEBUG

};


GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetEvent)

#endif // QUICKMATCHEVENT_H_INCLUDE
