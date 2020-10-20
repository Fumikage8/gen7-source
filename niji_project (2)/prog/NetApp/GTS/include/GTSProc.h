// ============================================================================
/*
 * @file GTSProc.h
 * @brief GTSプロック
 * @date 2015.03.17
 */
// ============================================================================
#if !defined( GTSPROC_H_INCLUDE )
#define GTSPROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/GTS/include/GTSAppParam.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class GTSWork;
class GTSApplicationSystem;
GFL_NAMESPACE_END(ApplicationSystem)


class GTSProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSProc );
public:
  GTSProc( NetApp::GTS::APP_PARAM* pAppParam );
  virtual~GTSProc();

  void SetGTSWork( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) { m_pGTSWork = pGTSWork; }

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

  NetApp::GTS::APP_PARAM*                                     m_pAppParam;
  NetApp::GTS::ApplicationSystem::GTSWork*                    m_pGTSWork;
  NetApp::GTS::ApplicationSystem::GTSApplicationSystem*       m_pApplicationSystem;

};


GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSPROC_H_INCLUDE
