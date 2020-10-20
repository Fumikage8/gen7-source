// ============================================================================
/*
 * @file GameSyncViewSystemAccessor.h
 * @brief ゲームシンクアプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNC_VIEWSYSTEMACCESSOR_H_INCLUDE )
#define GAMESYNC_VIEWSYSTEMACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(System)

class GameSyncViewList;
class GameSyncViewObjects;

class GameSyncViewSystemAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncViewSystemAccessor );

public:

  GameSyncViewSystemAccessor(
    NetApp::GameSync::System::GameSyncViewList* pGameSyncViewList,
    NetApp::GameSync::System::GameSyncViewObjects* pGameSyncViewObjects );

  virtual~GameSyncViewSystemAccessor();

  NetApp::GameSync::System::GameSyncViewList*  GetGameSyncViewList() { return m_pGameSyncViewList; }
  NetApp::GameSync::System::GameSyncViewObjects*  GetGameSyncViewObjects() { return m_pGameSyncViewObjects; }

private:

  NetApp::GameSync::System::GameSyncViewList*  m_pGameSyncViewList;
  NetApp::GameSync::System::GameSyncViewObjects*  m_pGameSyncViewObjects;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)


#endif // GAMESYNC_VIEWSYSTEMACCESSOR_H_INCLUDE
