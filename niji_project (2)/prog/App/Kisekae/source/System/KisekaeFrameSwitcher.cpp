// ============================================================================
/*
 * @file KisekaeFrameSwitcher.cpp
 * @brief きせかえアプリのフレームを切り替えるクラスです。
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Kisekae/source/System/KisekaeFrameSwitcher.h"
#include "App/Kisekae/source/KisekaeFrameID.h"
#include "App/Kisekae/source/KisekaeFrameResult.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID FrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    //  きせかえショップ
    case App::Kisekae::KISEKAE_FRAME_ID_SHOP:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case App::Kisekae::RESULT_CODE_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }

      break;
    }

    // =================================================================================
    //  きせかえ
    case App::Kisekae::KISEKAE_FRAME_ID_DRESS_UP:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case App::Kisekae::RESULT_CODE_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }

      break;
    }
  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)
