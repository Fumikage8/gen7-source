// ============================================================================
/*
 * @file ConnectionDefine.h
 * @brief レギュレーション選択アプリで使用する定数等の定義ファイルです。
 * @date 2015.08.21
 */
// ============================================================================
#if !defined( CONNECTIONDEFINE_H_INCLUDE )
#define CONNECTIONDEFINE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Connection)


static const u32  CONNECTION_APP_HEAP_SIZE                     = 1024 * 24;   //  24 kB
static const u32  CONNECTION_APP_DEVICE_HEAP_SIZE              = 1024 * 1024 * 2;   // 2 MB

static const u32  CONNECTION_APP_HEAP_SIZE_CONNECT                     = 1024 * 1024 *  1;   //  1 MB
static const u32  CONNECTION_APP_DEVICE_HEAP_SIZE_CONNECT              = 1024 * 1024 * 20;   // 20 MB

//! アプリの起動モード
enum BootMode
{
  BOOT_MODE_CHANGE_INTERNET_MODE,     //! インターネット通信に切り替える起動モード
  BOOT_MODE_CHANGE_LOCAL_MODE,        //! ローカル通信に切り替える起動モード
  BOOT_MODE_INTERNET_CONNECTION,      //! インターネット接続を行う起動モード
  BOOT_MODE_INTERNET_DISCONNECTION,   //! インターネット切断を行う起動モード
  BOOT_MODE_FATAL_ERROR               //! フェイタルエラー表示を行う起動モード
};



GFL_NAMESPACE_END(Connection)
GFL_NAMESPACE_END(NetApp)

#endif // CONNECTIONDEFINE_H_INCLUDE
