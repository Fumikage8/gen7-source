#if defined GF_PLATFORM_CTR
#include <nn/nlib/testing/testing.h>
#else
#include <gtest/gtest.h>
#endif

#include <math/include/gfl2_SRT.h>

TEST(SRT, ProdOperator){
  gfl2::math::SRT lhs = gfl2::math::SRT( gfl2::math::Vector3(2.0f, 2.0f, 2.0f),
                                         gfl2::math::Quaternion(gfl2::math::Vector3(1.0f, 1.0f, 2.0f), 1.0f),
                                         gfl2::math::Vector3(1.0f, 2.0f, 3.0f)
                                         );

  gfl2::math::SRT rhs = gfl2::math::SRT( gfl2::math::Vector3(3.0f, 2.0f, 1.0f),
                                         gfl2::math::Quaternion(gfl2::math::Vector3(1.0f, 2.0f, 1.0f), 1.0f),
                                         gfl2::math::Vector3(3.0f, 2.0f, 1.0f)
                                         );

  gfl2::math::SRT res = lhs * rhs;

  gfl2::math::Vector3 src = gfl2::math::Vector3(1.0f, 2.0f, 3.0f);

  gfl2::math::Matrix34 real = res.ToMatrix();

  gfl2::math::Matrix34 expected = lhs.ToMatrix() * rhs.ToMatrix();

  EXPECT_FLOAT_EQ(expected[0][0], real[0][0]);
  EXPECT_FLOAT_EQ(expected[0][1], real[0][1]);
  EXPECT_FLOAT_EQ(expected[0][2], real[0][2]);
  EXPECT_FLOAT_EQ(expected[0][3], real[0][3]);
  EXPECT_FLOAT_EQ(expected[1][0], real[1][0]);
  EXPECT_FLOAT_EQ(expected[1][1], real[1][1]);
  EXPECT_FLOAT_EQ(expected[1][2], real[1][2]);
  EXPECT_FLOAT_EQ(expected[1][3], real[1][3]);
  EXPECT_FLOAT_EQ(expected[2][0], real[2][0]);
  EXPECT_FLOAT_EQ(expected[2][1], real[2][1]);
  EXPECT_FLOAT_EQ(expected[2][2], real[2][2]);
  EXPECT_FLOAT_EQ(expected[2][3], real[2][3]);
}

TEST(SRT, ToMatrix){
  gfl2::math::SRT srt = gfl2::math::SRT( gfl2::math::Vector3(1.0f, 1.0f, 1.0f),
                                         gfl2::math::Quaternion(gfl2::math::Vector3(1.0f, 1.0f, 1.0f), 1.0f),
                                         gfl2::math::Vector3(1.0f, 2.0f, 3.0f)
                                         );

  gfl2::math::Matrix34 mat = srt.ToMatrix();

  EXPECT_FLOAT_EQ(1.0f, mat[0][3]);
  EXPECT_FLOAT_EQ(2.0f, mat[1][3]);
  EXPECT_FLOAT_EQ(3.0f, mat[2][3]);
}