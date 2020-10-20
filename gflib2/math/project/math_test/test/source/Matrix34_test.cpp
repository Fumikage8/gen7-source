//======================================================================
/**
 * @file Matrix34_test.cpp
 * @date 2015/07/16 19:41:17
 * @author araki_syo
 * @brief Matrix34のテストコード
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined GF_PLATFORM_CTR
#include <nn/nlib/testing/testing.h>
#else
#include <gtest/gtest.h>
#endif

#include <math/include/gfl2_Matrix34.h>

TEST(Matrix34, TransformVec){
  gfl2::math::Matrix34 id = gfl2::math::Matrix34::GetIdentity();

  gfl2::math::Vector4 src = gfl2::math::Vector4(1.0f, 2.0f, 3.0f, 4.0f);

  gfl2::math::Vector4 dst = id.TransformVec(src);

  EXPECT_FLOAT_EQ(1.0f, dst.GetX());
  EXPECT_FLOAT_EQ(2.0f, dst.GetY());
  EXPECT_FLOAT_EQ(3.0f, dst.GetZ());
  EXPECT_FLOAT_EQ(1.0f, dst.GetW());

  gfl2::math::Matrix34 mat = gfl2::math::Matrix34( gfl2::math::Vector4(0.0f, 0.0f, 0.0f, 1.0f),
                                                   gfl2::math::Vector4(0.0f, 0.0f, 1.0f, 0.0f),
                                                   gfl2::math::Vector4(0.0f, 1.0f, 0.0f, 0.0f)
                                                   );

  dst = mat.TransformVec(src);

  EXPECT_FLOAT_EQ(4.0f, dst.GetX());
  EXPECT_FLOAT_EQ(3.0f, dst.GetY());
  EXPECT_FLOAT_EQ(2.0f, dst.GetZ());
  EXPECT_FLOAT_EQ(1.0f, dst.GetW());

  gfl2::math::Matrix34 sum = id + mat;

  dst = sum.TransformVec(src);

  EXPECT_FLOAT_EQ(5.0f, dst.GetX());
  EXPECT_FLOAT_EQ(5.0f, dst.GetY());
  EXPECT_FLOAT_EQ(5.0f, dst.GetZ());
  EXPECT_FLOAT_EQ(1.0f, dst.GetW());
}

TEST(Matrix34, GetElemRotationZYX)
{
  gfl2::math::Matrix34 rot = gfl2::math::Matrix34::GetRotationZYX(PI/4.0f,PI/4.0f,PI/4.0f);

  gfl2::math::Vector3 v = rot.GetElemRotationZYX();
  EXPECT_FLOAT_EQ(PI/4.0f, v.GetX());
  EXPECT_FLOAT_EQ(PI/4.0f, v.GetY());
  EXPECT_FLOAT_EQ(PI/4.0f, v.GetZ());

  rot = gfl2::math::Matrix34::GetRotationZYX(PI/6.0f,PI/4.0f,PI/3.0f);
  v = rot.GetElemRotationZYX();
  EXPECT_FLOAT_EQ(PI/6.0f, v.GetZ());
  EXPECT_FLOAT_EQ(PI/4.0f, v.GetY());
  EXPECT_FLOAT_EQ(PI/3.0f, v.GetX());

  rot = gfl2::math::Matrix34::GetRotationZYX(PI/2.0f,PI/4.0f,PI/3.0f);
  v = rot.GetElemRotationZYX();
  EXPECT_FLOAT_EQ(PI/2.0f, v.GetZ());
  EXPECT_FLOAT_EQ(PI/4.0f, v.GetY());
  EXPECT_FLOAT_EQ(PI/3.0f, v.GetX());

  rot = gfl2::math::Matrix34::GetRotationZYX(-PI/2.0f,PI/4.0f,PI/3.0f);
  v = rot.GetElemRotationZYX();
  EXPECT_FLOAT_EQ(-PI/2.0f, v.GetZ());
  EXPECT_FLOAT_EQ(PI/4.0f, v.GetY());
  EXPECT_FLOAT_EQ(PI/3.0f, v.GetX());

  rot = gfl2::math::Matrix34::GetRotationZYX(3.0f * PI/2.0f,PI/4.0f,PI/3.0f);
  v = rot.GetElemRotationZYX();
  EXPECT_FLOAT_EQ(-PI/2.0f, v.GetZ());
  EXPECT_FLOAT_EQ(PI/4.0f, v.GetY());
  EXPECT_FLOAT_EQ(PI/3.0f, v.GetX());

  rot = gfl2::math::Matrix34::GetRotationZYX(-3.0f * PI/2.0f,PI/4.0f,PI/3.0f);
  v = rot.GetElemRotationZYX();
  EXPECT_FLOAT_EQ(PI/2.0f, v.GetZ());
  EXPECT_FLOAT_EQ(PI/4.0f, v.GetY());
  EXPECT_FLOAT_EQ(PI/3.0f, v.GetX());

  rot = gfl2::math::Matrix34::GetRotationZYX(PI/6.0f,PI/2.0f,PI/3.0f);
  v = rot.GetElemRotationZYX();
  EXPECT_FLOAT_EQ(-PI/6.0f, v.GetZ());
  EXPECT_FLOAT_EQ(PI/2.0f, v.GetY());
  EXPECT_FLOAT_EQ(0.0f, v.GetX());

  rot = gfl2::math::Matrix34::GetRotationZYX(PI/6.0f,-PI/2.0f,PI/3.0f);
  v = rot.GetElemRotationZYX();
  EXPECT_FLOAT_EQ(PI/2.0f, v.GetZ());
  EXPECT_FLOAT_EQ(-PI/2.0f, v.GetY());
  EXPECT_FLOAT_EQ(0.0f, v.GetX());

  rot = gfl2::math::Matrix34::GetRotationZYX(0.0f,PI/2.0f,0.0f);
  v = rot.GetElemRotationZYX();
  EXPECT_FLOAT_EQ(0.0f, v.GetZ());
  EXPECT_FLOAT_EQ(PI/2.0f, v.GetY());
  EXPECT_FLOAT_EQ(0.0f, v.GetX());

  rot = gfl2::math::Matrix34::GetRotationZYX(-PI,-PI/2.0f,5.0f*PI/6.0f);
  v = rot.GetElemRotationZYX();
  EXPECT_FLOAT_EQ(PI/6.0f, v.GetZ());
  EXPECT_FLOAT_EQ(-PI/2.0f, v.GetY());
  EXPECT_FLOAT_EQ(0.0f, v.GetX());
}

TEST(Matrix34, GetElemRotationZYX_fix)
{
  gfl2::math::Matrix34 rot = gfl2::math::Matrix34::GetRotationZYX(PI/4.0f,PI/4.0f,PI/4.0f);

  gfl2::math::Vector3 v = rot.GetElemRotationZYX_fix();
  EXPECT_FLOAT_EQ(PI/4.0f, v.GetX());
  EXPECT_FLOAT_EQ(PI/4.0f, v.GetY());
  EXPECT_FLOAT_EQ(PI/4.0f, v.GetZ());

  rot = gfl2::math::Matrix34::GetRotationZYX(PI/6.0f,PI/4.0f,PI/3.0f);
  v = rot.GetElemRotationZYX_fix();
  EXPECT_FLOAT_EQ(PI/6.0f, v.GetZ());
  EXPECT_FLOAT_EQ(PI/4.0f, v.GetY());
  EXPECT_FLOAT_EQ(PI/3.0f, v.GetX());

  rot = gfl2::math::Matrix34::GetRotationZYX(PI/2.0f,PI/4.0f,PI/3.0f);
  v = rot.GetElemRotationZYX_fix();
  EXPECT_FLOAT_EQ(PI/2.0f, v.GetZ());
  EXPECT_FLOAT_EQ(PI/4.0f, v.GetY());
  EXPECT_FLOAT_EQ(PI/3.0f, v.GetX());

  rot = gfl2::math::Matrix34::GetRotationZYX(-PI/2.0f,PI/4.0f,PI/3.0f);
  v = rot.GetElemRotationZYX_fix();
  EXPECT_FLOAT_EQ(-PI/2.0f, v.GetZ());
  EXPECT_FLOAT_EQ(PI/4.0f, v.GetY());
  EXPECT_FLOAT_EQ(PI/3.0f, v.GetX());

  rot = gfl2::math::Matrix34::GetRotationZYX(3.0f * PI/2.0f,PI/4.0f,PI/3.0f);
  v = rot.GetElemRotationZYX_fix();
  EXPECT_FLOAT_EQ(-PI/2.0f, v.GetZ());
  EXPECT_FLOAT_EQ(PI/4.0f, v.GetY());
  EXPECT_FLOAT_EQ(PI/3.0f, v.GetX());

  rot = gfl2::math::Matrix34::GetRotationZYX(-3.0f * PI/2.0f,PI/4.0f,PI/3.0f);
  v = rot.GetElemRotationZYX_fix();
  EXPECT_FLOAT_EQ(PI/2.0f, v.GetZ());
  EXPECT_FLOAT_EQ(PI/4.0f, v.GetY());
  EXPECT_FLOAT_EQ(PI/3.0f, v.GetX());

  rot = gfl2::math::Matrix34::GetRotationZYX(PI/6.0f,PI/2.0f,PI/3.0f);
  v = rot.GetElemRotationZYX_fix();
  EXPECT_FLOAT_EQ(-PI/6.0f, v.GetZ());
  EXPECT_FLOAT_EQ(PI/2.0f, v.GetY());
  EXPECT_FLOAT_EQ(0.0f, v.GetX());

  rot = gfl2::math::Matrix34::GetRotationZYX(PI/6.0f,-PI/2.0f,PI/3.0f);
  v = rot.GetElemRotationZYX_fix();
  EXPECT_FLOAT_EQ(PI/2.0f, v.GetZ());
  EXPECT_FLOAT_EQ(-PI/2.0f, v.GetY());
  EXPECT_FLOAT_EQ(0.0f, v.GetX());

  rot = gfl2::math::Matrix34::GetRotationZYX(0.0f,PI/2.0f,0.0f);
  v = rot.GetElemRotationZYX_fix();
  EXPECT_FLOAT_EQ(0.0f, v.GetZ());  
  EXPECT_FLOAT_EQ(PI/2.0f, v.GetY());
  EXPECT_FLOAT_EQ(0.0f, v.GetX());

  rot = gfl2::math::Matrix34::GetRotationZYX(-PI,-PI/2.0f,5.0f*PI/6.0f);
  v = rot.GetElemRotationZYX_fix();
  EXPECT_FLOAT_EQ(-PI/6.0f, v.GetZ());
  EXPECT_FLOAT_EQ(-PI/2.0f, v.GetY());
  EXPECT_FLOAT_EQ(0.0f, v.GetX());
}
