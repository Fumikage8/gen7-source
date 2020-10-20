// ============================================================================
/*
 * @file ConnectionFrameSwitcher.cpp
 * @brief インターネット接続アプリのフレームを切り替えるクラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/Connection/source/System/ConnectionFrameSwitcher.h"
#include "NetApp/Connection/source/ConnectionFrameID.h"
#include "NetApp/Connection/source/ConnectionFrameResult.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID ConnectionFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::Connection::CONNECTION_FRAME_ID_CHANGE_INTERNET_MODE:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::Connection::CHANGE_INTERNET_MODE_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::Connection::CONNECTION_FRAME_ID_CHANGE_LOCAL_MODE:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::Connection::CHANGE_LOCAL_MODE_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::Connection::CONNECTION_FRAME_ID_INTERNET_CONNECTION:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::Connection::INTERNET_CONNECTION_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::Connection::INTERNET_CONNECTION_RESULT_CHANGE_INTERNET_MODE:
          nextFrameID = NetApp::Connection::CONNECTION_FRAME_ID_CHANGE_INTERNET_MODE;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::Connection::CONNECTION_FRAME_ID_INTERNET_DISCONNECTION:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::Connection::INTERNET_DISCONNECTION_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;
  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)
