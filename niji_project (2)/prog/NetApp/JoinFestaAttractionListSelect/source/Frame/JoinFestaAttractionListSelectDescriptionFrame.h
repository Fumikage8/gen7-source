// ============================================================================
/*
 * @file JoinFestaAttractionListSelectDescriptionFrame.h
 * @brief アトラクション開催参加アプリの○○プロセスです。
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONLISTSELECTDESCRIPTION_FRAME_H_INCLUDE )
#define JOINFESTAATTRACTIONLISTSELECTDESCRIPTION_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaAttractionListSelect/source/Frame/JoinFestaAttractionListSelectFrameBase.h"
#include "NetApp/JoinFestaAttractionListSelect/source/System/JoinFestaAttractionListSelectWork.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class InfoWindowBookType;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaAttractionListSelect)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaAttractionListSelectDescriptionFrame :
  public NetApp::JoinFestaAttractionListSelect::Frame::JoinFestaAttractionListSelectFrameBase,
  public app::tool::InfoWindowBookTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionListSelectDescriptionFrame );
public:
  JoinFestaAttractionListSelectDescriptionFrame(
    NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectWork* pJoinFestaAttractionListSelectWork,
    NetApp::JoinFestaAttractionListSelect::System::JoinFestaAttractionListSelectViewSystemAccessor* pJoinFestaAttractionListSelectViewSystemAccessor );
  virtual~JoinFestaAttractionListSelectDescriptionFrame();

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
GFL_NAMESPACE_END(JoinFestaAttractionListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAATTRACTIONLISTSELECTSAMPLE_FRAME_H_INCLUDE
