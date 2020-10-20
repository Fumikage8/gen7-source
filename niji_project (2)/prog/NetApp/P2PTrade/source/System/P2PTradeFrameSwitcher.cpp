// ============================================================================
/*
 * @file P2PTradeFrameSwitcher.cpp
 * @brief P2P交換アプリのフレームを切り替えるクラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/P2PTrade/source/System/P2PTradeFrameSwitcher.h"
#include "NetApp/P2PTrade/source/P2PTradeFrameID.h"
#include "NetApp/P2PTrade/source/P2PTradeFrameResult.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID P2PTradeFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::P2PTrade::P2PTRADE_FRAME_ID_WARNING:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::P2PTrade::WARNING_FRAME_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::P2PTrade::P2PTRADE_FRAME_ID_SYNC_SAVE:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::P2PTrade::SYNC_SAVE_FRAME_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::P2PTrade::P2PTRADE_FRAME_ID_SAVE:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::P2PTrade::SAVE_FRAME_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;

  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)
