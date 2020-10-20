// ============================================================================
/*
 * @file JoinFestaProfileFrameBase.h
 * @brief プロフィールアプリのフレームの基底クラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILE_FRAMEBASE_H_INCLUDE )
#define JOINFESTAPROFILE_FRAMEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileWork.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileViewList.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileViewObjects.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)

GFL_NAMESPACE_BEGIN(System)
class JoinFestaProfileViewSystemAccessor;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(Frame)


class JoinFestaProfileFrameBase : public NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaProfileFrameBase );
public:
  JoinFestaProfileFrameBase(
    NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork,
    NetApp::JoinFestaProfile::System::JoinFestaProfileViewSystemAccessor* pJoinFestaProfileViewSystemAccessor );
  virtual~JoinFestaProfileFrameBase();

protected:

  virtual void exitFrame( NetAppLib::System::FrameResult result );

  NetApp::JoinFestaProfile::System::JoinFestaProfileWork*            m_pJoinFestaProfileWork;
  NetApp::JoinFestaProfile::System::JoinFestaProfileViewList*        m_pJoinFestaProfileViewList;
  NetApp::JoinFestaProfile::System::JoinFestaProfileViewObjects*     m_pJoinFestaProfileViewObjects;

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILE_FRAMEBASE_H_INCLUDE
