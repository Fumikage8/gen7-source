// ============================================================================
/*
 * @file GameSyncFrameSwitcher.cpp
 * @brief ゲームシンクアプリのフレームを切り替えるクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/GameSync/source/System/GameSyncFrameSwitcher.h"
#include "NetApp/GameSync/source/GameSyncFrameID.h"
#include "NetApp/GameSync/source/GameSyncFrameResult.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID GameSyncFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::GameSync::GAMESYNC_FRAME_ID_MAIN:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::GameSync::MAIN_FRAME_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::GameSync::MAIN_FRAME_RESULT_GO_TO_DESCRIPTION:
          nextFrameID = NetApp::GameSync::GAMESYNC_FRAME_ID_DESCRIPTION;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::GameSync::GAMESYNC_FRAME_ID_DESCRIPTION:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::GameSync::DESCRIPTION_FRAME_RESULT_BACK:
          nextFrameID = NetApp::GameSync::GAMESYNC_FRAME_ID_MAIN;
        break;
      }
    }
    break;
  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)
