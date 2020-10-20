// ============================================================================
/*
 * @file JoinFestaPlayerDetailFrameSwitcher.cpp
 * @brief 詳細アプリのフレームを切り替えるクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailFrameSwitcher.h"
#include "NetApp/JoinFestaPlayerDetail/source/JoinFestaPlayerDetailFrameID.h"
#include "NetApp/JoinFestaPlayerDetail/source/JoinFestaPlayerDetailFrameResult.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID JoinFestaPlayerDetailFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::JoinFestaPlayerDetail::JOINFESTAPLAYERDETAIL_FRAME_ID_DETAIL:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
      case NetApp::JoinFestaPlayerDetail::DETAIL_FRAME_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;

  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)
