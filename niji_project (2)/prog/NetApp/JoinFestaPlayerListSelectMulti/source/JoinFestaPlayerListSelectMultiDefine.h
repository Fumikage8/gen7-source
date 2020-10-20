// ============================================================================
/*
 * @file JoinFestaPlayerListSelectMultiDefine.h
 * @brief プレイヤー複数選択アプリで使用する定数等の定義ファイルです。
 * @date 2015.08.21
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTMULTIDEFINE_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTMULTIDEFINE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectMulti)


static const u32  JOINFESTAPLAYERLISTSELECTMULTI_APP_HEAP_SIZE                     = 1024 * 1024 *  1;   //  1 MB
static const u32  JOINFESTAPLAYERLISTSELECTMULTI_APP_DEVICE_HEAP_SIZE              = 1024 * 1024 * 10;   // 10 MB


static const u32  SELECT_PLAYER_MAX = 3;


GFL_NAMESPACE_END(JoinFestaPlayerListSelectMulti)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPLAYERLISTSELECTMULTIDEFINE_H_INCLUDE
