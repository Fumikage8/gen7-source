// ============================================================================
/*
 * @file JoinFestaOpenListFrameSwitcher.cpp
 * @brief 開催選択アプリのフレームを切り替えるクラスです。
 * @date 2015.10.29
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListFrameSwitcher.h"
#include "NetApp/JoinFestaOpenList/source/JoinFestaOpenListFrameID.h"
#include "NetApp/JoinFestaOpenList/source/JoinFestaOpenListFrameResult.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID JoinFestaOpenListFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_FRAME_ID_SELECT:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaOpenList::SELECT_FRAME_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaOpenList::SELECT_FRAME_RESULT_TO_DESCRIPTION:
          nextFrameID = NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_FRAME_ID_DESCRIPTION;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_FRAME_ID_DESCRIPTION:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaOpenList::DESCRIPTION_FRAME_RESULT_BACK:
          nextFrameID = NetApp::JoinFestaOpenList::JOINFESTAOPENLIST_FRAME_ID_SELECT;
        break;
      }
    }
    break;
  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)
