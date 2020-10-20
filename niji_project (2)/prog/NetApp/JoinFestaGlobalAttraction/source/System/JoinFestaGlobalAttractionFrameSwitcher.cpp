// ============================================================================
/*
 * @file JoinFestaGlobalAttractionFrameSwitcher.cpp
 * @brief グローバルアトラクションアプリのフレームを切り替えるクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionFrameSwitcher.h"
#include "NetApp/JoinFestaGlobalAttraction/source/JoinFestaGlobalAttractionFrameID.h"
#include "NetApp/JoinFestaGlobalAttraction/source/JoinFestaGlobalAttractionFrameResult.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID JoinFestaGlobalAttractionFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::JoinFestaGlobalAttraction::JOINFESTAGLOBALATTRACTION_FRAME_ID_MAIN:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaGlobalAttraction::MAIN_FRAME_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;

  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)
