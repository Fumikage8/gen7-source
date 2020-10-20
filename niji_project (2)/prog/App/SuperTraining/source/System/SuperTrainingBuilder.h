// ============================================================================
/*
 * @file SuperTrainingBuilder.h
 * @brief すごい特訓アプリを構築するクラスです。
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( SUPER_TRAINIG_BUILDER_H_INCLUDE )
#define SUPER_TRAINIG_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "App/SuperTraining/source/System/SuperTrainingWork.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(System)


  class SuperTrainingBuilder : public NetAppLib::System::IApplicationSystemBuilder
  {
    GFL_FORBID_COPY_AND_ASSIGN( SuperTrainingBuilder );
  public:
    SuperTrainingBuilder( App::SuperTraining::System::SuperTrainingWork* pSuperTrainingWork );
    virtual~SuperTrainingBuilder();

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

    //  システムメッセージを表示してからの選択表示を作成
    virtual bool IsCreateNetAppMessageMenu(){ return true; }

  private:
    App::SuperTraining::System::SuperTrainingWork*                m_pSuperTrainingWork;

  };


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)

#endif // MIRACLETRADEBUILDER_H_INCLUDE
