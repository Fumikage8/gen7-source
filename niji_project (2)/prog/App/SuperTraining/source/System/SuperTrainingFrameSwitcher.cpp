// ============================================================================
/*
 * @file SuperTrainingFrameSwitcher.cpp
 * @brief すごい特訓アプリのフレームを切り替えるクラスです。
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#include "App/SuperTraining/source/System/SuperTrainingFrameSwitcher.h"
#include "App/SuperTraining/source/SuperTrainingFrameID.h"
#include "App/SuperTraining/source/SuperTrainingFrameResult.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(System)


  NetAppLib::System::FrameID SuperTrainingFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
  {
    NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

    switch( executeFrameID )
    {
      // =================================================================================
      case App::SuperTraining::SUPER_TRAINIG_FRAME_ID_MAIN:
      {
        switch( frameResult )
        {
          // -----------------------------------------------------------------------------
          // 
          case App::SuperTraining::FRAME_RESULT_BACK:
            nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
          break;
        }
      }
      break;
    }

    return nextFrameID;
  }


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)
