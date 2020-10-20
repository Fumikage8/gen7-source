// ============================================================================
/*
 * @file JoinFestaPlayerListSelectFrameSwitcher.cpp
 * @brief プレイヤー単体選択アプリのフレームを切り替えるクラスです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectFrameSwitcher.h"
#include "NetApp/JoinFestaPlayerListSelect/source/JoinFestaPlayerListSelectFrameID.h"
#include "NetApp/JoinFestaPlayerListSelect/source/JoinFestaPlayerListSelectFrameResult.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID JoinFestaPlayerListSelectFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::JoinFestaPlayerListSelect::JOINFESTAPLAYERLISTSELECT_FRAME_ID_SELECT:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaPlayerListSelect::SELECT_FRAME_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;

  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)
