//=============================================================================
/**
 * @file   gflnet2_SessionPacketManager.h
 * @brief  セッション内パケット管理クラス
 * @author shin kosaka
 */
//=============================================================================
#ifndef __GFLNET2_SESSION_PACKET_MANAGER_H__
#define __GFLNET2_SESSION_PACKET_MANAGER_H__
#pragma once

#include <gflnet2/include/Session/gflnet2_SessionMessageManagerBase.h>
#include <gflnet2/source/Session/gflnet2_SessionPacketIOBase.h>
#include <gflnet2/include/Session/gflnet2_SessionPacketInitParam.h>
#include <gflnet2/source/Session/gflnet2_SessionPacketDatagram.h>
#include <gflnet2/include/Session/gflnet2_SessionMessageListener.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_FixedSizeContainer.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(Session)


/***************************************************************************!
 * @brief  セッション内パケット管理クラス
 ***************************************************************************/
class SessionPacketManager
{
  GFL_FORBID_COPY_AND_ASSIGN(SessionPacketManager); //コピーコンストラクタ＋代入禁止

public:

  enum
  {
    MAX_MESSAGE_MANAGER_NUM = 20,//メッセージマネージャ最大登録件数
  };

public:


  //------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  */
  //------------------------------------------------------------------
  SessionPacketManager();

  //------------------------------------------------------------------
  /**
  * @brief   デストラクタ
  */
  //------------------------------------------------------------------
  virtual ~SessionPacketManager();

  //------------------------------------------------------------------
  /**
  * @brief   初期化処理
  * @param[in] pHeap ヒープ
  * @param[in] pInitParam 初期化パラメータ
  */
  //------------------------------------------------------------------
  void Initialize( gfl2::heap::HeapBase* pHeap , SessionPacketInitParam* pInitParam );

  //------------------------------------------------------------------
  /**
  * @brief   終了処理
  */
  //------------------------------------------------------------------
  void Finalize( void );

  //------------------------------------------------------------------
  /**
  * @brief  更新処理。
  *
  * @note   	メインループ毎にポーリングすること
  *			      リスナーに通知されるイベントはすべてこの関数内で実行される
  */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
  * @brief  メッセージ管理クラスを登録
  *
  * @retval true： 登録成功
  * @retval false: 登録失敗
  */
  //------------------------------------------------------------------
  bool AddMessageManager( SessionMessageManagerBase* pManager );


  //------------------------------------------------------------------
  /**
  * @brief  メッセージ管理クラスを削除
  */
  //------------------------------------------------------------------
  void RemoveMessageManager( SessionMessageManagerBase* pManager );

  //------------------------------------------------------------------
  /**
  * @brief  TCPメッセージ送信
  * @param[in] messageID メッセージID
  * @param[in] pMessage  メッセージ本体
  * @param[in] messageSize メッセージサイズ
  */
  //------------------------------------------------------------------
  bool SendTCPMessage( u32 messageID , const u8* pMessage , u16 messageSize );

  //------------------------------------------------------------------
  /**
  * @brief  UDPメッセージ送信
  * @param[in] messageID メッセージID
  * @param[in] pMessage  メッセージ本体
  * @param[in] messageSize メッセージサイズ
  */
  //------------------------------------------------------------------
  bool SendUDPMessage( u32 messageID , const u8* pMessage , u16 messageSize );

private:
  //------------------------------------------------------------------
  /**
  * @brief  UDPデータ番号からメッセージIDに変換
  * @param[in] dataIndex UDPデータIndex
  * @retval messageID メッセージID
  */
  //------------------------------------------------------------------
  u16 ConvertUDPDataIndexToMessageID( u16 dataIndex );

private:

  gfl2::heap::HeapBase* m_pHeap;  //!<リクエスト実行用ヒープ
  gfl2::util::FixedSizeContainer<SessionMessageManagerBase*,MAX_MESSAGE_MANAGER_NUM>			m_messageManagerArray; //!<メッセージマネージャ
  SessionPacketIOBase* m_pUDPPacketIO;//!<UDP送受信クラス
  SessionPacketIOBase* m_pTCPPacketIO;//!<TCP送受信クラス
  SessionPacketDatagram m_UDPDatagram;//!<UDPパケットデータグラム
  gfl2::util::FixedSizeContainer<u16,MAX_TCP_MESSAGE_NUM>			m_TCPDataSizeArray;
  void* m_pRecvPacketBuffer;
};



GFL_NAMESPACE_END(Session)
GFL_NAMESPACE_END(gflnet2)



#endif // __GFLNET2_SESSION_PACKET_MANAGER_H__


