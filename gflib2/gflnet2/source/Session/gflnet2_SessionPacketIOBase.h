//=============================================================================
/**
 * @file   gflnet2_SessionPacketIOBase.h
 * @brief  セッション内送受信基底クラス
 * @author shin kosaka
 */
//=============================================================================
#ifndef __GFLNET2_SESSION_PACKET_IO_BASE_H__
#define __GFLNET2_SESSION_PACKET_IO_BASE_H__
#pragma once

#include <gflnet2/source/Session/gflnet2_SessionPacketIOBase.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_FixedSizeContainer.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(Session)

enum
{
  INVALID_MESSAGE_ID = 0,                 //!<無効なメッセージID
  SESSION_UDP_MESSAGE_ID = 1,             //!<UDPメッセージID
  SESSION_UDP_INNER_MESSAGE_ID_START = 2, //!<UDPの内部メッセージIDの開始番号
};

/***************************************************************************!
 * @brief  セッション内送受信基底クラス
 ***************************************************************************/
class SessionPacketIOBase
{
public:



public:
  //------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  */
  //------------------------------------------------------------------
  SessionPacketIOBase( gfl2::heap::HeapBase* pHeap )
    : m_pHeap( pHeap )
  {};
  //------------------------------------------------------------------
  /**
  * @brief   デストラクタ
  */
  //------------------------------------------------------------------
  virtual ~SessionPacketIOBase(){};

  //------------------------------------------------------------------
  /**
  * @brief  更新処理。
  *
  * @note   	メインループ毎にポーリングすること
  */
  //------------------------------------------------------------------
  virtual void Update( void ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief  メッセージ送信
  * @param[in] messageID メッセージID
  * @param[in] pInput  メッセージ本体
  * @param[in] bufferSize メッセージサイズ
  */
  //------------------------------------------------------------------
  virtual bool InputPacket( u32 messageID , const u8* pInput , u16 bufferSize ) = 0;
  //------------------------------------------------------------------
  /**
  * @brief  メッセージ受信
  * @param[in] pOutput  メッセージ本体
  * @param[in] bufferSize メッセージサイズ
  * @retval メッセージID
  */
  //------------------------------------------------------------------
  virtual u32  OutputPacket( u8* pOutput , u16 bufferSize ) = 0;

protected:

  gfl2::heap::HeapBase* m_pHeap;//!<ヒープ
};



GFL_NAMESPACE_END(Session)
GFL_NAMESPACE_END(gflnet2)



#endif // __GFLNET2_SESSION_PACKET_IO_BASE_H__


