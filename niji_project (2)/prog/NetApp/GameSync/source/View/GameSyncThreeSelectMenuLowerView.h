// ============================================================================
/*
 * @file GameSyncThreeSelectMenuLowerView.h
 * @brief ゲームシンクアプリの下画面を表示するクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNCTHREESELECTMENULOWERVIEW_H_INCLUDE )
#define GAMESYNCTHREESELECTMENULOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/UI/NetAppCursorView.h"
#include "NetApp/GameSync/source/System/GameSyncWork.h"
#include "NetApp/GameSync/source/View/GameSyncThreeSelectMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(View)


class GameSyncThreeSelectMenuLowerView : public NetAppLib::UI::NetAppCursorView
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncThreeSelectMenuLowerView );
public:
  GameSyncThreeSelectMenuLowerView( NetApp::GameSync::System::GameSyncWork* pGameSyncWork );
  virtual~GameSyncThreeSelectMenuLowerView();

  void SetListener( NetApp::GameSync::View::GameSyncThreeSelectMenuLowerViewListener* pGameSyncThreeSelectMenuLowerViewListener ) { m_pGameSyncThreeSelectMenuLowerViewListener = pGameSyncThreeSelectMenuLowerViewListener; }
  void RemoveListener() { m_pGameSyncThreeSelectMenuLowerViewListener = NULL; }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

  void SetMessage( u32 windowMessageID, u32 button1MessageID, bool bShowNewIcon1, u32 button2MessageID, bool bShowNewIcon2, u32 button3MessageID, bool bShowNewIcon3 );

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
    BUTTON_ID_BUTTON_02,
    BUTTON_MAX,
  };

  void setupButton();

private:

  NetApp::GameSync::System::GameSyncWork*                                m_pGameSyncWork;
  NetApp::GameSync::View::GameSyncThreeSelectMenuLowerViewListener*      m_pGameSyncThreeSelectMenuLowerViewListener;

  u32                                                                    m_WindowMessageID;
  u32                                                                    m_MenuMessageID1;
  u32                                                                    m_MenuMessageID2;
  u32                                                                    m_MenuMessageID3;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNCTHREESELECTMENULOWERVIEW_H_INCLUDE
