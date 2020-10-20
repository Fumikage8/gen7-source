//======================================================================
/**
 * @file Parameter.h
 * @date 2016/11/24 14:51:55
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RUN__PARAMETER_H_INCLUDED__
#define __LUNA_SOL_DIVE__RUN__PARAMETER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>

namespace LunaSolDive{
#if PM_DEBUG
namespace Debug{
class Menu;
}// namespace Debug
#endif// PM_DEBUG

namespace Run{
class Parameter
{
  GFL_FORBID_COPY_AND_ASSIGN(Parameter);
public:
  // Initialize
  static void Initialize(
              gfl2::heap::HeapBase* pHeap);
  // Terminate
  static void Terminate();
  // getter
  static const Parameter& Get();
  // Reset
  void Reset();
  // Speed: Maximum of forward speed
  const f32& ForwardSpeedMax() const;
  // Speed: Minimum of forward speed
  const f32& ForwardSpeedMin() const;
  // Speed: Maximum rate of speed on cross section
  const f32& CrossSectionSpeedMax() const;
  // Speed: Minimum rate of speed on cross section
  const f32& CrossSectionSpeedMin() const;
  // Speed: Speed on cross section when energy is empty
  const f32& CrossSectionSpeedEmpty() const;
  // Energy: Initial rate
  const f32& EnergyInitialRate() const;
  // Energy: Number of frames required for attenuation from 1 to 0
  const u32& EnergyAttenuationFrames() const;
  // Energy: Increase of Limit
  const f32& EnergyLimitIncrease() const;
  // Energy: Limit of Limit
  const f32& EnergyLimitLimit() const;
  // Gyroscope: Range ot yaw angle
  const f32& YawAngleRange() const;
  // Gyroscope: Range of pitch angle
  const f32& PitchAngleRange() const;
  // Reset: Threshold
  const f32& ResetThreshold() const;
  // Reset: Wait Frame
  const u32& ResetWaitFrame() const;
  // Reset: Interval Frame
  const u32& ResetIntervalFrame() const;
  // Translation: Threshold
  const f32& TranslationThreshold() const;
  // Translation: Wait Frame
  const u32& TranslationWaitFrame() const;
  // Beginning: Distance
  const f32& BeginningDistance() const;
  // Ending: Scale of player at the end of Entry stage
  const f32& EndingEntryEndPlayerScale() const;
  // Ending: Number of frames in Trajectory stage
  const u32& EndingTrajectoryFrames() const;
  // Ending: Scale change rate of player in Trajectory stage
  const f32& EndingTrajectoryPlayerScaleChangeRate() const;
  // Ending: Offset distance of hit effect
  const f32& EndingHitEffectOffset() const;
  // Ending: Scale of hit02 effect
  const f32& EndingHit02EffectScale() const;
private:
  // Instance
  static Parameter* pInstance;
  // constructor
  Parameter();
  // Speed: Maximum of forward speed
  f32 m_ForwardSpeedMax;
  // Speed: Minimum of forward speed
  f32 m_ForwardSpeedMin;
  // Speed: Maximum rate of speed on cross section
  f32 m_CrossSectionSpeedMax;
  // Speed: Minimum rate of speed on cross section
  f32 m_CrossSectionSpeedMin;
  // Speed: Speed on cross section when energy is empty
  f32 m_CrossSectionSpeedEmpty;
  // Energy: Initial rate
  f32 m_EnergyInitialRate;
  // Energy: Number of frames required for attenuation from 1 to 0
  u32 m_EnergyAttenuationFrames;
  // Energy: Increase of Limit
  f32 m_EnergyLimitIncrease;
  // Energy: Limit of Limit
  f32 m_EnergyLimitLimit;
  // Gyroscope: Range ot yaw angle
  f32 m_YawAngleRange;
  // Gyroscope: Range of pitch angle
  f32 m_PitchAngleRange;
  // Reset: Threshold
  f32 m_ResetThreshold;
  // Reset: Wait Frame
  u32 m_ResetWaitFrame;
  // Reset: Interval Frame
  u32 m_ResetIntervalFrame;
  // Translation: Threshold
  f32 m_TranslationThreshold;
  // Translation: Wait Frame
  u32 m_TranslationWaitFrame;
  // Beginning: Distance
  f32 m_BeginningDistance;
  // Ending: Scale of player at the end of Entry stage
  f32 m_EndingEntryEndPlayerScale;
  // Ending: Number of frames in Trajectory stage
  u32 m_EndingTrajectoryFrames;
  // Ending: Scale change rate of player in Trajectory stage
  f32 m_EndingTrajectoryPlayerScaleChangeRate;
  // Ending: Offset distance of hit effect
  f32 m_EndingHitEffectOffset;
  // Ending: Scale of hit02 effect
  f32 m_EndingHit02EffectScale;
#if PM_DEBUG
  friend class Debug::Menu;
#endif// PM_DEBUG
};
}// namespace Run
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RUN__PARAMETER_H_INCLUDED__
