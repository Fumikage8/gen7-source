// ============================================================================
/*
 * @file JoinFestaOpenListDescriptionFrame.h
 * @brief 
 * @date 2016.04.04
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAOPENLISTDESCRIPTION_FRAME_H_INCLUDE )
#define JOINFESTAOPENLISTDESCRIPTION_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaOpenList/source/Frame/JoinFestaOpenListFrameBase.h"
#include "NetApp/JoinFestaOpenList/source/System/JoinFestaOpenListWork.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class InfoWindowBookType;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaOpenListDescriptionFrame :
  public NetApp::JoinFestaOpenList::Frame::JoinFestaOpenListFrameBase,
  public app::tool::InfoWindowBookTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaOpenListDescriptionFrame );
public:
  JoinFestaOpenListDescriptionFrame(
    NetApp::JoinFestaOpenList::System::JoinFestaOpenListWork* pJoinFestaOpenListWork,
    NetApp::JoinFestaOpenList::System::JoinFestaOpenListViewSystemAccessor* pJoinFestaOpenListViewSystemAccessor );
  virtual~JoinFestaOpenListDescriptionFrame();

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
GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAOPENLISTDESCRIPTION_FRAME_H_INCLUDE
