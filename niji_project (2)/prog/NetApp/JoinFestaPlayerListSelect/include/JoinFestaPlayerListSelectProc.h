// ============================================================================
/*
 * @file JoinFestaPlayerListSelectProc.h
 * @brief プレイヤー単体選択アプリのプロッククラスです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECT_PROC_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECT_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/JoinFestaPlayerListSelect/include/JoinFestaPlayerListSelectAppParam.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaPlayerListSelectApplicationSystem;
class JoinFestaPlayerListSelectWork;
GFL_NAMESPACE_END(System)


class JoinFestaPlayerListSelectProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectProc );
public:
  JoinFestaPlayerListSelectProc( NetApp::JoinFestaPlayerListSelect::APP_PARAM* pAppParam );
  virtual~JoinFestaPlayerListSelectProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pProcManager );
  virtual void DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

  void SetAppResult( NetApp::JoinFestaPlayerListSelect::APP_RESULT* pAppResult ){ m_pAppResult = pAppResult; }

private:

  enum
  {
    INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM,
    INIT_SEQUENCE_RESOURCE_LOADING,
  };

  NetApp::JoinFestaPlayerListSelect::APP_PARAM                                               m_AppParam;
  NetApp::JoinFestaPlayerListSelect::APP_RESULT*                                             m_pAppResult;
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork*                  m_pJoinFestaPlayerListSelectWork;
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECT_PROC_H_INCLUDE
