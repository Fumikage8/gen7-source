// ============================================================================
/*
 * @file ConnectionUpperView.h
 * @brief インターネット接続アプリの上画面を表示するクラスです。
 * @date 2015.08.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( CONNECTIONUPPERVIEW_H_INCLUDE )
#define CONNECTIONUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/Connection/source/System/ConnectionWork.h"
#include <layout/include/gfl2_LytSys.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)
GFL_NAMESPACE_BEGIN(View)


class ConnectionUpperView :
  public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ConnectionUpperView );
public:
  ConnectionUpperView( NetApp::Connection::System::ConnectionWork* pConnectionWork );
  virtual~ConnectionUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  gfl2::lyt::LytPicture* GetIconPane(void);

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_Connection,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_UPPER,
  };

private:

  NetApp::Connection::System::ConnectionWork*                         m_pConnectionWork;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTIONUPPERVIEW_H_INCLUDE
