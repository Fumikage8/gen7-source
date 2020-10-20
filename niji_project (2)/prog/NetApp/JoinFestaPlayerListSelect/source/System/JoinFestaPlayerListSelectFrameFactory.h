// ============================================================================
/*
 * @file JoinFestaPlayerListSelectFrameFactory.h
 * @brief プレイヤー単体選択アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECT_FRAMEFACTORY_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECT_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/JoinFestaPlayerListSelect/source/System/JoinFestaPlayerListSelectWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelect)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaPlayerListSelectViewSystemAccessor;


class JoinFestaPlayerListSelectFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectFrameFactory );
public:
  JoinFestaPlayerListSelectFrameFactory(
    NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork* pJoinFestaPlayerListSelectWork,
    NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewSystemAccessor* pJoinFestaPlayerListSelectViewSystemAccessor );
  virtual~JoinFestaPlayerListSelectFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectWork*                m_pJoinFestaPlayerListSelectWork;
  NetApp::JoinFestaPlayerListSelect::System::JoinFestaPlayerListSelectViewSystemAccessor*  m_pJoinFestaPlayerListSelectViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerListSelect)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECT_FRAMEFACTORY_H_INCLUDE
