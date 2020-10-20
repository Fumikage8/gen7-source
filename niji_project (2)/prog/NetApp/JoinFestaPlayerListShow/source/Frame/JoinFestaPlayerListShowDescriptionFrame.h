// ============================================================================
/*
 * @file JoinFestaPlayerListShowDescriptionFrame.h
 * @brief 
 * @date 2016.03.16
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSHOWDESCRIPTION_FRAME_H_INCLUDE )
#define JOINFESTAPLAYERLISTSHOWDESCRIPTION_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaPlayerListShow/source/Frame/JoinFestaPlayerListShowFrameBase.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowWork.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class InfoWindowBookType;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaPlayerListShowDescriptionFrame :
  public NetApp::JoinFestaPlayerListShow::Frame::JoinFestaPlayerListShowFrameBase,
  public app::tool::InfoWindowBookTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListShowDescriptionFrame );
public:
  JoinFestaPlayerListShowDescriptionFrame(
    NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork* pJoinFestaPlayerListShowWork,
    NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewSystemAccessor* pJoinFestaPlayerListShowViewSystemAccessor );
  virtual~JoinFestaPlayerListShowDescriptionFrame();

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
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSHOWDESCRIPTION_FRAME_H_INCLUDE
