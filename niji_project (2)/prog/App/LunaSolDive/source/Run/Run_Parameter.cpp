//======================================================================
/**
 * @file Run_Parameter.cpp
 * @date 2016/11/24 14:57:12
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
namespace Run{
// Initialize
void Parameter::Initialize(
          gfl2::heap::HeapBase* pHeap)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pInstance == NULL);
  pInstance = GFL_NEW(pHeap) Parameter();
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
#if PM_DEBUG
  Debug::Parameter::Get().pRunParameter = pInstance;
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
// Instance
Parameter* Parameter::pInstance = NULL;
// constructor
Parameter::Parameter()
{
  Reset();
}
// Reset
void Parameter::Reset()
{
  // Speed
  m_ForwardSpeedMax = 120.0f;
  m_ForwardSpeedMin = 50.0f;
  m_CrossSectionSpeedMax = 0.035f;
  m_CrossSectionSpeedMin = 0.020f;
  m_CrossSectionSpeedEmpty = 0.005f;
  // Energy
  m_EnergyInitialRate = 0.7f;
  m_EnergyAttenuationFrames = 1200;
  m_EnergyLimitIncrease = 2.0e-2f;
  m_EnergyLimitLimit = 2.0f;
  // Gyroscope
  m_YawAngleRange = gfl2::math::ConvDegToRad(15.0f);
  m_PitchAngleRange = gfl2::math::ConvDegToRad(10.0f);
  m_ResetThreshold = 0.5f;
  m_ResetWaitFrame = 30;
  m_ResetIntervalFrame = 30;
  // Translation
  m_TranslationThreshold = 5.0e+4f;
  m_TranslationWaitFrame = 30;
  // Beginning
  m_BeginningDistance = 60.0e+2f;
  // Ending
  m_EndingEntryEndPlayerScale = 0.60f;
  m_EndingTrajectoryPlayerScaleChangeRate = 0.70f;
  m_EndingTrajectoryFrames = 20;
  m_EndingHitEffectOffset = 100.0f;
  m_EndingHit02EffectScale = 3.0f;
}
// Speed: Maximum of forward speed
const f32& Parameter::ForwardSpeedMax() const
{
  return m_ForwardSpeedMax;
}
// Speed: Minimum of forward speed
const f32& Parameter::ForwardSpeedMin() const
{
  return m_ForwardSpeedMin;
}
// Speed: Maximum rate of speed on cross section
const f32& Parameter::CrossSectionSpeedMax() const
{
  return m_CrossSectionSpeedMax;
}
// Speed: Minimum rate of speed on cross section
const f32& Parameter::CrossSectionSpeedMin() const
{
  return m_CrossSectionSpeedMin;
}
// Speed: Speed on cross section when energy is empty
const f32& Parameter::CrossSectionSpeedEmpty() const
{
  return m_CrossSectionSpeedEmpty;
}
// Energy: Initial rate
const f32& Parameter::EnergyInitialRate() const
{
  return m_EnergyInitialRate;
}
// Energy: Number of frames required for attenuation from 1 to 0
const u32& Parameter::EnergyAttenuationFrames() const
{
  return m_EnergyAttenuationFrames;
}
// Energy: Increase of Limit
const f32& Parameter::EnergyLimitIncrease() const
{
  return m_EnergyLimitIncrease;
}
// Energy: Limit of Limit
const f32& Parameter::EnergyLimitLimit() const
{
  return m_EnergyLimitLimit;
}
// Gyroscope: Range ot yaw angle
const f32& Parameter::YawAngleRange() const
{
  return m_YawAngleRange;
}
//Gyroscope:  Range of pitch angle
const f32& Parameter::PitchAngleRange() const
{
  return m_PitchAngleRange;
}
// Reset: Threshold
const f32& Parameter::ResetThreshold() const
{
  return m_ResetThreshold;
}
// Reset: Wait Frame
const u32& Parameter::ResetWaitFrame() const
{
  return m_ResetWaitFrame;
}
// Reset: Interval Frame
const u32& Parameter::ResetIntervalFrame() const
{
  return m_ResetIntervalFrame;
}
// Translation: Threshold
const f32& Parameter::TranslationThreshold() const
{
  return m_TranslationThreshold;
}
// Translation: Wait Frame
const u32& Parameter::TranslationWaitFrame() const
{
  return m_TranslationWaitFrame;
}
// Beginning: Distance
const f32& Parameter::BeginningDistance() const
{
  return m_BeginningDistance;
}
// Ending: Scale of player at the end of Entry stage
const f32& Parameter::EndingEntryEndPlayerScale() const
{
  return m_EndingEntryEndPlayerScale;
}
// Ending: Number of frames in Trajectory stage
const u32& Parameter::EndingTrajectoryFrames() const
{
  return m_EndingTrajectoryFrames;
}
// Ending: Scale change rate of player in Trajectory stage
const f32& Parameter::EndingTrajectoryPlayerScaleChangeRate() const
{
  return m_EndingTrajectoryPlayerScaleChangeRate;
}
// Ending: Offset distance of hit effect
const f32& Parameter::EndingHitEffectOffset() const
{
  return m_EndingHitEffectOffset;
}
// Ending: Scale of hit02 effect
const f32& Parameter::EndingHit02EffectScale() const
{
  return m_EndingHit02EffectScale;
}
}// namespace Run
}// namespace LunaSolDive
