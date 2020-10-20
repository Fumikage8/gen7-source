// ============================================================================
/*
 * @file ApplicationSystemTypedef.h
 * @brief NetApplicationSystemの型定義ファイルです。
 * @date 2015.03.26
 */
// ============================================================================
#if !defined( APPLICATIONSYSTEMTYPEDEF_H_INCLUDE )
#define APPLICATIONSYSTEMTYPEDEF_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(System)

enum
{
  FRAME_ID_INVALID      = -1,
  FRAME_ID_START        = 0,
};

enum
{
  FRAME_RESULT_INVALID  = -1,
};

typedef   s32   FrameID;
typedef   s32   FrameResult;


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(NetAppLib)

#endif // APPLICATIONSYSTEMTYPEDEF_H_INCLUDE
