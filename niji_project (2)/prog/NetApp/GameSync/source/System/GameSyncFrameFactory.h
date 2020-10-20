// ============================================================================
/*
 * @file GameSyncFrameFactory.h
 * @brief ゲームシンクアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNC_FRAMEFACTORY_H_INCLUDE )
#define GAMESYNC_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/GameSync/source/System/GameSyncWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(System)


class GameSyncViewSystemAccessor;


class GameSyncFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncFrameFactory );
public:
  GameSyncFrameFactory(
    NetApp::GameSync::System::GameSyncWork* pGameSyncWork,
    NetApp::GameSync::System::GameSyncViewSystemAccessor* pGameSyncViewSystemAccessor );
  virtual~GameSyncFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::GameSync::System::GameSyncWork*                m_pGameSyncWork;
  NetApp::GameSync::System::GameSyncViewSystemAccessor*  m_pGameSyncViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNC_FRAMEFACTORY_H_INCLUDE
