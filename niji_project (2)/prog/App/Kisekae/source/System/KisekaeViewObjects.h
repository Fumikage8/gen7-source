// ============================================================================
/*
 * @file KisekaeViewObjects.h
 * @brief きせかえアプリのViewクラスを持っているクラスです。
 * @date  2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( KISEKAE_VIEWOBJECTS_H_INCLUDE )
#define KISEKAE_VIEWOBJECTS_H_INCLUDE
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

//  Viewのインクルード
#include "App/Kisekae/source/View/KisekaeLowerInfoView.h"
#include "App/Kisekae/source/View/KisekaeLowerItemSelectView.h"
#include "App/Kisekae/source/View/KisekaeUpperBGView.h"

//  NetStaticのインクルード
#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(System)

//  前方宣言
  class Work;

  class ViewObjects
  {
    GFL_FORBID_COPY_AND_ASSIGN( ViewObjects );
  public:
    ViewObjects();
    virtual~ViewObjects();

    bool InitFunc( App::Kisekae::System::Work* pWork, NetAppLib::UI::NetAppMessageMenu* pMessageMenu );
    bool EndFunc();

    void InputDisableAll();

    bool IsDrawing();

    App::Kisekae::View::UpperBGView*            GetUpperBGView()          { return m_pUpperBGView; }
    App::Kisekae::View::LowerItemSelectView*    GetLowerItemSelectView()  { return m_pLowerItemSelectView; }
    App::Kisekae::View::LowerInfoView*          GetLowerInfoView()        { return m_pLowerInfoView; }

    NetAppLib::UI::NetAppMessageMenu*           GetMessageMenu()          { return m_pMessageMenu; }

  private:

    void _Clear();

    NetAppLib::UI::NetAppMessageMenu*           m_pMessageMenu;

    App::Kisekae::View::UpperBGView*            m_pUpperBGView;
    App::Kisekae::View::LowerItemSelectView*    m_pLowerItemSelectView;
    App::Kisekae::View::LowerInfoView*          m_pLowerInfoView;

    u32 m_initSeq, m_endSeq;
  };

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)


#endif // KISEKAE_VIEWOBJECTS_H_INCLUDE
