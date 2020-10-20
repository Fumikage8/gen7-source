//======================================================================
/**
 * @file Rail_Section.cpp
 * @date 2016/10/07 16:09:15
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <math/include/gfl2_MathTriangular.h>
#include <math/include/gfl2_Quaternion.h>
#include "./Directions.h"
#include "./Section.h"
#include "../Assert.h"

namespace LunaSolDive{
namespace Rail{
// constructor
Section::Section()
    : m_Index(0xffffffff),
      m_BeginPoint(gfl2::math::Vector3::GetZero()),
      m_EndPoint(gfl2::math::Vector3::GetZero()),
      m_Radius(0.0f),
      m_Direction(gfl2::math::Vector3::GetZero()),
      m_VerticalDirection(gfl2::math::Vector3::GetZero()),
      m_BeginCylindricSectionAngle(0.0f),
      m_BeginCylindricSectionGradient(0.0f),
      m_EndCylindricSectionAngle(0.0f),
      m_EndCylindricSectionGradient(0.0f){}
// constructor with Initialize
Section::Section(
            const u32& index,
            const gfl2::math::Vector3& beginPoint,
            const gfl2::math::Vector3& beginNoramalVector,
            const gfl2::math::Vector3& endPoint,
            const gfl2::math::Vector3& endNoramalVector,
            const f32& radius,
            const gfl2::math::Vector3& verticalDirection)
{
  Initialize(
              index,
              beginPoint,
              beginNoramalVector,
              endPoint,
              endNoramalVector,
              radius,
              verticalDirection);
}
// Initialize
void Section::Initialize(
            const u32& index,
            const gfl2::math::Vector3& beginPoint,
            const gfl2::math::Vector3& beginNormalVector,
            const gfl2::math::Vector3& endPoint,
            const gfl2::math::Vector3& endNormalVector,
            const f32& radius,
            const gfl2::math::Vector3& verticalDirection)
{
  LUNA_SOL_DIVE_ASSERT(radius > 0.0f);
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(beginNormalVector.Length(), 1.0f)
              || gfl2::math::IsAlmostZero(beginNormalVector.Length()));
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(endNormalVector.Length(), 1.0f)
              || gfl2::math::IsAlmostZero(endNormalVector.Length()));
  // set Index
  m_Index = index;
  // set Begin & End Point
  m_BeginPoint = beginPoint;
  m_EndPoint = endPoint;
  // set Rail Direction
  LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero((m_EndPoint - m_BeginPoint).Length()));
  m_Direction = (m_EndPoint - m_BeginPoint).Normalize();
  // set Vertical Direction
  m_VerticalDirection = verticalDirection;
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_VerticalDirection.Length(), 1.0f));
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsAlmostZero(m_VerticalDirection.Dot(m_Direction), EPSILON * 1.0e+1f));
  // set Radius
  m_Radius = radius;
  // Begin Cylindric-Section
  if(gfl2::math::IsAlmostZero(beginNormalVector.Length()))
  {// NormalVector is Zero Vector
    m_BeginCylindricSectionAngle = 0.0f;
    m_BeginCylindricSectionGradient = 0.0f;
  }
  else
  {
    const InternalCoordinates internalVector = ToInternalVector(beginNormalVector);
    m_BeginCylindricSectionAngle = internalVector.angle;
    m_BeginCylindricSectionGradient = internalVector.z;
  }
  // End Cylindric-Section
  if(gfl2::math::IsAlmostZero(endNormalVector.Length()))
  {// NormalVector is Zero Vector
    m_EndCylindricSectionAngle = 0.0f;
    m_EndCylindricSectionGradient = 0.0f;
  }
  else
  {
    const InternalCoordinates internalVector = ToInternalVector(endNormalVector);
    m_EndCylindricSectionAngle = internalVector.angle;
    m_EndCylindricSectionGradient = internalVector.z;
  }
}
// check if the Section is valid
bool Section::IsValid() const
{
  return m_Index != 0xffffffff;
}
// Terminate
void Section::Terminate()
{
  m_Index = 0xffffffff;
  m_BeginPoint = gfl2::math::Vector3::GetZero();
  m_EndPoint = gfl2::math::Vector3::GetZero(),
  m_Radius = 0.0f;
  m_Direction = gfl2::math::Vector3::GetZero();
  m_VerticalDirection = gfl2::math::Vector3::GetZero();
  m_BeginCylindricSectionAngle = 0.0f;
  m_BeginCylindricSectionGradient = 0.0f;
  m_EndCylindricSectionAngle = 0.0f;
  m_EndCylindricSectionGradient = 0.0f;
}
// convert: Internal Vector -> World Vector
gfl2::math::Vector3 Section::ToWorldVector(
            const InternalCoordinates& vector) const
{
  const gfl2::math::Quaternion rotation(
              m_Direction,
              vector.angle);
  return m_Direction * vector.z
          + rotation.Transform(m_VerticalDirection * m_Radius * vector.radius_rate);
}
// convert: Internal Point -> World Point
gfl2::math::Vector3 Section::ToWorldPoint(
            const InternalCoordinates& point) const
{
  const gfl2::math::Vector3 vector = ToWorldVector(point);
  return m_BeginPoint + vector;
}
// convert: World Vector -> Internal Vector
InternalCoordinates Section::ToInternalVector(
            const gfl2::math::Vector3& vector) const
{
  const gfl2::math::Matrix34 convertMatrix = gfl2::math::Matrix34(
              VerticalDirection(),
              HorizontalDirection(),
              Direction())
              .SetTranslationVector(gfl2::math::Vector3::GetZero());
  const gfl2::math::Vector3 internalXYZ = convertMatrix * vector;
  const InternalCoordinates result = {
              internalXYZ.GetZ(),
              (! gfl2::math::IsAlmostZero(m_Radius))
                      ? std::hypot(internalXYZ.GetX(), internalXYZ.GetY()) / m_Radius
                      : 0.0f,
              gfl2::math::Atan2Rad(internalXYZ.GetY(), internalXYZ.GetX())};
  return result;
}
// convert: World Point -> Internal Point
InternalCoordinates Section::ToInternalPoint(
            const gfl2::math::Vector3& point) const
{
  const gfl2::math::Vector3 displacement = point - m_BeginPoint;
  return ToInternalVector(displacement);
}
// Z value on the Cylindric-Section of the Begin Side
f32 Section::BeginCylindricSectionZ(
            const f32& radiusRate,
            const f32& angle) const
{
  return m_Radius * radiusRate * m_BeginCylindricSectionGradient
          * gfl2::math::CosRad(angle - m_BeginCylindricSectionAngle);
}
// Z value on the Cylindric-Section of the End Side
f32 Section::EndCylindricSectionZ(
            const f32& radiusRate,
            const f32& angle) const
{
  return (m_EndPoint - m_BeginPoint).Length()
          + m_Radius * radiusRate * m_EndCylindricSectionGradient
                  * gfl2::math::CosRad(angle - m_EndCylindricSectionAngle);
}
// a Point on the Cylindric-Section of the Begin side (x, y, z)
gfl2::math::Vector3 Section::BeginCylindricSection(
            const f32& radiusRate,
            const f32& angle) const
{
  const InternalCoordinates internalPoint = {
              BeginCylindricSectionZ(radiusRate, angle),
              radiusRate,
              angle};
  return ToWorldPoint(internalPoint);
}
// a Point on the Cylindric-Section of the End side (x, y, z)
gfl2::math::Vector3 Section::EndCylindricSection(
            const f32& radiusRate,
            const f32& angle) const
{
  const InternalCoordinates internalPoint = {
              EndCylindricSectionZ(radiusRate, angle),
              radiusRate,
              angle};
  return ToWorldPoint(internalPoint);
}
// check if the Internal Coordinates is in this Section
bool Section::IsIn(
            const InternalCoordinates& point,
            const f32& margin) const
{
  LUNA_SOL_DIVE_ASSERT(point.radius_rate >= - EPSILON);
  const f32 minZ = BeginCylindricSectionZ(
              point.radius_rate,
              point.angle);
  const f32 maxZ = EndCylindricSectionZ(
              point.radius_rate,
              point.angle);
  return point.radius_rate >= - EPSILON
              && point.radius_rate <= 1.0f + margin
              && (point.z - minZ) >= - margin * Length()
              && (point.z - maxZ) <= + margin * Length();
}
// check if the Internal Coordinates is in forward of this Section
bool Section::IsForward(
            const InternalCoordinates& point) const
{
  const f32 criterion = EndCylindricSectionZ(
              point.radius_rate,
              point.angle);
  return point.z > criterion;
}
// check if the Internal Coordinates is in backward of this Section
bool Section::IsBackward(
            const InternalCoordinates& point) const
{
  const f32 criterion = BeginCylindricSectionZ(
              point.radius_rate,
              point.angle);
  return point.z < criterion;
}
// index
const u32& Section::Index() const
{
  return m_Index;
}
// Begin Point (x, y, z)
const gfl2::math::Vector3& Section::BeginPoint() const
{
  return m_BeginPoint;
}
// End Point (x, y, z)
const gfl2::math::Vector3& Section::EndPoint() const
{
  return m_EndPoint;
}
// Length
f32 Section::Length() const
{
  return (m_EndPoint - m_BeginPoint).Length();
}
// Rail Direction: Begin Point -> End Point (x, y, z) (length = 1)
const gfl2::math::Vector3& Section::Direction() const
{
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_Direction.Length(), 1.0f));
  return m_Direction;
}
// Vertical Direction
const gfl2::math::Vector3& Section::VerticalDirection() const
{
  LUNA_SOL_DIVE_ASSERT(gfl2::math::IsSimilar(m_VerticalDirection.Length(), 1.0f));
  return m_VerticalDirection;
}
// Horizontal Direction
gfl2::math::Vector3 Section::HorizontalDirection() const
{
  const gfl2::math::Vector3 vector = Direction().Cross(VerticalDirection());
  LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(vector.Length()));
  return vector.Normalize();
}
// Translation
void Section::Translation(
            const gfl2::math::Vector3& displacement)
{
  m_BeginPoint += displacement;
  m_EndPoint += displacement;
}
// Adjusted Location
InternalCoordinates AdjustedLocation(
            const Section& section,
            const f32& z,
            const f32& radiusRate,
            const f32& angle)
{
  LUNA_SOL_DIVE_ASSERT(-EPSILON < z && z < section.Length() * (1.0f + EPSILON));
  const f32 beginZ = section.BeginCylindricSectionZ(
              radiusRate,
              angle);
  const f32 endZ = section.EndCylindricSectionZ(
              radiusRate,
              angle);
  const InternalCoordinates result = {
              beginZ + z * (endZ - beginZ) / section.Length(),
              radiusRate,
              angle};
  return result;
}
// Adjusted Directions
Directions AdjustedDirections(
            const Section& section,
            const InternalCoordinates& point,
            const Directions& nextDirections,
            const Directions& previousDirections)
{
  const gfl2::math::Vector3 location = section.ToWorldPoint(point);
  const f32 length = section.Length();
  const f32 progressionRate = point.z / length;
  if(progressionRate > 0.5f && nextDirections.IsValid())
  {
    const f32 maxZ = section.EndCylindricSectionZ(
                point.radius_rate,
                point.angle);
    const f32 fittingRate = 0.5f + 0.5f * (maxZ - point.z) / (maxZ - length / 2.0f);
    LUNA_SOL_DIVE_ASSERT((0.5f - 1.0e-3f < fittingRate) && (fittingRate < 1.0f + EPSILON));
    // Forward Direction
    const gfl2::math::Vector3 forwardVector =
            section.Direction() * fittingRate
            + nextDirections.Forward() * (1.0f - fittingRate);
    LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(forwardVector.Length()));
    const gfl2::math::Vector3 forwardDirection = forwardVector.Normalize();
    // Vertical Direction
    const gfl2::math::Vector3 verticalVector =
            section.VerticalDirection() * fittingRate
            + nextDirections.Vertical() * (1.0f - fittingRate);
    const gfl2::math::Vector3 adjustedVerticalDirection =
            verticalVector - forwardDirection * forwardDirection.Dot(verticalVector);
    LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(adjustedVerticalDirection.Length()));
    const gfl2::math::Vector3 verticalDirection = adjustedVerticalDirection.Normalize();
    return Directions(
                location,
                forwardDirection,
                verticalDirection);
  }
  else if(progressionRate < 0.5f && previousDirections.IsValid())
  {
    const f32 minZ = section.BeginCylindricSectionZ(
                point.radius_rate,
                point.angle);
    const f32 fittingRate = 0.5f + 0.5f * (point.z - minZ) / (length / 2.0f - minZ);
    LUNA_SOL_DIVE_ASSERT((0.5f - 1.0e-3f < fittingRate) && (fittingRate < 1.0f + EPSILON));
    // Forward Direction
    const gfl2::math::Vector3 forwardVector =
            section.Direction() * fittingRate
            + previousDirections.Forward() * (1.0f - fittingRate);
    LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(forwardVector.Length()));
    const gfl2::math::Vector3 forwardDirection = forwardVector.Normalize();
    // Vertical Direction
    const gfl2::math::Vector3 verticalVector =
            section.VerticalDirection() * fittingRate
            + previousDirections.Vertical() * (1.0f - fittingRate);
    const gfl2::math::Vector3 adjustedVerticalDirection =
            verticalVector - forwardDirection * forwardDirection.Dot(verticalVector);
    LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(adjustedVerticalDirection.Length()));
    const gfl2::math::Vector3 verticalDirection = adjustedVerticalDirection.Normalize();
    return Directions(
                location,
                forwardDirection,
                verticalDirection);
  }
  else
  {
    return Directions(
                location,
                section.Direction(),
                section.VerticalDirection());
  }
}
}// namespace Rail
}// namespace LunaSolDive

