// ============================================================================
/*
 * @file JoinFestaSelectWordFrameSwitcher.cpp
 * @brief 挨拶選択アプリのフレームを切り替えるクラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordFrameSwitcher.h"
#include "NetApp/JoinFestaSelectWord/source/JoinFestaSelectWordFrameID.h"
#include "NetApp/JoinFestaSelectWord/source/JoinFestaSelectWordFrameResult.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID JoinFestaSelectWordFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_FRAME_ID_MAIN:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaSelectWord::MAIN_FRAME_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;

        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaSelectWord::MAIN_FRAME_RESULT_TO_DESCRIPTION:
          nextFrameID = NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_FRAME_ID_DESCRIPTION;
        break;
      }
    }
    break;

    // =================================================================================
    case NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_FRAME_ID_DESCRIPTION:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaSelectWord::DESCRIPTION_FRAME_RESULT_BACK:
          nextFrameID = NetApp::JoinFestaSelectWord::JOINFESTASELECTWORD_FRAME_ID_MAIN;
        break;
      }
    }
    break;

  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)
