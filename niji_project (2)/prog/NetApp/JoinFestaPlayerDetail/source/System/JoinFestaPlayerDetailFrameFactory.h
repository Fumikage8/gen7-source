// ============================================================================
/*
 * @file JoinFestaPlayerDetailFrameFactory.h
 * @brief 詳細アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERDETAIL_FRAMEFACTORY_H_INCLUDE )
#define JOINFESTAPLAYERDETAIL_FRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/JoinFestaPlayerDetail/source/System/JoinFestaPlayerDetailWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerDetail)
GFL_NAMESPACE_BEGIN(System)


class JoinFestaPlayerDetailViewSystemAccessor;


class JoinFestaPlayerDetailFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerDetailFrameFactory );
public:
  JoinFestaPlayerDetailFrameFactory(
    NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork* pJoinFestaPlayerDetailWork,
    NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewSystemAccessor* pJoinFestaPlayerDetailViewSystemAccessor );
  virtual~JoinFestaPlayerDetailFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailWork*                m_pJoinFestaPlayerDetailWork;
  NetApp::JoinFestaPlayerDetail::System::JoinFestaPlayerDetailViewSystemAccessor*  m_pJoinFestaPlayerDetailViewSystemAccessor;

};


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(JoinFestaPlayerDetail)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERDETAIL_FRAMEFACTORY_H_INCLUDE
