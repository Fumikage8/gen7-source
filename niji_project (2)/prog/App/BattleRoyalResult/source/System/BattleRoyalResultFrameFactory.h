// ============================================================================
/*
 * @file BattleRoyalResultFrameFactory.h
 * @brief バトルロイヤルの結果アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( BATTLE_ROYAL_RESULT_FRAMEFACTORY_H_INCLUDE )
#define BATTLE_ROYAL_RESULT_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "App/BattleRoyalResult/source/System/BattleRoyalResultWork.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(System)


class BattleRoyalResultFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleRoyalResultFrameFactory );
public:
  BattleRoyalResultFrameFactory( App::BattleRoyalResult::System::BattleRoyalResultWork* pBattleRoyalResultWork );

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  App::BattleRoyalResult::System::BattleRoyalResultWork*                m_pBattleRoyalResultWork;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)

#endif // BATTLE_ROYAL_RESULT_FRAMEFACTORY_H_INCLUDE
