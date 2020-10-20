// ============================================================================
/*
 * @file MiracleTradeFrameFactory.cpp
 * @brief ミラクル交換アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.05.25
 */
// ============================================================================
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeFrameFactory.h"
#include "NetApp/MiracleTrade/source/MiracleTradeFrameID.h"
#include "NetApp/MiracleTrade/source/StartConfirm/MiracleTradeStartConfirmFrame.h"
#include "NetApp/MiracleTrade/source/Matching/MiracleTradeMatchingFrame.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


MiracleTradeFrameFactory::MiracleTradeFrameFactory( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork, NetApp::MiracleTrade::ApplicationSystem::MiracleTradeViewObjects* pMiracleTradeViewObjects ) :
  m_pMiracleTradeWork( pMiracleTradeWork ),
  m_pMiracleTradeViewObjects( pMiracleTradeViewObjects )
{
}


MiracleTradeFrameFactory::~MiracleTradeFrameFactory()
{
}


NetAppLib::System::NetAppFrameBase* MiracleTradeFrameFactory::CreateNetAppFrame( NetAppLib::System::FrameID frameID )
{
  app::util::Heap* pAppHeap = m_pMiracleTradeWork->GetAppHeap();

  NetAppLib::System::NetAppFrameBase* pFrame = NULL;

  switch( frameID )
  {
    case NetApp::MiracleTrade::MIRACLETRADE_FRAME_ID_START_CONFIRM:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::MiracleTrade::StartConfirm::MiracleTradeStartConfirmFrame( m_pMiracleTradeWork, m_pMiracleTradeViewObjects );
    break;

    case NetApp::MiracleTrade::MIRACLETRADE_FRAME_ID_MATCHING:
      pFrame = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::MiracleTrade::Matching::MiracleTradeMatchingFrame( m_pMiracleTradeWork, m_pMiracleTradeViewObjects );
    break;
  }

  return pFrame;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)
