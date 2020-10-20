// ============================================================================
/*
 * @file JoinFestaPlayerListShowProc.h
 * @brief 閲覧アプリのプロッククラスです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSHOW_PROC_H_INCLUDE )
#define JOINFESTAPLAYERLISTSHOW_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/JoinFestaPlayerListShow/include/JoinFestaPlayerListShowAppParam.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaPlayerListShowApplicationSystem;
class JoinFestaPlayerListShowWork;
GFL_NAMESPACE_END(System)


class JoinFestaPlayerListShowProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListShowProc );
public:
  JoinFestaPlayerListShowProc( NetApp::JoinFestaPlayerListShow::APP_PARAM* pAppParam );
  virtual~JoinFestaPlayerListShowProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pProcManager );
  virtual void DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

private:

  enum
  {
    INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM,
    INIT_SEQUENCE_RESOURCE_LOADING,
  };

  NetApp::JoinFestaPlayerListShow::APP_PARAM                                         m_AppParam;
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork*                  m_pJoinFestaPlayerListShowWork;
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSHOW_PROC_H_INCLUDE
