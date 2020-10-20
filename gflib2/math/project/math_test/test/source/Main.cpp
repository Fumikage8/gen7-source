/**
 * @file Main.cpp
 * @date 2015/07/28
 * @author araki_syo
 * @brief テストプロジェクトのエントリポイント
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */

#include <stdio.h>

#ifdef GF_PLATFORM_CTR

#include <nn/os.h>
#include <nn/dbg.h>

#include <nn/nlib/testing/testing.h>

#include "../include/Performance_test.h"

extern "C" int nnMain(int argc, char *argv[])
{
  TinyMT32PerformanceTest((u32)0, 10000);
  TinyMT64PerformanceTest((u64)0, 10000);
  SFMT32PerformanceTest((u32)0, 10000);
  SFMT64PerformanceTest((u32)0, 10000);
  PrintContextSize();

  ::testing::InitNintendoTest(&argc, argv);
  return RUN_ALL_TESTS();
}
#else

#include <gtest/gtest.h>

int main(int argc, char *argv[])
{
  testing::InitGoogleTest(&argc, argv);
  RUN_ALL_TESTS();
  getchar();
}

#endif
