//======================================================================
/**
 * @file Parameter.h
 * @date 2016/12/07 19:40:06
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__PLAYER__PARAMETER_H_INCLUDED__
#define __LUNA_SOL_DIVE__PLAYER__PARAMETER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>

namespace LunaSolDive{
#if PM_DEBUG
// forward declarations
namespace Debug{
class Menu;
}// namespace Debug
#endif// PM_DEBUG

namespace Player{

struct MotionBrendFrames
{
  u32 reset_end_walk;
  u32 reset_end_run;
  u32 reset_end_glide;
  u32 reset_begin_walk;
  u32 reset_begin_run;
  u32 reset_begin_glide;
  u32 direction_change_walk;
  u32 direction_change_run;
  u32 direction_change_glide;
  u32 walk_to_run;
  u32 walk_to_glide;
  u32 run_to_walk;
  u32 run_to_glide;
  u32 glide_to_walk;
  u32 glide_to_run;
  u32 turn_end_walk;
  u32 turn_end_run;
  u32 turn_end_glide;
};

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
  // Motion: Boundary energy between Glide and Run
  const f32& GlideBoundary() const;
  // Motion: Boundary energy between Run and Walk
  const f32& RunBoundary() const;
  // Motion: Boundary movement rate between Straight and Left/Right;
  const f32& StraightBoundary() const;
  // Elevation Angle: Max Angle
  const f32& ElevationAngleMax() const;
  // Elevation Angle: Min Angle
  const f32& ElevationAngleMin() const;
  // Elevation Angle: Change Rate
  const f32& ElevationAngleChangeRate() const;
  // Effect: Boundary energy of high speed Trajectory
  const f32& HighSpeedTrajectoryBoundary() const;
  // Damage: Number of frames to recover
  const u32& DamageRecoveryFrames() const;
  // Damage: Number of frames to freeze motion
  const u32& DamageMotionFreezeFrames() const;
  // Energy Aura: Maximum scale
  const f32& EnergyAuraMaxScale() const;
  // Energy Aura: Minimum scale
  const f32& EnergyAuraMinScale() const;
  // Energy Aura: Maximum scale change
  const f32& EnergyAuraMaxScaleChange() const;
  // Energy Aura: Forward difference from Pokemon's waist position(Sol)
  const f32& EnergyAuraForwardSol() const;
  // Energy Aura: Vertical difference from Pokemon's waist position(Sol)
  const f32& EnergyAuraVerticalSol() const;
  // Energy Aura: Forward difference from Pokemon's waist position(Luna)
  const f32& EnergyAuraForwardLuna() const;
  // Energy Aura: Vertical difference from Pokemon's waist position(Luna)
  const f32& EnergyAuraVerticalLuna() const;
  // Motion Brend Frames Sol version
  const MotionBrendFrames& MotionBrendFramesSol() const;
  // Motion Brend Frames Luna version
  const MotionBrendFrames& MotionBrendFramesLuna() const;
private:
  // Instance
  static Parameter* pInstance;
  // constructor
  Parameter();
  // Motion: Boundary energy between Glide and Run
  f32 m_GlideBoundary;
  // Motion: Boundary energy between Run and Walk
  f32 m_RunBoundary;
  // Motion: Boundary movement rate between Straight and Left/Right
  f32 m_StraightBoundary;
  // Elevation Angle: Max Angle
  f32 m_ElevationAngleMax;
  // Elevation Angle: Min Angle
  f32 m_ElevationAngleMin;
  // Elevation Angle: Change Rate
  f32 m_ElevationAngleChangeRate;
  // Effect: Boundary energy of high speed Trajectory
  f32 m_HighSpeedTrajectoryBoundary;
  // Damage: Number of frames to recover
  u32 m_DamageRecoveryFrames;
  // Damage: Number of frames to freeze motion
  u32 m_DamageMotionFreezeFrames;
  // Energy Aura: Maximum scale
  f32 m_EnergyAuraMaxScale;
  // Energy Aura: Minimum scale
  f32 m_EnergyAuraMinScale;
  // Energy Aura: Maximum scale change
  f32 m_EnergyAuraMaxScaleChange;
  // Energy Aura: Forward difference from Pokemon's waist position(Sol)
  f32 m_EnergyAuraForwardSol;
  // Energy Aura: Vertical difference from Pokemon's waist position(Sol)
  f32 m_EnergyAuraVerticalSol;
  // Energy Aura: Forward difference from Pokemon's waist position(Luna)
  f32 m_EnergyAuraForwardLuna;
  // Energy Aura: Vertical difference from Pokemon's waist position(Luna)
  f32 m_EnergyAuraVerticalLuna;
  // Motion Brend Frames Sol version
  MotionBrendFrames m_MotionBrendFramesSol;
  // Motion Brend Frames Luna version
  MotionBrendFrames m_MotionBrendFramesLuna;
#if PM_DEBUG
  friend class Debug::Menu;
#endif// PM_DEBUG
};
}// namespace Player
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__PLAYER__PARAMETER_H_INCLUDED__
