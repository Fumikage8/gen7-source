//======================================================================
/**
 * @file Gimmick_Collision_Cylinder.cpp
 * @date 2016/11/14 18:48:34
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <algorithm>
#include <cmath>
#include <math/include/gfl2_Matrix34.h>
#include "./Cylinder.h"
#include "../../Assert.h"

namespace LunaSolDive{
namespace Gimmick{
namespace Collision{
// constructor
Cylinder::Cylinder()
    : m_Location(gfl2::math::Vector3::GetZero()),
      m_Axis(gfl2::math::Vector3::GetZero()),
      m_PolarAxis(gfl2::math::Vector3::GetZero()),
      m_Radius(0.0f),
      m_Height(0.0f){}
// Initialize
void Cylinder::Initialize(
            const gfl2::math::Vector3& location,
            const gfl2::math::Vector3& axis,
            const gfl2::math::Vector3& polarAxis,
            const f32& radius,
            const f32& height)
{
  m_Location = location;
  m_Axis = axis;
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_Axis.Length(), 1.0f));
  m_PolarAxis = polarAxis;
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_PolarAxis.Length(), 1.0f));
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsAlmostZero(m_Axis.Dot(m_PolarAxis), EPSILON * 1.0e+1f));
  m_Radius = radius;
  LUNA_SOL_DIVE_ASSERT(m_Radius >= 0.0f);
  m_Height = height;
}
// Terminate
void Cylinder::Terminate()
{
  m_Location = gfl2::math::Vector3::GetZero();
  m_Axis = gfl2::math::Vector3::GetZero();
  m_PolarAxis = gfl2::math::Vector3::GetZero();
  m_Radius = 0.0f;
  m_Height = 0.0f;
}
// Whether the point is in this cylinder or not
bool Cylinder::IsIn(
            const gfl2::math::Vector3& target) const
{
  const gfl2::math::Vector3 displacement = target - m_Location;
  // projection & rejection on Axis
  const f32 projection = m_Axis.Dot(displacement);
  const f32 rejection = (displacement - m_Axis * projection).Length();
  return 0.0f <= projection
              && projection <= m_Height
              && rejection <= m_Radius;
}
// Whether the line intersect or not
bool Cylinder::IsIntersect(
            const gfl2::math::Vector3& beginPoint,
            const gfl2::math::Vector3& endPoint) const
{
  if(gfl2::math::IsAlmostZero((endPoint - beginPoint).Length()))
  {// length of line is equal to zero
    return IsIn(endPoint);
  }
  // transfer origin to begin point
  const gfl2::math::Vector3 qXYZ = endPoint - beginPoint;
  const gfl2::math::Vector3 rXYZ = Location() - beginPoint;
  // element of line orthogonal to axis
  const gfl2::math::Vector3 qXYZOrthogonalToAxis = qXYZ - Axis() * qXYZ.Dot(Axis());
  // x,y,z coordinates -> u,v,w coordinates
  // u direction = axis
  // v direction = element of line orthogonal to axis
  //               or polar axis(line is parallel to axis)
  // origin = begin point
  // i.e. line is in the uv plane
  const gfl2::math::Vector3 uDirection = Axis();
  const gfl2::math::Vector3 vDirection =
          ! gfl2::math::IsAlmostZero(qXYZOrthogonalToAxis.Length())
          ? qXYZOrthogonalToAxis.Normalize()
          : PolarAxis();
  const gfl2::math::Matrix34 convertMatrix = gfl2::math::Matrix34(
              uDirection,
              vDirection,
              uDirection.Cross(vDirection))
              .SetTranslationVector(gfl2::math::Vector3::GetZero());
  const gfl2::math::Vector3 qUVW = convertMatrix * qXYZ;
  const gfl2::math::Vector3 rUVW = convertMatrix * rXYZ;
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsAlmostZero(qUVW.GetZ(), qUVW.Length() * 1.0e-4f));
  // distance between cylinder and uv plane
  if(gfl2::math::FAbs(rUVW.GetZ()) >= Radius())
  {
    return false;
  }
  // project the line in uv plane
  const gfl2::math::Vector2 q(qUVW.GetX(), qUVW.GetY());
  // project the cylinder in uv plane -> aabb
  const f32 projectionRadius =
          gfl2::math::FSqrt(std::pow(Radius(), 2) - std::pow(rUVW.GetZ(), 2));
  const f32 aabbMinU = rUVW.GetX();
  const f32 aabbMaxU = rUVW.GetX() + Height();
  const f32 aabbMinV = rUVW.GetY() - projectionRadius;
  const f32 aabbMaxV = rUVW.GetY() + projectionRadius;
  // intersection judgement between line and aabb
  if(q.GetX() == 0.0f)
  {
    return (aabbMinU < 0.0f && 0.0f < aabbMaxU)
            && ! (aabbMaxV <= 0.0f || q.GetY() <= aabbMinV);
  }
  else if(q.GetY() == 0.0f)
  {
    return (aabbMinV < 0.0f && 0.0f < aabbMaxV)
            && ! (aabbMaxU <= 0.0f || q.GetX() <= aabbMinU);
  }
  else
  {
    const f32 tUmin = std::min(aabbMinU / q.GetX(), aabbMaxU / q.GetX());
    const f32 tUmax = std::max(aabbMinU / q.GetX(), aabbMaxU / q.GetX());
    const f32 tVmin = std::min(aabbMinV / q.GetY(), aabbMaxV / q.GetY());
    const f32 tVmax = std::max(aabbMinV / q.GetY(), aabbMaxV / q.GetY());
    const f32 t0 = std::max(tUmin, tVmin);
    const f32 t1 = std::min(tUmax, tVmax);
    return (t0 < t1)
            && ! (t1 <= 0.0f || 1.0f <= t0);
  }
}
// Detect Collision
f32 Cylinder::Detect(
            const gfl2::math::Vector3& target) const
{
  const gfl2::math::Vector3 displacement = target - m_Location;
  // projection & rejection on Axis
  const f32 projection = m_Axis.Dot(displacement);
  const f32 rejection = (displacement - m_Axis * projection).Length();
  if(0.0f <= projection
              && projection <= m_Height
              && rejection <= m_Radius)
  {
    return (m_Radius > 0.0f)? 1.0f - rejection / m_Radius: 0.0f;
  }
  else
  {
    return 0.0f;
  }
}
// Translation
void Cylinder::Translation(
            const gfl2::math::Vector3& displacement)
{
  m_Location += displacement;
}
// Location
const gfl2::math::Vector3& Cylinder::Location() const
{
  return m_Location;
}
// Axis
const gfl2::math::Vector3&  Cylinder::Axis() const
{
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_Axis.Length(), 1.0f));
  return m_Axis;
}
// Polar Axis
const gfl2::math::Vector3& Cylinder::PolarAxis() const
{
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_PolarAxis.Length(), 1.0f));
  return m_PolarAxis;
}
// Radius
const f32&  Cylinder::Radius() const
{
  return m_Radius;
}
// Height
const f32&  Cylinder::Height() const
{
  return m_Height;
}
}// namespace Collision
}// namespace Gimmick
}// namespace LunaSolDive
