//======================================================================
/**
 * @file Gimmick_Collision_Hemispere.cpp
 * @date 2016/11/17 18:24:13
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <algorithm>
#include <cmath>
#include <math/include/gfl2_Matrix34.h>
#include "./Hemisphere.h"
#include "../../Assert.h"

namespace LunaSolDive{
namespace Gimmick{
namespace Collision{
// constructor
Hemisphere::Hemisphere()
    : m_Location(gfl2::math::Vector3::GetZero()),
      m_Axis(gfl2::math::Vector3::GetZero()),
      m_PolarAxis(gfl2::math::Vector3::GetZero()),
      m_Radius(0.0f){}
// initialize
void Hemisphere::Initialize(
            const gfl2::math::Vector3& location,
            const gfl2::math::Vector3& axis,
            const gfl2::math::Vector3& polarAxis,
            const f32& radius)
{
  m_Location = location;
  m_Axis = axis;
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_Axis.Length(), 1.0f));
  m_PolarAxis = polarAxis;
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_PolarAxis.Length(), 1.0f));
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsAlmostZero(m_Axis.Dot(m_PolarAxis), EPSILON * 1.0e+1f));
  m_Radius = radius;
  LUNA_SOL_DIVE_ASSERT(m_Radius > 0.0f);
}
// Terminate
void Hemisphere::Terminate()
{
  m_Location = gfl2::math::Vector3::GetZero();
  m_Axis = gfl2::math::Vector3::GetZero();
  m_PolarAxis = gfl2::math::Vector3::GetZero();
  m_Radius = 0.0f;
}
// Whether the point is in this hemisphere or not
bool Hemisphere::IsIn(
            const gfl2::math::Vector3& point) const
{
  const gfl2::math::Vector3 displacement = point - m_Location;
  const f32 projection = m_Axis.Dot(displacement);
  return projection >= 0.0f
              && displacement.Length() <= m_Radius;
}
// Whether the line intersect or not
bool Hemisphere::IsIntersect(
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
  // i.e. line is in the u,v plane
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
  // distance between hemispere & uv plane
  if(gfl2::math::FAbs(rUVW.GetZ()) >= Radius())
  {
    return false;
  }
  // project the hemisphere in uv plane -> hemicircle
  const gfl2::math::Vector2 r(rUVW.GetX(), rUVW.GetY());
  const f32 radius =
          gfl2::math::FSqrt(std::pow(Radius(), 2) - std::pow(rUVW.GetZ(), 2));
  // project the line in uv plane
  if(0.0f <= r.GetX() && qUVW.GetX() <= r.GetX())
  {// line is under the hemicircle
    return false;
  }
  else if(qUVW.GetX() == 0.0f)
  {// line is parallel to section line of hemicircle
    if(r.GetX() < 0.0f && 0.0f < r.GetX() + radius)
    {
      const f32 interceptRadius =
              gfl2::math::FSqrt(std::pow(radius, 2) - std::pow(r.GetX(), 2));
      return std::max(0.0f, r.GetY() - interceptRadius)
              < std::min(qUVW.GetY(), r.GetY() + interceptRadius);
    }
    else
    {
      return false;
    }
  }
  else
  {
    const f32 tBoundary = r.GetX() / qUVW.GetX();
    const gfl2::math::Vector2 p =
            (0.0f <= tBoundary && tBoundary <= 1.0f && qUVW.GetX() > 0.0f)
            ? gfl2::math::Vector2(
                      qUVW.GetX() * tBoundary,
                      qUVW.GetY() * tBoundary)
            : gfl2::math::Vector2::GetZero();
    const gfl2::math::Vector2 q =
            (0.0f <= tBoundary && tBoundary <= 1.0f && qUVW.GetX() < 0.0f)
            ? gfl2::math::Vector2(
                      qUVW.GetX() * tBoundary,
                      qUVW.GetY() * tBoundary)
            : gfl2::math::Vector2(qUVW.GetX(), qUVW.GetY());
    if(gfl2::math::IsAlmostZero((q - p).Length()))
    {
      return (r - q).Length() <= radius;
    }
    const f32 tNearest = gfl2::math::clamp(
                (q - p).Dot(r - p) / std::pow((q - p).Length(), 2),
                0.0f,
                1.0f);
    const gfl2::math::Vector2 neaestPoint =
            p * (1 - tNearest) + q * tNearest;
    return (neaestPoint - r).Length() <= radius;
  }
}
// Translation
void Hemisphere::Translation(
            const gfl2::math::Vector3& displacement)
{
  m_Location += displacement;
}
// Location
const gfl2::math::Vector3& Hemisphere::Location() const
{
  return m_Location;
}
// Axis
const gfl2::math::Vector3& Hemisphere::Axis() const
{
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_Axis.Length(), 1.0f));
  return m_Axis;
}
// Polar Axis
const gfl2::math::Vector3& Hemisphere::PolarAxis() const
{
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_PolarAxis.Length(), 1.0f));
  return m_PolarAxis;
}
// Radius
const f32& Hemisphere::Radius() const
{
  return m_Radius;
}
}// namespace Collision
}// namespace Gimmick
}// namespace LunaSolDive
