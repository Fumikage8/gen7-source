//======================================================================
/**
 * @file Performance_test.h
 * @date 2015/12/25 9:37:45
 * @author araki_syo
 * @brief MT/TinyMTの処理速度計測コード
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined(GF_PLATFORM_CTR)

#include <debug/include/gfl2_DebugPrint.h>
#include <math/include/gfl2_math_random.h>

#include <nn.h>

void TinyMT32PerformanceTest(u32 seed, u32 number)
{
  gfl2::math::Random* pRandom32 = new gfl2::math::Random();
  pRandom32->Initialize((u32)0);

  nn::os::Tick start, end;

  GFL_PRINT("Tiny Mersenne Twister 32bit :%d\n", number);

  start = nn::os::Tick::GetSystemCurrent();

  for (u32 i = 0; i < number; i++)
  {
    pRandom32->Next();
  }

  end = nn::os::Tick::GetSystemCurrent();

  GFL_PRINT("%ld nanosec spent.\n", (end - start).ToNanoSeconds());
  GFL_PRINT("TinyMT32 internal state size: %d\n", sizeof(*pRandom32));
}

void TinyMT64PerformanceTest(u64 seed, u32 number)
{
  gfl2::math::Random64* pRandom64 = new gfl2::math::Random64();
  pRandom64->Initialize((u64)0);

  nn::os::Tick start, end;

  GFL_PRINT("Tiny Mersenne Twister 64bit :%d\n", number);

  start = nn::os::Tick::GetSystemCurrent();

  for (u32 i = 0; i < number; i++)
  {
    pRandom64->Next();
  }

  end = nn::os::Tick::GetSystemCurrent();

  GFL_PRINT("%ld nanosec spent.\n", (end - start).ToNanoSeconds());
  GFL_PRINT("TinyMT64 internal state size: %d\n", sizeof(*pRandom64));
}

void SFMT32PerformanceTest(u32 seed, u32 number)
{
  gfl2::math::SFMTRandom* pSFMT = new gfl2::math::SFMTRandom();
  pSFMT->Initialize((u32)0);

  nn::os::Tick start, end;

  GFL_PRINT("SFMT 32bit :%d\n", number);

  start = nn::os::Tick::GetSystemCurrent();

  for (u32 i = 0; i < number; i++)
  {
    pSFMT->Next();
  }

  end = nn::os::Tick::GetSystemCurrent();

  GFL_PRINT("%ld nanosec spent.\n", (end - start).ToNanoSeconds());
  GFL_PRINT("SFMT32 internal state size: %d\n", sizeof(*pSFMT));
}

void SFMT64PerformanceTest(u32 seed, u32 number)
{
  gfl2::math::SFMTRandom* pSFMT = new gfl2::math::SFMTRandom();
  pSFMT->Initialize((u32)0);

  nn::os::Tick start, end;

  GFL_PRINT("SFMT 64bit :%d\n", number);

  start = nn::os::Tick::GetSystemCurrent();

  for (u32 i = 0; i < number; i++)
  {
    pSFMT->Next64();
  }

  end = nn::os::Tick::GetSystemCurrent();

  GFL_PRINT("%ld nanosec spent.\n", (end - start).ToNanoSeconds());
  GFL_PRINT("SFMT64 internal state size: %d\n", sizeof(*pSFMT));
}

void PrintContextSize()
{
  GFL_PRINT("TinyMT32 context size : %d\n", sizeof(tinymt32_t));
  GFL_PRINT("TinyMT64 context size : %d\n", sizeof(tinymt64_t));
  GFL_PRINT("SFMT context size : %d\n", sizeof(sfmt_t));
}

#endif // GF_PLATFORM_CTR
