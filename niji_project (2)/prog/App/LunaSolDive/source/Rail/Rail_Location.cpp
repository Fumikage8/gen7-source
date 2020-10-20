//======================================================================
/**
 * @file Rail_LocaationInformation.cpp
 * @date 2016/11/09 16:43:45
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <math/include/gfl2_MathTriangular.h>
#include <math/include/gfl2_Quaternion.h>
#include "./Location.h"
#include "../Assert.h"

namespace LunaSolDive{
namespace Rail{
// constructor
Location::Location()
    : m_TravelDistance(0.0f),
      m_RailRadius(0.0f){}
// constructor with Initialize
Location::Location(
            const f32& travelDistance,
            const Rail::Directions& object,
            const Rail::Directions& rail,
            const f32& railRadius)
{
  Initialize(
              travelDistance,
              object,
              rail,
              railRadius);
}
// Initialize
void Location::Initialize(
            const f32& travelDistance,
            const Rail::Directions& object,
            const Rail::Directions& rail,
            const f32& railRadius)
{
  m_TravelDistance = travelDistance;
  m_Object = object;
  m_Rail = rail;
  m_RailRadius = railRadius;
  LUNA_SOL_DIVE_ASSERT(m_RailRadius >= 0.0f);
}
// Terminatea
void Location::Terminate()
{
  m_TravelDistance = 0.0f;
  m_Object.Terminate();
  m_Rail.Terminate();
  m_RailRadius = 0.0f;
}
// check if this Location Information is valid
bool Location::IsValid() const
{
  return m_Object.IsValid() && m_Rail.IsValid();
}
// Travel Distance
const f32& Location::TravelDistance() const
{
  return m_TravelDistance;
}
// Location
const gfl2::math::Vector3& Location::Point() const
{
  return m_Object.Point();
}
// Forward Direction
const gfl2::math::Vector3& Location::Forward() const
{
  return m_Object.Forward();
}
// Vertical Direction
const gfl2::math::Vector3& Location::Vertical() const
{
  return m_Object.Vertical();
}
// Horizontal Direction
gfl2::math::Vector3 Location::Horizontal() const
{
  return m_Object.Horizontal();
}
// Rotation Quaternion
gfl2::math::Quaternion Location::RotationQuaternion() const
{
  return m_Object.RotationQuaternion();
}
// Directions
const Directions& Location::Directions() const
{
  return m_Object;
}
// Rail Directions
const Directions& Location::Rail() const
{
  return m_Rail;
}
// Rail Radius
const f32& Location::RailRadius() const
{
  return m_RailRadius;
}
// Radius Rate
f32 Location::RadiusRate() const
{
  return (m_RailRadius != 0.0f)
          ? (m_Object.Point() - m_Rail.Point()).Length() / m_RailRadius
          : 0.0f;
}
// Angle
f32 Location::Angle() const
{
  const gfl2::math::Vector3 displacement = m_Object.Point() - m_Rail.Point();
  return gfl2::math::Atan2Rad(
              displacement.Dot(m_Rail.Horizontal()),
              displacement.Dot(m_Rail.Vertical()));
}
// Point on the cross section
gfl2::math::Vector2 Location::CrossSectionPoint() const
{
  const gfl2::math::Vector3 displacement = m_Object.Point() - m_Rail.Point();
  return gfl2::math::Vector2(
              displacement.Dot(m_Rail.Horizontal()),
              displacement.Dot(m_Rail.Vertical()));
}
// Normalized point on the cross section
gfl2::math::Vector2 Location::NormalizedCrossSectionPoint() const
{
  return (m_RailRadius > 0.0f)
          ? CrossSectionPoint() / m_RailRadius
          : gfl2::math::Vector2::GetZero();
}
// Translation
void Location::Translation(
            const gfl2::math::Vector3& displacement)
{
  m_Object.Translation(displacement);
  m_Rail.Translation(displacement);
}
}// namespace Rail
}// namespace LunaSolDive
