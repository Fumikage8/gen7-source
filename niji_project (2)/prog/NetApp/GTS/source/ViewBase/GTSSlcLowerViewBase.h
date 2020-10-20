// ============================================================================
/*
 * @file GTSSlcLowerViewBase.h
 * @brief GTS_SLC_LOW_000.flytを扱うDrawクラスのベースとなるクラスです。
 * @date 2015.05.14
 */
// ============================================================================
#if !defined( GTSSLCLOWERVIEWBASE_H_INCLUDE )
#define GTSSLCLOWERVIEWBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/UI/NetAppCursorView.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(ViewBase)


class GTSSlcLowerViewBase : public NetAppLib::UI::NetAppCursorView
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSSlcLowerViewBase );
public:
  GTSSlcLowerViewBase( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSSlcLowerViewBase();

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  void SetVisibleBackButton( bool bVisible );
  void SetVisibleBlackFilter( bool bVisible );

protected:
  
  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };


};


GFL_NAMESPACE_END(ViewBase)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSSLCLOWERVIEWBASE_H_INCLUDE
