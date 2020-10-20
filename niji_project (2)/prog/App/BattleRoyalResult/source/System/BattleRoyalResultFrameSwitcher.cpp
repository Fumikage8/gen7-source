// ============================================================================
/*
 * @file BattleRoyalResultFrameSwitcher.cpp
 * @brief バトルロイヤルの結果アプリのフレームを切り替えるクラスです。
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#include "App/BattleRoyalResult/source/System/BattleRoyalResultFrameSwitcher.h"
#include "App/BattleRoyalResult/source/BattleRoyalResultFrameID.h"
#include "App/BattleRoyalResult/source/BattleRoyalResultFrameResult.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(System)


NetAppLib::System::FrameID BattleRoyalResultFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
{
  NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

  switch( executeFrameID )
  {
    // =================================================================================
    case App::BattleRoyalResult::BATTLE_ROYAL_RESULT_FRAME_ID_EVENT:
    {
      switch( frameResult )
      {
        // -----------------------------------------------------------------------------
        // 
        case App::BattleRoyalResult::RESULT_BACK:
          nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
        break;
      }
    }
    break;
  }

  return nextFrameID;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)
