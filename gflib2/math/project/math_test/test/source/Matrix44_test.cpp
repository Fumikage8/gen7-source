//======================================================================
/**
 * @file Matrix44_test.cpp
 * @date 2015/07/16 19:51:47
 * @author araki_syo
 * @brief Matrix44のテストコード
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined GF_PLATFORM_CTR
#include <nn/nlib/testing/testing.h>
#else
#include <gtest/gtest.h>
#endif

#include <math/include/gfl2_Matrix44.h>

TEST(Matrix44, Transformvec){
  gfl2::math::Matrix44 id = gfl2::math::Matrix44::GetIdentity();

  gfl2::math::Vector4 src = gfl2::math::Vector4(1.0f, 2.0f, 3.0f, 1.0f);

  gfl2::math::Vector4 dst = id.TransformVec(src);

  EXPECT_FLOAT_EQ(1.0f, dst.GetX());
  EXPECT_FLOAT_EQ(2.0f, dst.GetY());
  EXPECT_FLOAT_EQ(3.0f, dst.GetZ());
  EXPECT_FLOAT_EQ(1.0f, dst.GetW());

  gfl2::math::Vector4 vec = gfl2::math::Vector4(1.0f, 2.0f, 3.0f, 2.0f);

  dst = id.TransformVec(vec);

  EXPECT_FLOAT_EQ(0.5f, dst.GetX());
  EXPECT_FLOAT_EQ(1.0f, dst.GetY());
  EXPECT_FLOAT_EQ(1.5f, dst.GetZ());
  EXPECT_FLOAT_EQ(1.0f, dst.GetW());

  gfl2::math::Matrix44 rev  = gfl2::math::Matrix44( gfl2::math::Vector4(0.0f, 0.0f, 0.0f, 1.0f),
                                                    gfl2::math::Vector4(0.0f, 0.0f, 1.0f, 0.0f),
                                                    gfl2::math::Vector4(0.0f, 1.0f, 0.0f, 0.0f),
                                                    gfl2::math::Vector4(1.0f, 0.0f, 0.0f, 0.0f)
                                                    );

  dst = rev.TransformVec(src);

  EXPECT_FLOAT_EQ(1.0f, dst.GetX());
  EXPECT_FLOAT_EQ(3.0f, dst.GetY());
  EXPECT_FLOAT_EQ(2.0f, dst.GetZ());
  EXPECT_FLOAT_EQ(1.0f, dst.GetW());

  dst = rev.TransformVec(vec);

  EXPECT_FLOAT_EQ(2.0f, dst.GetX());
  EXPECT_FLOAT_EQ(3.0f, dst.GetY());
  EXPECT_FLOAT_EQ(2.0f, dst.GetZ());
  EXPECT_FLOAT_EQ(1.0f, dst.GetW());
}
