//======================================================================
/**
 * @file Run_Ending.cpp
 * @date 2017/01/11 16:37:16
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <algorithm>
#include <Debug/include/gfl2_DebugPrint.h>
#include <math/include/gfl2_Quaternion.h>
#include "./Ending.h"
#include "./Parameter.h"
#include "../Assert.h"
#include "../Manager.h"
#include "../Camera/Main.h"
#include "../Effect/Manager.h"
#include "../Effect/Particle.h"
#include "../Gimmick/DistortionVortex.h"
#include "../Gimmick/Manager.h"
#include "../Player/Manager.h"
#include "../Rail/Manager.h"
#include "../Sound/Manager.h"
#include "../Sound/Parameter.h"
#include "arc_index/LunaSolDiveEffect.gaix"

namespace LunaSolDive{
namespace Run{
// constructor
Ending::Ending()
    : m_Version(VERSION__NONE),
      m_pManager(NULL),
      m_pCamera(NULL),
      m_State(STATE__NONE),
      m_FrameCount(0),
      m_Speed(0.0f),
      m_TrajectoryProgressDistance(0.0f),
      m_pHitEffect1(NULL),
      m_pHitEffect2(NULL),
      m_pDistortionVortex(NULL){}
// Initialize
void Ending::Initialize(
            const Version& version,
            gfl2::heap::HeapBase* pHeap,
            LunaSolDive::Manager* pManager,
            Camera::Main* pCamera)
{
  // set parameter
  m_Version = version;
  LUNA_SOL_DIVE_ASSERT(m_Version != VERSION__NONE);
  // set pointer
  m_pManager = pManager;
  LUNA_SOL_DIVE_ASSERT(m_pManager != NULL);
  m_pCamera = pCamera;
  LUNA_SOL_DIVE_ASSERT(m_pCamera != NULL);
  // initialize distortion vortex
  m_pDistortionVortex = GFL_NEW(pHeap) Gimmick::DistortionVortex();
  LUNA_SOL_DIVE_ASSERT(m_pDistortionVortex != NULL);
  m_pDistortionVortex->Initialize(
              pHeap,
              m_pManager->RenderingPipelinePtr());
}
// Terminate
void Ending::Terminate()
{
  // delete model
  if(m_pDistortionVortex != NULL)
  {
    m_pDistortionVortex->Terminate();
    GFL_DELETE(m_pDistortionVortex);
    m_pDistortionVortex = NULL;
  }
  // delete effect
  GFL_SAFE_DELETE(m_pHitEffect1);
  GFL_SAFE_DELETE(m_pHitEffect2);
  // unset pointer
  m_pManager = NULL;
  m_pCamera = NULL;
  // reset parameter
  m_Version = VERSION__NONE;
  m_State = STATE__NONE;
  m_FrameCount = 0;
  m_Speed = 0.0f;
  m_DistortionLocation.Terminate();
  m_TrajectoryProgressDistance = 0.0f;
}
// Create
void Ending::Create(
            gfl2::heap::HeapBase* pHeap)
{
  LUNA_SOL_DIVE_ASSERT(m_pDistortionVortex != NULL);
  m_pDistortionVortex->Create(
              pHeap,
              m_pManager->AllocatorPtr());
}
// Set Up
void Ending::SetUp(
            const f32& speed,
            const Rail::Location& distortionLocation)
{
  // change state
  m_State = STATE__ENTRY;
  // set parameter
  m_Speed = speed;
  m_DistortionLocation = distortionLocation;
  // reset parameter
  m_FrameCount = 0;
  m_TrajectoryProgressDistance = 0.0f;
  // distortion vortex
  m_pDistortionVortex->SetUp(
              m_DistortionLocation);
  m_pDistortionVortex->Visualize();
  // create hit 1 effect
  GFL_SAFE_DELETE(m_pHitEffect1);
  m_pHitEffect1 = m_pManager->EffectManager().Create(
              GARC_LunaSolDiveEffect_ea_dive_warphole_hit01_lz_BPTCL);
  // create hit 2 effect
  GFL_SAFE_DELETE(m_pHitEffect2);
  LUNA_SOL_DIVE_ASSERT(m_Version != VERSION__NONE);
  m_pHitEffect2 = m_pManager->EffectManager().Create(
              (m_Version == VERSION__SOL)
              ? GARC_LunaSolDiveEffect_ea_dive_warphole_s_hit02_lz_BPTCL
              : GARC_LunaSolDiveEffect_ea_dive_warphole_m_hit02_lz_BPTCL);
}
// Update
void Ending::Update()
{
  GFL_ASSERT(m_State != STATE__NONE);
  ARAI_PRINT("Ending: %s, %03d\n",
              (m_State == STATE__ENTRY)? "Entry":
              (m_State == STATE__TRAJECTORY)? "Trajectory":
              (m_State == STATE__FADE)? "Fade":
              "Error",
              m_FrameCount);
  if(m_State == STATE__ENTRY)
  {// Entry
    UpdateEntry();
  }
  else
  {// Trajectory & Fade
    UpdateTrajectory();
  }
}
// Update in Entry stage
void Ending::UpdateEntry()
{
  // present location
  const Rail::Location location = m_pManager->RailManager().Location();
  const gfl2::math::Vector2 point =
          location.NormalizedCrossSectionPoint();
  // distortion location
  const Rail::Location& distortionLocation = m_DistortionLocation;
  const gfl2::math::Vector2 distortionPoint =
          distortionLocation.NormalizedCrossSectionPoint();
  // displacement
  const f32& distanceDisplacement =
          distortionLocation.TravelDistance() - location.TravelDistance();
  LUNA_SOL_DIVE_ASSERT(m_Speed > 0.0f);
  const u32 requiredFrames =
          (m_Speed > 0.0f)
          ? std::ceil(distanceDisplacement / m_Speed)
          : 0xffffffff;
  ARAI_PRINT("  required frames: %d\n", requiredFrames);
  // destination
  const f32 forward = std::min(m_Speed, distanceDisplacement);
  ARAI_PRINT("  forward: %.5f\n", forward);
  const gfl2::math::Vector2 crossSectionMove =
          (requiredFrames != 0)
          ? (distortionPoint - point) / requiredFrames
          : distortionPoint - point;
  ARAI_PRINT("  cross-section move: (%.5f, %.5f)\n",
              crossSectionMove.GetX(),
              crossSectionMove.GetY());
  // scale change rate of player
  const f32 scale = m_pManager->PlayerManager().Scale();
  const f32& endScale = Parameter::Get().EndingEntryEndPlayerScale();
  const f32 scaleChangeRate =
              (requiredFrames != 0)
              ? std::pow(endScale / scale, 1.0f / requiredFrames)
              : 0.0f;
  // progress
  m_pManager->RailManager().Progress(
              forward,
              point + crossSectionMove);
  const Rail::Location updatedLocation = m_pManager->RailManager().Location();
  // update
  m_pDistortionVortex->Update();
  m_pManager->RailManager().Update();
  m_pManager->GimmickManager().Update();
  m_pManager->PlayerManager().UpdateEnding(
              updatedLocation.Directions(),
              scaleChangeRate);
  m_pManager->PlayerManager().UpdateEnergyAura(
              m_pCamera->Directions().Point());
  m_pManager->EffectManager().Update();
  ARAI_PRINT("  player scale: %.5f\n", m_pManager->PlayerManager().Scale());
  // increment frame count
  ++m_FrameCount;
  // state change
  if(requiredFrames <= 1)
  {
    // start effect
    const Rail::Directions& cameraDirections = m_pCamera->Directions();
    const f32& hitEffectOffset = Parameter::Get().EndingHitEffectOffset();
    // hit effect 1
    LUNA_SOL_DIVE_ASSERT(m_pHitEffect1 != NULL);
    if(m_pHitEffect1 != NULL)
    {
      m_pHitEffect1->Start();
      m_pHitEffect1->Update(
                  updatedLocation.Point()
                          - cameraDirections.Forward() * hitEffectOffset,
                  cameraDirections.RotationQuaternion(),
                  1.0f);
    }
    // hit effect 2
    LUNA_SOL_DIVE_ASSERT(m_pHitEffect2 != NULL);
    if(m_pHitEffect2 != NULL)
    {
      m_pHitEffect2->Start();
      m_pHitEffect2->Update(
                  updatedLocation.Point()
                          - cameraDirections.Forward() * hitEffectOffset,
                  cameraDirections.RotationQuaternion(),
                  Parameter::Get().EndingHit02EffectScale());
    }
    // start SE
    Sound::Manager::Get().PlaySE(Sound::SE__HIT_DISTORTION);
    // register volume control
    Sound::Manager::Get().RegisterVolumeControl(
                Sound::Parameter::Get().VolumeControlHitDistortion());
    // start the collapse of the distortion after collision
    m_pManager->GimmickManager().StartDistortionCollapse();
    // change state: Entry -> Trajectory
    m_State = STATE__TRAJECTORY;
    m_FrameCount = 0;
  }
}
// Update in Trajectory stage & Fade stage
void Ending::UpdateTrajectory()
{
  // trajectory location
  m_TrajectoryProgressDistance += m_Speed;
  const Rail::Location trajectoryLocation = m_pManager->RailManager().Location(
              m_TrajectoryProgressDistance,
              m_DistortionLocation.NormalizedCrossSectionPoint());
  // update
  m_pDistortionVortex->Update();
  m_pManager->RailManager().Update();
  m_pManager->GimmickManager().Update();
  m_pManager->PlayerManager().UpdateEnding(
              trajectoryLocation.Directions(),
              Parameter::Get().EndingTrajectoryPlayerScaleChangeRate());
  m_pManager->PlayerManager().UpdateEnergyAura(
              m_pCamera->Directions().Point());
  // update effect
  m_pManager->EffectManager().Update();
  // increment frame count
  ++m_FrameCount;
  // state change
  if(m_State == STATE__TRAJECTORY
          &&m_FrameCount >= Parameter::Get().EndingTrajectoryFrames())
  {
    // change stage: Trajectory -> Fade
    m_State = STATE__FADE;
    m_FrameCount = 0;
  }
}
// Whether this stage has been finished
bool Ending::IsFinished() const
{
  return m_State == STATE__FADE;
}
#if PM_DEBUG
// Quit
void Ending::Quit()
{
  // non-visualize distortion vortex
  LUNA_SOL_DIVE_ASSERT(m_pDistortionVortex != NULL);
  m_pDistortionVortex->NonVisualize();
}
#endif// PM_DEBUG
}// namespace Run
}// namespace LunaSolDive
