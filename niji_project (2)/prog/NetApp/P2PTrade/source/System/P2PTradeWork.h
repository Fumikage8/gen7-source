// ============================================================================
/*
 * @file P2PTradeWork.h
 * @brief P2P交換アプリのワーククラスです。
 * @date 2015.10.28
 * @author endo_akira
 */
// ============================================================================
#if !defined( P2PTRADE_WORK_H_INCLUDE )
#define P2PTRADE_WORK_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include "NetApp/P2PTrade/source/P2PTradeDefineApp.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(P2PTrade)
class P2PTradeRequestClient;
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)
GFL_NAMESPACE_BEGIN(System)


class P2PTradeWork : public NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( P2PTradeWork );

private:

  static const u32  APP_HEAP_SIZE             = NetApp::P2PTrade::P2PTRADE_APP_HEAP_SIZE;
  static const u32  APP_DEVICE_HEAP_SIZE      = NetApp::P2PTrade::P2PTRADE_APP_DEVICE_HEAP_SIZE;

public:
  P2PTradeWork();
  virtual~P2PTradeWork();

  NetApp::P2PTrade::BootMode GetBootMode() const { return m_eBootMode; }
  void SetBootMode( NetApp::P2PTrade::BootMode eBootMode ) { m_eBootMode = eBootMode; }

  bool IsFinalTradeSuccess(){ return m_bIsFinalTradeSuccess; }
  bool IsFinalTradeComplete(){ return m_bIsFinalTradeComplete; }

  void SetFinalTradeSuccess( bool bFlag ){ m_bIsFinalTradeSuccess = bFlag; }
  void SetFinalTradeComplete( bool bFlag ){ m_bIsFinalTradeComplete = bFlag; }

  bool IsSyncSaveCancel(){ return m_bSyncSaveCancel; }
  void SetSyncSaveCancel( bool bFlag ){ m_bSyncSaveCancel = bFlag; }

  bool IsFinalSyncComplete(){ return m_bIsFinalSyncComplete; }
  void SetFinalSyncComplete( bool bFlag ){ m_bIsFinalSyncComplete = bFlag; }

  bool IsFinalSyncCancelComplete(){ return m_bIsFinalSyncCancelComplete; }
  void SetFinalSyncCancelComplete( bool bFlag ){ m_bIsFinalSyncCancelComplete = bFlag; }

  NetAppLib::P2PTrade::P2PTradeRequestClient* GetP2PTradeRequestClient(){ return m_pP2PTradeRequestClient; }
  void SetP2PTradeRequestClient( NetAppLib::P2PTrade::P2PTradeRequestClient* pP2PTradeRequestClient ){ m_pP2PTradeRequestClient = pP2PTradeRequestClient; }

private:

  NetApp::P2PTrade::BootMode                             m_eBootMode;
  bool                                                   m_bIsFinalTradeSuccess;
  bool                                                   m_bIsFinalTradeComplete;
  bool                                                   m_bSyncSaveCancel;
  bool                                                   m_bIsFinalSyncComplete;
  bool                                                   m_bIsFinalSyncCancelComplete;
  NetAppLib::P2PTrade::P2PTradeRequestClient*            m_pP2PTradeRequestClient;

};



GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

#endif // P2PTRADEWORK_H_INCLUDE
