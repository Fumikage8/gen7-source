// ============================================================================
/*
 * @file GameSyncApplicationSystem.h
 * @brief ゲームシンクアプリのアプリケーションシステムです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNC_APPLICATIONSYSTEM_H_INCLUDE )
#define GAMESYNC_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(System)

class GameSyncWork;
class GameSyncBuilder;
class GameSyncViewList;
class GameSyncViewObjects;
class GameSyncViewSystemAccessor;

class GameSyncApplicationSystem : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncApplicationSystem );

public:
  GameSyncApplicationSystem();
  virtual~GameSyncApplicationSystem();

  void Build( NetApp::GameSync::System::GameSyncWork* pGameSyncWork, NetApp::GameSync::System::GameSyncBuilder* pApplicationSystemBuilder );


private:
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();

private:

  static const u32    MESSAGE_NUM   = 1;

  NetApp::GameSync::System::GameSyncWork*                  m_pGameSyncWork;
  NetApp::GameSync::System::GameSyncViewList*              m_pGameSyncViewList;
  NetApp::GameSync::System::GameSyncViewObjects*           m_pGameSyncViewObjects;
  NetApp::GameSync::System::GameSyncViewSystemAccessor*    m_pGameSyncViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)


#endif // GAMESYNC_APPLICATIONSYSTEM_H_INCLUDE
