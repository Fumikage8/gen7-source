// ============================================================================
/*
 * @file MiracleTradeFrameSwitcher.cpp
 * @brief ミラクル交換アプリのフレームを切り替えるクラスです。
 * @date 2015.05.25
 */
// ============================================================================
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeFrameSwitcher.h"
#include "NetApp/MiracleTrade/source/MiracleTradeFrameID.h"
#include "NetApp/MiracleTrade/source/MiracleTradeFrameResult.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


NetAppLib::System::FrameID MiracleTradeFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::MiracleTrade::MIRACLETRADE_FRAME_ID_START_CONFIRM:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::MiracleTrade::START_CONFIRM_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::MiracleTrade::START_CONFIRM_RESULT_START:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::MiracleTrade::MIRACLETRADE_FRAME_ID_MATCHING:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::MiracleTrade::MATCHING_RESULT_TRADE_SUCCESS:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::MiracleTrade::MATCHING_RESULT_TRADE_ERROR:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;

  }

  return nextFrameID;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)
