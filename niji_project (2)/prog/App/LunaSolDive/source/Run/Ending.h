//======================================================================
/**
 * @file Ending.h
 * @date 2017/01/11 16:32:34
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RUN__ENDING_H_INCLUDED__
#define __LUNA_SOL_DIVE__RUN__ENDING_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "../Version.h"
#include "../Rail/Location.h"

namespace LunaSolDive{
// forward declaration
class Manager;
namespace Camera{
class Main;
}// namespace Camera
namespace Effect{
class Particle;
}// namespace Effect
namespace Gimmick{
class DistortionVortex;
}// namespace Gimmick

namespace Run{
class Ending
{
  GFL_FORBID_COPY_AND_ASSIGN(Ending);
public:
  // constructor
  Ending();
  // Initialize
  void Initialize(
              const Version& version,
              gfl2::heap::HeapBase* pHeap,
              LunaSolDive::Manager* pManager,
              Camera::Main* pCamera);
  // Terminate
  void Terminate();
  // Create
  void Create(
              gfl2::heap::HeapBase* pHeap);
  // Set Up
  void SetUp(
              const f32& speed,
              const Rail::Location& distortionLocation);
  // Update
  void Update();
  // Whether this stage has been finished
  bool IsFinished() const;
private:
  // Update in Entry stage
  void UpdateEntry();
  // Update in Trajectory stage & Fade stage
  void UpdateTrajectory();
  // State
  enum State
  {
    STATE__NONE,
    STATE__ENTRY,
    STATE__TRAJECTORY,
    STATE__FADE,
  };
  // Version
  Version m_Version;
  // Root Manager
  LunaSolDive::Manager* m_pManager;
  // Camera
  Camera::Main* m_pCamera;
  // State
  State m_State;
  // Frame Count
  u32 m_FrameCount;
  // Speed
  f32 m_Speed;
  // Distortion Location
  Rail::Location m_DistortionLocation;
  // Progress Distance of Trajectory
  f32 m_TrajectoryProgressDistance;
  // Effect: Hit 1
  Effect::Particle* m_pHitEffect1;
  // Effect: Hit 2 (Luna ver/Sol ver)
  Effect::Particle* m_pHitEffect2;
  // Distortion Vortex
  Gimmick::DistortionVortex* m_pDistortionVortex;
#if PM_DEBUG
public:
  // Quit
  void Quit();
#endif// PM_DEBUG
};
}// namespace Run
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RUN__ENDING_H_INCLUDED__
