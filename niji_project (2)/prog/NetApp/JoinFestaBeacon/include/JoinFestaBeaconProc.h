// ============================================================================
/*
 * @file JoinFestaBeaconProc.h
 * @brief ビーコンアプリのプロッククラスです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#if !defined( JOINFESTABEACON_PROC_H_INCLUDE )
#define JOINFESTABEACON_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/JoinFestaBeacon/include/JoinFestaBeaconAppParam.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaBeaconApplicationSystem;
class JoinFestaBeaconWork;
GFL_NAMESPACE_END(System)


class JoinFestaBeaconProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaBeaconProc );
public:
  JoinFestaBeaconProc( NetApp::JoinFestaBeacon::APP_PARAM* pAppParam );
  virtual~JoinFestaBeaconProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

private:

  enum
  {
    INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM,
    INIT_SEQUENCE_RESOURCE_LOADING,
  };

  NetApp::JoinFestaBeacon::APP_PARAM*                                   m_pAppParam;
  NetApp::JoinFestaBeacon::System::JoinFestaBeaconWork*                  m_pJoinFestaBeaconWork;
  NetApp::JoinFestaBeacon::System::JoinFestaBeaconApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTABEACON_PROC_H_INCLUDE
