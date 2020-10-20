//======================================================================
/**
 * @file InputCTR.h
 * @date 2016/09/29 19:46:55
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#ifdef GF_PLATFORM_CTR
#if !defined __LUNA_SOL_DIVE__RUN__INPUT_CTR_H_INCLUDED__
#define __LUNA_SOL_DIVE__RUN__INPUT_CTR_H_INCLUDED__
#pragma once

#include <math/include/gfl2_Matrix34.h>
#include <math/include/gfl2_Vector2.h>
#include "./Input.h"

// forward declaration
namespace gfl2{
namespace ui{
class Accelerometer;
class DeviceManager;
class Gyroscope;
}// namespace ui
}// namespace gfl2

namespace LunaSolDive{
namespace Run{
class InputCTR: public Input
{
  GFL_FORBID_COPY_AND_ASSIGN(InputCTR);
public:
  // constructor
  InputCTR();
  // Initialize (ovverride)
  virtual void Initialize(
              const Maneuver& maneuver,
              gfl2::heap::HeapBase* pHeap,
              gfl2::ui::DeviceManager* pDeviceManager);
  // Terminate (override)
  virtual void Terminate();
  // Update (override)
  virtual void Update();
  // Aim (override)
  virtual const gfl2::math::Vector2& Aim() const;
  // Set Initial Direction (override)
  virtual void SetInitialDirection();
  // Reset Signal (override)
  virtual Run::ResetSignal ResetSignal() const;
private:
  // Update: Gyroscope Maneuver
  void UpdateGyroscope();
  // Update: Stick Maneuver
  void UpdateStick();
  // Maneuver (Gyroscope or Stick)
  Maneuver m_Maneuver;
  // Accelerometer
  gfl2::ui::Accelerometer* m_pAccelerometer;
  // Gyroscope pointer
  gfl2::ui::Gyroscope* m_pGyroscope;
  // Aim
  gfl2::math::Vector2 m_Aim;
  // Direction Convert Matrix
  gfl2::math::Matrix34 m_DirectionConvertMatrix;
  // Direction X
  gfl2::math::Vector3 m_DirectionX;
  // Direction Y
  gfl2::math::Vector3 m_DirectionY;
  // Direction Z
  gfl2::math::Vector3 m_DirectionZ;
  // Acceleration
  gfl2::math::Vector3 m_Acceleration;
  // Pitch Angle
  f32 m_PitchAngle;
  // Yaw Angle
  f32 m_YawAngle;
  // Roll Angle
  f32 m_RollAngle;
  // Whether initial direction is set
  bool m_IsInitialDirectionSet;
  // Whether the reset process is in progress or not
  bool m_IsInResetProcess;
  // Frame count in reset process
  u32 m_ResetProcessCount;
  // Frame count in reset interval
  u32 m_ResetIntervalCount;
  // Reset Signal
  Run::ResetSignal m_ResetSignal;
};
}// namespace Run
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RUN__INPUT_CTR_H_INCLUDED__
#endif // GF_PLATFORM_CTR
