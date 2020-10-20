#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexMessageClientListener.h
 * @brief  MessageClient関連通知イベントハンドラ
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_MESSAGE_CLIENT_LISTENER_H__
#define __GFLNET_NEX_MESSAGE_CLIENT_LISTENER_H__
#pragma once

#include <nn.h>
#include <nex.h>



namespace gflnet2{
namespace nex{

//------------------------------------------------------------------
/*!
 * @brief メッセージ通知イベントハンドラ
 *
 * @note インスタントメッセージ関連の通知を受け取る場合は、
 *       本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class NexMessageClientListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(NexMessageClientListener); //コピーコンストラクタ＋代入禁止
public:

  //------------------------------------------------------------------
  /**
  * @brief メッセージ送信の成功通知
  *
  * @param[in] message 送信したメッセージデータ
  */
  //------------------------------------------------------------------
  virtual void OnNexMessageSendingSuccess( nn::nex::TextMessage* message ){};

  //------------------------------------------------------------------
  /**
  * @brief メッセージ受信の通知
  *
  * @param[in] message 受信したメッセージデータ
  */
  //------------------------------------------------------------------
  virtual void OnNexMessageReceived( nn::nex::TextMessage* message ){};

  //------------------------------------------------------------------
  /**
  * @brief 通信キャンセルの通知
  */
  //------------------------------------------------------------------
  virtual void OnNexMessageCanceled( void ){};

  //------------------------------------------------------------------
  /**
  * @brief NexMessageClient通信のエラー通知
  *
  * @param[in] result     実行結果
  * @param[in] errorCode  エラーアプレット用エラーコード
  */
  //------------------------------------------------------------------
  virtual void OnNexMessageClientError( const nn::nex::qResult& result, u32 errorCode ) = 0;
};


} // nex
} // gflnet



#endif // __GFLNET_NEX_MESSAGE_CLIENT_LISTENER_H__
#endif
