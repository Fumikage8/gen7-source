// ============================================================================
/*
 * @file GameSyncMainUpperView.h
 * @brief ゲームシンクアプリの上画面を表示するクラスです。
 * @date 2015.11.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( GAMESYNCMAINUPPERVIEW_H_INCLUDE )
#define GAMESYNCMAINUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <str/include/gfl2_StrBuf.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/GameSync/source/System/GameSyncWork.h"
#include <layout/include/gfl2_LytSys.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)
GFL_NAMESPACE_BEGIN(View)


class GameSyncMainUpperView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GameSyncMainUpperView );
public:
  GameSyncMainUpperView( NetApp::GameSync::System::GameSyncWork* pGameSyncWork );
  virtual~GameSyncMainUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  void SetGameSyncID( u32 descriptionMessageID, bool bExpandDateTime );

  gfl2::lyt::LytPicture* GetIconPane(void);

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

  NetApp::GameSync::System::GameSyncWork*       m_pGameSyncWork;
  gfl2::str::StrBuf                             m_GameSyncID;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNCMAINUPPERVIEW_H_INCLUDE
