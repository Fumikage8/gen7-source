// ============================================================================
/*
 * @file BattleTreeFrameSwitcher.cpp
 * @brief バトルツリーアプリのフレームを切り替えるクラスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#include "App/BattleTree/source/System/BattleTreeFrameSwitcher.h"
#include "App/BattleTree/source/BattleTreeFrameID.h"
#include "App/BattleTree/source/BattleTreeFrameResult.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(System)


  NetAppLib::System::FrameID BattleTreeFrameSwitcher::GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult )
  {
    NetAppLib::System::FrameID nextFrameID = NetAppLib::System::FRAME_ID_INVALID;

    switch( executeFrameID )
    {
      // =================================================================================
      case App::BattleTree::BATTLE_TREE_FRAME_ID:
      {
        switch( frameResult )
        {
          // -----------------------------------------------------------------------------
          // 
          case App::BattleTree::FRAME_RESULT_BACK:
          {
            nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
            break;
          }
          case App::BattleTree::FRAME_RESULT_SELECT:
          {
            nextFrameID = NetAppLib::System::FRAME_ID_INVALID;
            break;
          }
        }
      }
      break;
    }

    return nextFrameID;
  }


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)
