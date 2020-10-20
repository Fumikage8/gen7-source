// ============================================================================
/*
 * @file EndRollFrameSwitcher.cpp
 * @brief エンドロールアプリのフレームを切り替えるクラスです。
 * @date 2016.03.16
 * @author uchida_yuto
 */
// ============================================================================
#include "App/EndRoll/source/System/EndRollFrameSwitcher.h"
#include "App/EndRoll/source/EndRollFrameID.h"
#include "App/EndRoll/source/EndRollFrameResult.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID EndRollFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case App::EndRoll::APPENDROLL_FRAME_ID_NORMAL:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case App::EndRoll::FRAME_RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;
  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)
