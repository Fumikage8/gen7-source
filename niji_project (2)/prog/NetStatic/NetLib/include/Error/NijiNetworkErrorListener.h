//==============================================================================
/**
 * @file	NijiNetworkErrorListener.h
 * @brief	Niji通信エラー通知リスナー
 * @author	ichiraku_katsuhiko
 * @date	2015/05/14 12:30:09
 */
// =============================================================================

#ifndef __NIJI_NETWORK_ERROR_LISENER_H__
#define __NIJI_NETWORK_ERROR_LISENER_H__
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
class NijiNetworkErrorListener
{
public:
  //------------------------------------------------------------------
  /**
  * @brief 通信エラー通知
  */
  //------------------------------------------------------------------
  virtual void OnNijiNetworkError() = 0;
};

GFL_NAMESPACE_END(Error)
GFL_NAMESPACE_END(NetLib)

#endif // __NIJI_NETWORK_ERROR_LISENER_H__