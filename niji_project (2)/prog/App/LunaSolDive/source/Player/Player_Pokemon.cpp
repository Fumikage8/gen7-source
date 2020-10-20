//======================================================================
/**
 * @file Player_Pokemon.cpp
 * @date 2016/09/07 20:06:29
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <model/include/gfl2_CharaModel.h>
#include <model/include/gfl2_CharaModelFactory.h>
#include "System/include/nijiAllocator.h"
#include "./MotionController.h"
#include "./Parameter.h"
#include "./Pokemon.h"
#include "../Assert.h"
#include "../Effect/Manager.h"
#include "../Effect/Particle.h"
#include "arc_index/LunaSolDiveEffect.gaix"
#include "niji_conv_header/field/chara/chara_model_id.h"
#if PM_DEBUG
#include "../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Player{
// constructor
Pokemon::Pokemon()
    : SimpleModel(),
      m_Version(VERSION__NONE),
      m_pModel(NULL),
      m_pEffectManager(NULL),
      m_pEffectBodyAura(NULL),
      m_pEffectTrajectory(NULL),
      m_pEffectHighSpeedTrajectory(NULL),
      m_pEffectFootAura(NULL){}
// Initialize
void Pokemon::Initialize(
            const Version& version,
            gfl2::heap::HeapBase* pHeap,
            RenderingPipeline* pRenderingPipeline,
            Effect::Manager* pEffectManager,
            MotionController* pMotionController)
{
  // initialize Base
  SimpleModel::Initialize(
              pRenderingPipeline);
  // set version
  m_Version = version;
  LUNA_SOL_DIVE_ASSERT(m_Version != VERSION__NONE);
  // set parameter
  m_pEffectManager = pEffectManager;
  LUNA_SOL_DIVE_ASSERT(m_pEffectManager != NULL);
  // initialize Model
  m_pModel = GFL_NEW(pHeap) poke_3d::model::CharaModel();
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  // register the model with the MotionController
  LUNA_SOL_DIVE_ASSERT(pMotionController != NULL);
  pMotionController->RegisterPokemonModel(m_pModel);
}
// Terminate
void Pokemon::Terminate()
{
  // delete Model
  if(m_pModel != NULL)
  {
    NonVisualize();
    m_pModel->Destroy();
    GFL_DELETE(m_pModel);
    m_pModel = NULL;
  }
  // delete Effect
  GFL_SAFE_DELETE(m_pEffectBodyAura);
  GFL_SAFE_DELETE(m_pEffectTrajectory);
  GFL_SAFE_DELETE(m_pEffectHighSpeedTrajectory);
  GFL_SAFE_DELETE(m_pEffectFootAura);
  // reset Parameter
  m_Version = VERSION__NONE;
  m_Waist.Terminate();
  // terminate Base
  SimpleModel::Terminate();
}
// Create
void Pokemon::Create(
            gfl2::heap::HeapBase* pHeap,
            System::nijiAllocator* pAllocator,
            poke_3d::model::CharaModelFactory* pCharaModelFactory)
{
  using namespace gfl2::renderingengine::scenegraph;
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pAllocator != NULL);
  LUNA_SOL_DIVE_ASSERT(pCharaModelFactory != NULL);
  // create Model
  const u32 charaID =
              (m_Version == VERSION__SOL)
              ? CHARA_MODEL_ID_PM0873_01
              : CHARA_MODEL_ID_PM0874_01;
  pCharaModelFactory->LoadModelSync(charaID);
  pCharaModelFactory->SetupModel(
              pAllocator,
              charaID);
  pCharaModelFactory->CreateModel(
              pAllocator,
              pHeap,
              m_pModel,
              charaID);
  m_pModel->SetPosition(gfl2::math::Vector3::GetZero());
  m_pModel->SetRotation(gfl2::math::Vector3::GetZero());
  m_pModel->SetScale(1.0f, 1.0f, 1.0f);
  // visualize
  Visualize();
}
// Update
void Pokemon::Update(
            const Rail::Directions& location,
            const gfl2::math::Vector3& offset,
            const f32& elevationAngle)
{
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pEffectBodyAura != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pEffectTrajectory != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pEffectHighSpeedTrajectory != NULL);
  LUNA_SOL_DIVE_ASSERT(location.IsValid());
  // update Waist Location
  const s32 indexWaistJoint = m_pModel->GetJointIndex("Waist");
  LUNA_SOL_DIVE_ASSERT(indexWaistJoint != -1);
  const gfl2::math::Vector3 waistRelativeLocation =
          m_pModel->GetJointWorldPosition(indexWaistJoint)
          - m_pModel->GetPosition();
  const gfl2::math::Vector3 forwardDirection =
          m_pModel->GetRotationQuat().Transform(gfl2::math::Vector3::GetZUnit());
  const gfl2::math::Vector3 verticalDirection =
          m_pModel->GetRotationQuat().Transform(gfl2::math::Vector3::GetYUnit());
  const gfl2::math::Vector3 horizontalDirection =
          m_pModel->GetRotationQuat().Transform(- gfl2::math::Vector3::GetXUnit());
  const f32 waistRelativeForward = forwardDirection.Dot(waistRelativeLocation);
  const f32 waistRelativeVertical = verticalDirection.Dot(waistRelativeLocation);
  const f32 waistRelativeHorizontal = horizontalDirection.Dot(waistRelativeLocation);
  m_Waist.Initialize(
              location.Point()
                      + offset
                      + location.Forward() * waistRelativeForward
                      + location.Vertical() * waistRelativeVertical
                      + location.Horizontal() * waistRelativeHorizontal,
              location.Forward(),
              location.Vertical());
  // update Model Position & Rotation
  m_pModel->SetPosition(
              location.Point() + offset);
  m_pModel->SetRotationQuat(
              gfl2::math::Quaternion(
                          location.Horizontal(),
                          elevationAngle)
              * location.RotationQuaternion());
  // motion update is execute by MotionController
  // update effect
  const f32 scale = GetScale();
  m_pEffectBodyAura->Update(
              m_Waist.Point(),
              m_Waist.RotationQuaternion(),
              scale);
  m_pEffectTrajectory->Update(
              m_Waist.Point(),
              m_Waist.RotationQuaternion(),
              scale);
  m_pEffectHighSpeedTrajectory->Update(
              m_Waist.Point(),
              m_Waist.RotationQuaternion(),
              scale);
  if(m_pEffectFootAura != NULL)
  {
    m_pEffectFootAura->Update(
                m_pModel->GetPosition(),
                m_Waist.RotationQuaternion(),
                scale);
  }
}
// Update Energy
void Pokemon::UpdateEnergy(
            const f32& energy)
{
  const f32& highSpeedBoundary = Parameter::Get().HighSpeedTrajectoryBoundary();
  // start/stop high speed trajectory
  if(energy < highSpeedBoundary
          && m_pEffectHighSpeedTrajectory->IsAlive())
  {
    m_pEffectHighSpeedTrajectory->End();
  }
  if(energy >= highSpeedBoundary
          && ! m_pEffectHighSpeedTrajectory->IsAlive())
  {
    m_pEffectHighSpeedTrajectory->Start();
    m_pEffectHighSpeedTrajectory->Update(
                m_Waist.Point(),
                m_Waist.RotationQuaternion(),
                GetScale());
  }
}
// Visualize (override)
void Pokemon::Visualize()
{
  // visualize effect
  if(! IsVisible())
  {
    // delete previous effect
    GFL_SAFE_DELETE(m_pEffectBodyAura);
    GFL_SAFE_DELETE(m_pEffectTrajectory);
    GFL_SAFE_DELETE(m_pEffectHighSpeedTrajectory);
    GFL_SAFE_DELETE(m_pEffectFootAura);
    // create effect: Energy Aura at Body
    m_pEffectBodyAura = m_pEffectManager->Create(
                GARC_LunaSolDiveEffect_ea_dive_run_lo_lz_BPTCL);
    LUNA_SOL_DIVE_ASSERT(m_pEffectBodyAura != NULL);
    if(m_pEffectBodyAura != NULL)
    {
      m_pEffectBodyAura->Start();
    }
    // create effect: Trajectory
    m_pEffectTrajectory =  m_pEffectManager->Create(
                GARC_LunaSolDiveEffect_ea_dive_run_fol_lo_lz_BPTCL);
    LUNA_SOL_DIVE_ASSERT(m_pEffectTrajectory != NULL);
    if(m_pEffectTrajectory != NULL)
    {
      m_pEffectTrajectory->Start();
    }
    // create effect: High Speed Trajectory
    m_pEffectHighSpeedTrajectory = m_pEffectManager->Create(
                GARC_LunaSolDiveEffect_ea_dive_run_fol_max_lo_lz_BPTCL);
    LUNA_SOL_DIVE_ASSERT(m_pEffectHighSpeedTrajectory != NULL);
    // create Effect: Energy Aura at Foot (Sol only)
    m_pEffectFootAura =
            (m_Version == VERSION__SOL)
            ? m_pEffectManager->Create(
                        GARC_LunaSolDiveEffect_ea_dive_run_root_lo_lz_BPTCL)
            : NULL;
     if(m_pEffectFootAura != NULL)
     {
      m_pEffectFootAura->Start();
     }
    }
  // visualize model
#if PM_DEBUG
  if(! Debug::Parameter::Get().modelVisualizePokemon)
  {
    OverwriteVisualizeFlag(true);
    return;
  }
#endif// PM_DEBUG
  SimpleModel::Visualize();
}
// Non-Visualize (override)
void Pokemon::NonVisualize()
{
  // end effect
  if(IsVisible())
  {
    if(m_pEffectBodyAura != NULL)
    {
      m_pEffectBodyAura->End();
    }
    if(m_pEffectTrajectory != NULL)
    {
      m_pEffectTrajectory->End();
    }
    if(m_pEffectHighSpeedTrajectory != NULL)
    {
      m_pEffectHighSpeedTrajectory->End();
    }
    if(m_pEffectFootAura != NULL)
    {
      m_pEffectFootAura->End();
    }
  }
  // non-visualize model
  SimpleModel::NonVisualize();
}
// Waist Location
const Rail::Directions& Pokemon::Waist() const
{
  return m_Waist;
}
// Model (override)
poke_3d::model::BaseModel& Pokemon::Model() const
{
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  return *m_pModel;
}
// Scale: getter
f32 Pokemon::GetScale() const
{
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  return m_pModel->GetScale().GetX();
}
// Scale: setter
void Pokemon::SetScale(
            const f32& value)
{
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  m_pModel->SetScale(
              value,
              value,
              value);
}
// Translation (override)
void Pokemon::Translation(
            const gfl2::math::Vector3& displacement)
{
  SimpleModel::Translation(displacement);
  m_Waist.Translation(displacement);
  // translation of effects is executed by Effect::Manager
}
}// namespace Player
}// namespace LunaSolDive
