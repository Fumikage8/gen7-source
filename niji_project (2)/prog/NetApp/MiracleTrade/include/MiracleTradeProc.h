// ============================================================================
/*
 * @file MiracleTradeProc.h
 * @brief ミラクル交換プロック
 * @date 2015.05.25
 */
// ============================================================================
#if !defined( MIRACLETRADEPROC_H_INCLUDE )
#define MIRACLETRADEPROC_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameProc.h"
#include "NetApp/MiracleTrade/include/MiracleTradeAppParam.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(ApplicationSystem)
class MiracleTradeWork;
GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class MiracleTradeApplicationSystem;
GFL_NAMESPACE_END(ApplicationSystem)


class MiracleTradeProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( MiracleTradeProc );
public:
  MiracleTradeProc( NetApp::MiracleTrade::APP_PARAM* pAppParam );
  virtual~MiracleTradeProc();

  void SetMiracleTradeWork( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork ) { m_pMiracleTradeWork = pMiracleTradeWork; }

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void PreDrawFunc( gfl2::proc::Manager* pProcManager );
  virtual void DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

private:

  enum
  {
    INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM,
    INIT_SEQUENCE_RESOURCE_LOADING,
  };

  NetApp::MiracleTrade::APP_PARAM                                             m_AppParam;
  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork*                  m_pMiracleTradeWork;
  NetApp::MiracleTrade::ApplicationSystem::MiracleTradeApplicationSystem*     m_pApplicationSystem;

};


GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

#endif // MIRACLETRADEPROC_H_INCLUDE
