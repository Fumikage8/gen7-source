// ============================================================================
/*
 * @file MiracleTradeFrameFactory.h
 * @brief ミラクル交換アプリのフレーム(プロセス)を作成するファクトリーメソッドです。
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADEFRAMEFACTORY_H_INCLUDE )
#define MIRACLETRADEFRAMEFACTORY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(ApplicationSystem)


class MiracleTradeViewObjects;


class MiracleTradeFrameFactory : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( MiracleTradeFrameFactory );
public:
  MiracleTradeFrameFactory( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork, NetApp::MiracleTrade::ApplicationSystem::MiracleTradeViewObjects* pMiracleTradeViewObjects );
  virtual~MiracleTradeFrameFactory();

  virtual NetAppLib::System::NetAppFrameBase* CreateNetAppFrame( NetAppLib::System::FrameID frameID );

private:
  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork*              m_pMiracleTradeWork;
  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeViewObjects*       m_pMiracleTradeViewObjects;

};


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEFRAMEFACTORY_H_INCLUDE
