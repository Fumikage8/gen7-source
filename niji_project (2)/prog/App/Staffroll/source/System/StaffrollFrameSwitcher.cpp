// ============================================================================
/*
 * @file StaffrollFrameSwitcher.cpp
 * @brief スタッフロールアプリのフレームを切り替えるクラスです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Staffroll/source/System/StaffrollFrameSwitcher.h"
#include "App/Staffroll/source/StaffrollFrameID.h"
#include "App/Staffroll/source/StaffrollFrameResult.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID StaffrollFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case App::Staffroll::STAFFROLL_FRAME_ID_EVENT:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case App::Staffroll::RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;
  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)
