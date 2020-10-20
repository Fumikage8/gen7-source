// ============================================================================
/*
 * @file JoinFestaProfileDescriptionFrame.h
 * @brief 
 * @date 2015.11.04
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILEDESCRIPTION_FRAME_H_INCLUDE )
#define JOINFESTAPROFILEDESCRIPTION_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaProfile/source/Frame/JoinFestaProfileFrameBase.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileWork.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class InfoWindowBookType;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaProfileDescriptionFrame :
  public NetApp::JoinFestaProfile::Frame::JoinFestaProfileFrameBase,
  public app::tool::InfoWindowBookTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaProfileDescriptionFrame );
public:
  JoinFestaProfileDescriptionFrame(
    NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork,
    NetApp::JoinFestaProfile::System::JoinFestaProfileViewSystemAccessor* pJoinFestaProfileViewSystemAccessor );
  virtual~JoinFestaProfileDescriptionFrame();

  // InfoWindowBookTypeListener
  virtual void OnClose();
  virtual void OnChangePage( u32 pageNo );

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual bool cleanup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

private:

  app::tool::InfoWindowBookType*      m_pInfoWindowBookTypeLowerView;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILEDESCRIPTION_FRAME_H_INCLUDE
