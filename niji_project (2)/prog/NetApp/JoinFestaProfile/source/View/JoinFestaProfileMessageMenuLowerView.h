// ============================================================================
/*
 * @file JoinFestaProfileMessageMenuLowerView.h
 * @brief メッセージメニュー表示
 * @date 2015.11.04
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILEMESSAGEMENULOWERVIEW_H_INCLUDE )
#define JOINFESTAPROFILEMESSAGEMENULOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileWork.h"
#include "NetApp/JoinFestaProfile/source/View/JoinFestaProfileMessageMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaProfileMessageMenuLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public NetAppLib::UI::NetAppMessageMenu::IEventHandler,
  public NetAppLib::UI::NetAppMessageMenu::IAddItemHandler

{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaProfileMessageMenuLowerView );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTAPROFILE,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  enum
  {
    MENU_MAX = 3
  };

public:

  JoinFestaProfileMessageMenuLowerView(
    NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork,
    NetAppLib::UI::NetAppMessageMenu&                       netAppMessageMenu );
  virtual~JoinFestaProfileMessageMenuLowerView();

  void SetListener( NetApp::JoinFestaProfile::View::JoinFestaProfileMessageMenuLowerViewListener* pJoinFestaProfileMessageMenuLowerViewListener ) { m_pJoinFestaProfileMessageMenuLowerViewListener = pJoinFestaProfileMessageMenuLowerViewListener; }
  void RemoveListener() { m_pJoinFestaProfileMessageMenuLowerViewListener = NULL; }

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // NetAppMessageMenu::IEventHandler
  virtual void MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode );
  virtual void MessageMenuView_OnAddItem( app::tool::MenuWindow* pMenuWindow );

  void SetStreamMessageBlackFilter( u32 messageID, bool isShowTimerIcon = false );
  void SetMessageMenuBlackFilter( u32 messageID, u32 menuID1, u32 menuID2, u32 menuID3 );

private:

  void showMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG );

private:

  NetApp::JoinFestaProfile::System::JoinFestaProfileWork*                          m_pJoinFestaProfileWork;
  NetApp::JoinFestaProfile::View::JoinFestaProfileMessageMenuLowerViewListener*    m_pJoinFestaProfileMessageMenuLowerViewListener;
  NetAppLib::UI::NetAppMessageMenu&                                                m_NetAppMessageMenu;
  gfl2::str::StrBuf                                                                m_WindowMessage;
  u32                                                                              m_MessageID;
  u32                                                                              m_MenuIDs[ MENU_MAX ];


};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILEMESSAGEMENULOWERVIEW_H_INCLUDE
