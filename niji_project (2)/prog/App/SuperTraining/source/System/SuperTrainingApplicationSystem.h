// ============================================================================
/*
 * @file SuperTrainingApplicationSystem.h
 * @brief すごい特訓アプリのアプリケーションシステムです。
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( SUPER_TRAINIG_APPLICATIONSYSTEM_H_INCLUDE )
#define SUPER_TRAINIG_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(System)

  //  前方宣言
  class SuperTrainingWork;
  class SuperTrainingBuilder;
  class ViewList;
  class ViewObjects;
  class ViewSystemAccessor;
  class SuperTrainingApplicationSystem : public NetAppLib::System::ApplicationSystemBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( SuperTrainingApplicationSystem );

  public:
    SuperTrainingApplicationSystem();
    virtual~SuperTrainingApplicationSystem();

    void Build( App::SuperTraining::System::SuperTrainingWork* pSuperTrainingWork, App::SuperTraining::System::SuperTrainingBuilder* pApplicationSystemBuilder );


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

    //! @brief ビューの作成が完了したか
    virtual bool IsCreateViewComplete();

  private:

    void _Clear()
    {
      m_pWork                 = NULL;
      m_pViewList             = NULL;
      m_pViewObjects          = NULL;
      m_pViewSystemAccessor   = NULL;

      m_endSeq  = 0;
    }

    App::SuperTraining::System::SuperTrainingWork*      m_pWork;
    App::SuperTraining::System::ViewList*               m_pViewList;
    App::SuperTraining::System::ViewObjects*            m_pViewObjects;
    App::SuperTraining::System::ViewSystemAccessor*     m_pViewSystemAccessor;

    u32 m_endSeq;
  };


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)


#endif // SUPER_TRAINIG_APPLICATIONSYSTEM_H_INCLUDE
