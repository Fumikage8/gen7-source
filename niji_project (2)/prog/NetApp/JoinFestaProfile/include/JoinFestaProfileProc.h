// ============================================================================
/*
 * @file JoinFestaProfileProc.h
 * @brief プロフィールアプリのプロッククラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILE_PROC_H_INCLUDE )
#define JOINFESTAPROFILE_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/JoinFestaProfile/include/JoinFestaProfileAppParam.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaProfileApplicationSystem;
class JoinFestaProfileWork;
GFL_NAMESPACE_END(System)


class JoinFestaProfileProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaProfileProc );
public:
  JoinFestaProfileProc( NetApp::JoinFestaProfile::APP_PARAM* pAppParam );
  virtual~JoinFestaProfileProc();

  void SetProfileWork( NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork ){ m_pJoinFestaProfileWork = pJoinFestaProfileWork; }

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

  NetApp::JoinFestaProfile::APP_PARAM                                      m_AppParam;
  NetApp::JoinFestaProfile::System::JoinFestaProfileWork*                  m_pJoinFestaProfileWork;
  NetApp::JoinFestaProfile::System::JoinFestaProfileApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILE_PROC_H_INCLUDE
