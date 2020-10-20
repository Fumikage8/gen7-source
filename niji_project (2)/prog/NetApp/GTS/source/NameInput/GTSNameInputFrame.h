// ============================================================================
/*
 * @file GTSNameInputFrame.h
 * @brief GTSの名前入力のプロセスです。
 * @date 2015.07.03
 */
// ============================================================================
#if !defined( GTSNAMEINPUTFRAME_H_INCLUDE )
#define GTSNAMEINPUTFRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/GTS/source/GTSFrameBase.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "App/StrInput/include/StrInputAppParam.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(NameInput)


class GTSNameInputFrame : public NetApp::GTS::GTSFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSNameInputFrame );
public:
  GTSNameInputFrame(
    NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSNameInputFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

private:

  NetApp::GTS::ApplicationSystem::GTSWork*         m_pGTSWork;
  App::StrInput::APP_PARAM                         m_StrInputAppParam;

};


GFL_NAMESPACE_END(NameInput)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSNAMEINPUTFRAME_H_INCLUDE
