//======================================================================
/**
 * @file Float_test.cpp
 * @date 2015/07/16 20:34:45
 * @author araki_syo
 * @brief Floatのテストコード
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined GF_PLATFORM_CTR
#include <nn/nlib/testing/testing.h>
#else
#include <gtest/gtest.h>
#endif

#include <math/include/gfl2_Float.h>

TEST(Float, Ceil){
  EXPECT_FLOAT_EQ(1.0f, gfl2::math::Float::Ceil(1.0f, 0));
  EXPECT_FLOAT_EQ(0.001f, gfl2::math::Float::Ceil(0.0001f, 3));
  EXPECT_FLOAT_EQ(0.0002f, gfl2::math::Float::Ceil(0.00011f, 4));
  EXPECT_FLOAT_EQ(0.00020f, gfl2::math::Float::Ceil(0.00019999f, 5));
}

TEST(Float, Floor){
  EXPECT_FLOAT_EQ(1.0f, gfl2::math::Float::Floor(1.0f, 0));
  EXPECT_FLOAT_EQ(0.0001f, gfl2::math::Float::Floor(0.00019999f, 4));
  EXPECT_FLOAT_EQ(0.0f, gfl2::math::Float::Floor(0.0009999f, 3));
  EXPECT_DOUBLE_EQ(99.71f, gfl2::math::Float::Floor(99.711113f, 2));
}

TEST(Float, Round){
  EXPECT_FLOAT_EQ(1.0f, gfl2::math::Float::Round(1.0f, 0));
  EXPECT_FLOAT_EQ(1.0f, gfl2::math::Float::Round(0.5f, 0));
  EXPECT_FLOAT_EQ(0.0f, gfl2::math::Float::Round(0.049f, 1));
  EXPECT_FLOAT_EQ(0.5f, gfl2::math::Float::Round(0.495f, 2));
  EXPECT_FLOAT_EQ(0.49f, gfl2::math::Float::Round(0.494f, 2));
}

TEST(Float, powf){
  EXPECT_FLOAT_EQ(0.01f, powf(10.0f, -2));
}