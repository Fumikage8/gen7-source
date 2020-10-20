// ============================================================================
/*
 * @file GameSyncViewSystemAccessor.cpp
 * @brief ゲームシンクアプリのViewシステム関連のクラスを取得するクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/GameSync/source/System/GameSyncViewSystemAccessor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(System)


GameSyncViewSystemAccessor::GameSyncViewSystemAccessor(
  NetApp::GameSync::System::GameSyncViewList* pGameSyncViewList,
  NetApp::GameSync::System::GameSyncViewObjects* pGameSyncViewObjects ) :
  m_pGameSyncViewList( pGameSyncViewList ),
  m_pGameSyncViewObjects( pGameSyncViewObjects )
{
}


GameSyncViewSystemAccessor::~GameSyncViewSystemAccessor()
{
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)
