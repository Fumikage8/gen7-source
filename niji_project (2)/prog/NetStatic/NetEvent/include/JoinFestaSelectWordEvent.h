// ============================================================================
/*
 * @file JoinFestaSelectWordEvent.h
 * @brief 挨拶選択アプリのイベント
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORDEVENT_H_INCLUDE )
#define JOINFESTASELECTWORDEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/JoinFestaSelectWord/include/JoinFestaSelectWordAppParam.h"
#include "GameSys/include/GameEvent.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
class JoinFestaSelectWordProc;
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)


struct EVENT_PARAM
{
  u32 editMenuID;
};


class JoinFestaSelectWordEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager, EVENT_PARAM eventParam );

  JoinFestaSelectWordEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaSelectWordEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  void SetEventParam( EVENT_PARAM eventParam ){ m_EventParam = eventParam; }

private:

  EVENT_PARAM                                             m_EventParam;
  NetApp::JoinFestaSelectWord::APP_PARAM                  m_AppParam;
  NetApp::JoinFestaSelectWord::JoinFestaSelectWordProc*   m_pJoinFestaSelectWordProc;

};


GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetEvent)

#endif // JOINFESTASELECTWORDEVENT_H_INCLUDE
