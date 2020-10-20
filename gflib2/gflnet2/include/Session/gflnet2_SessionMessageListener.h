//=============================================================================
/**
 * @file   gflnet2_SessionMessageListener.h
 * @brief  メッセージリスナー
 * @author shin kosaka
 */
//=============================================================================
#ifndef __GFLNET2_SESSION_MESSAGE_LISTENER_H__
#define __GFLNET2_SESSION_MESSAGE_LISTENER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(Session)


/***************************************************************************!
 * @brief  メッセージリスナークラス
 ***************************************************************************/
class SessionMessageListener
{
  //GFL_FORBID_COPY_AND_ASSIGN(SessionMessageListener); //コピーコンストラクタ＋代入禁止


public:


  //------------------------------------------------------------------
  /**
  * @brief  TCPメッセージ受信イベント
  * @param[in] messageID メッセージID
  * @param[in] pMessage  メッセージ本体
  * @param[in] messageSize メッセージサイズ
  */
  //------------------------------------------------------------------
  virtual void OnRecvTCPMessage( u32 messageID , const u8* pMessage , u16 messageSize ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief  UDPメッセージ受信イベント
  * @param[in] messageID メッセージID
  * @param[in] pMessage  メッセージ本体
  * @param[in] messageSize メッセージサイズ
  */
  //------------------------------------------------------------------
  virtual void OnRecvUDPMessage( u32 messageID , const u8* pMessage , u16 messageSize ) = 0;

};


GFL_NAMESPACE_END(Session)
GFL_NAMESPACE_END(gflnet2)



#endif // __GFLNET2_SESSION_MESSAGE_LISTENER_H__


