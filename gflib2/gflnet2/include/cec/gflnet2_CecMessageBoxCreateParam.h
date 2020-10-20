// ============================================================================
/*
 * @file gflnet2_CecMessageBoxCreateParam.h
 * @brief すれ違い通信のメッセージボックスを作成するパラメータです。
 * @date 2015.02.13
 */
// ============================================================================
#if !defined( GFLNET2_CECMESSAGEBOXCREATEPARAM_H_INCLUDE )
#define GFLNET2_CECMESSAGEBOXCREATEPARAM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)

enum
{
  HMACKEY_LENGTH  = 32,
  ICON_LENGTH     = 48 * 48 * 2,
  NAME_LENGTH     = 64,
};

typedef struct
{
  u32         titleID;                      // すれ違い通信ID
  u32         privateID;                    // 送受信ボックスをオープンする為に必要なキー

  size_t      sendDataSize;                 // 送信データの最大サイズ
  size_t      sendIconSize;                 // 送信アイコンの最大サイズ
  size_t      sendInfoTextSize;             // 送信情報テキストの最大サイズ

  u32         recvPacketLength;             // ライブラリ内で管理する受信パケット配列の長さ
  size_t      recvDataSize;                 // 受信データの最大サイズ
  size_t      recvIconSize;                 // 受信アイコンの最大サイズ
  size_t      recvInfoTextSize;             // 受信情報テキストの最大サイズ

  int         threadPriority;               // ライブラリ内で使用するスレッドのプライオリティ

} CecSystemInitParam;


typedef struct
{
  char        hmackey[ HMACKEY_LENGTH ];    // メッセージを読み出す時に必要なキー
  u8          icon[ ICON_LENGTH ];          // メッセージボックスのアイコンデータ
  size_t      iconSize;                     // アイコンデータのサイズ
  wchar_t     name[ NAME_LENGTH ];          // アプリ名
  size_t      nameSize;                     // アプリ名のサイズ
  size_t      recvBoxSize;                  // 受信ボックスの最大サイズ
  size_t      sendBoxSize;                  // 送信ボックスの最大サイズ
  size_t      recvMessageCount;             // 受信ボックスの最大保存メッセージ数
  size_t      sendMessageCount;             // 送信ボックスの最大保存メッセージ数
} CecMessageBoxCreateCoreParam;


GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_CECMESSAGEBOXCREATEPARAM_H_INCLUDE
