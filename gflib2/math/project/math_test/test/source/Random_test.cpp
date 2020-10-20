//======================================================================
/**
 * @file Random_test.cpp
 * @date 2015/12/24 18:21:51
 * @author araki_syo
 * @brief 乱数系列のテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if defined GF_PLATFORM_CTR
#include <nn/nlib/testing/testing.h>
#else
#include <gtest/gtest.h>
#endif

#include <math/include/gfl2_math_random.h>

TEST(Random, Sequence){
  gfl2::math::Random* rand = new gfl2::math::Random();
  rand->Initialize((u32)1);
  EXPECT_EQ(2545341989, rand->Next());
  EXPECT_EQ(981918433, rand->Next());
  EXPECT_EQ(3715302833, rand->Next());
  EXPECT_EQ(2387538352, rand->Next());
  EXPECT_EQ(3591001365, rand->Next());
  EXPECT_EQ(3820442102, rand->Next());
  EXPECT_EQ(2114400566, rand->Next());
  EXPECT_EQ(2196103051, rand->Next());
  EXPECT_EQ(2783359912, rand->Next());
  EXPECT_EQ(764534509, rand->Next());
  EXPECT_EQ(643179475, rand->Next());
  EXPECT_EQ(1822416315, rand->Next());

  delete rand;
}

TEST(Random, Resume){
  gfl2::math::Random* rand = new gfl2::math::Random();
  rand->Initialize((u32)1);
  EXPECT_EQ(2545341989, rand->Next());
  EXPECT_EQ(981918433, rand->Next());
  EXPECT_EQ(3715302833, rand->Next());
  EXPECT_EQ(2387538352, rand->Next());
  EXPECT_EQ(3591001365, rand->Next());
  EXPECT_EQ(3820442102, rand->Next());

  gfl2::math::Random::State state = rand->SaveState();
  delete rand;

  rand = new gfl2::math::Random();
  rand->Initialize(state);

  EXPECT_EQ(2114400566, rand->Next());
  EXPECT_EQ(2196103051, rand->Next());
  EXPECT_EQ(2783359912, rand->Next());
  EXPECT_EQ(764534509, rand->Next());
  EXPECT_EQ(643179475, rand->Next());
  EXPECT_EQ(1822416315, rand->Next());

  delete rand;

  EXPECT_EQ(16, sizeof(state));
}

TEST(Random64, Sequence){
  gfl2::math::Random64* rand64 = new gfl2::math::Random64();
  rand64->Initialize((u64)1);
  EXPECT_EQ(15503804787016557143, rand64->Next());
  EXPECT_EQ(17280942441431881838, rand64->Next());
  EXPECT_EQ(2177846447079362065, rand64->Next());
  EXPECT_EQ(10087979609567186558, rand64->Next());
  EXPECT_EQ(8925138365609588954, rand64->Next());
  EXPECT_EQ(13030236470185662861, rand64->Next());
  EXPECT_EQ(4821755207395923002, rand64->Next());
  EXPECT_EQ(11414418928600017220, rand64->Next());
  EXPECT_EQ(18168456707151075513, rand64->Next());
  EXPECT_EQ(1749899882787913913, rand64->Next());
  EXPECT_EQ(2383809859898491614, rand64->Next());
  EXPECT_EQ(4819668342796295952, rand64->Next());
}

TEST(Random64, Resume){
  gfl2::math::Random64* rand64 = new gfl2::math::Random64();
  rand64->Initialize((u64)1);
  EXPECT_EQ(15503804787016557143, rand64->Next());
  EXPECT_EQ(17280942441431881838, rand64->Next());
  EXPECT_EQ(2177846447079362065, rand64->Next());
  EXPECT_EQ(10087979609567186558, rand64->Next());
  EXPECT_EQ(8925138365609588954, rand64->Next());
  EXPECT_EQ(13030236470185662861, rand64->Next());

  gfl2::math::Random64::State state = rand64->SaveState();

  delete rand64;

  rand64 = new gfl2::math::Random64();
  rand64->Initialize(state);

  EXPECT_EQ(4821755207395923002, rand64->Next());
  EXPECT_EQ(11414418928600017220, rand64->Next());
  EXPECT_EQ(18168456707151075513, rand64->Next());
  EXPECT_EQ(1749899882787913913, rand64->Next());
  EXPECT_EQ(2383809859898491614, rand64->Next());
  EXPECT_EQ(4819668342796295952, rand64->Next());

  delete rand64;

  EXPECT_EQ(16, sizeof(state));
}

TEST(SFMTRandom, Sequence){
  gfl2::math::SFMTRandom* sfmt = new gfl2::math::SFMTRandom();
  sfmt->Initialize((u32)1234);
  EXPECT_EQ(3440181298, sfmt->Next());
  EXPECT_EQ(1564997079, sfmt->Next());
  EXPECT_EQ(1510669302, sfmt->Next());
  EXPECT_EQ(2930277156, sfmt->Next());
  EXPECT_EQ(1452439940, sfmt->Next());
  EXPECT_EQ(3796268453, sfmt->Next());
  EXPECT_EQ(423124208, sfmt->Next());
  EXPECT_EQ(2143818589, sfmt->Next());
  EXPECT_EQ(3827219408, sfmt->Next());
  EXPECT_EQ(2987036003, sfmt->Next());
  EXPECT_EQ(2674978610, sfmt->Next());
  EXPECT_EQ(1536842514, sfmt->Next());
}

TEST(SFMTRandom, Sequence64){
  gfl2::math::SFMTRandom* sfmt = new gfl2::math::SFMTRandom();
  sfmt->Initialize((u32)1234);
  EXPECT_EQ(6721611276080709682, sfmt->Next64());
  EXPECT_EQ(12585444554746559478, sfmt->Next64());
  EXPECT_EQ(16304848853923953028, sfmt->Next64());
  EXPECT_EQ(9207630728734989552, sfmt->Next64());
  EXPECT_EQ(12829221948686777296, sfmt->Next64());
  EXPECT_EQ(6600688339407400754, sfmt->Next64());
  EXPECT_EQ(10887302133822135185, sfmt->Next64());
  EXPECT_EQ(2342338980101906157, sfmt->Next64());
  EXPECT_EQ(5886399791876835913, sfmt->Next64());
  EXPECT_EQ(17157548324798079420, sfmt->Next64());
  EXPECT_EQ(3541619318103065525, sfmt->Next64());
  EXPECT_EQ(10607556679147582604, sfmt->Next64());
}
