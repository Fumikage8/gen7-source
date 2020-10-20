// ============================================================================
/*
 * @file JoinFestaGlobalAttractionDetailLowerView.h
 * @brief グローバルアトラクションアプリの下画面を表示するクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTIONDETAILLOWERVIEW_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTIONDETAILLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionWork.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionDetailLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaGlobalAttractionDetailLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaGlobalAttractionDetailLowerView );
public:
  JoinFestaGlobalAttractionDetailLowerView( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork );
  virtual~JoinFestaGlobalAttractionDetailLowerView();

  void SetListener( NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionDetailLowerViewListener* pJoinFestaGlobalAttractionDetailLowerViewListener ) { m_pJoinFestaGlobalAttractionDetailLowerViewListener = pJoinFestaGlobalAttractionDetailLowerViewListener; }
  void RemoveListener() { m_pJoinFestaGlobalAttractionDetailLowerViewListener = NULL; }

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );


  void SetGlobalAttractionInfo( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pGlobalAttractionInfo );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTAGLOBALATTRACTION,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  enum ButtonID
  {
    BUTTON_ID_BACK_BUTTON,
    BUTTON_MAX
  };

  void setupButton();

private:

  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork*                         m_pJoinFestaGlobalAttractionWork;
  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionDetailLowerViewListener*        m_pJoinFestaGlobalAttractionDetailLowerViewListener;


};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAGLOBALATTRACTIONDETAILLOWERVIEW_H_INCLUDE
