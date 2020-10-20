#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_HttpSystem.h
 * @brief  HTTPライブラリシステムラッパークラス。
 * @author h.suzuki
 * @date   2011.10.20
 */
//=============================================================================
#ifndef __GFLNET_HTTP_SYSTEM_H__
#define __GFLNET_HTTP_SYSTEM_H__
#pragma once

#include <nn/init.h>
#include <nn.h>
#include <nn/http.h>
#include <nn/ac.h>
#include <nn/ac/ac_Result.h>


#include <gflnet2/include/gflnet2_NetworkDefine.h>


/**
* @namespace gflnet2::http
* @brief HTTP通信関連（デバッグ用途）
*/

namespace gflnet2{
namespace http{

/**
 * @brief HTTP通信用システムクラス
 */
class HttpSystem
{
  GFL_FORBID_COPY_AND_ASSIGN(HttpSystem); //コピーコンストラクタ＋代入禁止
private:
  //------------------------------------------------------------------
  /**
   * @brief   HttpSystemコンストラクタ＆デストラクタ(隠蔽)
   */
  //------------------------------------------------------------------
  HttpSystem();
  virtual ~HttpSystem();

private:
  static void   *m_postParamBuffer;    // Postパラメータ用バッファ
  static bool   m_isInit;              // 初期済みフラグ

private:
  static void   FreeBufferMemory( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief  HttpSystemの初期化
   *         事前にfsを初期化する必要があります！
   *         一度初期した後、Finalize()をコールしないで本関数をコールした場合は
   *         ResultSuccess()を返却するものの、何も行いません。
   *
   * @param[in] heap           ヒープベースへのポインタ。デバイスヒープは利用不可能！
   * @param[in] postBufferSize Postパラメータ用に割り当てるバッファのバイトサイズ
   */
  //------------------------------------------------------------------
  static nn::Result Initialize( gfl2::heap::HeapBase* heap = NULL, size_t postBufferSize = 0  );

  //------------------------------------------------------------------
  /**
   * @brief  HttpSystemの終了処理
   */
  //------------------------------------------------------------------
  static nn::Result Finalize( void );

#if GFL_DEBUG
  //------------------------------------------------------------------
  /**
   * @brief エラーレポート（デバッグ用）
   *
   * @param[in] result 実行結果
   */
  //------------------------------------------------------------------
  static void ReportHttpError( const nn::Result& result );
#endif

};

} // http
} // gflnet

#endif // __GFLNET_HTTP_SYSTEM_H__

#endif
