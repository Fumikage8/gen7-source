// ============================================================================
/*
 * @file GameSyncDefine.h
 * @brief ゲームシンクアプリで使用する定数等の定義ファイルです。
 * @date 2015.08.21
 */
// ============================================================================
#if !defined( GAMESYNCDEFINE_H_INCLUDE )
#define GAMESYNCDEFINE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GameSync)


static const u32  GAMESYNC_APP_HEAP_SIZE                     = 1024 * 1024 *  1;   //  1 MB
static const u32  GAMESYNC_APP_DEVICE_HEAP_SIZE              = 1024 * 1024 * 20;   // 20 MB



GFL_NAMESPACE_END(GameSync)
GFL_NAMESPACE_END(NetApp)

#endif // GAMESYNCDEFINE_H_INCLUDE
