//======================================================================
/**
 * @file Location.h
 * @date 2016/11/09 16:31:23
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RAIL__LOCATION_INFORMATION_H_INCLUDED__
#define __LUNA_SOL_DIVE__RAIL__LOCATION_INFORMATION_H_INCLUDED__
#pragma once

#include "./Directions.h"

namespace LunaSolDive{
namespace Rail{
class Location
{
public:
  // constructor
  Location();
  // constructor with Initialize
  Location(
              const f32& travelDistance,
              const Directions& object,
              const Directions& rail,
              const f32& railRadius);
  // Initialize
  void Initialize(
              const f32& travelDistance,
              const Directions& object,
              const Directions& rail,
              const f32& railRadius);
  // Terminatea
  void Terminate();
  // check if this Location Information is valid
  bool IsValid() const;
  // Travel Distance
  const f32& TravelDistance() const;
  // Point
  const gfl2::math::Vector3& Point() const;
  // Forward Direction
  const gfl2::math::Vector3& Forward() const;
  // Vertical Direction
  const gfl2::math::Vector3& Vertical() const;
  // Horizontal Direction
  gfl2::math::Vector3 Horizontal() const;
  // Rotation Quaternion
  gfl2::math::Quaternion RotationQuaternion() const;
  // Directions
  const Rail::Directions& Directions() const;
  // Rail Directions
  const Rail::Directions& Rail() const;
  // Rail Radius
  const f32& RailRadius() const;
  // Radius Rate
  f32 RadiusRate() const;
  // Angle
  f32 Angle() const;
  // Point on the cross section
  gfl2::math::Vector2 CrossSectionPoint() const;
  // Normalized point on cross section
  gfl2::math::Vector2 NormalizedCrossSectionPoint() const;
  // Translation
  void Translation(
              const gfl2::math::Vector3& displacement);
private:
  // Travel Distance
  f32 m_TravelDistance;
  // Object Directions
  Rail::Directions m_Object;
  // Rail Directions
  Rail::Directions m_Rail;
  // Rail Radius
  f32 m_RailRadius;
};
}// namespace Rail
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RAIL__LOCATION_INFORMATION_H_INCLUDED__
