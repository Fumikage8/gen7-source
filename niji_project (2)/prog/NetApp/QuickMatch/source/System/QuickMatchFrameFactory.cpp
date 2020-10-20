// ============================================================================
/*
 * @file QuickMatchFrameFactory.cpp
 * @brief クイックマッチアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/QuickMatch/source/System/QuickMatchFrameFactory.h"
#include "NetApp/QuickMatch/source/QuickMatchFrameID.h"
#include "NetApp/QuickMatch/source/Frame/QuickMatchInitFrame.h"
#include "NetApp/QuickMatch/source/Frame/QuickMatchHostFrame.h"
#include "NetApp/QuickMatch/source/Frame/QuickMatchClientFrame.h"
#include "NetApp/QuickMatch/source/Frame/QuickMatchCancelExitFrame.h"
#include "NetApp/QuickMatch/source/Frame/QuickMatchBattleTreeFrame.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(System)


QuickMatchFrameFactory::QuickMatchFrameFactory( NetApp::QuickMatch::System::QuickMatchWork* pQuickMatchWork ) :
  m_pQuickMatchWork( pQuickMatchWork )
{
}


QuickMatchFrameFactory::~QuickMatchFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* QuickMatchFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  app::util::Heap* pAppHeap = m_pQuickMatchWork->GetAppHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::QuickMatch::QUICKMATCH_FRAME_ID_INIT:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::QuickMatch::Frame::QuickMatchInitFrame( m_pQuickMatchWork );
    break;
    case NetApp::QuickMatch::QUICKMATCH_FRAME_ID_HOST:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::QuickMatch::Frame::QuickMatchHostFrame( m_pQuickMatchWork );
    break;
    case NetApp::QuickMatch::QUICKMATCH_FRAME_ID_CLIENT:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::QuickMatch::Frame::QuickMatchClientFrame( m_pQuickMatchWork );
    break;
    case NetApp::QuickMatch::QUICKMATCH_FRAME_ID_CANCEL_EXIT:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::QuickMatch::Frame::QuickMatchCancelExitFrame( m_pQuickMatchWork );
    break;
    case NetApp::QuickMatch::QUICKMATCH_FRAME_ID_BATTLE_TREE:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::QuickMatch::Frame::QuickMatchBattleTreeFrame( m_pQuickMatchWork );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)
