// ============================================================================
/*
 * @file BattleTreeFrameFactory.h
 * @brief バトルツリーアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_TREE_FRAMEFACTORY_H_INCLUDE )
#define BATTLE_TREE_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "App/BattleTree/source/System/BattleTreeWork.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(System)


  class BattleTreeFrameFactory : public NetAppLib::System::IApplicationFrameFactory
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleTreeFrameFactory );
  public:
    BattleTreeFrameFactory( App::BattleTree::System::BattleTreeWork* pBattleTreeWork );

    virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

  private:
    App::BattleTree::System::BattleTreeWork*                m_pBattleTreeWork;

  };


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)

#endif // BATTLE_TREE_FRAMEFACTORY_H_INCLUDE
