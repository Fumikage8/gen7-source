// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiFrameFactory.h
 * @brief プレイヤー複数選択アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.20
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTMULTI_FRAMEFACTORY_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTMULTI_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/JoinFestaPlayerListSelectMulti/source/System/JoinFestaPlayerListSelectMultiWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaPlayerListSelectMultiViewSystemAccessor;


class JoinFestaPlayerListSelectMultiFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectMultiFrameFactory );
public:
  JoinFestaPlayerListSelectMultiFrameFactory(
    NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork* pJoinFestaPlayerListSelectMultiWork,
    NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewSystemAccessor* pJoinFestaPlayerListSelectMultiViewSystemAccessor );
  virtual~JoinFestaPlayerListSelectMultiFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiWork*                m_pJoinFestaPlayerListSelectMultiWork;
  NetApp::JoinFestaPlayerListSelectMulti::System::JoinFestaPlayerListSelectMultiViewSystemAccessor*  m_pJoinFestaPlayerListSelectMultiViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTMULTI_FRAMEFACTORY_H_INCLUDE
