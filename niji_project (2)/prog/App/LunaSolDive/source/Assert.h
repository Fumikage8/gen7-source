//======================================================================
/**
 * @file Assert.h
 * @date 2017/02/02 15:08:00
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__ASSERT_H_INCLUDED__
#define __LUNA_SOL_DIVE__ASSERT_H_INCLUDED__
#pragma once

#include <debug/include/gfl2_Assert.h>

#if GFL_ENABLE_ASSERT
#define LUNA_SOL_DIVE_ASSERT(exp) if(gfl2::debug::GetAssertEnable()) ((exp)? (void(0)): gfl2::debug::GFLassert(__FILE__, __LINE__, false, #exp))
#else
#define LUNA_SOL_DIVE_ASSERT(exp)
#endif// GFL_ENABLE_ASSERT
#endif// __LUNA_SOL_DIVE__ASSERT_H_INCLUDED__
