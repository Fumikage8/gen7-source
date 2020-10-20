#ifdef  GF_PLATFORM_CTR

//=============================================================================
/**
 * @file   gflnet2_CTR_UDPPacketIO.h
 * @brief  CTRのUDP送受信クラス
 * @author shin kosaka
 */
//=============================================================================
#ifndef __GFLNET2_CTR_UDP_PACKET_IO_H__
#define __GFLNET2_CTR_UDP_PACKET_IO_H__
#pragma once

#include <gflnet2/source/Session/gflnet2_SessionPacketIOBase.h>
#include <gflnet2/include/p2p/gflnet2_NetGame.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_FixedSizeContainer.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(Session)

/***************************************************************************!
 * @brief  CTRのUDP送受信クラス
 ***************************************************************************/
class CTR_UDPPacketIO : public SessionPacketIOBase
{
public:
  //------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  */
  //------------------------------------------------------------------
  CTR_UDPPacketIO( gfl2::heap::HeapBase* pHeap );
  //------------------------------------------------------------------
  /**
  * @brief   デストラクタ
  */
  //------------------------------------------------------------------
  virtual ~CTR_UDPPacketIO();

  //------------------------------------------------------------------
  /**
  * @brief  更新処理。
  *
  * @note   	メインループ毎にポーリングすること
  */
  //------------------------------------------------------------------
  virtual void Update( void );
  //------------------------------------------------------------------
  /**
  * @brief  メッセージ送信
  * @param[in] messageID メッセージID
  * @param[in] pInput  メッセージ本体
  * @param[in] bufferSize メッセージサイズ
  */
  //------------------------------------------------------------------
  virtual bool InputPacket( u32 messageID , const u8* pInput , u16 bufferSize );
  //------------------------------------------------------------------
  /**
  * @brief  メッセージ受信
  * @param[in] pOutput  メッセージ本体
  * @param[in] bufferSize メッセージサイズ
  * @retval メッセージID
  */
  //------------------------------------------------------------------
  virtual u32  OutputPacket( u8* pOutput , u16 bufferSize );

private:

  gflnet2::p2p::NetGame* m_pNetGame;
  bool m_isInitialized;
};


GFL_NAMESPACE_END(Session)
GFL_NAMESPACE_END(gflnet2)



#endif // __GFLNET2_CTR_UDP_PACKET_IO_H__


#endif