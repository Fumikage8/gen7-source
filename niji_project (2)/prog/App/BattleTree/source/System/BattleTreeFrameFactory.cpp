// ============================================================================
/*
 * @file BattleTreeFrameFactory.cpp
 * @brief バトルツリーアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#include "App/BattleTree/source/System/BattleTreeFrameFactory.h"
#include "App/BattleTree/source/BattleTreeFrameID.h"
#include "App/BattleTree/source/Frame/BattleTreeFrame.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(System)


  BattleTreeFrameFactory::BattleTreeFrameFactory( App::BattleTree::System::BattleTreeWork* pBattleTreeWork ) :
    m_pBattleTreeWork( pBattleTreeWork )
  {
  }


  NetAppLib::System::NetAppFrameBase* BattleTreeFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
  {
    app::util::Heap* pAppHeap = m_pBattleTreeWork->GetAppHeap();

    NetAppLib::System::NetAppFrameBase* pFrame = NULL;

    switch( frameID )
    {
      case App::BattleTree::BATTLE_TREE_FRAME_ID:
        pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleTree::Frame::BattleTreeFrame( m_pBattleTreeWork );
      break;
    }

    return pFrame;
  }


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)
