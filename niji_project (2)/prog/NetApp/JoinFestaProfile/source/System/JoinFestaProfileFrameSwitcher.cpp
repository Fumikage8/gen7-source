// ============================================================================
/*
 * @file JoinFestaProfileFrameSwitcher.cpp
 * @brief プロフィールアプリのフレームを切り替えるクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileFrameSwitcher.h"
#include "NetApp/JoinFestaProfile/source/JoinFestaProfileFrameID.h"
#include "NetApp/JoinFestaProfile/source/JoinFestaProfileFrameResult.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID JoinFestaProfileFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::JoinFestaProfile::JOINFESTAPROFILE_FRAME_ID_SELECT_EDIT_MENU:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaProfile::SELECT_EDIT_MENU_FRAME_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaProfile::SELECT_EDIT_MENU_FRAME_RESULT_TO_DESCRIPTION:
          nextFrameID = NetApp::JoinFestaProfile::JOINFESTAPROFILE_FRAME_ID_DESCRIPTION;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaProfile::SELECT_EDIT_MENU_FRAME_RESULT_TO_NAME_INPUT:
          nextFrameID = NetApp::JoinFestaProfile::JOINFESTAPROFILE_FRAME_ID_NAME_INPUT;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::JoinFestaProfile::JOINFESTAPROFILE_FRAME_ID_DESCRIPTION:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaProfile::DESCRIPTION_FRAME_RESULT_BACK:
          nextFrameID = NetApp::JoinFestaProfile::JOINFESTAPROFILE_FRAME_ID_SELECT_EDIT_MENU;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::JoinFestaProfile::JOINFESTAPROFILE_FRAME_ID_NAME_INPUT:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaProfile::NAME_INPUT_FRAME_RESULT_BACK:
          nextFrameID = NetApp::JoinFestaProfile::JOINFESTAPROFILE_FRAME_ID_SELECT_EDIT_MENU;
        break;
      }
    }
    break;

  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)
