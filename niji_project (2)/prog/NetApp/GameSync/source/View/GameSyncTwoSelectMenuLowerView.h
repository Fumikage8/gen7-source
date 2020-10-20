// ============================================================================
/*
 * @file GameSyncTwoSelectMenuLowerView.h
 * @brief ゲームシンクアプリの下画面を表示するクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNCTWOSELECTMENULOWERVIEW_H_INCLUDE )
#define GAMESYNCTWOSELECTMENULOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/UI/NetAppCursorView.h"
#include "NetApp/GameSync/source/System/GameSyncWork.h"
#include "NetApp/GameSync/source/View/GameSyncTwoSelectMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(View)


class GameSyncTwoSelectMenuLowerView : public NetAppLib::UI::NetAppCursorView
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncTwoSelectMenuLowerView );
public:
  GameSyncTwoSelectMenuLowerView( NetApp::GameSync::System::GameSyncWork* pGameSyncWork );
  virtual~GameSyncTwoSelectMenuLowerView();

  void SetListener( NetApp::GameSync::View::GameSyncTwoSelectMenuLowerViewListener* pGameSyncTwoSelectMenuLowerViewListener ) { m_pGameSyncTwoSelectMenuLowerViewListener = pGameSyncTwoSelectMenuLowerViewListener; }
  void RemoveListener() { m_pGameSyncTwoSelectMenuLowerViewListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

  void SetMessage( u32 windowMessageID, u32 button1MessageID, bool bShowNewIcon1, u32 button2MessageID, bool bShowNewIcon2 );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_GAMESYNC,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  enum ButtonID
  {
    BUTTON_ID_BACK_BUTTON,
    BUTTON_ID_BUTTON_00,
    BUTTON_ID_BUTTON_01,
    BUTTON_MAX,
  };

  void setupButton();

private:

  NetApp::GameSync::System::GameSyncWork*                                m_pGameSyncWork;
  NetApp::GameSync::View::GameSyncTwoSelectMenuLowerViewListener*        m_pGameSyncTwoSelectMenuLowerViewListener;

  u32                                                                    m_WindowMessageID;
  u32                                                                    m_MenuMessageID1;
  u32                                                                    m_MenuMessageID2;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNCTWOSELECTMENULOWERVIEW_H_INCLUDE
