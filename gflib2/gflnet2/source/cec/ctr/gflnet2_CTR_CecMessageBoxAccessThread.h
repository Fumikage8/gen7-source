// ============================================================================
/*
 * @file gflnet2_CTR_CecMessageBoxAccessThread.h
 * @brief すれ違い通信のメッセージボックスへアクセスする為のスレッドです
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#if !defined( GFLNET2_CTR_CECMESSAGEBOXACCESSTHREAD_H_INCLUDE )
#define GFLNET2_CTR_CECMESSAGEBOXACCESSTHREAD_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <thread/include/gfl2_Thread.h>
#include <gflnet2/include/cec/gflnet2_CecResultCode.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_CecMessageBoxAccessor;
class CTR_CecControlObject;


class CTR_CecMessageBoxAccessThread : public gfl2::thread::ThreadInterface
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_CecMessageBoxAccessThread );

public:
  CTR_CecMessageBoxAccessThread( CTR_CecMessageBoxAccessor* pCecMessageBoxAccessor );
  virtual~CTR_CecMessageBoxAccessThread();

  // --------------------------------------------------------------------------
  /**
   * @brief  メッセージボックスへのアクセスを実行します。
   */
  // --------------------------------------------------------------------------
  virtual void Function();

  // --------------------------------------------------------------------------
  /**
   * @brief  実行するオブジェクトを設定します。
   *         スレッドを走らせる前に設定しておく必要があります。
   * @param  pExecuteObject メッセージボックスへのアクセスコマンドオブジェクト
   */
  // --------------------------------------------------------------------------
  void SetExecuteObject( CTR_CecControlObject* pExecuteObject );

  // --------------------------------------------------------------------------
  /**
   * @brief  メッセージボックスへアクセスした時の実行結果を取得します。
   * @return 実行結果
   */
  // --------------------------------------------------------------------------
  ResultCode GetResultCode() const;

private:
  CTR_CecMessageBoxAccessor*      m_pCecMessageBoxAccessor;
  CTR_CecControlObject*           m_pExecuteObject;
  ResultCode                      m_eResultCode;

};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_CTR_CECMESSAGEBOXACCESSTHREAD_H_INCLUDE
#endif
