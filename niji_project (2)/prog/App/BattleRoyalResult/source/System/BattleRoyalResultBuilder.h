// ============================================================================
/*
 * @file BattleRoyalResultBuilder.h
 * @brief バトルロイヤルの結果アプリを構築するクラスです。
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( BATTLE_ROYAL_RESULT_BUILDER_H_INCLUDE )
#define BATTLE_ROYAL_RESULT_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "App/BattleRoyalResult/source/System/BattleRoyalResultWork.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(System)


class BattleRoyalResultBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleRoyalResultBuilder );
public:
  BattleRoyalResultBuilder( App::BattleRoyalResult::System::BattleRoyalResultWork* pBattleRoyalResultWork );
  virtual~BattleRoyalResultBuilder();

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();

  virtual bool IsCreateMessageUtility(){ return true; }

  //  レンダー環境データを作成返す
  virtual app::util::AppRenderingManager::Description* CreateAppRenderingManagerDescriptions();

  virtual bool IsCreateAppToolTrainerIconRendering(){ return true; }
  virtual u32  GetTrainerIconNum(){ return 4; }
  virtual app::util::G2DUtil::SETUP_DISPLAY_TYPE GetTrainerIconDrawDisplayType(){ return app::util::G2DUtil::SETUP_UPPER; }

private:
  enum
  {
    RENDER_DESCRIPTION_NORMAL = 0,
    RENDER_DESCRIPTION_MAX
  };

  App::BattleRoyalResult::System::BattleRoyalResultWork*                m_pWork;
};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)

#endif // MIRACLETRADEBUILDER_H_INCLUDE
