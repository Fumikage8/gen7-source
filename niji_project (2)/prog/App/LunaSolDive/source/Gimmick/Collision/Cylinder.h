//======================================================================
/**
 * @file Cylinder.h
 * @date 2016/11/14 18:42:49
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__COLLISION__CYLINDER_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__COLLISION__CYLINDER_H_INCLUDED__
#pragma once

#include <math/include/gfl2_Vector3.h>

namespace LunaSolDive{
namespace Gimmick{
namespace Collision{
class Cylinder
{
public:
  // constructor
  Cylinder();
  // Initialize
  void Initialize(
              const gfl2::math::Vector3& location,
              const gfl2::math::Vector3& axis,
              const gfl2::math::Vector3& polarAxis,
              const f32& radius,
              const f32& height);
  // Terminate
  void Terminate();
  // Whether the point is in this cylinder or not
  bool IsIn(
              const gfl2::math::Vector3& target) const;
  // Whether the line intersect or not
  bool IsIntersect(
              const gfl2::math::Vector3& beginPoint,
              const gfl2::math::Vector3& endPoint) const;
  // Detect Collision
  f32 Detect(
              const gfl2::math::Vector3& target) const;
  // Translation
  void Translation(
              const gfl2::math::Vector3& displacement);
  // Location
  const gfl2::math::Vector3& Location() const;
  // Axis
  const gfl2::math::Vector3& Axis() const;
  // Polar Axis
  const gfl2::math::Vector3& PolarAxis() const;
  // Radius
  const f32& Radius() const;
  // Height
  const f32& Height() const;
private:
  // Location
  gfl2::math::Vector3 m_Location;
  // Axis
  gfl2::math::Vector3 m_Axis;
  // Polar Axis
  gfl2::math::Vector3 m_PolarAxis;
  // Radius
  f32 m_Radius;
  // Height
  f32 m_Height;
};
}// namespace Collision
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__COLLISION__CYLINDER_H_INCLUDED__
