// ============================================================================
/*
 * @file MiracleTradeViewObjects.h
 * @brief ミラクル交換のDrawクラスを管理するクラスです。
 * @date 2015.05.29
 */
// ============================================================================
#if !defined( MIRACLETRADEDRAWOBJECTS_H_INCLUDE )
#define MIRACLETRADEDRAWOBJECTS_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(ApplicationSystem)
class MiracleTradeWork;
GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)

GFL_NAMESPACE_BEGIN(StartConfirm)
class MiracleTradeStartConfirmUpperView;
class MiracleTradeStartConfirmLowerView;
GFL_NAMESPACE_END(StartConfirm)

GFL_NAMESPACE_BEGIN(Matching)
class MiracleTradeMatchingWaitLowerView;
class MiracleTradePlayerInfoLowerView;
class MiracleTradePokemonInfoUpperView;
GFL_NAMESPACE_END(Matching)

GFL_NAMESPACE_BEGIN(ApplicationSystem)

class MiracleTradeViewObjects
{
  GFL_FORBID_COPY_AND_ASSIGN( MiracleTradeViewObjects );
public:
  MiracleTradeViewObjects();
  virtual~MiracleTradeViewObjects();

  void RemoveFromSuperViewAll();

  void CreateDraw( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork );

  void InputDisableAll();

  bool IsDrawing();

  NetApp::MiracleTrade::StartConfirm::MiracleTradeStartConfirmUpperView*    GetMiracleTradeStartConfirmUpperView() { return m_pMiracleTradeStartConfirmUpperView; }
  NetApp::MiracleTrade::StartConfirm::MiracleTradeStartConfirmLowerView*    GetMiracleTradeStartConfirmLowerView() { return m_pMiracleTradeStartConfirmLowerView; }
  NetApp::MiracleTrade::Matching::MiracleTradeMatchingWaitLowerView*        GetMiracleTradeMatchingWaitLowerView() { return m_pMiracleTradeMatchingWaitLowerView; }
  NetApp::MiracleTrade::Matching::MiracleTradePlayerInfoLowerView*          GetMiracleTradePlayerInfoLowerView() { return m_pMiracleTradePlayerInfoLowerView; }
  NetApp::MiracleTrade::Matching::MiracleTradePokemonInfoUpperView*         GetMiracleTradePokemonInfoUpperView() { return m_pMiracleTradePokemonInfoUpperView; }

private:

  NetApp::MiracleTrade::StartConfirm::MiracleTradeStartConfirmUpperView*    m_pMiracleTradeStartConfirmUpperView;
  NetApp::MiracleTrade::StartConfirm::MiracleTradeStartConfirmLowerView*    m_pMiracleTradeStartConfirmLowerView;
  NetApp::MiracleTrade::Matching::MiracleTradeMatchingWaitLowerView*        m_pMiracleTradeMatchingWaitLowerView;
  NetApp::MiracleTrade::Matching::MiracleTradePlayerInfoLowerView*          m_pMiracleTradePlayerInfoLowerView;
  NetApp::MiracleTrade::Matching::MiracleTradePokemonInfoUpperView*         m_pMiracleTradePokemonInfoUpperView;


};

GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)


#endif // MIRACLETRADEDRAWOBJECTS_H_INCLUDE
