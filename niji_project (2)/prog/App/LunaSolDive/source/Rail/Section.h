//======================================================================
/**
 * @file Section.h
 * @date 2016/10/07 15:15:54
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RAIL__SECTION_H_INCLUDED__
#define __LUNA_SOL_DIVE__RAIL__SECTION_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>

namespace LunaSolDive{
namespace Rail{
// forward declarations
class Directions;

struct InternalCoordinates{
  f32 z;
  f32 radius_rate;
  f32 angle;
};

class Section{
public:
  // constructor
  Section();
  // constructor with Initialize
  Section(
              const u32& index,
              const gfl2::math::Vector3& beginPoint,
              const gfl2::math::Vector3& beginNoramalVector,
              const gfl2::math::Vector3& endPoint,
              const gfl2::math::Vector3& endNoramalVector,
              const f32& radius,
              const gfl2::math::Vector3& verticalDirection);
  // Initialize
  void Initialize(
              const u32& index,
              const gfl2::math::Vector3& beginPoint,
              const gfl2::math::Vector3& beginNoramalVector,
              const gfl2::math::Vector3& endPoint,
              const gfl2::math::Vector3& endNoramalVector,
              const f32& radius,
              const gfl2::math::Vector3& verticalDirection);
  // Terminate
  void Terminate();
  // check if the Section is valid;
  bool IsValid() const;
  // convert: Internal Vector -> World Vector
  gfl2::math::Vector3 ToWorldVector(
              const InternalCoordinates& vector) const;
  // convert: Internal Point -> World Point
  gfl2::math::Vector3 ToWorldPoint(
              const InternalCoordinates& point) const;
  // convert: World Vector -> Internal Vector
  InternalCoordinates ToInternalVector(
              const gfl2::math::Vector3& vector) const;
  // convert: World Point -> Internal Point
  InternalCoordinates ToInternalPoint(
              const gfl2::math::Vector3& point) const;
  // Z value on the Cylindric-Section of the Begin Side
  f32 BeginCylindricSectionZ(
              const f32& radiusRate,
              const f32& angle) const;
  // Z value on the Cylindric-Section of the End Side
  f32 EndCylindricSectionZ(
              const f32& radiusRate,
              const f32& angle) const;
  // a Point on the Cylindric Section of the Begin side (x, y, z)
  gfl2::math::Vector3 BeginCylindricSection(
              const f32& radiusRate,
              const f32& angle) const;
  // a Point on the Cylindric Section of the End side (x, y, z)
  gfl2::math::Vector3 EndCylindricSection(
              const f32& radiusRate,
              const f32& angle) const;
  // check if the Internal Point is in this Section
  bool IsIn(
              const InternalCoordinates& point,
              const f32& margin) const;
  // check if the Internal Point is in forward of this Section
  bool IsForward(
              const InternalCoordinates& point) const;
  // check if the Internal Point is in backward of this Section
  bool IsBackward(
              const InternalCoordinates& point) const;
  // index
  const u32& Index() const;
  // Begin Point (x, y, z)
  const gfl2::math::Vector3& BeginPoint() const;
  // End Point (x, y, z)
  const gfl2::math::Vector3& EndPoint() const;
  // Length
  f32 Length() const;
  // Rail Direction: Begin Point -> End Point (x, y, z) (length = 1)
  const gfl2::math::Vector3& Direction() const;
  // Vertical Direction
  const gfl2::math::Vector3& VerticalDirection() const;
  // Horizontal Direction
  gfl2::math::Vector3 HorizontalDirection() const;
  // Translation
  void Translation(
              const gfl2::math::Vector3& displacement);
private:
  // index
  u32 m_Index;
  // Begin Point (x, y, z)
  gfl2::math::Vector3 m_BeginPoint;
  // End Point (x, y, z)
  gfl2::math::Vector3 m_EndPoint;
  // Radius
  f32 m_Radius;
  // Rail Direction: Begin Point -> End Point (x, y, z) (length = 1)
  gfl2::math::Vector3 m_Direction;
  // Vertical Direction
  gfl2::math::Vector3 m_VerticalDirection;
  // Begin Cylindric-Section Angle
  f32 m_BeginCylindricSectionAngle;
  // Begin Cylindric-Section Gradient
  f32 m_BeginCylindricSectionGradient;
  // End Cylindric-Section Angle
  f32 m_EndCylindricSectionAngle;
  // End Cylindric-Section Gradient
  f32 m_EndCylindricSectionGradient;
};
// Adjusted Location
InternalCoordinates AdjustedLocation(
            const Section& section,
            const f32& z,
            const f32& radiusRate,
            const f32& angle);
// Adjusted Directions
Directions AdjustedDirections(
            const Section& section,
            const InternalCoordinates& point,
            const Directions& nextDirections,
            const Directions& previousDirections);
}// namespace Rail
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RAIL__SECTION_H_INCLUDED__
