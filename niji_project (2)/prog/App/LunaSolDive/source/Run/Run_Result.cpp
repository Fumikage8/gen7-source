//======================================================================
/**
 * @file Run_Result.cpp
 * @date 2017/01/17 11:24:38
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <algorithm>
#include <cmath>
#include <debug/include/gfl2_DebugPrint.h>
#include "./Result.h"
#include "../Assert.h"
#if PM_DEBUG
#include "../Debug/Parameter.h"
#include "../Debug/Timer.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Run{
// constructor
Result::Result()
    : m_Type(DISTORTION_TYPE__SIZE),
      m_Rarity(DISTORTION_RARITY__NONE),
      m_Distance(0.0f){}
// initialize
void Result::Initialize(
            const DistortionType& type,
            const DistortionRarity& rarity,
            const f32& distance)
{
  m_Type = type;
  LUNA_SOL_DIVE_ASSERT(m_Type != DISTORTION_TYPE__SIZE);
  m_Rarity = rarity;
  LUNA_SOL_DIVE_ASSERT(m_Rarity != DISTORTION_RARITY__NONE);
  m_Distance = distance;
  LUNA_SOL_DIVE_ASSERT(m_Distance >= 0.0f);
#if PM_DEBUG
  // output
  GFL_PRINT("LunaSolDive Result\n");
  GFL_PRINT("Time    : %ls\n", Debug::Parameter::Get().pTimer->ElapsedTimeStr());
  GFL_PRINT("Distance: %.5f[cm]\n", RawDistance());
  GFL_PRINT("Type    : %s\n",
          (type == DISTORTION_TYPE__UB)? "UB":
          (type == DISTORTION_TYPE__CLIFF)? "崖":
          (type == DISTORTION_TYPE__WATER)? "水":
          (type == DISTORTION_TYPE__CAVE)? "洞窟":
          (type == DISTORTION_TYPE__PLAINS)? "平地":
          "Error");
  GFL_PRINT("Rarity  : %s\n",
          (rarity == DISTORTION_RARITY__0)? "1":
          (rarity == DISTORTION_RARITY__1)? "2":
          (rarity == DISTORTION_RARITY__2)? "3":
          (rarity == DISTORTION_RARITY__3)? "4":
          "Error");
#endif// PM_DEBUG
}
// Terminate
void Result::Terminate()
{
  m_Type = DISTORTION_TYPE__SIZE;
  m_Rarity =  DISTORTION_RARITY__NONE;
  m_Distance = 0.0f;
}
// Distortion Type
const DistortionType& Result::Type() const
{
  return m_Type;
}
// Distortion Rarity
const DistortionRarity& Result::Rarity() const
{
  return m_Rarity;
}
// Travel Distance (raw)
const f32& Result::RawDistance() const
{
  return m_Distance;
}
// Travel Distance in 16bits integer
u32 Result::Distance() const
{
#if PM_DEBUG
  if(Debug::Parameter::Get().isOutputDistanceOverwritten)
  {
    return Debug::Parameter::Get().distanceForOverwriting;
  }
#endif// PM_DEBUG
 // [cm] -> [m] (Rounded Up)
 // Fit in 16bits
  return std::min(
              static_cast<u32>(std::ceil(m_Distance * 1.0e-2f)),
              0xffffu);
}
}// namespace Run
}// namespace LunaSolDive
