// ============================================================================
/*
 * @file gflnet2_ICecRecvMessageBox.h
 * @brief すれ違い通信の受信ボックスに関する操作を行うクラスです。
 * @date 2015.02.13
 */
// ============================================================================
#if !defined( GFLNET2_ICECRECVMESSAGEBOX_H_INCLUDE )
#define GFLNET2_ICECRECVMESSAGEBOX_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gflnet2/include/cec/gflnet2_CecPacketInfo.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)

class ICecMessageBoxEventListener;

class ICecRecvMessageBox
{
public:

  virtual~ICecRecvMessageBox(){}

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックス内にある受信データ数を取得します。
   * @return 受信ボックス内にある受信データ数
   */
  // --------------------------------------------------------------------------
  virtual s32 GetMessageCount() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックス内にある受信データ数を取得します。
   *         非同期で行われるので結果はイベントリスナーに通知されます。
   * @param  pEventListener 結果が通知されるイベントリスナーを指定します
   */
  // --------------------------------------------------------------------------
  virtual void GetMessageCountAsync( ICecMessageBoxEventListener* pEventListener ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックス内にある受信データを読み込みます。
   * @param  recvPacketCount 読み込んだ受信データの数
   * @return 読み込んだ受信データ
   */
  // --------------------------------------------------------------------------
  virtual const gflnet2::cec::CecRecvPacket* ReadMessage( u32& recvPacketCount ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックス内にある受信データを読み込みます。
   *         非同期で行われるので結果はイベントリスナーに通知されます。
   * @param  pEventListener 結果が通知されるイベントリスナーを指定します
   */
  // --------------------------------------------------------------------------
  virtual void ReadMessageAsync( ICecMessageBoxEventListener* pEventListener ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックスにある受信データを全て削除します。
   * @retval true 削除に成功
   * @retval false 削除に失敗
   */
  // --------------------------------------------------------------------------
  virtual bool DeleteAllMessage() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックスにある受信データを全て削除します。
   *         非同期で行われるので結果はイベントリスナーに通知されます。
   * @param  pEventListener 結果が通知されるイベントリスナーを指定します
   */
  // --------------------------------------------------------------------------
  virtual void DeleteAllMessageAsync( ICecMessageBoxEventListener* pEventListener ) = 0;
};


GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_ICECRECVMESSAGEBOX_H_INCLUDE
