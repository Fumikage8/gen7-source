// ============================================================================
/*
 * @file RegulationSelectEvent.h
 * @brief レギュレーション選択イベント
 * @date 2015.06.12
 */
// ============================================================================
#if !defined( REGULATIONSELECTEVENT_H_INCLUDE )
#define REGULATIONSELECTEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "GameSys/include/GameManager.h"
#include "NetApp/RegulationSelect/include/RegulationSelectAppParam.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
class RegulationSelectProc;
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(RegulationSelect)

class RegulationSelectEvent : public NetEvent::NetEventBase
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationSelectEvent );
public:

  static void StartEvent( GameSys::GameManager* pGameManager, const NetApp::RegulationSelect::APP_PARAM& appParam );
  static void StartEvent( GameSys::GameManager* pGameManager, const NetApp::RegulationSelect::APP_PARAM& appParam, NetApp::RegulationSelect::APP_RESULT* pAppResult );

  RegulationSelectEvent( gfl2::heap::HeapBase* pHeap );
  virtual~RegulationSelectEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  void SetAppParam( const NetApp::RegulationSelect::APP_PARAM& appParam ){ m_AppParam = appParam; }
  void SetAppResult( NetApp::RegulationSelect::APP_RESULT* pAppResult ){ m_pAppResult = pAppResult; }

private:

  NetApp::RegulationSelect::APP_PARAM               m_AppParam;
  NetApp::RegulationSelect::APP_RESULT*             m_pAppResult;
  NetApp::RegulationSelect::APP_RESULT_LOCAL        m_AppResultLocal;
  NetApp::RegulationSelect::RegulationSelectProc*   m_pProc;


};


GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetEvent)

#endif // REGULATIONSELECTEVENT_H_INCLUDE
