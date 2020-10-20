// ============================================================================
/*
 * @file KisekaeApplicationSystem.h
 * @brief きせかえアプリのアプリケーションシステムです。
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( NETAPPSKELETON_APPLICATIONSYSTEM_H_INCLUDE )
#define NETAPPSKELETON_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

//  nijiのインクルード
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class AppUtilFileControlState;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class CharaModelViewer;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(System)

//  前方宣言
  class Work;
  class Builder;
  class ViewList;
  class ViewObjects;
  class ViewSystemAccessor;
  class CharaModelViewr;

  class ApplicationSystem : public NetAppLib::System::ApplicationSystemBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( ApplicationSystem );

  public:
    ApplicationSystem();
    virtual~ApplicationSystem();

    virtual void Build( App::Kisekae::System::Work* pWork, App::Kisekae::System::Builder* pApplicationSystemBuilder );

    //! @brief アプリケーションの終了処理を行います。
    //! @retval true 終了処理中
    //! @retval false 終了処理完了
    virtual bool End();

  private:
    virtual void LoadResourceRequest();
    virtual void CreateMessageData();
    virtual void CreateView();
    virtual void UpdateView();
    virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
    virtual bool IsExitView();

    //  カメラ初期化/終了
    virtual void SetupCamera();
    virtual void UnsetCamera();

    //! @brief ビューの作成が完了したか
    virtual bool IsCreateViewComplete();

  private:

    void _Clear()
    {
      m_pWork                 = NULL;
      m_pViewList             = NULL;
      m_pViewObjects          = NULL;
      m_pViewSystemAccessor   = NULL;
      m_pCharaModelViewer     = NULL;

      m_pUpperDrawEnvNode     = NULL;
      m_pUpperCamera          = NULL;
      m_pIconFileCtrlState    = NULL;

      m_endSeq  = 0;
    }

    App::Kisekae::System::Work*                   m_pWork;
    App::Kisekae::System::ViewList*               m_pViewList;
    App::Kisekae::System::ViewObjects*            m_pViewObjects;
    App::Kisekae::System::ViewSystemAccessor*     m_pViewSystemAccessor;
    app::tool::CharaModelViewer*                  m_pCharaModelViewer;

    gfl2::renderingengine::scenegraph::instance::DrawEnvNode*           m_pUpperDrawEnvNode;
    poke_3d::model::BaseCamera*                                         m_pUpperCamera;

    app::util::AppUtilFileControlState*         m_pIconFileCtrlState;

    u32 m_endSeq;
  };

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)


#endif // NETAPPSKELETON_APPLICATIONSYSTEM_H_INCLUDE
