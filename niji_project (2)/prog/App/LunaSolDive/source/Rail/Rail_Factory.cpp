//======================================================================
/**
 * @file Factory.cpp
 * @date 2016/10/21 11:49:59
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <System/include/GflUse.h>
#include "./Factory.h"
#include "./Parameter.h"
#include "./PartStorage.h"
#include "../Assert.h"
#if PM_DEBUG
#include "../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Rail{
// static function
// Get f32 (range [0:1]) at random
static f32 GetRandomF32();
// Get Angle (range [-π:π]) at random
static f32 GetRandomAngle();
// Get Angle at random
static f32 GetAngle(
            const PartShape& shape);
// constructor
Factory::Factory()
    : m_PartCount(0){}
// Initialize
void Factory::Initialize(){}
// Terminate
void Factory::Terminate()
{
  // reset parameter
  m_PartCount = 0;
}
// Next Part
PartSetUpParameter Factory::Next()
{
  PartSetUpParameter result;
#if PM_DEBUG
  if(Debug::Parameter::Get().isRailShapeFixed
          || Debug::Parameter::Get().isRailAngleFixed)
  {
    result.shape =
            Debug::Parameter::Get().isRailShapeFixed
            ? Debug::Parameter::Get().railShapeInFixedMode
            : static_cast<PartShape>(System::GflUse::GetPublicRand(PART_SHAPE__SIZE));
    result.angle =
            Debug::Parameter::Get().isRailAngleFixed
            ? Debug::Parameter::Get().railAngleInFixedMode
            : GetAngle(result.shape);
  }
  else
#endif// PM_DEBUG
  if(m_PartCount < Parameter::Get().InitialStraightNumber())
  {
    result.shape = PART_SHAPE__STRAIGHT;
    result.angle = GetAngle(result.shape);
  }
  else
  {
    result.shape = static_cast<PartShape>(System::GflUse::GetPublicRand(PART_SHAPE__SIZE));
    result.angle = GetAngle(result.shape);
  }
  LUNA_SOL_DIVE_ASSERT(result.shape != PART_SHAPE__SIZE);
  // increment Part Count
  ++m_PartCount;
  return result;
}
// Get f32 (range [0:1]) at random
static f32 GetRandomF32()
{
  return  static_cast<f32>(System::GflUse::GetPublicRand()) / 0xffffffff;
}
// Get angle (range [-π:π]) at random
static f32 GetRandomAngle()
{
  return PI * (2.0f * GetRandomF32() - 1.0f);
}
// Get Angle at random
static f32 GetAngle(
            const PartShape& shape)
{
  if(shape == PART_SHAPE__STRAIGHT)
  {// straight
    return GetRandomAngle();
  }
  else
  {// right curve
    const f32 offset = -PI / 2.0f;
    const f32 min = Parameter::Get().CurveForbiddenAngleUpperRange();
    const f32 max = PI - Parameter::Get().CurveForbiddenAngleLowerRange();
    const s32 sign =
            (System::GflUse::GetPublicRand(2) == 0)
            ? -1
            : 1;
    return offset + sign * (min + (max - min) * GetRandomF32());
  }
}
}// namespace Rail
}// namespace LunaSolDive
