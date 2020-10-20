#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gfl_FriendAddThread.h
 * @author suzuki_hisanao
 */
//=============================================================================

#ifndef __GFLNET_FREINDS_ADD_THREAD_H__
#define __GFLNET_FREINDS_ADD_THREAD_H__
#pragma once

#include <nn.h>
#include <nn/ngc.h>


#include <gflnet2/include/friends/gflnet2_FriendAddListener.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_ThreadStatic.h>

namespace gflnet2{
namespace friends{

/**
 *  @brief  フレンド登録用スレッドクラス。<p>
 *          ３ＤＳ本体のフレンド登録を非同期に行う為に準備されたクラスです。
 */
class FriendAddThread : public gfl2::thread::ThreadInterface
{
//  GFL_FORBID_COPY_AND_ASSIGN(FriendAddThread); //コピーコンストラクタ＋代入禁止
private:
  nn::os::Event*           m_event;            //!< 完了通知イベント
  FriendAddListener*       m_listener;         //!< イベントリスナ
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
  FriendAddThread( gfl2::heap::HeapBase* heap, u32 stack_size, FriendAddListener* listener, nn::os::Event* event, nn::fnd::TimeSpan* timeout );

  //--------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //--------------------------------------------------------------------------------
  virtual ~FriendAddThread();

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
