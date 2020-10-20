// ============================================================================
/*
 * @file JoinFestaAttractionResultFrameSwitcher.cpp
 * @brief アトラクション結果アプリのフレームを切り替えるクラスです。
 * @date 2015.11.02
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaAttractionResult/source/System/JoinFestaAttractionResultFrameSwitcher.h"
#include "NetApp/JoinFestaAttractionResult/source/JoinFestaAttractionResultFrameID.h"
#include "NetApp/JoinFestaAttractionResult/source/JoinFestaAttractionResultFrameResult.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionResult)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID JoinFestaAttractionResultFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case NetApp::JoinFestaAttractionResult::JOINFESTAATTRACTIONRESULT_FRAME_ID_RESULT:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case NetApp::JoinFestaAttractionResult::RESULT_FRAME_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;

  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaAttractionResult)
GFL_NAMESPACE_END(NetApp)
