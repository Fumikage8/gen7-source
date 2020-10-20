// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptEvent.h
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のイベント
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTFORSCRIPTEVENT_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTFORSCRIPTEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/include/JoinFestaPlayerListSelectForScriptAppParam.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase
#include <Field/FieldStatic/include/BattleFes/BattleFesTool.h>
#include <NetStatic/NetEvent/include/JoinFestaPokeStatusEvent.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
class JoinFestaPlayerListSelectForScriptProc;
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)


class JoinFestaPlayerListSelectForScriptEvent : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectForScriptEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager, const NetApp::JoinFestaPlayerListSelectForScript::APP_PARAM& appParam, NetApp::JoinFestaPlayerListSelectForScript::APP_RESULT* pAppResult );

  JoinFestaPlayerListSelectForScriptEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaPlayerListSelectForScriptEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  void SetAppParam( const NetApp::JoinFestaPlayerListSelectForScript::APP_PARAM& appParam ){ m_AppParam = appParam; }
  void SetAppResult( NetApp::JoinFestaPlayerListSelectForScript::APP_RESULT* pAppResult ){ m_pAppResult = pAppResult; }

private:

  NetApp::JoinFestaPlayerListSelectForScript::APP_PARAM                                 m_AppParam;
  NetApp::JoinFestaPlayerListSelectForScript::APP_RESULT*                               m_pAppResult;
  NetApp::JoinFestaPlayerListSelectForScript::JoinFestaPlayerListSelectForScriptProc*   m_pJoinFestaPlayerListSelectForScriptProc;

  /*  BFC拡張  */
  NetApp::JoinFestaPlayerListSelectForScript::BFCModeInfo                               m_BFCInfo;
  NetEvent::JoinFestaPokeStatus::JoinFestaPokeStatusEvent::Param                        m_PokeStatusParam;
  BattleFesTool::BFCTrainerInfo                                                         m_BFCTrainerInfo;
};


GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetEvent)

#endif // JOINFESTAPLAYERLISTSELECTFORSCRIPTEVENT_H_INCLUDE
