// ============================================================================
/*
 * @file JoinFestaPlayerDetailProc.h
 * @brief 詳細アプリのプロッククラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERDETAIL_PROC_H_INCLUDE )
#define JOINFESTAPLAYERDETAIL_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/JoinFestaPlayerDetail/include/JoinFestaPlayerDetailAppParam.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaPlayerDetailApplicationSystem;
class JoinFestaPlayerDetailWork;
GFL_NAMESPACE_END(System)


class JoinFestaPlayerDetailProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerDetailProc );
public:
  JoinFestaPlayerDetailProc( NetApp::JoinFestaPlayerDetail::APP_PARAM* pAppParam );
  virtual~JoinFestaPlayerDetailProc();

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

  NetApp::JoinFestaPlayerDetail::APP_PARAM                                         m_AppParam;
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork*                  m_pJoinFestaPlayerDetailWork;
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERDETAIL_PROC_H_INCLUDE
