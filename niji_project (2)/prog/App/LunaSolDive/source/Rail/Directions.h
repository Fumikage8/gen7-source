//======================================================================
/**
 * @file Directions.h
 * @date 2016/11/07 11:13:47
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RAIL__DIRECTIONS_H_INCLUDED__
#define __LUNA_SOL_DIVE__RAIL__DIRECTIONS_H_INCLUDED__
#pragma once

#include <math/include/gfl2_Vector3.h>

// forward declarations
namespace gfl2{
namespace math{
class Quaternion;
}// namespace math
}// namespace gfl2

namespace LunaSolDive{
namespace Rail{
class Directions
{
public:
  // constructor
  Directions();
  // constructor with Initialize
  Directions(
              const gfl2::math::Vector3& point,
              const gfl2::math::Vector3& forward,
              const gfl2::math::Vector3& vertical);
  // constructor with Initialize (direction only)
  Directions(
              const gfl2::math::Vector3& forward,
              const gfl2::math::Vector3& vertical);
  // Initialize
  void Initialize(
              const gfl2::math::Vector3& point,
              const gfl2::math::Vector3& forward,
              const gfl2::math::Vector3& vertical);
  // Initialize (direction only)
  void Initialize(
              const gfl2::math::Vector3& forward,
              const gfl2::math::Vector3& vertical);
  // Terminate
  void Terminate();
  // check if this Directions is valid
  bool IsValid() const;
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
  // Translation
  void Translation(
              const gfl2::math::Vector3& displacement);
private:
  // Point
  gfl2::math::Vector3 m_Point;
  // Forward Direction
  gfl2::math::Vector3 m_Forward;
  // Vertical Direction
  gfl2::math::Vector3 m_Vertical;
};
}// namespace Rail
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RAIL__DIRECTIONS_H_INCLUDED__
