// ============================================================================
/*
 * @file RegulationSelectFrameSwitcher.cpp
 * @brief レギュレーション選択アプリのフレームを切り替えるクラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectFrameSwitcher.h"
#include "NetApp/RegulationSelect/source/RegulationSelectFrameID.h"
#include "NetApp/RegulationSelect/source/RegulationSelectFrameResult.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


NetAppLib::System::FrameID RegulationSelectFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_SELECT_LOAD:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::RegulationSelect::SELECT_LOAD_RESULT_SUCCESS_TO_MAIN:
          nextFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_SELECT_MAIN;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_SELECT_MAIN:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::RegulationSelect::SELECT_MAIN_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::RegulationSelect::SELECT_MAIN_RESULT_SELECT_REGULATION:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::RegulationSelect::SELECT_MAIN_RESULT_TO_DETAIL:
          nextFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DETAIL;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DL_LOAD:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::RegulationSelect::DL_LOAD_RESULT_SUCCESS_TO_MAIN:
          nextFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DL_MAIN;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DL_MAIN:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::RegulationSelect::DL_MAIN_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::RegulationSelect::DL_MAIN_RESULT_SUCCESS:
          nextFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DL_SELECT;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DL_SELECT:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::RegulationSelect::DL_SELECT_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::RegulationSelect::DL_SELECT_RESULT_TO_DELETE:
          nextFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DL_DELETE;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::RegulationSelect::DL_SELECT_RESULT_TO_DETAIL:
          nextFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DETAIL;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DL_DELETE:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::RegulationSelect::DL_DELETE_RESULT_BACK:
          nextFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DL_SELECT;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::RegulationSelect::DL_DELETE_RESULT_TO_DETAIL:
          nextFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DETAIL;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DETAIL:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::RegulationSelect::DETAIL_RESULT_BACK_TO_SELECT_MAIN:
          nextFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_SELECT_MAIN;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::RegulationSelect::DETAIL_RESULT_BACK_TO_DL_SELECT:
          nextFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DL_SELECT;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::RegulationSelect::DETAIL_RESULT_BACK_TO_DL_DELETE:
          nextFrameID = NetApp::RegulationSelect::REGULATIONSELECT_FRAME_ID_DL_DELETE;
        break;
        
        // -----------------------------------------------------------------------------
        // 
        case NetApp::RegulationSelect::DETAIL_RESULT_BACK_TO_BATTLE_TEAM_SELECT:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;
  }

  return nextFrameID;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)
