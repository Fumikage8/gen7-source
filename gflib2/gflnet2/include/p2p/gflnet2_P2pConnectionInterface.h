#ifdef  GF_PLATFORM_CTR
//==============================================================================
/**
 * @file	gflnet2_P2pConnectionInterface.h
 * @brief	P2Pプロセス：接続インターフェイス
 * @author	ichiraku_katsuhiko
 * @date	2015/03/18 17:44:08
 */
// =============================================================================

#if !defined( P2P_CONNECTION_INTERFACE_H_INCLUDED )
#define P2P_CONNECTION_INTERFACE_H_INCLUDED

#pragma once

#include <gflnet2/include/p2p/gflnet2_P2pProcBaseProcess.h>
#include <gflnet2/include/p2p/gflnet2_P2pDefine.h>
#include <gflnet2/include/beacon/gflnet2_BeaconManager.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(p2p)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class P2pConnectionInterface : public P2pBaseProcess
{
  GFL_FORBID_COPY_AND_ASSIGN(P2pConnectionInterface);
public:
  P2pConnectionInterface():m_connectType(),m_pBeaconManager(NULL){};
  virtual ~P2pConnectionInterface(){};

private:
  CONNECT_TYPE m_connectType;  ///< 接続種別
  gflnet2::base::BeaconManager* m_pBeaconManager; ///< ビーコンマネージャー

public:
  inline void SetConnectType( CONNECT_TYPE connectType ){ m_connectType = connectType; };
  inline CONNECT_TYPE GetConnectType( void ){ return m_connectType; };
  inline void SetBeaconManager( gflnet2::base::BeaconManager *pBeaconManager){ m_pBeaconManager = pBeaconManager; };
  inline gflnet2::base::BeaconManager * GetBeaconManager( void){ return m_pBeaconManager; };

  //------------------------------------------------------------------
  /**
   * @brief   Nexの送受信機能を有効化
   */
  //------------------------------------------------------------------
  void DirectStreamEnable( void );

};

GFL_NAMESPACE_END(p2p)
GFL_NAMESPACE_END(gflnet2)

#endif  // P2P_CONNECTION_INTERFACE_H_INCLUDED
#endif  // GF_PLATFORM_CTR