//======================================================================
/**
 * @file Aim.cpp
 * @date 2016/11/17 21:28:22
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <math/include/gfl2_MathTriangular.h>
#include "./Aim.h"
#include "../Assert.h"
#include "../Gimmick/Action.h"

namespace LunaSolDive{
namespace Run{
// constructor
Aim::Aim()
    : m_PresentPoint(gfl2::math::Vector2::GetZero()),
      m_PreviousPoint(gfl2::math::Vector2::GetZero()),
      m_PresentInput(gfl2::math::Vector2::GetZero()),
      m_PreviousInput(gfl2::math::Vector2::GetZero()){}
// Initialize
void Aim::Initialize()
{
  // initialize parameter
  m_PresentPoint = gfl2::math::Vector2::GetZero();
  m_PreviousPoint = gfl2::math::Vector2::GetZero();
  m_PresentInput = gfl2::math::Vector2::GetZero();
  m_PreviousInput = gfl2::math::Vector2::GetZero();
}
// Terminate
void Aim::Terminate()
{
  // reset parameter
  m_PresentPoint = gfl2::math::Vector2::GetZero();
  m_PreviousPoint = gfl2::math::Vector2::GetZero();
  m_PresentInput = gfl2::math::Vector2::GetZero();
  m_PreviousInput = gfl2::math::Vector2::GetZero();
}
// Update
void Aim::Update(
            const gfl2::math::Vector2& input,
            const f32& maxDisplacement,
            const Gimmick::Action& gimmickAction)
{
  LUNA_SOL_DIVE_ASSERT(input.Length() < 1.0f + EPSILON);
  LUNA_SOL_DIVE_ASSERT(maxDisplacement >= 0.0f);
  // update input & previous point
  m_PreviousInput = m_PresentInput;
  m_PreviousPoint = m_PresentPoint;
  m_PresentInput = input;
  // movement
  const u32 steps = 5;
  LUNA_SOL_DIVE_ASSERT(steps != 0);
  for(u32 i = 0; i < steps; ++i)
  {
    // move to input point
    const f32 stepDisplacement = maxDisplacement / steps;
    const gfl2::math::Vector2 movement =
            ((input - m_PresentPoint).Length() > stepDisplacement)
            ? (input - m_PresentPoint).Normalize() * stepDisplacement
            : input - m_PresentPoint;
    m_PresentPoint += movement;
    // gravity
    gfl2::math::Vector2 gravity = gfl2::math::Vector2::GetZero();
    for(u32 k = 0; k < gimmickAction.GravityNumber(); ++k)
    {
      const Gimmick::Gravity& gravityInfo = gimmickAction.Gravity(k);
      const gfl2::math::Vector2 displacement = gravityInfo.center - m_PresentPoint;
      const f32 distance = displacement.Length();
      if(gfl2::math::IsAlmostZero(distance)) continue;
      gravity += displacement.Normalize() * gfl2::math::min(
                  distance,
                  gravityInfo.strength / steps);
    }
    m_PresentPoint += gravity;
  }
  // normalize
  if(m_PresentPoint.Length() > 1.0f)
  {
    m_PresentPoint = m_PresentPoint.Normalize();
  }
}
// Overwrite Point (for Drifting Movement)
void Aim::OverwritePoint(
            const gfl2::math::Vector2& point)
{
  m_PresentPoint = point;
}
// Point
const gfl2::math::Vector2& Aim::Point() const
{
  LUNA_SOL_DIVE_ASSERT(m_PresentPoint.Length() < 1.0f + EPSILON);
  return m_PresentPoint;
}
// Point Movement
gfl2::math::Vector2 Aim::PointMovement() const
{
  return m_PresentPoint - m_PreviousPoint;
}
// Input
const gfl2::math::Vector2& Aim::Input() const
{
  LUNA_SOL_DIVE_ASSERT(m_PresentInput.Length() < 1.0f + EPSILON);
  return m_PresentInput;
}
// Input Movement
gfl2::math::Vector2 Aim::InputMovement() const
{
  return m_PresentInput - m_PreviousInput;
}
}// namespace Run
}// namespace LunaSolDive
