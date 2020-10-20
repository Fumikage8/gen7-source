// ============================================================================
/*
 * @file GameSyncBGUpperView.h
 * @brief ゲームシンクアプリの上画面を表示するクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNCBGUPPERVIEW_H_INCLUDE )
#define GAMESYNCBGUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/GameSync/source/System/GameSyncWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(View)


class GameSyncBGUpperView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncBGUpperView );
public:
  GameSyncBGUpperView( NetApp::GameSync::System::GameSyncWork* pGameSyncWork );
  virtual~GameSyncBGUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );



private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_GameSync,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_UPPER,
  };

private:

  NetApp::GameSync::System::GameSyncWork*                         m_pGameSyncWork;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNCBGUPPERVIEW_H_INCLUDE
