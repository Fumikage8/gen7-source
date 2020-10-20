// ============================================================================
/*
 * @file GTSSmallMessageWindowUpperView.h
 * @brief 上画面に小さいメッセージウィンドウを表示するクラスです。
 * @date 2015.05.22
 */
// ============================================================================
#if !defined( GTSSMALLMESSAGEWINDOWUPPERVIEW_H_INCLUDE )
#define GTSSMALLMESSAGEWINDOWUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(Window)


class GTSSmallMessageWindowUpperView : public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSSmallMessageWindowUpperView );

public:

  GTSSmallMessageWindowUpperView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSSmallMessageWindowUpperView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  void SetMessage( u32 messageID );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_GTS,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_UPPER,
  };

private:

  NetApp::GTS::ApplicationSystem::GTSWork*                      m_pGTSWork;

};


GFL_NAMESPACE_END(Window)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSSMALLMESSAGEWINDOWUPPERVIEW_H_INCLUDE
