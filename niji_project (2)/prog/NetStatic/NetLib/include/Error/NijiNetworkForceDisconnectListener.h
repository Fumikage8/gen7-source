//==============================================================================
/**
 * @file	NijiNetworkForceDisconnectListener.h
 * @brief	WiFi強制切断通知リスナー
 * @author	ichiraku_katsuhiko
 * @date	2015/12/29 13:31
 */
// =============================================================================

#ifndef __NIJI_NETWORK_FORCE_DISCONNECT_LISENER_H__
#define __NIJI_NETWORK_FORCE_DISCONNECT_LISENER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Error)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief   Niji通信エラー通知リスナー
 */
//------------------------------------------------------------------
class NijiNetworkForceDisconnectListener
{
public:
  //------------------------------------------------------------------
  /**
  * @brief WiFi強制切断通知
  */
  //------------------------------------------------------------------
  virtual void OnForceDisconnect() = 0;
};

GFL_NAMESPACE_END(Error)
GFL_NAMESPACE_END(NetLib)

#endif // __NIJI_NETWORK_FORCE_DISCONNECT_LISENER_H__