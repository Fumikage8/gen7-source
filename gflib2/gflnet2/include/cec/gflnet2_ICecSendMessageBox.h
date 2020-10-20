// ============================================================================
/*
 * @file gflnet2_ICecSendMessageBox.h
 * @brief すれ違い通信の送信ボックスに関する操作を行うクラスです。
 * @date 2015.02.13
 */
// ============================================================================
#if !defined( GFLNET2_ICECSENDMESSAGEBOX_H_INCLUDE )
#define GFLNET2_ICECSENDMESSAGEBOX_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gflnet2/include/cec/gflnet2_CecPacketInfo.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)

class ICecMessageBoxEventListener;

class ICecSendMessageBox
{
public:

  virtual~ICecSendMessageBox(){}

  // --------------------------------------------------------------------------
  /**
   * @brief  送信ボックスにある送信データを全て削除します。
   * @retval true 削除に成功
   * @retval false 削除に失敗
   */
  // --------------------------------------------------------------------------
  virtual bool DeleteAllMessage() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  送信ボックスにある送信データを全て削除します。
   *         非同期で行われるので結果はイベントリスナーに通知されます。
   * @param  pEventListener 結果が通知されるイベントリスナーを指定します
   */
  // --------------------------------------------------------------------------
  virtual void DeleteAllMessageAsync( ICecMessageBoxEventListener* pEventListener ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  送信ボックスに送信データを書き込みます。
   *         書き込んだデータは自動的に相手に送信されます。
   * @param  pSendPacket 書き込む送信データです
   * @retval true 書き込みに成功
   * @retval false 書き込みに失敗
   */
  // --------------------------------------------------------------------------
  virtual bool WriteMessage( const CecSendPacket* pSendPacket ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  送信ボックスに送信データを書き込みます。
   *         書き込んだデータは自動的に相手に送信されます。
   *         非同期で行われるので結果はイベントリスナーに通知されます。
   * @param  pSendPacket 書き込む送信データです
   * @param  pEventListener 結果が通知されるイベントリスナーを指定します
   */
  // --------------------------------------------------------------------------
  virtual void WriteMessageAsync( const CecSendPacket* pSendPacket, ICecMessageBoxEventListener* pEventListener ) = 0;
};


GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_ICECSENDMESSAGEBOX_H_INCLUDE
