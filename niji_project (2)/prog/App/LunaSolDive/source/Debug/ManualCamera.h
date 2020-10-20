//======================================================================
/**
 * @file ManualCamera.h
 * @date 2017/01/10 12:17:26
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG
#if !defined __LUNA_SOL_DIVE__DEBUG__MANUAL_CAMERA_H_INCLUDED__
#define __LUNA_SOL_DIVE__DEBUG__MANUAL_CAMERA_H_INCLUDED__
#pragma once

#include "../Camera/Base.h"
#include "../Rail/Directions.h"

// forward declaration
namespace gfl2{
namespace ui{
class Button;
}// namespace ui
}// namespace gfl2

namespace LunaSolDive{
namespace Debug{
class ManualCamera: public Camera::Base
{
public:
  // constructor
  ManualCamera();
  // Initialize
  void Initialize(
              const Rail::Directions& directions);
  // Terminate
  void Terminate();
  // Update
  void Update(
              const gfl2::ui::Button& button,
              const gfl2::math::Vector2& stick);
  // Projection Matrix (override)
  virtual gfl2::math::Matrix44 ProjectionMatrix(
              const gfl2::gfx::CtrDisplayNo& displayNo) const;
  // View Matrix (override)
  virtual gfl2::math::Matrix34 ViewMatrix() const;
  // Directions (override)
  virtual const Rail::Directions& Directions() const;
private:
  // Initial Directions
  Rail::Directions m_InitialDirections;
  // Present Directions
  Rail::Directions m_PresentDirections;
  // Pitch Angle
  f32 m_PitchAngle;
  // YawAngle
  f32 m_YawAngle;
  // Distance
  f32 m_Distance;
  // Displacement
  gfl2::math::Vector3 m_Displacement;
};
}// namespace Debug
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__DEBUG__MANUAL_CAMERA_H_INCLUDED__
#endif // PM_DEBUG
