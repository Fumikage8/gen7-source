// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptFrameSwitcher.cpp
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のフレームを切り替えるクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptFrameSwitcher.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/JoinFestaPlayerListSelectForScriptFrameID.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/JoinFestaPlayerListSelectForScriptFrameResult.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID JoinFestaPlayerListSelectForScriptFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAME_ID_SELECT:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaPlayerListSelectForScript::SELECT_FRAME_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaPlayerListSelectForScript::SELECT_FRAME_RESULT_SELECT_PLAYER:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaPlayerListSelectForScript::SELECT_FRAME_RESULT_TO_DETAIL:
          nextFrameID = NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAME_ID_DETAIL;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAME_ID_DETAIL:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaPlayerListSelectForScript::DETAIL_FRAME_RESULT_BACK:
          nextFrameID = NetApp::JoinFestaPlayerListSelectForScript::JOINFESTAPLAYERLISTSELECTFORSCRIPT_FRAME_ID_SELECT;
        break;
      }
    }
    break;

  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)
