// ============================================================================
/*
 * @file gfl2_CryptoThreadController.h
 * @brief スレッド制御クラスです。
 * @date 2015.03.12
 */
// ============================================================================
#if !defined( GFL2_CRYPTOTHREADCONTROLLER_H_INCLUDE )
#define GFL2_CRYPTOTHREADCONTROLLER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <thread/include/gfl2_Thread.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(crypto)

class CryptoThreadController
{
  GFL_FORBID_COPY_AND_ASSIGN( CryptoThreadController );
public:
  CryptoThreadController( gfl2::heap::HeapBase* pHeap, int threadPriority );
  virtual~CryptoThreadController();

  // --------------------------------------------------------------------------
  /**
   * @brief  スレッドを生成して開始します。
   * @param  pThreadInterface スレッドから呼び出される関数を持っているクラスです
   * @param  bMainLoop スレッドから呼び出される関数のループ指定(false:で１回呼び)
   */
  // --------------------------------------------------------------------------
  void Start( gfl2::thread::ThreadInterface* pThreadInterface, bool bMainLoop );

  // --------------------------------------------------------------------------
  /**
   * @brief  スレッドの終了を監視して、終了したらスレッドを破棄します。
   * @retval true スレッドが起動中です
   * @retval false スレッドが終了してます
   */
  // --------------------------------------------------------------------------
  bool Update();

  // --------------------------------------------------------------------------
  /**
   * @brief  スレッドが起動中か調べます。
   * @retval true スレッドが起動中です
   * @retval false スレッドが終了してます
   */
  // --------------------------------------------------------------------------
  bool IsBusy();

private:
  gfl2::heap::HeapBase*     m_pHeap;
  gfl2::thread::Thread*     m_pThread;
  int                       m_ThreadPriority;
  static const u32          STACK_SIZE = 4096;

};


GFL_NAMESPACE_END(crypto)
GFL_NAMESPACE_END(gfl2)

#endif // GFL2_CRYPTOTHREADCONTROLLER_H_INCLUDE
