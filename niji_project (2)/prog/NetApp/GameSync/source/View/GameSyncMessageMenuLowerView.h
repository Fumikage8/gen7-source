// ============================================================================
/*
 * @file GameSyncMessageMenuLowerView.h
 * @brief ゲームシンクアプリの下画面を表示するクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNCMESSAGEMENULOWERVIEW_H_INCLUDE )
#define GAMESYNCMESSAGEMENULOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"
#include "NetApp/GameSync/source/System/GameSyncWork.h"
#include "NetApp/GameSync/source/View/GameSyncMessageMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(View)


class GameSyncMessageMenuLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public NetAppLib::UI::NetAppMessageMenu::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncMessageMenuLowerView );
public:
  GameSyncMessageMenuLowerView(
    NetApp::GameSync::System::GameSyncWork* pGameSyncWork,
    NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu );
  virtual~GameSyncMessageMenuLowerView();

  void SetListener( NetApp::GameSync::View::GameSyncMessageMenuLowerViewListener* pGameSyncMessageMenuLowerViewListener ) { m_pGameSyncMessageMenuLowerViewListener = pGameSyncMessageMenuLowerViewListener; }
  void RemoveListener() { m_pGameSyncMessageMenuLowerViewListener = NULL; }

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // NetAppMessageMenu::IEventHandler
  virtual void MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode );

  void SetMessageBlackFilter( u32 messageID, bool isShowTimerIcon = true );
  void SetStreamMessage( u32 messageID, bool isShowTimerIcon = false );
  void SetStreamMessageBlackFilter( u32 messageID, bool isShowTimerIcon = false );
  void SetStreamMessageYesNoBlackFilter( u32 messageID );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_GAMESYNC,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  void showMessage( u32 messageID, bool bImmediate, bool isShowTimerIcon, bool isShowBG );

private:

  NetApp::GameSync::System::GameSyncWork*                                m_pGameSyncWork;
  NetApp::GameSync::View::GameSyncMessageMenuLowerViewListener*          m_pGameSyncMessageMenuLowerViewListener;
  NetAppLib::UI::NetAppMessageMenu&                                      m_NetAppMessageMenu;
  u32                                                                    m_MessageID;


};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNCMESSAGEMENULOWERVIEW_H_INCLUDE
