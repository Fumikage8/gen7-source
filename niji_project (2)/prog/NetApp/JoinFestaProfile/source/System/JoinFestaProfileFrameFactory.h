// ============================================================================
/*
 * @file JoinFestaProfileFrameFactory.h
 * @brief プロフィールアプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILE_FRAMEFACTORY_H_INCLUDE )
#define JOINFESTAPROFILE_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaProfileViewSystemAccessor;


class JoinFestaProfileFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaProfileFrameFactory );
public:
  JoinFestaProfileFrameFactory(
    NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork,
    NetApp::JoinFestaProfile::System::JoinFestaProfileViewSystemAccessor* pJoinFestaProfileViewSystemAccessor );
  virtual~JoinFestaProfileFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::JoinFestaProfile::System::JoinFestaProfileWork*                m_pJoinFestaProfileWork;
  NetApp::JoinFestaProfile::System::JoinFestaProfileViewSystemAccessor*  m_pJoinFestaProfileViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILE_FRAMEFACTORY_H_INCLUDE
