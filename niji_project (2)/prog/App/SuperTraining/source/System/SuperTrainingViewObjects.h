// ============================================================================
/*
 * @file SuperTrainingViewObjects.h
 * @brief すごい特訓アプリのViewクラスを持っているクラスです。
 * @date 2015.12.07
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( SUPER_TRAINING_VIEWOBJECTS_H_INCLUDE )
#define SUPER_TRAINING_VIEWOBJECTS_H_INCLUDE
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

//  Viewのインクルード
#include "App/SuperTraining/source/View/SuperTrainingLowerItemListView.h"
#include <App/StatusUpper/include/AppToolStatusUpperDraw.h>

//  NetStaticのインクルード
#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class AppCommonGrpIconData;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(System)

//  前方宣言
  class SuperTrainingWork;

  class ViewObjects
  {
    GFL_FORBID_COPY_AND_ASSIGN( ViewObjects );
  public:
    ViewObjects();
    virtual~ViewObjects();

    bool InitFunc( App::SuperTraining::System::SuperTrainingWork* pWork, NetAppLib::UI::NetAppMessageMenu* pMessageMenu );
    bool EndFunc();

    void InputDisableAll();

    bool IsDrawing();

    App::SuperTraining::View::LowerItemListView*      GetLowerItemListView()    { return m_pLowerItemListView; }
    App::Tool::FieldStatusUpperDraw*                  GetUpperStatusView()      { return m_pUpperStatusView; }
    NetAppLib::UI::NetAppMessageMenu*                 GetMessageMenu()          { return m_pMessageMenu; }

  private:

    void _Clear();

    App::SuperTraining::View::LowerItemListView*             m_pLowerItemListView;
    App::Tool::FieldStatusUpperDraw*                         m_pUpperStatusView;

    NetAppLib::UI::NetAppMessageMenu*                        m_pMessageMenu;

    app::tool::AppCommonGrpIconData*                         m_pAppCommonGrpIconData;

    u32 m_initSeq, m_endSeq;
  };

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)


#endif // SUPER_TRAINING_VIEWOBJECTS_H_INCLUDE
