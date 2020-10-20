//======================================================================
/**
 * @file Aim.h
 * @date 2016/11/17 21:26:12
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RUN__AIM_H_INCLUDED__
#define __LUNA_SOL_DIVE__RUN__AIM_H_INCLUDED__
#pragma once

#include <math/include/gfl2_Vector2.h>

namespace LunaSolDive{
// forward declaration
namespace Gimmick{
class Action;
}// namespace Gimmick
namespace Run{
class Aim
{
public:
  // constructor
  Aim();
  // Initialize
  void Initialize();
  // Terminate
  void Terminate();
  // Update
  void Update(
              const gfl2::math::Vector2& input,
              const f32& maxDisplacement,
              const Gimmick::Action& gimmickAction);
  // Overwrite point (for Drifting Movement)
  void OverwritePoint(
              const gfl2::math::Vector2& point);
  // Point
  const gfl2::math::Vector2& Point() const;
  // Point Movement
  gfl2::math::Vector2 PointMovement() const;
  // Input
  const gfl2::math::Vector2& Input() const;
  // Input Movement
  gfl2::math::Vector2 InputMovement() const;
private:
  // Present Point
  gfl2::math::Vector2 m_PresentPoint;
  // Previous Point
  gfl2::math::Vector2 m_PreviousPoint;
  // Present Input
  gfl2::math::Vector2 m_PresentInput;
  // Previous Input
  gfl2::math::Vector2 m_PreviousInput;
};
}// namespace Run
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RUN__AIM_H_INCLUDED__
