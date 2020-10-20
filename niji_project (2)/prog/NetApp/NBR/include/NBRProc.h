// ============================================================================
/*
 * @file NBRProc.h
 * @brief 通信対戦受付アプリのプロッククラスです。
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_PROC_H_INCLUDE )
#define NBR_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/NBR/include/NBRAppParam.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(ApplicationSystem)
class NBRWork;
GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class NBRApplicationSystem;
GFL_NAMESPACE_END(ApplicationSystem)


class NBRProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( NBRProc );
public:
  NBRProc();
  NBRProc( NetApp::NBR::APP_PARAM* pAppParam );
  virtual~NBRProc();

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

  NetApp::NBR::APP_PARAM*                                              m_pAppParam;
  NetApp::NBR::ApplicationSystem::NBRWork*                              m_pNBRWork;
  NetApp::NBR::ApplicationSystem::NBRApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_PROC_H_INCLUDE
