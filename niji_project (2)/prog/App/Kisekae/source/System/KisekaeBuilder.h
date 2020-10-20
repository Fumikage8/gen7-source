// ============================================================================
/*
 * @file KisekaeBuilder.h
 * @brief きせかえアプリのビルダー用
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( KISEKAE_BUILDER_H_INCLUDE )
#define KISEKAE_BUILDER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"
#include "App/Kisekae/source/System/KisekaeWork.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(System)

class Builder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( Builder );

public:
  Builder( App::Kisekae::System::Work* pWork );
  virtual~Builder();

  virtual NetAppLib::System::IApplicationFrameSwitcher* CreateApplicationFrameSwitcher();
  virtual NetAppLib::System::IApplicationFrameFactory* CreateApplicationFrameFactory();

  virtual NetAppLib::System::ApplicationWorkBase* GetApplicationWork();
  virtual NetAppLib::System::FrameID GetApplicationStartFrameID();
  virtual u32 GetApplicationResourceCount();

  //  描画レンダーの設定
  virtual u32 CreateAppRenderingManagerCount();
  app::util::AppRenderingManager::Description* CreateAppRenderingManagerDescriptions();

  //  メッセージのユーティティー作成
  //  NetAppLib::System::ApplicationSystemBase::m_pMessageUtility のインスタンス作成している
  virtual bool IsCreateMessageUtility() { return true; }

  //  システムメッセージを表示してからの選択表示を作成
  virtual bool IsCreateNetAppMessageMenu(){ return true; }

  // @fix GFNMCat[2022] 
  //  ペインリストとそれ以外のボタンを同時に検知できないようにするために必要
  // UIViewマネージャのボタン監視モード
  virtual bool IsEnableInputExclusiveKeyMode(){ return true; }

private:
  enum
  {
    RENDER_DESCRIPTION_NORMAL = 0,
    RENDER_DESCRIPTION_MAX
  };

  App::Kisekae::System::Work*                m_pWork;
};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)

#endif // MIRACLETRADEBUILDER_H_INCLUDE
