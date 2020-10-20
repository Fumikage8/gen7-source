// ============================================================================
/*
 * @file JoinFestaPlayerListBGLowerView.h
 * @brief プレイヤーリストアプリの下画面を表示するクラスです。
 * @date 2015.10.05
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTBGLOWERVIEW_H_INCLUDE )
#define JOINFESTAPLAYERLISTBGLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaPlayerListBGLowerView : public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListBGLowerView );
public:
  JoinFestaPlayerListBGLowerView( 
    NetAppLib::System::ApplicationWorkBase* pWorkBase,
    NetAppLib::System::ResourceID layoutResourceID );
  virtual~JoinFestaPlayerListBGLowerView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

private:

  NetAppLib::System::ApplicationWorkBase*   m_pWorkBase;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAPLAYERLISTBGLOWERVIEW_H_INCLUDE
