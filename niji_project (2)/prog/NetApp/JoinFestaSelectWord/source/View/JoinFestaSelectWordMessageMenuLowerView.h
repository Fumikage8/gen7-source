// ============================================================================
/*
 * @file JoinFestaSelectWordMessageMenuLowerView.h
 * @brief メッセージメニュー表示
 * @date 2015.10.14
 * @author endo_akira
 */
// ============================================================================
#if !defined( JoinFestaSelectWordMessageMenuLowerView_H_INCLUDE )
#define JoinFestaSelectWordMessageMenuLowerView_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordMessageMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaSelectWordMessageMenuLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public NetAppLib::UI::NetAppMessageMenu::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordMessageMenuLowerView );

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

  JoinFestaSelectWordMessageMenuLowerView(
    NetAppLib::System::ApplicationWorkBase* pWorkBase,
    NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu );
  virtual~JoinFestaSelectWordMessageMenuLowerView();

  void SetListener( NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordMessageMenuLowerViewListener* pJoinFestaSelectWordMessageMenuLowerViewListener ) { m_pJoinFestaSelectWordMessageMenuLowerViewListener = pJoinFestaSelectWordMessageMenuLowerViewListener; }
  void RemoveListener() { m_pJoinFestaSelectWordMessageMenuLowerViewListener = NULL; }

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // NetAppMessageMenu::IEventHandler
  virtual void MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode );

  void SetStreamMessageBlackFilter( u32 messageID, bool isShowTimerIcon = false );
  void SetStreamMessageBlackFilter( u32 windowMessageID, u32 selectMessageID, bool isShowTimerIcon );

private:

  void showMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG );

private:

  NetAppLib::System::ApplicationWorkBase*                                                    m_pWorkBase;
  NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordMessageMenuLowerViewListener*        m_pJoinFestaSelectWordMessageMenuLowerViewListener;
  NetAppLib::UI::NetAppMessageMenu&                                                          m_NetAppMessageMenu;
  gfl2::str::StrBuf                                                                          m_WindowMessage;
  gfl2::str::StrBuf                                                                          m_SelectMessage;
  u32                                                                                        m_MessageID;


};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JoinFestaSelectWordMessageMenuLowerView_H_INCLUDE
