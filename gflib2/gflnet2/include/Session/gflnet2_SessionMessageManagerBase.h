//=============================================================================
/**
 * @file   gflnet2_SessionMessageManagerBase.h
 * @brief  メッセージ管理基底クラス
 * @author shin kosaka
 */
//=============================================================================
#ifndef __GFLNET2_SESSION_MESSAGE_MANAGER_BASE_H__
#define __GFLNET2_SESSION_MESSAGE_MANAGER_BASE_H__
#pragma once


#include <gflnet2/include/Session/gflnet2_SessionMessageListener.h>
#include <gflnet2/include/Session/gflnet2_SessionPacketInitParam.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_FixedSizeContainer.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(Session)

class SessionPacketManager;

/***************************************************************************!
 * @brief  メッセージ管理基底クラス
 ***************************************************************************/
class SessionMessageManagerBase
{
  GFL_FORBID_COPY_AND_ASSIGN(SessionMessageManagerBase); //コピーコンストラクタ＋代入禁止


public:


  //------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  */
  //------------------------------------------------------------------
  SessionMessageManagerBase( u32 sessionID , SessionPacketManager* pPakectManager );

  //------------------------------------------------------------------
  /**
  * @brief   デストラクタ
  */
  //------------------------------------------------------------------
  virtual ~SessionMessageManagerBase();

  //------------------------------------------------------------------
  /**
  * @brief   初期化処理
  * @param[in] pHeap ヒープ
  */
  //------------------------------------------------------------------
  void Initialize( gfl2::heap::HeapBase* pHeap );

  //------------------------------------------------------------------
  /**
  * @brief   終了処理
  */
  //------------------------------------------------------------------
  void Finalize( void );

  //------------------------------------------------------------------
  /**
  * @brief   管理するメッセージIDを追加
  */
  //------------------------------------------------------------------
  void AddTargetMessageID( u16 messageID );

  //------------------------------------------------------------------
  /**
  * @brief   リスナー設定
  * @param[in] pListener
  */
  //------------------------------------------------------------------
  void SetListener( SessionMessageListener* pListener );

  //------------------------------------------------------------------
  /**
  * @brief   リスナー削除
  */
  //------------------------------------------------------------------
  void RemoveListener( void );

  //------------------------------------------------------------------
  /**
  * @brief  UDPメッセージ受信
  * @param[in] messageID メッセージID
  * @param[in] pMessage  メッセージ本体
  * @param[in] messageSize メッセージサイズ
  */
  //------------------------------------------------------------------
  bool SetRecvUDPMessage( u32 messageID , const u8* pMessage , u16 messageSize );

  //------------------------------------------------------------------
  /**
  * @brief  TCPメッセージ受信
  * @param[in] messageID メッセージID
  * @param[in] pMessage  メッセージ本体
  * @param[in] messageSize メッセージサイズ
  */
  //------------------------------------------------------------------
  bool SetRecvTCPMessage( u32 messageID , const u8* pMessage , u16 messageSize );


private:

  //------------------------------------------------------------------
  /**
  * @brief   自分が管理するメッセージIDかどうか判定
  * @param[in] messageID
  */
  //------------------------------------------------------------------
  bool IsMyTargetMessageID( u16 messageID );

protected:

  //------------------------------------------------------------------
  /**
  * @brief  TCPメッセージ送信
  * @param[in] messageID メッセージID
  * @param[in] pMessage  メッセージ本体
  * @param[in] messageSize メッセージサイズ
  */
  //------------------------------------------------------------------
  bool SendTCPMessageInternal( u32 messageID , const u8* pMessage , u16 messageSize );

  //------------------------------------------------------------------
  /**
  * @brief  UDPメッセージ送信
  * @param[in] messageID メッセージID
  * @param[in] pMessage  メッセージ本体
  * @param[in] messageSize メッセージサイズ
  */
  //------------------------------------------------------------------
  bool SendUDPMessageInternal( u32 messageID , const u8* pMessage , u16 messageSize );

private:

  u32 m_sessionID;//!<セッションID
  SessionMessageListener* m_pListener;
  SessionPacketManager* m_pPacketManager;
  gfl2::util::FixedSizeContainer<u16,MAX_TCP_MESSAGE_NUM+MAX_UDP_MESSAGE_NUM>			m_targeMessageIDArray;//!<このマネージャーが管理するTCPメッセージID
};


GFL_NAMESPACE_END(Session)
GFL_NAMESPACE_END(gflnet2)



#endif // __GFLNET2_SESSION_MESSAGE_MANAGER_BASE_H__


