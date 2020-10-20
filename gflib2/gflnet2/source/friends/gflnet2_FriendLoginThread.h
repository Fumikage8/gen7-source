#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_FriendLoginThread.h
 * @author suzuki_hisanao
 */
//=============================================================================

#ifndef __GFLNET_FREINDS_LOGIN_THREAD_H__
#define __GFLNET_FREINDS_LOGIN_THREAD_H__
#pragma once

#include <nn.h>
#include <nn/ngc.h>
#include <nn/friends.h>


#include <gflnet2/include/friends/gflnet2_FriendLoginListener.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

namespace gflnet2{
namespace friends{


/**
 * @brief  フレンドサーバログイン用スレッドクラス。<p>
 *         フレンドサーバへのログイン処理を非同期に行う為に準備されたクラスです。
 */
class FriendLoginThread : public gfl2::thread::ThreadInterface
{
//  GFL_FORBID_COPY_AND_ASSIGN(FriendLoginThread); //コピーコンストラクタ＋代入禁止
private:
  nn::os::Event*           m_event;            //!< ログイン待ちシグナルイベント
  FriendLoginListener*     m_listener;         //!< イベントリスナ
  nn::fnd::TimeSpan*       m_timeout;          //!< 非同期処理タイムアウト
  bool                     m_isCancel;         //!< キャンセルフラグ
  nn::os::CriticalSection  m_criticalSection;  //!< クリティカルセクション
  gfl2::thread::Thread* m_pThread;

public:
  //--------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param[in] heap           スレッドのスタック領域に使用するヒープ
   * @param[in] stack_size     スレッドのスタック領域のサイズ
   * @param[in] listener       ログイン完了通知用リスナ
   * @param[in] event          ログイン同期用イベント
   * @param[in] timeout        タイムアウト時間
   */
  //--------------------------------------------------------------------------------
  FriendLoginThread( gfl2::heap::HeapBase* heap, u32 stack_size, FriendLoginListener* listener, nn::os::Event* event, nn::fnd::TimeSpan* timeout );

  //--------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //--------------------------------------------------------------------------------
  virtual ~FriendLoginThread();

public:

  //--------------------------------------------------------------------------------
  /**
   * @brief 開始
   */
  //--------------------------------------------------------------------------------
  void Start( void );

  //--------------------------------------------------------------------------------
  /**
   * @brief キャンセル要求
   */
  //--------------------------------------------------------------------------------
  void Cancel( void );

public:
  
  //--------------------------------------------------------------------------------
  /**
   * @brief 生存確認
   */
  //--------------------------------------------------------------------------------
  void Kill( void ){ m_pThread->Kill(); };

  //--------------------------------------------------------------------------------
  /**
   * @brief 生存確認
   */
  //--------------------------------------------------------------------------------
  bool IsAlive( void ){ return m_pThread->IsAlive(); };
private:
  //--------------------------------------------------------------------------------
  /**
   * @brief ログイン終了リスナをコールする
   */
  //--------------------------------------------------------------------------------
  void   CallFinishListener( void );

private:

  virtual void Function( void );
};

} // friends
} // gflnet


#endif

#endif
