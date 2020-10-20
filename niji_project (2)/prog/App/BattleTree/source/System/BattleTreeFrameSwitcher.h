// ============================================================================
/*
 * @file BattleTreeFrameSwitcher.h
 * @brief バトルツリーアプリのフレームを切り替えるクラスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_TREE_FRAMESWITCHER_H_INCLUDE )
#define BATTLE_TREE_FRAMESWITCHER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameSwitcher.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(System)


  class BattleTreeFrameSwitcher: public NetAppLib::System::IApplicationFrameSwitcher
  {
  public:
    virtual NetAppLib::System::FrameID GetNextFrameID( NetAppLib::System::FrameID executeFrameID, NetAppLib::System::FrameResult frameResult );
  };


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)

#endif // BATTLE_TREE_FRAMESWITCHER_H_INCLUDE
