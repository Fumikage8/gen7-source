// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiProc.h
 * @brief プレイヤー複数選択アプリのプロッククラスです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTMULTI_PROC_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTMULTI_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/include/JoinFestaPlayerListSelectMultiAppParam.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaPlayerListSelectMultiApplicationSystem;
class JoinFestaPlayerListSelectMultiWork;
GFL_NAMESPACE_END(System)


class JoinFestaPlayerListSelectMultiProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectMultiProc );
public:
  JoinFestaPlayerListSelectMultiProc( NetApp::JoinFestaPlayerListSelectMulti::APP_PARAM* pAppParam );
  virtual~JoinFestaPlayerListSelectMultiProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pProcManager );
  virtual void DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

  void SetAppResult( NetApp::JoinFestaPlayerListSelectMulti::APP_RESULT* pAppResult ){ m_pAppResult = pAppResult; }

private:

  enum
  {
    INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM,
    INIT_SEQUENCE_RESOURCE_LOADING,
  };

  NetApp::JoinFestaPlayerListSelectMulti::APP_PARAM                                                    m_AppParam;
  NetApp::JoinFestaPlayerListSelectMulti::APP_RESULT*                                                  m_pAppResult;
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork*                  m_pJoinFestaPlayerListSelectMultiWork;
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTMULTI_PROC_H_INCLUDE
