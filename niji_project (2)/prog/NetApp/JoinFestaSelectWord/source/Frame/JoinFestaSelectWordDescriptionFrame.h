﻿// ============================================================================
/*
 * @file JoinFestaSelectWordDescriptionFrame.h
 * @brief 
 * @date 2016.03.16
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORDDESCRIPTION_FRAME_H_INCLUDE )
#define JOINFESTASELECTWORDDESCRIPTION_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaSelectWord/source/Frame/JoinFestaSelectWordFrameBase.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordWork.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class InfoWindowBookType;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaSelectWordDescriptionFrame :
  public NetApp::JoinFestaSelectWord::Frame::JoinFestaSelectWordFrameBase,
  public app::tool::InfoWindowBookTypeListener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordDescriptionFrame );
public:
  JoinFestaSelectWordDescriptionFrame(
    NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork,
    NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordViewSystemAccessor* pJoinFestaSelectWordViewSystemAccessor );
  virtual~JoinFestaSelectWordDescriptionFrame();

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
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORDDESCRIPTION_FRAME_H_INCLUDE
