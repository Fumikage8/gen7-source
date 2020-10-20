#if PM_DEBUG

#if !defined(__IMAGEDB_TEST_TICK_H_INCLUDED__)
#define __IMAGEDB_TEST_TICK_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    ImageDBTestTick.h
 * @date    2016/10/24 17:44:39
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined(GF_PLATFORM_CTR)

#include <nn.h>

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//----------------------------------------------------------------------
// @brief   Tick
//----------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

enum {
  TICK_ID_IMGDB_SAVE,
  TICK_ID_PHOTO_SHOOT,
  TICK_ID_IMGDB_SEARCH,
  TICK_ID_IMGDB_LOAD,
  TICK_ID_IMGDB_LOAD_THUMB,
  TICK_ID_IMGDB_LOAD_THUMB_TOTAL,
  TICK_ID_NUM,
};

class DebugTick
{
public:
  static void StartTick( u8 idx );
  static void EndTick( u8 idx );

  static void DumpTick( u8 idx, const char* name );

private:
  static void ResetTick( u8 idx );

private:
  static const u8 TICK_MAX = 32;

  static nn::os::Tick    m_start[TICK_MAX];
  static nn::os::Tick    m_end[TICK_MAX];
};

GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )

#endif // defined(GF_PLATFORM_CTR)

#endif // __IMAGEDB_TEST_TICK_H_INCLUDED__

#endif // #if PM_DEBUG
