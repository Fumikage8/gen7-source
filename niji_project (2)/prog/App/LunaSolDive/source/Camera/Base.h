//======================================================================
/**
 * @file Base.h
 * @date 2016/09/07 16:36:47
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__CAMERA__BASE_H_INCLUDED__
#define __LUNA_SOL_DIVE__CAMERA__BASE_H_INCLUDED__
#pragma once

// forward declaration
namespace gfl2{
namespace gfx{
class CtrDisplayNo;
}// namespace gfx
namespace math{
class Matrix34;
class Matrix44;
}// namespace math
}// namespace gfl2

namespace LunaSolDive{
// forward declarations
namespace Rail{
class Directions;
}// namespace Rail

namespace Camera{
class Base
{
public:
  // destructor
  virtual ~Base() = 0;
  // Projection Matrix
  virtual gfl2::math::Matrix44 ProjectionMatrix(
              const gfl2::gfx::CtrDisplayNo& displayNo) const = 0;
  // View Matrix
  virtual gfl2::math::Matrix34 ViewMatrix() const = 0;
  // Directions
  virtual const Rail::Directions& Directions() const = 0;
private:
};
}// namespace Camera
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__CAMERA__BASE_H_INCLUDED__
