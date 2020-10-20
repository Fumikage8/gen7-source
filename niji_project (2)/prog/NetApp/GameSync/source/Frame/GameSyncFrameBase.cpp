// ============================================================================
/*
 * @file GameSyncFrameBase.cpp
 * @brief ゲームシンクアプリのフレームの基底クラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/GameSync/source/Frame/GameSyncFrameBase.h"
#include "NetApp/GameSync/source/System/GameSyncViewSystemAccessor.h"



GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(Frame)


GameSyncFrameBase::GameSyncFrameBase(
  NetApp::GameSync::System::GameSyncWork* pGameSyncWork,
  NetApp::GameSync::System::GameSyncViewSystemAccessor* pGameSyncViewSystemAccessor ) :
  NetAppLib::System::NetAppFrameBase( pGameSyncWork ),
  m_pGameSyncWork( pGameSyncWork ),
  m_pGameSyncViewList( pGameSyncViewSystemAccessor->GetGameSyncViewList() ),
  m_pGameSyncViewObjects( pGameSyncViewSystemAccessor->GetGameSyncViewObjects() )
{
}


GameSyncFrameBase::~GameSyncFrameBase()
{
}


void GameSyncFrameBase::exitFrame( NetAppLib::System::FrameResult result )
{
  NetAppLib::System::NetAppFrameBase::exitFrame( result );

  m_pGameSyncViewObjects->InputDisableAll();
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)
