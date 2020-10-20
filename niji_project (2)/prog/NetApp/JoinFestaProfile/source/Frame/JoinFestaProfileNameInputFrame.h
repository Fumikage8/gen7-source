// ============================================================================
/*
 * @file JoinFestaProfileNameInputFrame.h
 * @brief 
 * @date 2015.11.05
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILENAMEINPUT_FRAME_H_INCLUDE )
#define JOINFESTAPROFILENAMEINPUT_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/JoinFestaProfile/source/Frame/JoinFestaProfileFrameBase.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileWork.h"
#include "App/StrInput/include/StrInputAppParam.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaProfileNameInputFrame : public NetApp::JoinFestaProfile::Frame::JoinFestaProfileFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaProfileNameInputFrame );
public:
  JoinFestaProfileNameInputFrame(
    NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork,
    NetApp::JoinFestaProfile::System::JoinFestaProfileViewSystemAccessor* pJoinFestaProfileViewSystemAccessor );
  virtual~JoinFestaProfileNameInputFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();

private:

  App::StrInput::APP_PARAM                         m_StrInputAppParam;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILENAMEINPUT_FRAME_H_INCLUDE
