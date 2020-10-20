// ============================================================================
/*
 * @file JoinFestaPlayerListShowFrameFactory.h
 * @brief 閲覧アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSHOW_FRAMEFACTORY_H_INCLUDE )
#define JOINFESTAPLAYERLISTSHOW_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/JoinFestaPlayerListShow/source/System/JoinFestaPlayerListShowWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListShow)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaPlayerListShowViewSystemAccessor;


class JoinFestaPlayerListShowFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListShowFrameFactory );
public:
  JoinFestaPlayerListShowFrameFactory(
    NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork* pJoinFestaPlayerListShowWork,
    NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewSystemAccessor* pJoinFestaPlayerListShowViewSystemAccessor );
  virtual~JoinFestaPlayerListShowFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowWork*                m_pJoinFestaPlayerListShowWork;
  NetApp::JoinFestaPlayerListShow::System::JoinFestaPlayerListShowViewSystemAccessor*  m_pJoinFestaPlayerListShowViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListShow)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSHOW_FRAMEFACTORY_H_INCLUDE
