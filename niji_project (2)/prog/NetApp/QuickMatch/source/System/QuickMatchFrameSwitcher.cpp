// ============================================================================
/*
 * @file QuickMatchFrameSwitcher.cpp
 * @brief クイックマッチアプリのフレームを切り替えるクラスです。
 * @date 2015.08.28
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/QuickMatch/source/System/QuickMatchFrameSwitcher.h"
#include "NetApp/QuickMatch/source/QuickMatchFrameID.h"
#include "NetApp/QuickMatch/source/QuickMatchFrameResult.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QuickMatch)
GFL_NAMESPACE_BEGIN(System)

NetAppLib::System::FrameID QuickMatchFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::QuickMatch::QUICKMATCH_FRAME_ID_INIT:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::QuickMatch::RESULT_START_HOST:
          nextFrameID = NetApp::QuickMatch::QUICKMATCH_FRAME_ID_HOST;
        break;
        case NetApp::QuickMatch::RESULT_START_CLIENT:
          nextFrameID = NetApp::QuickMatch::QUICKMATCH_FRAME_ID_CLIENT;
        break;
      }
    }
    break;
    // =================================================================================
    case NetApp::QuickMatch::QUICKMATCH_FRAME_ID_HOST:
    case NetApp::QuickMatch::QUICKMATCH_FRAME_ID_CLIENT:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::QuickMatch::RESULT_START_HOST:
          nextFrameID = NetApp::QuickMatch::QUICKMATCH_FRAME_ID_INIT;
        break;
        case NetApp::QuickMatch::RESULT_START_CLIENT:
          nextFrameID = NetApp::QuickMatch::QUICKMATCH_FRAME_ID_INIT;
        break;
        case NetApp::QuickMatch::RESULT_CANCEL_EXIT:
          nextFrameID = NetApp::QuickMatch::QUICKMATCH_FRAME_ID_CANCEL_EXIT;
        break;
        case NetApp::QuickMatch::RESULT_EXIT_APP:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;
    // =================================================================================
    // =================================================================================
    case NetApp::QuickMatch::QUICKMATCH_FRAME_ID_CANCEL_EXIT:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::QuickMatch::RESULT_EXIT_APP:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;
    case NetApp::QuickMatch::QUICKMATCH_FRAME_ID_BATTLE_TREE:
      {
        nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
      }
    break;
  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(QuickMatch)
GFL_NAMESPACE_END(NetApp)
