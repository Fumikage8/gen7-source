// ============================================================================
/*
 * @file gflnet2_CTR_CecController.h
 * @brief すれ違い通信を制御するクラスです
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#if !defined( GFLNET2_CTR_CECCONTROLLER_H_INCLUDE )
#define GFLNET2_CTR_CECCONTROLLER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <thread/include/gfl2_Thread.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_ICecControlObjectRegister.h>
#include <gflnet2/include/cec/gflnet2_CecMessageBoxCreateParam.h>
#include <gflnet2/include/cec/gflnet2_CecResultCode.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_CecAllocator;
class CTR_CecMessageBoxAccessor;
class CTR_CecMessageBoxAccessThread;


class CTR_CecController : public CTR_ICecControlObjectRegister
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_CecController );

public:
  CTR_CecController( gfl2::heap::HeapBase* pHeap );
  virtual~CTR_CecController();

  // --------------------------------------------------------------------------
  /**
   * @brief  初期化処理を行います。
   * @param  pInitParam 初期化用のパラメータです
   * @param  pCecAllocator すれ違い通信ライブラリ内で使用するメモリアロケータです
   */
  // --------------------------------------------------------------------------
  void Initialize( const gflnet2::cec::CecSystemInitParam* pInitParam, CTR_CecAllocator* pCecAllocator );

  // --------------------------------------------------------------------------
  /**
   * @brief  更新処理を行います。
   *         制御オブジェクトが登録されたらスレッドを作成してメッセージボックスへアクセスします。
   */
  // --------------------------------------------------------------------------
  void Update();

  // --------------------------------------------------------------------------
  /**
   * @brief  すれ違い通信の制御オブジェクトを登録します。
   * @param  pCecControlObject すれ違い通信の制御オブジェクト
   */
  // --------------------------------------------------------------------------
  virtual void RegisterControlObject( CTR_CecControlObject* pCecControlObject );

  // --------------------------------------------------------------------------
  /**
   * @brief  すれ違い通信ライブラリのコマンドを即実行します。
   * @param  pCecControlObject すれ違い通信の制御オブジェクト
   */
  // --------------------------------------------------------------------------
  virtual void QuickExecute( CTR_CecControlObject* pCecControlObject );

private:

  enum State
  {
    START_WAIT,
    END_WAIT
  };

  gfl2::heap::HeapBase*           m_pHeap;
  CTR_CecMessageBoxAccessor*      m_pCecMessageBoxAccessor;
  CTR_CecMessageBoxAccessThread*  m_pCecMessageBoxAccessThread;
  CTR_CecControlObject*           m_pExecuteObject;
  gfl2::thread::Thread*           m_pThread;
  State                           m_eState;
  int                             m_ThreadPriority;
  static const u32 THREAD_STACK_SIZE = 8192;  //!< スレッドスタック用サイズ

};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_CTR_CECCONTROLLER_H_INCLUDE
#endif
