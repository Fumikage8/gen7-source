//======================================================================
/**
 * @file Rail_Parameter.cpp
 * @date 2017/02/22 18:52:58
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_MathCommon.h>
#include "./Parameter.h"
#include "../Assert.h"
#if PM_DEBUG
#include "../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Rail{
// initialize
void Parameter::Initialize(
            gfl2::heap::HeapBase* pHeap)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pInstance == NULL);
  pInstance = GFL_NEW(pHeap) Parameter();
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
#if PM_DEBUG
  Debug::Parameter::Get().pRailParameter = pInstance;
#endif// PM_DEBUG
}
// Terminate
void Parameter::Terminate()
{
  GFL_SAFE_DELETE(pInstance);
}
// getter
const Parameter& Parameter::Get()
{
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
  return *pInstance;
}
// Instance
Parameter* Parameter::pInstance = NULL;
// constructor
Parameter::Parameter()
{
  Reset();
}
// Reset
void Parameter::Reset()
{
  m_Radius = 450.0f;
  m_InitialStraightNumber = 15;
  // Curve Forbidden Angle
  m_CurveForbiddenAngleUpperRange = gfl2::math::ConvDegToRad(25.0f);
  m_CurveForbiddenAngleLowerRange = gfl2::math::ConvDegToRad(30.0f);
}
// Radius
const f32& Parameter::Radius() const
{
  return m_Radius;
}
// Number of initial straight rail
const u32& Parameter::InitialStraightNumber() const
{
  return m_InitialStraightNumber;
}
// Curve Forbidden Angle: Upper Range
const f32& Parameter::CurveForbiddenAngleUpperRange() const
{
  return m_CurveForbiddenAngleUpperRange;
}
// Curve Forbidden Angle: Lower Range
const f32& Parameter::CurveForbiddenAngleLowerRange() const
{
  return m_CurveForbiddenAngleLowerRange;
}
}// namespace Rail
}// namespace LunaSolDive
