// ============================================================================
/*
 * @file GTSMessageWindowUpperView.h
 * @brief 上画面に大きいメッセージウィンドウを表示するクラスです。
 * @date 2015.04.30
 */
// ============================================================================
#if !defined( GTSMESSAGEWINDOWUPPERVIEW_H_INCLUDE )
#define GTSMESSAGEWINDOWUPPERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(Window)


class GTSMessageWindowUpperView : public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSMessageWindowUpperView );

public:

  GTSMessageWindowUpperView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSMessageWindowUpperView();

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

#endif // GTSMESSAGEWINDOWUPPERVIEW_H_INCLUDE
