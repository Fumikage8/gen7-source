#if defined GF_PLATFORM_CTR
#include <nn/nlib/testing/testing.h>
#else
#include <gtest/gtest.h>
#endif

#include <math/include/gfl2_Vector3.h>

TEST(Vector3Test, Dot){
  gfl2::math::Vector3 one = gfl2::math::Vector3(1.0f, 1.0f, 1.0f);
  EXPECT_FLOAT_EQ(sqrt(3.0f), one.Length());

  gfl2::math::Vector3 zero = gfl2::math::Vector3(0.0f, 0.0f, 0.0f);
  EXPECT_FLOAT_EQ(0, zero.Length());
}
