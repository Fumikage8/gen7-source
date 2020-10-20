// ============================================================================
/*
 * @file BattleRoyalResultFrameFactory.cpp
 * @brief バトルロイヤル結果アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#include "App/BattleRoyalResult/source/System/BattleRoyalResultFrameFactory.h"
#include "App/BattleRoyalResult/source/BattleRoyalResultFrameID.h"
#include "App/BattleRoyalResult/source/Frame/BattleRoyalResultEventFrame.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(System)


BattleRoyalResultFrameFactory::BattleRoyalResultFrameFactory( App::BattleRoyalResult::System::BattleRoyalResultWork* pBattleRoyalResultWork ) :
  m_pBattleRoyalResultWork( pBattleRoyalResultWork )
{
}


NetAppLib::System::NetAppFrameBase* BattleRoyalResultFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  app::util::Heap* pAppHeap = m_pBattleRoyalResultWork->GetAppHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case App::BattleRoyalResult::BATTLE_ROYAL_RESULT_FRAME_ID_EVENT:
    {
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleRoyalResult::Frame::BattleRoyalResultEventFrame( m_pBattleRoyalResultWork );
      pFrame->FadeInRequest();
      pFrame->FadeOutRequest();

      break;
    }
  }

  return pFrame;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)
