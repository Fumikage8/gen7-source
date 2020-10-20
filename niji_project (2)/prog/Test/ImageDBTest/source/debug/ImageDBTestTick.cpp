#if PM_DEBUG

//======================================================================
/**
 * @file    ImageDBTestTick.cpp
 * @date    2016/10/24 17:50:37
 * @author  fukushima_yuya
 * @brief   ImageDBテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined(GF_PLATFORM_CTR)

#include "ImageDBTestTick.h"

#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( ImageDB )

nn::os::Tick DebugTick::m_start[TICK_MAX];
nn::os::Tick DebugTick::m_end[TICK_MAX];

void DebugTick::StartTick( u8 idx )
{
  ResetTick( idx );

  m_start[idx] = nn::os::Tick::GetSystemCurrent();
}

void DebugTick::EndTick( u8 idx )
{
  m_end[idx] = nn::os::Tick::GetSystemCurrent();
}

void DebugTick::DumpTick( u8 idx, const char* name )
{
  nn::os::Tick result = m_end[idx] - m_start[idx];

  GFL_PRINT( "%s_Tick : Result\n", name );
  GFL_PRINT( " milli_sec=[%ld]\n", result.ToMilliSeconds() );
  GFL_PRINT( " micro_sec=[%ld]\n", result.ToMicroSeconds() );
}

void DebugTick::ResetTick( u8 idx )
{
  m_start[idx] = nn::os::Tick();
  m_end[idx] = nn::os::Tick();
}

GFL_NAMESPACE_END( ImageDB )
GFL_NAMESPACE_END( Test )


#endif // defined(GF_PLATFORM_CTR)

#endif // #if PM_DEBUG
