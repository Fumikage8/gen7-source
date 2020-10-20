//======================================================================
/**
 * @file Camera_Parameter.cpp
 * @date 2017/01/10 15:29:27
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_MathCommon.h>
#include "./Parameter.h"
#include "../Assert.h"
#if PM_DEBUG
#include "../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Camera{
// Initialize
void Parameter::Initialize(
            gfl2::heap::HeapBase* pHeap)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pInstance == NULL);
  pInstance = GFL_NEW(pHeap) Parameter();
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
#if PM_DEBUG
  Debug::Parameter::Get().pCameraParameter = pInstance;
#endif// PM_DEBUG
}
// Terminate
void Parameter::Terminate()
{
  GFL_SAFE_DELETE(pInstance);
}
// getter
const Parameter& Parameter::Get()
{
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
  return *pInstance;
}
// instance
Parameter* Parameter::pInstance = NULL;
// constructor
Parameter::Parameter()
{
  Reset();
}
// Reset
void Parameter::Reset()
{
  // Perspective
  m_FieldOfViewY = gfl2::math::ConvDegToRad(35.0f);
  m_NearClip = 30.0f;
  m_FarClip = 30000.0f;
  // View
  m_CameraForward = -1250.0f;
  m_CameraVertical = 430.0f;
  m_TargetForward = 1000.0f;
  m_TargetVertical = 100.0f;
  // Acceleration
  m_AccelerationDisplacement = -500.0f;
  m_DecelerationDisplacement = 400.0f;
  m_AccelerationKickstartRate = 0.150f;
  m_AccelerationAttenuationRate = 0.950f;
  m_AccelerationEndThreshold = 1.0e-2f;
  // Trace
  m_TraceMovement = 0.01f;
  m_TraceVerticalRange = 0.70f;
  m_TraceHorizontalRange = 0.90f;
  // Damage Vibration
  m_DamageVibrationAmplitude = gfl2::math::ConvDegToRad(15.0f);
  m_DamageVibrationDampingFactor = 10.0f;
  m_DamageVibrationPeriod = 4.0f;
  // Gravity Vibration
  m_GravityVibrationAmplitude = 10.0f;
  m_GravityVibrationPeriod = 4.0f;
  // Effect
  m_EffectDistance = 250.0f;
  // Beginning
  m_BeginningEntryFrames = 70;
  m_BeginningEntryEndFrames = 15;
  m_BeginningOvertakenFrames = 102;
  m_BeginningOvertakenAngleIncreasingRate = 5.0f;
  m_BeginningOvertakenRadiusIncreasingRate = 4.0f;
  m_BeginningZoomInFrames = 10;
}
// Perspective: Field of View, Y
const f32& Parameter::FieldOfViewY() const
{
  return m_FieldOfViewY;
}
// Perspective: Near Clip
const f32& Parameter::NearClip() const
{
  return m_NearClip;
}
// Perspective: Far Clip
const f32& Parameter::FarClip() const
{
  return m_FarClip;
}
// View: Forward Displacement of Camera Point
const f32& Parameter::CameraForward() const
{
  return m_CameraForward;
}
// View: Vertical Displacement of Camera Point
const f32& Parameter::CameraVertical() const
{
  return m_CameraVertical;
}
// View: Forward Displacement of Target Point
const f32& Parameter::TargetForward() const
{
  return m_TargetForward;
}
// View: Vertical Displacement of Target Point
const f32& Parameter::TargetVertical() const
{
  return m_TargetVertical;
}
// Trace: Movement
const f32& Parameter::TraceMovement() const
{
  return m_TraceMovement;
}
// Trace: Vertical Range
const f32& Parameter::TraceVerticalRange() const
{
  return m_TraceVerticalRange;
}
// Trace: Horizontal Range
const f32& Parameter::TraceHorizontalRange() const
{
  return m_TraceHorizontalRange;
}
// Acceleration: Displacement of distance by acceleration
const f32& Parameter::AccelerationDisplacement() const
{
  return m_AccelerationDisplacement;
}
// Acceleration: Displacement of distance by deceleration
const f32& Parameter::DecelerationDisplacement() const
{
  return m_DecelerationDisplacement;
}
// Acceleration: Kickstart rate
const f32& Parameter::AccelerationKickstartRate() const
{
  return m_AccelerationKickstartRate;
}
// Acceleration: Attenuation rate
const f32& Parameter::AccelerationAttenuationRate() const
{
  return m_AccelerationAttenuationRate;
}
// Acceleration: Threshold to end acceleration / deceleration
const f32& Parameter::AccelerationEndThreshold() const
{
  return m_AccelerationEndThreshold;
}
// Damage Vibration: Amplitude
const f32& Parameter::DamageVibrationAmplitude() const
{
  return m_DamageVibrationAmplitude;
}
// Damage Vibration: Damping Factor
const f32& Parameter::DamageVibrationDampingFactor() const
{
  return m_DamageVibrationDampingFactor;
}
// Damage Vibration: Period
const f32& Parameter::DamageVibrationPeriod() const
{
  return m_DamageVibrationPeriod;
}
// Gravity Vibration: Amplitude
const f32& Parameter::GravityVibrationAmplitude() const
{
  return m_GravityVibrationAmplitude;
}
// Gravity Vibration: Period
const f32& Parameter::GravityVibrationPeriod() const
{
  return m_GravityVibrationPeriod;
}
// Effect: Distance
const f32& Parameter::EffectDistance() const
{
  return m_EffectDistance;
}
// Beginning: Number of frames for Entry stage
const u32& Parameter::BeginningEntryFrames() const
{
  return m_BeginningEntryFrames;
}
// Beginning: Number of frames for Entry End stage
const u32& Parameter::BeginningEntryEndFrames() const
{
  return m_BeginningEntryEndFrames;
}
// Beginning: Number of frames for Overtaken stage
const u32& Parameter::BeginningOvertakenFrames() const
{
  return m_BeginningOvertakenFrames;
}
// Beginning: Angle increasing rate in Overtaken stage
const f32& Parameter::BeginningOvertakenAngleIncreasingRate() const
{
  return m_BeginningOvertakenAngleIncreasingRate;
}
// Beginning: Radius increasing rate in Overtaken stage
const f32& Parameter::BeginningOvertakenRadiusIncreasingRate() const
{
  return m_BeginningOvertakenRadiusIncreasingRate;
}
// Beginning: Number of frames for Zoom In stage
const u32& Parameter::BeginningZoomInFrames() const
{
  return m_BeginningZoomInFrames;
}
}// namespace Camera
}// namespace LunaSolDive
