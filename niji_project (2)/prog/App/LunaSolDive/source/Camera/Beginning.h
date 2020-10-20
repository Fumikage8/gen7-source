//======================================================================
/**
 * @file Beginning.h
 * @date 2017/01/06 13:06:49
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__CAMERA__BEGINNING_H_INCLUDED__
#define __LUNA_SOL_DIVE__CAMERA__BEGINNING_H_INCLUDED__
#pragma once

#include "./Base.h"
#include "../Version.h"
#include "../Rail/Directions.h"

namespace LunaSolDive{
namespace Camera{
class Beginning: public Base
{
public:
  // constructor
  Beginning();
  // Initialize
  void Initialize(
              const Version& version);
  // Terminate
  void Terminate();
  // Update
  void Update(
              const Rail::Directions& location);
  // Projection Matrix (override)
  virtual gfl2::math::Matrix44 ProjectionMatrix(
              const gfl2::gfx::CtrDisplayNo& displayNo) const;
  // View Matrix (override)
  virtual gfl2::math::Matrix34 ViewMatrix() const;
  // Directions (override)
  virtual const Rail::Directions& Directions() const;
  // Whether camera operation has been finished
  bool IsFinished() const;
  // Whether camera is zooming in or not
  bool IsZoomIn() const;
  // Number of frames in camera operation
  u32 TotalFrameNumber() const;
private:
  // State
  enum State
  {
    STATE__INITIAL,
    STATE__ENTRY,
    STATE__ENTRY_END,
    STATE__OVERTAKEN,
    STATE__ZOOM_IN,
    STATE__TERMINATE,
  };
  // Relay Point
  struct RelayPoint
  {
    gfl2::math::Vector3 initial;
    gfl2::math::Vector3 entry_end;
    gfl2::math::Vector3 zoom_in_start;
    gfl2::math::Vector3 terminate;
  };
  // Eye Point
  gfl2::math::Vector3 EyePoint() const;
  // Aim Point
  gfl2::math::Vector3 AimPoint() const;
  // State
  State m_State;
  // Numbert of frames at each stage
  u32 m_FrameCount;
  // Relay point of eye point
  RelayPoint m_EyeRelayPoint;
  // Relay point of aim point
  RelayPoint m_AimRelayPoint;
  // Player Location
  Rail::Directions m_PlayerLocation;
  // Eye Point relative to the Player
  gfl2::math::Vector3 m_EyePoint;
  // Aim Point relative to the Player
  gfl2::math::Vector3 m_AimPoint;
  // Directions
  Rail::Directions m_Directions;
};
}// namespace Camera
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__CAMERA__BEGINNING_H_INCLUDED__
