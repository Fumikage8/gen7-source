// ============================================================================
/*
 * @file JoinFestaPlayerListPlayerInfoUpperView.h
 * @brief プレイヤーリストアプリの上画面にプレイヤー情報を表示するクラスです。
 * @date 2015.10.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTPLAYERINFOUPPERVIEW_H_INCLUDE )
#define JOINFESTAPLAYERLISTPLAYERINFOUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <str/include/gfl2_StrBuf.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListManager.h"
#include "NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaPlayerListPlayerInfoUpperView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListPlayerInfoUpperView );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_UPPER,
  };

public:

  JoinFestaPlayerListPlayerInfoUpperView(
    NetAppLib::System::ApplicationWorkBase*       pWorkBase,
    NetAppLib::System::ResourceID                 layoutResourceID,
    NetAppLib::Util::NetAppTrainerIconUtility&    netAppTrainerIconUtility );
  virtual~JoinFestaPlayerListPlayerInfoUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  void SetJoinFestaPlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData );

  void ResetCurrentJoinFestaFriendKey();

  void TimerIconDisable();

private:

  void setJoinFestaPlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData );
  void setJoinFestaBlackPlayerData( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData );

private:

  NetAppLib::System::ApplicationWorkBase*         m_pWorkBase;
  gfl2::str::StrBuf                               m_NullString;
  NetAppLib::Util::NetAppTrainerIconUtility&      m_NetAppTrainerIconUtility;
  JoinFestaFriendKey                              m_CurrentJoinFestaFriendKey;
  bool                                            m_bDrawFlag;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAPLAYERLISTPLAYERINFOUPPERVIEW_H_INCLUDE
