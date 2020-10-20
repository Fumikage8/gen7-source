// ============================================================================
/*
 * @file JoinFestaBeaconFrameSwitcher.cpp
 * @brief ビーコンアプリのフレームを切り替えるクラスです。
 * @date 2015.10.05
 * @author kousaka_shin
 */
// ============================================================================
#include "NetApp/JoinFestaBeacon/source/System/JoinFestaBeaconFrameSwitcher.h"
#include "NetApp/JoinFestaBeacon/source/JoinFestaBeaconFrameID.h"
#include "NetApp/JoinFestaBeacon/source/JoinFestaBeaconFrameResult.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaBeacon)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID JoinFestaBeaconFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::JoinFestaBeacon::JOINFESTABEACON_FRAME_ID_INVITE_SINGLE:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaBeacon::RESULT_CONNECT_SUCCESS:
        case NetApp::JoinFestaBeacon::RESULT_ERROR:
        case NetApp::JoinFestaBeacon::RESULT_CONNECT_NG:
        case NetApp::JoinFestaBeacon::RESULT_CONNECT_CANCEL:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;
    case NetApp::JoinFestaBeacon::JOINFESTABEACON_FRAME_ID_WAIT_INVITED:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaBeacon::RESULT_CONNECT_SUCCESS:
        case NetApp::JoinFestaBeacon::RESULT_ERROR:
        case NetApp::JoinFestaBeacon::RESULT_CONNECT_CANCEL:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;
  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaBeacon)
GFL_NAMESPACE_END(NetApp)
