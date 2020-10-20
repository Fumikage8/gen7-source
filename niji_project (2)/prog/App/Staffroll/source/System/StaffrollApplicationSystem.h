// ============================================================================
/*
 * @file StaffrollApplicationSystem.h
 * @brief スタッフロールアプリのアプリケーションシステムです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( STAFFROLL_APPLICATIONSYSTEM_H_INCLUDE )
#define STAFFROLL_APPLICATIONSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

//  nijiのインクルード
#include "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "GameSys/include/GameManager.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class AppUtilFileControlState;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(System)

  //  前方宣言
  class StaffrollWork;
  class StaffrollBuilder;
  class StaffrollViewList;
  class StaffrollViewObjects;
  class StaffrollViewSystemAccessor;

  class StaffrollApplicationSystem : public NetAppLib::System::ApplicationSystemBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( StaffrollApplicationSystem );

  public:
    StaffrollApplicationSystem();
    virtual~StaffrollApplicationSystem();

    void Build( App::Staffroll::System::StaffrollWork* pStaffrollWork, App::Staffroll::System::StaffrollBuilder* pApplicationSystemBuilder );

    virtual bool End();

  private:
    virtual void LoadResourceRequest();
    virtual void CreateMessageData();
    virtual void CreateView();
    virtual void UpdateView();
    virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
    virtual bool IsExitView();
    virtual bool IsCreateViewComplete();

  private:

    void _Clear()
    {
      m_pWork = NULL;
      m_pViewList = NULL;
      m_pViewObjects = NULL;
      m_pViewSystemAccessor = NULL;
//      m_pImgFileCtrlState = NULL;

      m_initSeq = m_endSeq  = 0;
    }

    App::Staffroll::System::StaffrollWork*                   m_pWork;
    App::Staffroll::System::StaffrollViewList*               m_pViewList;
    App::Staffroll::System::StaffrollViewObjects*            m_pViewObjects;
    App::Staffroll::System::StaffrollViewSystemAccessor*     m_pViewSystemAccessor;

    //  変更前の画面全体のクリア設定
    GameSys::GameManager::BUFFER_CLEAR_SETTING  m_aOldClearSetting[ gfl2::gfx::CtrDisplayNo::NumberOf ];

    u32 m_initSeq, m_endSeq;

//   app::util::AppUtilFileControlState*         m_pImgFileCtrlState;

#if PM_DEBUG
    u32 m_debugHeapSize;
    u32 m_debugDevHeapSize;
#endif

  };

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)


#endif // STAFFROLL_APPLICATIONSYSTEM_H_INCLUDE
