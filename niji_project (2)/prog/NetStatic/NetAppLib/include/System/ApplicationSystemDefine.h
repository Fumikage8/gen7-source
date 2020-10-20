// ============================================================================
/*
 * @file ApplicationSystemDefine.h
 * @brief アプリケーションシステム側で使用する定数の定義ファイルです。
 * @date 2015.09.01
 */
// ============================================================================
#if !defined( APPLICATION_SYSTEM_DEFINE_H_INCLUDE )
#define APPLICATION_SYSTEM_DEFINE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)


static const u32 DEFAULT_RENDERING_MANAGER_COUNT    = 1;
static const u32 DEFAULT_MESSAGE_DATA_COUNT         = 4;
static const u32 DEFAULT_WORD_SET_WORD_MAX          = 8;
static const u32 DEFAULT_WORD_SET_BUFFER_LENGTH     = 64;


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)

#endif // APPLICATION_SYSTEM_DEFINE_H_INCLUDE
