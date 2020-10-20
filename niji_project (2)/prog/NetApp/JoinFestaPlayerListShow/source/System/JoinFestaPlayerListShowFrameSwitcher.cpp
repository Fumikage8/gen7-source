// ============================================================================
/*
 * @file JoinFestaPlayerListShowFrameSwitcher.cpp
 * @brief 閲覧アプリのフレームを切り替えるクラスです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowFrameSwitcher.h"
#include "NetApp/JoinFestaPlayerListShow/source/JoinFestaPlayerListShowFrameID.h"
#include "NetApp/JoinFestaPlayerListShow/source/JoinFestaPlayerListShowFrameResult.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID JoinFestaPlayerListShowFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_FRAME_ID_SELECT:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaPlayerListShow::SELECT_FRAME_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaPlayerListShow::SELECT_FRAME_RESULT_SELECT_PLAYER:
          nextFrameID = NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_FRAME_ID_DETAIL;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaPlayerListShow::SELECT_FRAME_RESULT_TO_DESCRIPTION:
          nextFrameID = NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_FRAME_ID_DESCRIPTION;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_FRAME_ID_DETAIL:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaPlayerListShow::DETAIL_FRAME_RESULT_BACK:
          nextFrameID = NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_FRAME_ID_SELECT;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_FRAME_ID_DESCRIPTION:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaPlayerListShow::DESCRIPTION_FRAME_RESULT_BACK:
          nextFrameID = NetApp::JoinFestaPlayerListShow::JOINFESTAPLAYERLISTSHOW_FRAME_ID_SELECT;
        break;
      }
    }
    break;

  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)
