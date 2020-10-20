//======================================================================
/**
 * @file Performance_test.h
 * @date 2015/12/25 10:18:04
 * @author araki_syo
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined(GF_PLATFORM_CTR)

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

void TinyMT32PerformanceTest(u32 seed, u32 number);
void TinyMT64PerformanceTest(u64 seed, u32 number);
void SFMT32PerformanceTest(u32 seed, u32 number);
void SFMT64PerformanceTest(u32 seed, u32 number);
void PrintContextSize();

#endif // GF_PLATFORM_CTR
