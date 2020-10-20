//==============================================================================
/**
 * @file	P2pTimeout.h
 * @brief	P2Pタイムアウト管理
 * @author	ichiraku_katsuhiko
 * @date	2015/05/08 12:44:52
 */
// =============================================================================

#ifndef __P2P_TIMEOUT_HEADER__
#define __P2P_TIMEOUT_HEADER__
#pragma once

#if defined(GF_PLATFORM_CTR)
#include <nn.h>
#endif // GF_PLATFORM_CTR
#include <macro/include/gfl2_Macros.h>
#include <base/include/gfl2_Singleton.h>

#include "P2pTimeoutListener.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(P2P)

class P2pTimeout
{
  GFL_FORBID_COPY_AND_ASSIGN(P2pTimeout);
private:

  // デフォルトのタイムアウト値
  static const int DEFAULT_TIMEOUT = 30;  // ３０秒

  int  m_Sec;             ///< タイムアウト時間
  bool m_isUpdateEnable;  ///< 更新許可フラグ
#if defined(GF_PLATFORM_CTR)
  nn::fnd::DateTime   m_Now;      ///< スタートした時間
#endif // GF_PLATFORM_CTR
  P2pTimeoutListener* m_listener; ///< 通知リスナー

  void Initalize( int time );
  
public:
  // コンストラクタ
  P2pTimeout( void );
  //デストラクタ
  virtual ~P2pTimeout(){};

  //------------------------------------------------------------------
  /**
   * @brief   エラーリスナー登録
   * @param   listener   通知を受け取るIrAdapterErrorListener
   */
  //------------------------------------------------------------------
  void RegistErrorListener( P2pTimeoutListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief   エラーリスナー解除
   */
  //------------------------------------------------------------------
  void UnregistErrorListener( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update();

  //------------------------------------------------------------------
  /**
   * @brief   スタート
   * @param   タイムアウト値　省略でデフォルト値
   */
  //------------------------------------------------------------------
  void Start( int time = DEFAULT_TIMEOUT );

  //------------------------------------------------------------------
  /**
   * @brief   止める  これ以降エラーを出さない
   */
  //------------------------------------------------------------------
  void Stop();

};

GFL_NAMESPACE_END(P2P)
GFL_NAMESPACE_END(NetLib)

#endif // __P2P_TIMEOUT_HEADER__