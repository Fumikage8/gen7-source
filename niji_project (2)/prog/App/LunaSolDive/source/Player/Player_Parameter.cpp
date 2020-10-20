//======================================================================
/**
 * @file Player_Parameter.cpp
 * @date 2016/12/07 19:40:36
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
namespace Player{
// initialize
void Parameter::Initialize(
            gfl2::heap::HeapBase* pHeap)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pInstance == NULL);
  pInstance = GFL_NEW(pHeap) Parameter();
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
#if PM_DEBUG
  Debug::Parameter::Get().pPlayerParameter = pInstance;
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
  // Motion
  m_GlideBoundary = 0.75f;
  m_RunBoundary = 0.3f;
  m_StraightBoundary = 0.2f;
  // Elevation Angle
  m_ElevationAngleMax = gfl2::math::ConvDegToRad(25.0f);
  m_ElevationAngleMin = gfl2::math::ConvDegToRad(-25.0f);
  m_ElevationAngleChangeRate = 0.80f;
  // Effect
  m_HighSpeedTrajectoryBoundary = 1.00f;
  // Damage
  m_DamageRecoveryFrames = 30;
  m_DamageMotionFreezeFrames = 15;
  // Energy Aura
  m_EnergyAuraMaxScale = 1.5f;
  m_EnergyAuraMinScale = 0.5f;
  m_EnergyAuraMaxScaleChange = 0.05f;
  m_EnergyAuraForwardSol = -113.092f;
  m_EnergyAuraVerticalSol = 69.457f;
  m_EnergyAuraForwardLuna = -147.17f;
  m_EnergyAuraVerticalLuna = 78.202f;
  // Brend Frames Sol
  m_MotionBrendFramesSol.reset_end_walk = 5;
  m_MotionBrendFramesSol.reset_end_run = 5;
  m_MotionBrendFramesSol.reset_end_glide = 5;
  m_MotionBrendFramesSol.reset_begin_walk = 5;
  m_MotionBrendFramesSol.reset_begin_run = 5;
  m_MotionBrendFramesSol.reset_begin_glide = 5;
  m_MotionBrendFramesSol.direction_change_walk = 8;
  m_MotionBrendFramesSol.direction_change_run = 5;
  m_MotionBrendFramesSol.direction_change_glide = 4;
  m_MotionBrendFramesSol.walk_to_run = 5;
  m_MotionBrendFramesSol.walk_to_glide = 0;
  m_MotionBrendFramesSol.run_to_walk = 5;
  m_MotionBrendFramesSol.run_to_glide = 5;
  m_MotionBrendFramesSol.glide_to_walk = 0;
  m_MotionBrendFramesSol.glide_to_run = 5;
  m_MotionBrendFramesSol.turn_end_walk = 8;
  m_MotionBrendFramesSol.turn_end_run = 3;
  m_MotionBrendFramesSol.turn_end_glide = 1;
  // Brend Frames Luna
  m_MotionBrendFramesLuna.reset_end_walk = 5;
  m_MotionBrendFramesLuna.reset_end_run = 5;
  m_MotionBrendFramesLuna.reset_end_glide = 5;
  m_MotionBrendFramesLuna.reset_begin_walk = 5;
  m_MotionBrendFramesLuna.reset_begin_run = 5;
  m_MotionBrendFramesLuna.reset_begin_glide = 5;
  m_MotionBrendFramesLuna.direction_change_walk = 8;
  m_MotionBrendFramesLuna.direction_change_run = 5;
  m_MotionBrendFramesLuna.direction_change_glide = 8;
  m_MotionBrendFramesLuna.walk_to_run = 5;
  m_MotionBrendFramesLuna.walk_to_glide = 0;
  m_MotionBrendFramesLuna.run_to_walk = 30;
  m_MotionBrendFramesLuna.run_to_glide = 30;
  m_MotionBrendFramesLuna.glide_to_walk = 0;
  m_MotionBrendFramesLuna.glide_to_run = 5;
  m_MotionBrendFramesLuna.turn_end_walk = 1;
  m_MotionBrendFramesLuna.turn_end_run = 1;
  m_MotionBrendFramesLuna.turn_end_glide = 1;
}
// Motion: Boundary enrgy between Glide and Run
const f32& Parameter::GlideBoundary() const
{
  return m_GlideBoundary;
}
// Motion: Boundary energy between Run and Walk
const f32& Parameter::RunBoundary() const
{
  return m_RunBoundary;
}
// Motion: Boundary movement rate between Straight and Left/Right;
const f32& Parameter::StraightBoundary() const
{
  return m_StraightBoundary;
}
// Elevation Angle: Max Angel
const f32& Parameter::ElevationAngleMax() const
{
  return m_ElevationAngleMax;
}
// Elevation Angle: Min Angle
const f32& Parameter::ElevationAngleMin() const
{
  return m_ElevationAngleMin;
}
// Elevation Angle: Change Rate
const f32& Parameter::ElevationAngleChangeRate() const
{
  return m_ElevationAngleChangeRate;
}
// Effect: Boundary energy of high speed Trajectory
const f32& Parameter::HighSpeedTrajectoryBoundary() const
{
  return m_HighSpeedTrajectoryBoundary;
}
// Damage: Number of frames to recover
const u32& Parameter::DamageRecoveryFrames() const
{
  return m_DamageRecoveryFrames;
}
// Damage: Number of frames to freeze motion
const u32& Parameter::DamageMotionFreezeFrames() const
{
  return m_DamageMotionFreezeFrames;
}
// Energy Aura: Maximum scale
const f32& Parameter::EnergyAuraMaxScale() const
{
  return m_EnergyAuraMaxScale;
}
// Energy Aura: Minimum scale
const f32& Parameter::EnergyAuraMinScale() const
{
  return m_EnergyAuraMinScale;
}
// Energy Aura: Maximum scale change
const f32& Parameter::EnergyAuraMaxScaleChange() const
{
  return m_EnergyAuraMaxScaleChange;
}
// Energy Aura: Forward difference from Pokemon's waist position(Sol)
const f32& Parameter::EnergyAuraForwardSol() const
{
  return m_EnergyAuraForwardSol;
}
// Energy Aura: Vertical difference from Pokemon's waist position(Sol)
const f32& Parameter::EnergyAuraVerticalSol() const
{
  return m_EnergyAuraVerticalSol;
}
// Energy Aura: Forward difference from Pokemon's waist position(Luna)
const f32& Parameter::EnergyAuraForwardLuna() const
{
  return m_EnergyAuraForwardLuna;
}
// Energy Aura: Vertical difference from Pokemon's waist position(Luna)
const f32& Parameter::EnergyAuraVerticalLuna() const
{
  return m_EnergyAuraVerticalLuna;
}
// Motion Brend Frames Sol version
const MotionBrendFrames& Parameter::MotionBrendFramesSol() const
{
  return m_MotionBrendFramesSol;
}
// Motion Brend Frames Luna version
const MotionBrendFrames& Parameter::MotionBrendFramesLuna() const
{
  return m_MotionBrendFramesLuna;
}
}// namespace Player
}// namespace LunaSolDive
