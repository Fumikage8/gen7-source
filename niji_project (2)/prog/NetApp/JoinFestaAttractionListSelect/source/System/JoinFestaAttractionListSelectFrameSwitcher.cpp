// ============================================================================
/*
 * @file JoinFestaAttractionListSelectFrameSwitcher.cpp
 * @brief アトラクション開催参加アプリのフレームを切り替えるクラスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectFrameSwitcher.h"
#include "NetApp/JoinFestaAttractionListSelect/source/JoinFestaAttractionListSelectFrameID.h"
#include "NetApp/JoinFestaAttractionListSelect/source/JoinFestaAttractionListSelectFrameResult.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID JoinFestaAttractionListSelectFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_OPEN:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaAttractionListSelect::OPEN_FRAME_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaAttractionListSelect::OPEN_FRAME_RESULT_TO_ENTRY:
          nextFrameID = NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_ENTRY;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaAttractionListSelect::OPEN_FRAME_RESULT_TO_DESCRIPTION:
          nextFrameID = NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_DESCRIPTION;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_ENTRY:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaAttractionListSelect::ENTRY_FRAME_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaAttractionListSelect::ENTRY_FRAME_RESULT_TO_OPEN:
          nextFrameID = NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_OPEN;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaAttractionListSelect::ENTRY_FRAME_RESULT_TO_DESCRIPTION:
          nextFrameID = NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_DESCRIPTION;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_DESCRIPTION:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaAttractionListSelect::DESCRIPTION_FRAME_RESULT_BACK_TO_OPEN:
          nextFrameID = NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_OPEN;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaAttractionListSelect::DESCRIPTION_FRAME_RESULT_BACK_TO_ENTRY:
          nextFrameID = NetApp::JoinFestaAttractionListSelect::JOINFESTAATTRACTIONLISTSELECT_FRAME_ID_ENTRY;
        break;
      }
    }
    break;

  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)
