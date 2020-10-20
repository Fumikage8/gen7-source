//==============================================================================
/**
 * @file	P2pTimeoutListener.h
 * @brief	P2Pタイムアウトイベントリスナー
 * @author	ichiraku_katsuhiko
 * @date	2015/05/08 15:36:29
 */
// =============================================================================

#ifndef __P2P_TIMEOUT_LISTENER_HEADER__
#define __P2P_TIMEOUT_LISTENER_HEADER__
#pragma once

namespace NetLib{
namespace P2P{

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class P2pTimeoutListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief タイムアウト発生通知
   */
  //------------------------------------------------------------------
  virtual void OnP2pTimeoutError() = 0;
};

} // namespace P2P
} // namespace NetLib

#endif // __NEX_RATING_CLIENT_LISTENER_H__
