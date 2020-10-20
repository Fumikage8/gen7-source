// ============================================================================
/*
 * @file GameSyncProc.h
 * @brief ゲームシンクアプリのプロッククラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNC_PROC_H_INCLUDE )
#define GAMESYNC_PROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/GameSync/include/GameSyncAppParam.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)

GFL_NAMESPACE_BEGIN(System)
class GameSyncApplicationSystem;
class GameSyncWork;
GFL_NAMESPACE_END(System)


class GameSyncProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncProc );
public:
  GameSyncProc( NetApp::GameSync::APP_PARAM* pAppParam );
  virtual~GameSyncProc();

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

  NetApp::GameSync::APP_PARAM                                         m_AppParam;
  NetApp::GameSync::System::GameSyncWork*                  m_pGameSyncWork;
  NetApp::GameSync::System::GameSyncApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNC_PROC_H_INCLUDE
