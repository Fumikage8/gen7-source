// ============================================================================
/*
 * @file JoinFestaPlayerListMessageMenuLowerView.h
 * @brief メッセージメニュー表示
 * @date 2015.10.14
 * @author endo_akira
 */
// ============================================================================
#if !defined( JoinFestaPlayerListMessageMenuLowerView_H_INCLUDE )
#define JoinFestaPlayerListMessageMenuLowerView_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListMessageMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaPlayerListMessageMenuLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public NetAppLib::UI::NetAppMessageMenu::IEventHandler,
  public NetAppLib::UI::NetAppMessageMenu::IAddItemHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListMessageMenuLowerView );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST,
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

  JoinFestaPlayerListMessageMenuLowerView(
    NetAppLib::System::ApplicationWorkBase* pWorkBase,
    NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu );
  virtual~JoinFestaPlayerListMessageMenuLowerView();

  void SetListener( NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener* pJoinFestaPlayerListMessageMenuLowerViewListener ) { m_pJoinFestaPlayerListMessageMenuLowerViewListener = pJoinFestaPlayerListMessageMenuLowerViewListener; }
  void RemoveListener() { m_pJoinFestaPlayerListMessageMenuLowerViewListener = NULL; }

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // NetAppMessageMenu::IEventHandler
  virtual void MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode );
  virtual void MessageMenuView_OnAddItem( app::tool::MenuWindow* pMenuWindow );

  void SetMessage( u32 messageID, bool isShowTimerIcon = true );
  void SetMessageBlackFilter( u32 messageID, bool isShowTimerIcon = true );
  void SetStreamMessage( u32 messageID, bool isShowTimerIcon = false );
  void SetStreamMessage( u32 messageID, gfl2::str::StrBuf& strbuf, bool isShowTimerIcon = false );
  void SetStreamMessageBlackFilter( u32 messageID, bool isShowTimerIcon = false );
  void SetStreamMessageBlackFilter( u32 messageID, const gfl2::str::STRCODE* pPlayerName, bool isShowTimerIcon = false );
  void SetMessageYesNoBlackFilter( u32 messageID );
  void SetMessageYesNoBlackFilter( u32 messageID, const gfl2::str::STRCODE* pPlayerName );
  void SetStreamMessageYesNoBlackFilter( u32 messageID );
  void SetStreamMessageYesNoBlackFilter( u32 messageID, const gfl2::str::STRCODE* pPlayerName );
  void SetMessageMenuBlackFilter( u32 messageID, u32 menuID1, u32 menuID2 );
  void SetMessageMenuBlackFilter( u32 messageID, const gfl2::str::STRCODE* pPlayerName, u32 menuID1, u32 menuID2 );
  void SetMessageMenuBlackFilter( u32 messageID, u32 menuID1, u32 menuID2, u32 menuID3 );
  void SetMessageMenuBlackFilter( u32 messageID, const gfl2::str::STRCODE* pPlayerName, u32 menuID1, u32 menuID2, u32 menuID3 );

private:

  void showMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG );
  void showMessage( const gfl2::str::STRCODE* pPlayerName, u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG );
  void showMessage( u32 messageID, gfl2::str::StrBuf& strbuf, bool bImmediate, bool isShowTimerIcon, bool isShowBG );
  void expandPlayerName( const gfl2::str::STRCODE* pPlayerName, u32 messageID );

private:

  NetAppLib::System::ApplicationWorkBase*                                         m_pWorkBase;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListMessageMenuLowerViewListener*        m_pJoinFestaPlayerListMessageMenuLowerViewListener;
  NetAppLib::UI::NetAppMessageMenu&                                               m_NetAppMessageMenu;
  u32                                                                             m_MessageID;
  u32                                                                             m_MenuIDs[ MENU_MAX ];
  u32                                                                             m_MenuNum;
  gfl2::str::StrBuf                                                               m_PlayerName;
  gfl2::str::StrBuf                                                               m_WindowMessage;
  gfl2::str::StrBuf                                                               m_ListMessage0;
  gfl2::str::StrBuf                                                               m_ListMessage1;
  gfl2::str::StrBuf                                                               m_ListMessage2;


};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JoinFestaPlayerListMessageMenuLowerView_H_INCLUDE
