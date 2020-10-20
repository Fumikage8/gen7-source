// ============================================================================
/*
 * @file BattleTreeBuilder.h
 * @brief バトルツリーアプリを構築するクラスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#if !defined( BATTLE_TREE_BUILDER_H_INCLUDE )
#define BATTLE_TREE_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "App/BattleTree/source/System/BattleTreeWork.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(System)


  class BattleTreeBuilder : public NetAppLib::System::IApplicationSystemBuilder
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleTreeBuilder );
  public:
    BattleTreeBuilder( App::BattleTree::System::BattleTreeWork* pBattleTreeWork );
    virtual~BattleTreeBuilder();

    virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
    virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();

    virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
    virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
    virtual u32 GetApplicationResourceCount();

    //  レンダー環境設定
    virtual app::util::AppRenderingManager::Description* CreateAppRenderingManagerDescription();

    //  メッセージのユーティティー作成
    //  NetAppLib::System::ApplicationSystemBase::m_pMessageUtility のインスタンス作成している
    virtual bool IsCreateMessageUtility() { return true; }

    //  トレーナーアイコンを使う
    virtual bool IsCreateAppToolTrainerIconRendering(){ return true; }
    //  トレーナーNPCのみの場合なのでtrue
    virtual bool IsCreateAppToolTrainerIconRenderingByNPCOnly() const { return true; }

    //  トレーナーアイコンを使う個数
    virtual u32  GetTrainerIconNum(){ return BattleTreeWork::TRAINER_LOAD_ICON_MAX; }

    // UIViewマネージャのボタン監視モード
    virtual bool IsEnableInputExclusiveKeyMode(){ return true; }

  private:
    App::BattleTree::System::BattleTreeWork*                m_pBattleTreeWork;

  };


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)

#endif // MIRACLETRADEBUILDER_H_INCLUDE
