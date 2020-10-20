// ============================================================================
/*
 * @file JoinFestaAttractionResultEvent.h
 * @brief アトラクション結果アプリのイベント
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONRESULTEVENT_H_INCLUDE )
#define JOINFESTAATTRACTIONRESULTEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/JoinFestaAttractionResult/include/JoinFestaAttractionResultAppParam.h"
#include "GameSys/include/GameEvent.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
class JoinFestaAttractionResultProc;
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)


class JoinFestaAttractionResultEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionResultEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager, NetApp::JoinFestaAttractionResult::APP_PARAM& appParam );

  JoinFestaAttractionResultEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaAttractionResultEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  void SetAppParam( const NetApp::JoinFestaAttractionResult::APP_PARAM& appParam ){ m_AppParam = appParam; }

private:

  NetApp::JoinFestaAttractionResult::APP_PARAM                               m_AppParam;
  NetApp::JoinFestaAttractionResult::JoinFestaAttractionResultProc*          m_pJoinFestaAttractionResultProc;

};


GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetEvent)

#endif // JOINFESTAATTRACTIONRESULTEVENT_H_INCLUDE
