// ============================================================================
/*
 * @file StaffrollStaffrollViewObjects.h
 * @brief スタッフロールアプリのViewクラスを持っているクラスです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( STAFFROLL_VIEW_OBJECTS_H_INCLUDE )
#define STAFFROLL_VIEW_OBJECTS_H_INCLUDE
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

//  Viewのインクルード
#include "App/Staffroll/source/System/StaffrollViewList.h"
#include "App/Staffroll/source/View/StaffrollTextScrollView.h"
#include "App/Staffroll/source/View/StaffrollImgView.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(System)

//  前方宣言
  class StaffrollWork;

  //  Viewオブジェクト管理
  class StaffrollViewObjects
  {
    GFL_FORBID_COPY_AND_ASSIGN( StaffrollViewObjects );
  public:
    StaffrollViewObjects();
    virtual~StaffrollViewObjects();

    bool InitFunc( App::Staffroll::System::StaffrollWork* pWork );
    bool EndFunc();

    void InputDisableAll();

    bool IsDrawing();

    //  上のスクロールテキストview
    App::Staffroll::View::StaffrollTextScrollView*  GetTextScrollUpperView() const { return m_pUpperTextScrollView; }

    //  一枚絵View
    App::Staffroll::View::StaffrollImgView*     GetImgUpperView() const { return m_pUpperImgView; }

  private:

    void _Clear();

    App::Staffroll::View::StaffrollTextScrollView*  m_pUpperTextScrollView;
    App::Staffroll::View::StaffrollImgView*         m_pUpperImgView;

    u32 m_initSeq, m_endSeq;
  };

  //  Viewデータアクセサー
  class StaffrollViewSystemAccessor
  {
    GFL_FORBID_COPY_AND_ASSIGN( StaffrollViewSystemAccessor );

  public:

    StaffrollViewSystemAccessor(
      App::Staffroll::System::StaffrollViewList* pViewList,
      App::Staffroll::System::StaffrollViewObjects* pViewObjects );

    virtual~StaffrollViewSystemAccessor();

    App::Staffroll::System::StaffrollViewList*    GetViewList() { return m_pViewList; }
    App::Staffroll::System::StaffrollViewObjects*  GetViewObjects() { return m_pViewObjects; }

  private:
    void _Clear()
    {
      m_pViewList     = NULL;
      m_pViewObjects  = NULL;
    }

    App::Staffroll::System::StaffrollViewList*     m_pViewList;
    App::Staffroll::System::StaffrollViewObjects*  m_pViewObjects;
  };

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)


#endif // STAFFROLL_VIEW_OBJECTS_H_INCLUDE
