// ============================================================================
/*
 * @file JoinFestaPlayerListBGUpperView.h
 * @brief プレイヤーリストアプリの上画面を表示するクラスです。
 * @date 2015.10.05
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTBGUPPERVIEW_H_INCLUDE )
#define JOINFESTAPLAYERLISTBGUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaPlayerListBGUpperView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListBGUpperView );
public:
  JoinFestaPlayerListBGUpperView(
    NetAppLib::System::ApplicationWorkBase* pWorkBase,
    NetAppLib::System::ResourceID layoutResourceID );
  virtual~JoinFestaPlayerListBGUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_UPPER,
  };

private:

  NetAppLib::System::ApplicationWorkBase*   m_pWorkBase;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAPLAYERLISTBGUPPERVIEW_H_INCLUDE
