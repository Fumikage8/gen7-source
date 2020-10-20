// ============================================================================
/*
 * @file JoinFestaGlobalAttractionDefine.h
 * @brief グローバルアトラクションアプリで使用する定数等の定義ファイルです。
 * @date 2015.08.21
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTIONDEFINE_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTIONDEFINE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)


static const u32  JOINFESTAGLOBALATTRACTION_APP_HEAP_SIZE                     = 1024 * 1024 *  1;   //  1 MB
static const u32  JOINFESTAGLOBALATTRACTION_APP_DEVICE_HEAP_SIZE              = 1024 * 1024 * 10;   // 10 MB



GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAGLOBALATTRACTIONDEFINE_H_INCLUDE
