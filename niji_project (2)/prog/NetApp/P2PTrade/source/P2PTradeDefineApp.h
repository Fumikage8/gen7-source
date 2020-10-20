// ============================================================================
/*
 * @file P2PTradeDefineApp.h
 * @brief P2P交換アプリで使用する定数等の定義ファイルです。
 * @date 2015.08.21
 */
// ============================================================================
#if !defined( P2PTRADEDEFINEAPP_H_INCLUDE )
#define P2PTRADEDEFINEAPP_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(P2PTrade)


static const u32  P2PTRADE_APP_HEAP_SIZE                     = 1024 * 1024 *  1;   //  1 MB
static const u32  P2PTRADE_APP_DEVICE_HEAP_SIZE              = 1024 * 1024 * 10;   // 10 MB


//! アプリの起動モード
enum BootMode
{
  BOOT_MODE_WARNING,
  BOOT_MODE_SYNC_SAVE,
  BOOT_MODE_SAVE
};


GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetApp)

#endif // P2PTRADEDEFINEAPP_H_INCLUDE
