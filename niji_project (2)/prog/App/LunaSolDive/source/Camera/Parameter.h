//======================================================================
/**
 * @file Parameter.h
 * @date 2017/01/10 15:24:15
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__CAMERA__PARAMETER_H_INCLUDED__
#define __LUNA_SOL_DIVE__CAMERA__PARAMETER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <types/include/gfl2_Typedef.h>

namespace LunaSolDive{
#if PM_DEBUG
// forward declaration
namespace Debug{
class Menu;
}// namespace Debug
#endif// PM_DEBUG

namespace Camera{
class Parameter
{
  GFL_FORBID_COPY_AND_ASSIGN(Parameter);
public:
  // initialize
  static void Initialize(
              gfl2::heap::HeapBase* pHeap);
  // Terminate
  static void Terminate();
  // getter
  static const Parameter& Get();
  // Reset
  void Reset();
  // Perspective: Field of View, Y
  const f32& FieldOfViewY() const;
  // Perspective: Near Clip
  const f32& NearClip() const;
  // Perspective: Far Clip
  const f32& FarClip() const;
  // View: Forward Displacement of Camera Point
  const f32& CameraForward() const;
  // View: Vertical Displacement of Camera Point
  const f32& CameraVertical() const;
  // View: Forward Displacement of Target Point
  const f32& TargetForward() const;
  // View: Vertical Displacement of Target Point
  const f32& TargetVertical() const;
  // Trace: Movement
  const f32& TraceMovement() const;
  // Trace: Vertical Range
  const f32& TraceVerticalRange() const;
  // Trace: Horizontal Range
  const f32& TraceHorizontalRange() const;
  // Acceleration: Displacement of distance by acceleration
  const f32& AccelerationDisplacement() const;
  // Acceleration: Displacement of distance by deceleration
  const f32& DecelerationDisplacement() const;
  // Acceleration: Kickstart rate
  const f32& AccelerationKickstartRate() const;
  // Acceleration: Attenuation rate
  const f32& AccelerationAttenuationRate() const;
  // Acceleration: Threshold to end acceleration / deceleration
  const f32& AccelerationEndThreshold() const;
  // Damage Vibration: Amplitude
  const f32& DamageVibrationAmplitude() const;
  // Damage Vibration: Damping Factor
  const f32& DamageVibrationDampingFactor() const;
  // Damage Vibration: Period
  const f32& DamageVibrationPeriod() const;
  // Gravity Vibration: Amplitude
  const f32& GravityVibrationAmplitude() const;
  // Gravity Vibration: Period
  const f32& GravityVibrationPeriod() const;
  // Effect: Distance
  const f32& EffectDistance() const;
  // Beginning: Number of frames for Entry stage
  const u32& BeginningEntryFrames() const;
  // Beginning: Number of frames for Entry End stage
  const u32& BeginningEntryEndFrames() const;
  // Beginning: Number of frames for Overtaken stage
  const u32& BeginningOvertakenFrames() const;
  // Beginning: Angle increasing rate in Overtaken stage
  const f32& BeginningOvertakenAngleIncreasingRate() const;
  // Beginning: Radius increasing rate in Overtaken stage
  const f32& BeginningOvertakenRadiusIncreasingRate() const;
  // Beginning: Number of frames for Zoom In stage
  const u32& BeginningZoomInFrames() const;
private:
  // instance
  static Parameter* pInstance;
  // constructor
  Parameter();
  // Perspective: Field of View, Y
  f32 m_FieldOfViewY;
  // Perspective: Near Clip
  f32 m_NearClip;
  // Perspective: Far Clip
  f32 m_FarClip;
  // View: Forward Displacement of Camera Point
  f32 m_CameraForward;
  // View: Vertical Displacement of Camera Point
  f32 m_CameraVertical;
  // View: Forward Displacement of Target Point
  f32 m_TargetForward;
  // View: Vertical Displacement of Target Point
  f32 m_TargetVertical;
  // Trace: Movement
  f32 m_TraceMovement;
  // Trace: Vertical Range
  f32 m_TraceVerticalRange;
  // Trace: Horizontal Range
  f32 m_TraceHorizontalRange;
  // Acceleration: Displacement of distance by acceleration
  f32 m_AccelerationDisplacement;
  // Acceleration: Displacement of distance by deceleration
  f32 m_DecelerationDisplacement;
  // Acceleration: Kickstart rate
  f32 m_AccelerationKickstartRate;
  // Acceleration: Attenuation rate
  f32 m_AccelerationAttenuationRate;
  // Acceleration: Threshold to end acceleration / deceleration
  f32 m_AccelerationEndThreshold;
  // Damage Vibration: Amplitude
  f32 m_DamageVibrationAmplitude;
  // Damage Vibration: Damping Factor
  f32 m_DamageVibrationDampingFactor;
  // Damage Vibration: Period
  f32 m_DamageVibrationPeriod;
  // Gravity Vibration: Amplitude
  f32 m_GravityVibrationAmplitude;
  // Gravity Vibration: Period
  f32 m_GravityVibrationPeriod;
  // Effect: Distance
  f32 m_EffectDistance;
  // Beginning: Number of frames for Entry stage
  u32 m_BeginningEntryFrames;
  // Beginning: Number of frames for Entry End stage
  u32 m_BeginningEntryEndFrames;
  // Beginning: Number of frames for Overtaken stage
  u32 m_BeginningOvertakenFrames;
  // Beginning: Angle increasing rate in Overtaken stage
  f32 m_BeginningOvertakenAngleIncreasingRate;
  // Beginning: Radius increasing rate in Overtaken stage
  f32 m_BeginningOvertakenRadiusIncreasingRate;
  // Beginning: Number of frames for Zoom In stage
  u32 m_BeginningZoomInFrames;
#if PM_DEBUG
  friend class Debug::Menu;
#endif// PM_DEBUG
};
}// namespace Camera
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__CAMERA__PARAMETER_H_INCLUDED__
