﻿#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_SocketSystem.h
 *
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_SOCKET_SYSTEM_H__
#define __GFLNET_SOCKET_SYSTEM_H__
#pragma once

#include <nn/init.h>
#include <nn.h>
#include <nn/http.h>
#include <nn/socket.h>


#include <gflnet2/include/gflnet2_NetworkDefine.h>



#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(gflnet2)  

/**
 * @namespace gflnet2::socket
 * @brief     Socket関連
 */
GFL_NAMESPACE_BEGIN(socket)


//------------------------------------------------------------------
/**
* @brief  Socketライブラリを管理するクラス
*
* @attention 事前にfs, acライブラリを初期化しておく必要がある。
*/
//------------------------------------------------------------------
class SocketSystem
{
  GFL_FORBID_COPY_AND_ASSIGN(SocketSystem); //コピーコンストラクタ＋代入禁止
private:
  //------------------------------------------------------------------
  /**
   * @brief   SocketSystemコンストラクタ＆デストラクタ(隠蔽)
   */
  //------------------------------------------------------------------
  SocketSystem();
  virtual ~SocketSystem();

private:
  static void*  m_socketBuffer;     //!< Socketが内部で使用するメモリ
  static bool   m_isInit;           //!< Socket初期化フラグ

  static const s32  MAX_SESSION        = 4;                      //!< ソケットに割り当てるスレッド数
  static const u32  SOCKET_BUFFER_SIZE = (1024*32*MAX_SESSION);  //!< ソケット通信用に割り当てるヒープメモリ
  static const u32  TOTAL_BUFFER_SIZE  = (SOCKET_BUFFER_SIZE*2); //!< ソケットライブラリに割り当てるヒープメモリ(1socket64KBが必要)
  static const s32  SUB_MEMORY_SIZE    = (1024*16);              //!< ソケット予備用メモリサイズ

public:
  //------------------------------------------------------------------
  /**
   * @brief  SocketSystemの初期化
   *
   * @attention 事前にfs, acを初期化する必要！
   * @attention ※デバイスメモリは指定不可能！
   *
   * @param  heap ソケット用に割り当てるヒープメモリエリア。LOWから確保される。
   */
  //------------------------------------------------------------------
  static nn::Result Initialize( gfl2::heap::HeapBase* heap );

  //------------------------------------------------------------------
  /**
   * @brief  SocketSystemの終了処理
   */
  //------------------------------------------------------------------
  static nn::Result Finalize( void );

  //------------------------------------------------------------------
  /**
   * @brief  内部情報のダンプする（デバッグ用）
   */
  //------------------------------------------------------------------
  static void DumpInfo( void );

  //------------------------------------------------------------------
  /**
   * @brief  エラー内容をコンソールに出力する
   *
   * @param[in] result 出力対象となるリザルト情報
   */
  //------------------------------------------------------------------
  static void PrintError( nn::Result &result );

  //------------------------------------------------------------------
  /**
   * @brief  IPアドレスを文字列に変換する
   *
   * @param[in]  addr 文字列変換対象となるIPアドレス
   *
   * @return アドレス情報文字列
   */
  //------------------------------------------------------------------
  static const char* GetAddressString(nn::socket::InAddr addr);

  //------------------------------------------------------------------
  /**
   * @brief  DNSを取得する
   *
   * @param[out]  dns1 プライマリDNSの格納先
   * @param[out]  dns2 セカンダリDNSの格納先
   */
  //------------------------------------------------------------------
  static void GetDNS(nn::socket::InAddr* dns1 , nn::socket::InAddr* dns2 );
	
private:
  //------------------------------------------------------------------
  /**
   * @brief 内部ワークメモリを確保する
   *
   * @attention デバイスメモリは指定不可能！
   *
   * @param[in,out]  heap           ソケット用に割り当てるヒープメモリエリア。LOWから確保される。
   * @param[in]      needBufferSize Socketが最低限必要とするヒープサイズ
   * @param[out]     alocateSize    確保されたワークエリアサイズ
   *
   * @retval true  確保に成功、または既に確保済み
   * @retval false 確保に失敗した
   */
  //------------------------------------------------------------------
  static bool AllocateWorkBuffer( gfl2::heap::HeapBase* heap, u32 needBufferSize, u32* alocateSize );

  //------------------------------------------------------------------
  /**
   * @brief ワークメモリを解放する
   */
  //------------------------------------------------------------------
  static void FreeWorkBuffer( void );
};

GFL_NAMESPACE_END(socket)
GFL_NAMESPACE_END(gflnet2)

#endif // __GFLNET_SOCKET_SYSTEM_H__

#endif
