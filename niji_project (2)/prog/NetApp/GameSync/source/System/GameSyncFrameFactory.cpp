// ============================================================================
/*
 * @file GameSyncFrameFactory.cpp
 * @brief ゲームシンクアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/GameSync/source/System/GameSyncFrameFactory.h"
#include "NetApp/GameSync/source/GameSyncFrameID.h"
#include "NetApp/GameSync/source/Frame/GameSyncMainFrame.h"
#include "NetApp/GameSync/source/Frame/GameSyncDescriptionFrame.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(System)


GameSyncFrameFactory::GameSyncFrameFactory( NetApp::GameSync::System::GameSyncWork* pGameSyncWork, NetApp::GameSync::System::GameSyncViewSystemAccessor* pGameSyncViewSystemAccessor ) :
  m_pGameSyncWork( pGameSyncWork ),
  m_pGameSyncViewSystemAccessor( pGameSyncViewSystemAccessor )
{
}


GameSyncFrameFactory::~GameSyncFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* GameSyncFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  gfl2::heap::HeapBase* pDeviceHeap = m_pGameSyncWork->GetDeviceHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::GameSync::GAMESYNC_FRAME_ID_MAIN:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::GameSync::Frame::GameSyncMainFrame( m_pGameSyncWork, m_pGameSyncViewSystemAccessor );
    break;

    case NetApp::GameSync::GAMESYNC_FRAME_ID_DESCRIPTION:
      pFrame = GFL_NEW( pDeviceHeap ) NetApp::GameSync::Frame::GameSyncDescriptionFrame( m_pGameSyncWork, m_pGameSyncViewSystemAccessor );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)
