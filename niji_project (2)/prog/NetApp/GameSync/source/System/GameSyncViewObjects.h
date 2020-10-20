// ============================================================================
/*
 * @file GameSyncViewObjects.h
 * @brief ゲームシンクアプリのViewクラスを持っているクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNC_VIEWOBJECTS_H_INCLUDE )
#define GAMESYNC_VIEWOBJECTS_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)
class NetAppMessageMenu;
GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)

GFL_NAMESPACE_BEGIN(View)
class GameSyncBGUpperView;
class GameSyncUppCaptureUpperView;
class GameSyncLowCaptureUpperView;
class GameSyncMainUpperView;

class GameSyncBGLowerView;
class GameSyncTwoSelectMenuLowerView;
class GameSyncThreeSelectMenuLowerView;
class GameSyncMessageMenuLowerView;
GFL_NAMESPACE_END(View)


GFL_NAMESPACE_BEGIN(System)

class GameSyncWork;

class GameSyncViewObjects
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncViewObjects );
public:
  GameSyncViewObjects();
  virtual~GameSyncViewObjects();

  void CreateView( NetApp::GameSync::System::GameSyncWork* pGameSyncWork, NetAppLib::UI::NetAppMessageMenu& netAppMessageMenu );

  void InputDisableAll();

  bool IsDrawing();

  NetApp::GameSync::View::GameSyncBGUpperView*                 GetGameSyncBGUpperView(){ return m_pGameSyncBGUpperView; }
  NetApp::GameSync::View::GameSyncUppCaptureUpperView*         GetGameSyncUppCaptureUpperView(){ return m_pGameSyncUppCaptureUpperView; }
  NetApp::GameSync::View::GameSyncLowCaptureUpperView*         GetGameSyncLowCaptureUpperView(){ return m_pGameSyncLowCaptureUpperView; }
  NetApp::GameSync::View::GameSyncMainUpperView*               GetGameSyncMainUpperView(){ return m_pGameSyncMainUpperView; }

  NetApp::GameSync::View::GameSyncBGLowerView*                 GetGameSyncBGLowerView(){ return m_pGameSyncBGLowerView; }
  NetApp::GameSync::View::GameSyncTwoSelectMenuLowerView*      GetGameSyncTwoSelectMenuLowerView(){ return m_pGameSyncTwoSelectMenuLowerView; }
  NetApp::GameSync::View::GameSyncThreeSelectMenuLowerView*    GetGameSyncThreeSelectMenuLowerView(){ return m_pGameSyncThreeSelectMenuLowerView; }
  NetApp::GameSync::View::GameSyncMessageMenuLowerView*        GetGameSyncMessageMenuLowerView(){ return m_pGameSyncMessageMenuLowerView; }

private:

  NetApp::GameSync::View::GameSyncBGUpperView*                 m_pGameSyncBGUpperView;
  NetApp::GameSync::View::GameSyncUppCaptureUpperView*         m_pGameSyncUppCaptureUpperView;
  NetApp::GameSync::View::GameSyncLowCaptureUpperView*         m_pGameSyncLowCaptureUpperView;
  NetApp::GameSync::View::GameSyncMainUpperView*               m_pGameSyncMainUpperView;

  NetApp::GameSync::View::GameSyncBGLowerView*                 m_pGameSyncBGLowerView;
  NetApp::GameSync::View::GameSyncTwoSelectMenuLowerView*      m_pGameSyncTwoSelectMenuLowerView;
  NetApp::GameSync::View::GameSyncThreeSelectMenuLowerView*    m_pGameSyncThreeSelectMenuLowerView;
  NetApp::GameSync::View::GameSyncMessageMenuLowerView*        m_pGameSyncMessageMenuLowerView;

};

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)


#endif // GAMESYNC_VIEWOBJECTS_H_INCLUDE
