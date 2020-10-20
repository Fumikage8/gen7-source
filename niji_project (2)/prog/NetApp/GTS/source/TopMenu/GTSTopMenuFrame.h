// ============================================================================
/*
 * @file GTSTopMenuFrame.h
 * @brief GTSのトップメニューのプロセスです。
 * @date 2015.03.18
 */
// ============================================================================
#if !defined( GTSTOPMENUFRAME_H_INCLUDE )
#define GTSTOPMENUFRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/GTSFrameBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/TopMenu/GTSTopMenuLowerViewListener.h"
#include "NetApp/GTS/source/SelectMenu/GTSOneSelectMenuLowerViewListener.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class GTSViewObjects;
GFL_NAMESPACE_END(ApplicationSystem)

GFL_NAMESPACE_BEGIN(TopMenu)


class GTSTopMenuFrame : public NetApp::GTS::GTSFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSTopMenuFrame );
public:
  GTSTopMenuFrame(
    NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork,
    NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects );
  virtual~GTSTopMenuFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

private:

  NetApp::GTS::ApplicationSystem::GTSWork*                             m_pGTSWork;
  NetApp::GTS::ApplicationSystem::GTSViewObjects*                     m_pGTSViewObjects;
  NetApp::GTS::TopMenu::GTSTopMenuLowerViewListener                   m_GTSTopMenuLowerViewListener;
  NetApp::GTS::SelectMenu::GTSOneSelectMenuLowerViewListener          m_GTSOneSelectMenuLowerViewListener;

};


GFL_NAMESPACE_END(TopMenu)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTOPMENUFRAME_H_INCLUDE
