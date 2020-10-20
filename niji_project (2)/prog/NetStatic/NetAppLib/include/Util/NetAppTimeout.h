//==============================================================================
/**
 * @file	NetAppTimeout.h
 * @brief	タイムアウト計測クラス
 * @author	ichiraku_katsuhiko
 * @date	2015/07/25 15:59:28
 */
// =============================================================================

#ifndef __NET_APP_TIMEOUT_H__
#define __NET_APP_TIMEOUT_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <system/include/Date/gfl2_Date.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class NetAppTimeout
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppTimeout);

public:
  static const u32 DEFAULT_TIMEOUT_MILLI_SECOND = 1500; //! 1.5秒
  // @fix NMCat[4112]：IsTimeOutが１フレームで何度も呼ばれる場合がある為、時間を追加
  static const u32 EMERGENCY_TIMEOUT_FRAME = 5400; //! 30FPSで3分

public:
  // コンストラクタ
  NetAppTimeout();
  // デストラクタ
  virtual ~NetAppTimeout();

  /**
   * @brief タイムアウト計測開始
   * @param[in] timeout  タイムアウト値（ms)
   */
  void TimeOutStart( const u32 timeout = DEFAULT_TIMEOUT_MILLI_SECOND );

  /**
   * @brief タイムアウトチェック
   * @param[out] pElapsedTime 経過時間(ms)
   *             ※異常状態による強制時間経過でtrueが返るとタイムアウト値にならない事があります。
   * @return タイムアウトならtrue
   */
  bool IsTimeOut( u32* pElapsedTime = NULL );

  /**
   * @brief 計測一時停止
   * @note  Resumeで再開して下さい
   */
  void Suspend();

  /**
   * @brief 計測再開
   */
  void Resume();

private:
  u32                   m_timeout;        //! タイムアウト時間(ms)
  gfl2::system::Date    m_startingTime;   //! 計測開始時間
  u32                   m_timerCheckWait; //! 時間監視
  u32                   m_elapsedTime;    //! 経過時間(ms)
  bool                  m_isTimeout;      //! タイムアウトになったかフラグ
  bool                  m_isSuspend;      //! 一時停止フラグ
  gfl2::system::Date    m_suspendTime;    //! 一時停止時間

  u32                   m_emergencyFrame; //! NMCat[390]の非常時用処理
};

GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)

#endif // __NET_APP_TIMEOUT_H__
