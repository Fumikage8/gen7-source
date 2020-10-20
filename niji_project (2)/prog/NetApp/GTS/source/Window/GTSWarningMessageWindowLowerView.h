// ============================================================================
/*
 * @file GTSWarningMessageWindowLowerView.h
 * @brief 警告文ウィンドウを表示するクラスです。
 * @date 2015.04.28
 */
// ============================================================================
#if !defined( GTSWARNINGMESSAGEWINDOWLOWERVIEW_H_INCLUDE )
#define GTSWARNINGMESSAGEWINDOWLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(Window)


class GTSWarningMessageWindowLowerView : public NetAppLib::System::NetApplicationViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSWarningMessageWindowLowerView );

public:

  GTSWarningMessageWindowLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSWarningMessageWindowLowerView();

  // UIView
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  void SetMessage( u32 messageID, bool isShowTimerIcon = true );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_GTS,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

private:

  NetApp::GTS::ApplicationSystem::GTSWork*                      m_pGTSWork;

};


GFL_NAMESPACE_END(Window)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSWARNINGMESSAGEWINDOWLOWERVIEW_H_INCLUDE
