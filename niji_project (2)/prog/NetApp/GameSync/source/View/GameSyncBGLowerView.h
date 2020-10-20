// ============================================================================
/*
 * @file GameSyncBGLowerView.h
 * @brief ゲームシンクアプリの下画面を表示するクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNCBGLOWERVIEW_H_INCLUDE )
#define GAMESYNCBGLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/GameSync/source/System/GameSyncWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(View)


class GameSyncBGLowerView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncBGLowerView );
public:
  GameSyncBGLowerView(
    NetApp::GameSync::System::GameSyncWork* pGameSyncWork );
  virtual~GameSyncBGLowerView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_GAMESYNC,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

private:

  NetApp::GameSync::System::GameSyncWork*                                m_pGameSyncWork;


};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNCBGLOWERVIEW_H_INCLUDE
