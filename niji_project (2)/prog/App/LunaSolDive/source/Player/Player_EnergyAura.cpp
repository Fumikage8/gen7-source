//======================================================================
/**
 * @file Player_EnergyAura.cpp
 * @date 2016/12/01 12:15:07
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <algorithm>
#include <model/include/gfl2_BaseModel.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNodeFactory.h>
#include "System/include/nijiAllocator.h"
#include "./EnergyAura.h"
#include "./Parameter.h"
#include "../Assert.h"
#include "../Archive/Model.h"
#include "../Effect/Manager.h"
#include "../Effect/Particle.h"
#include "../Run/Parameter.h"
#include "../Sound/Manager.h"
#include "arc_index/LunaSolDiveEffect.gaix"
#include "arc_index/LunaSolDiveModel.gaix"
#if PM_DEBUG
#include "../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Player{
// static function
// Update Scale
static f32 UpdateScale(
            const f32& energy,
            const f32& previousScale);
// constructor
EnergyAura::EnergyAura()
    : SimpleModel(),
      m_pEffectManager(NULL),
      m_pModel(NULL),
      m_pMotionNode(NULL),
      m_State(STATE__INACTIVE),
      m_pUpDownEffect(NULL),
      m_pStartEffect(NULL),
      m_EnergyRate(0.0f){}
// Initialize
void EnergyAura::Initialize(
            gfl2::heap::HeapBase* pHeap,
            RenderingPipeline* pRenderingPipeline,
            Effect::Manager* pEffectManager)
{
  // initialize Base
  SimpleModel::Initialize(
              pRenderingPipeline);
  // set pointer
  m_pEffectManager = pEffectManager;
  LUNA_SOL_DIVE_ASSERT(m_pEffectManager != NULL);
  // initialize Model
  m_pModel = GFL_NEW(pHeap) poke_3d::model::BaseModel();
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
}
// Terminate
void EnergyAura::Terminate()
{
  // delete Model
  if(m_pModel != NULL)
  {
    if(IsVisible())
    {
      NonVisualize();
    }
    m_pModel->Destroy();
    GFL_DELETE(m_pModel);
    m_pModel = NULL;
  }
  GFL_SAFE_DELETE(m_pMotionNode);
  // delete Effect
  GFL_SAFE_DELETE(m_pUpDownEffect);
  GFL_SAFE_DELETE(m_pStartEffect);
  // reset parameter
  m_pEffectManager = NULL;
  m_State = STATE__INACTIVE;
  m_EnergyRate = 0.0f;
  // terminate Base
  SimpleModel::Terminate();
}
// Create
void EnergyAura::Create(
            gfl2::heap::HeapBase* pHeap,
            System::nijiAllocator* pAllocator)
{
  using namespace gfl2::renderingengine::scenegraph;
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pAllocator != NULL);
  // create Model
  const u32 modelIndex = GARC_LunaSolDiveModel_aura_GFBMDLP;
  SimpleModel::Create(
              pHeap,
              pAllocator,
              Archive::Model::Get().Data(modelIndex));
  // create Motion ResourceNode
  const u32 motionIndex = GARC_LunaSolDiveModel_aura_GFBMOT;
  resource::AnimationResourceNodeFactory animationFactory;
  m_pMotionNode = animationFactory.CreateResourceNode(
             pAllocator,
             Archive::Model::Get().Data(motionIndex),
             0);
  LUNA_SOL_DIVE_ASSERT(m_pMotionNode != NULL);
  // set animation
  m_pModel->ChangeAnimationByResourceNode(
              m_pMotionNode);
  m_pModel->SetAnimationLoop(true);
  // set Scale
  m_pModel->SetScale(0.0f, 0.0f, 0.0f);
  // initialize parameter
  m_EnergyRate = Run::Parameter::Get().EnergyInitialRate();
}
// Update
void EnergyAura::Update(
            const gfl2::math::Vector3& location,
            const gfl2::math::Quaternion& rotation)
{
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  LUNA_SOL_DIVE_ASSERT(m_State != STATE__INACTIVE);
  // update scale
  const f32 scale =
          (m_State == STATE__ACTIVE)
          ? UpdateScale(m_EnergyRate, Scale())
          : 0.8f * Scale();
  m_pModel->SetScale(
              scale,
              scale,
              scale);
  // non-visualize
  const f32 visualizeScale = 1.0e-3f;
  if(IsVisible() && scale < visualizeScale)
  {
    NonVisualize();
    return;
  }
  // visualize
  if(! IsVisible())
  {
    if(scale < visualizeScale)
    {
      return;
    }
    Visualize();
  }
  // update animation
  m_pModel->UpdateAnimation();
  // update location & rotation
  m_pModel->SetPosition(location);
  m_pModel->SetRotationQuat(rotation);
  // update Effect
  if(m_pUpDownEffect != NULL
          && m_pUpDownEffect->IsAlive())
  {
    m_pUpDownEffect->Update(
                m_pModel->GetPosition(),
                m_pModel->GetRotationQuat(),
                Scale());
  }
  if(m_pStartEffect != NULL
          && m_pStartEffect->IsAlive())
  {
    m_pStartEffect->Update(
                m_pModel->GetPosition(),
                m_pModel->GetRotationQuat(),
                Scale());
  }
}
// Update Energy
void EnergyAura::UpdateEnergy(
            const f32& energyRate)
{
  // update energy rate
  m_EnergyRate = energyRate;
}
// Up
void EnergyAura::Up(
            const bool& isEnergyOverLimit)
{
  if(! IsVisible()) return;
  GFL_SAFE_DELETE(m_pUpDownEffect);
  m_pUpDownEffect = m_pEffectManager->Create(
              isEnergyOverLimit
                  ? GARC_LunaSolDiveEffect_ea_dive_auradome_maxup_lz_BPTCL
                  : GARC_LunaSolDiveEffect_ea_dive_auradome_up_lz_BPTCL);
  if(m_pUpDownEffect != NULL)
  {
    m_pUpDownEffect->Start();
    m_pUpDownEffect->Update(
                m_pModel->GetPosition(),
                m_pModel->GetRotationQuat(),
                Scale());
  }
}
// Down
void EnergyAura::Down()
{
  if(! IsVisible()) return;
  GFL_SAFE_DELETE(m_pUpDownEffect);
  m_pUpDownEffect = m_pEffectManager->Create(
              GARC_LunaSolDiveEffect_ea_dive_auradome_down_lz_BPTCL);
  if(m_pUpDownEffect != NULL)
  {
    m_pUpDownEffect->Start();
    m_pUpDownEffect->Update(
                m_pModel->GetPosition(),
                m_pModel->GetRotationQuat(),
                Scale());
  }
}
// Vanish
void EnergyAura::Vanish()
{
  LUNA_SOL_DIVE_ASSERT(m_State == STATE__ACTIVE);
  m_State = STATE__VANISH;
}
// Scale
f32 EnergyAura::Scale() const
{
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  return m_pModel->GetScale().GetX();
}
// Model (override)
poke_3d::model::BaseModel& EnergyAura::Model() const
{
  LUNA_SOL_DIVE_ASSERT(m_pModel);
  return *m_pModel;
}
// Visualize with Effect & SE
void EnergyAura::Activate()
{
  LUNA_SOL_DIVE_ASSERT(! IsVisible());
  LUNA_SOL_DIVE_ASSERT(m_State == STATE__INACTIVE);
  // delete previous effect
  GFL_SAFE_DELETE(m_pStartEffect);
  // create effect
  m_pStartEffect = m_pEffectManager->Create(
              GARC_LunaSolDiveEffect_ea_dive_auradome_st_lz_BPTCL);
  if(m_pStartEffect != NULL)
  {
    m_pStartEffect->Start();
    m_pStartEffect->Update(
                m_pModel->GetPosition(),
                m_pModel->GetRotationQuat(),
                Scale());
  }
  // call SE
  Sound::Manager::Get().PlaySE(Sound::SE__ACTIVATE_ENERGY_AURA);
  // visualize model
  Visualize();
}
// Visualize (override)
void EnergyAura::Visualize()
{
  // change state
  m_State = STATE__ACTIVE;
  // visualize model
#if PM_DEBUG
  if(! Debug::Parameter::Get().modelVisualizeEnergyAura)
  {
    OverwriteVisualizeFlag(true);
    return;
  }
#endif// PM_DEBUG
  SimpleModel::Visualize();
}
// Non-Visualize (override)
void EnergyAura::NonVisualize()
{
  // end effect
  if(IsVisible())
  {
    if(m_pUpDownEffect != NULL)
    {
      m_pUpDownEffect->End();
    }
    if(m_pStartEffect != NULL)
    {
      m_pStartEffect->End();
    }
  }
  // set scale
  m_pModel->SetScale(0.0f, 0.0f, 0.0f);
  // non-visualize model
  SimpleModel::NonVisualize();
}
// static function
// Update Scale
static f32 UpdateScale(
            const f32& energyRate,
            const f32& previousScale)
{
  const f32& minScale = Parameter::Get().EnergyAuraMinScale();
  const f32& maxScale = Parameter::Get().EnergyAuraMaxScale();
  const f32& maxChange = Parameter::Get().EnergyAuraMaxScaleChange();
  const f32 scale =
          (energyRate < EPSILON)
          ? 0.0f
          : minScale + (maxScale - minScale) * std::min(energyRate, 1.0f);
   return gfl2::math::Clamp(
              scale,
              previousScale - maxChange,
              previousScale + maxChange);
}
}// namespace Player
}// namespace LunaSolDive
