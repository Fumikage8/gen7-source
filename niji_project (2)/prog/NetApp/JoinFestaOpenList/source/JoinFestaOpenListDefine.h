// ============================================================================
/*
 * @file JoinFestaOpenListDefine.h
 * @brief 開催選択アプリで使用する定数等の定義ファイルです。
 * @date 2015.08.21
 */
// ============================================================================
#if !defined( JOINFESTAOPENLISTDEFINE_H_INCLUDE )
#define JOINFESTAOPENLISTDEFINE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaOpenList)


static const u32  JOINFESTAOPENLIST_APP_HEAP_SIZE                     = 1024 * 1024 *  1;   //  1 MB
static const u32  JOINFESTAOPENLIST_APP_DEVICE_HEAP_SIZE              = 1024 * 1024 * 20;   // 20 MB



GFL_NAMESPACE_END(JoinFestaOpenList)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAOPENLISTDEFINE_H_INCLUDE
