//======================================================================
/**
 * @file Gimmick_Distortion.cpp
 * @date 2016/11/16 17:12:21
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <model/include/gfl2_BaseModel.h>
#include "./Distortion.h"
#include "./DistortionStorage.h"
#include "./Parameter.h"
#include "../Assert.h"
#include "../RenderingPipeline.h"
#include "../Effect/Particle.h"
#if PM_DEBUG
#include "../Debug/DrawUtil.h"
#include "../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Gimmick{
// static function
// Strength of Gravity
static f32 GravityStrength(
            const Rail::Location& distortionLocation,
            const Rail::Location& playerLocation);
// constructor
Distortion::Distortion()
    : Object(),
      m_pStorage(NULL),
      m_pRenderingPipeline(NULL),
      m_pModel(NULL),
      m_DistortionType(DISTORTION_TYPE__SIZE),
      m_Rarity(DISTORTION_RARITY__NONE),
      m_State(STATE__NONE),
      m_pRarityEffect(NULL){}
// Initialize
void Distortion::Initialize(
            DistortionStorage* pStorage,
            RenderingPipeline* pRenderingPipeline,
            const DistortionType& distortionType,
            const DistortionRarity& rarity,
            const Rail::Location& location,
            const Rail::Location& collisionLocation,
            Effect::Particle* pRarityEffect)
{
  // initialize Base
  Object::Initialize(
              location);
  // set Storage pointer
  m_pStorage = pStorage;
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
  // set RenderingPipline pointer
  m_pRenderingPipeline = pRenderingPipeline;
  LUNA_SOL_DIVE_ASSERT(m_pRenderingPipeline != NULL);
  // set Distortion Type
  m_DistortionType = distortionType;
  LUNA_SOL_DIVE_ASSERT(m_DistortionType != DISTORTION_TYPE__SIZE);
  // set Rarity
  m_Rarity = rarity;
  LUNA_SOL_DIVE_ASSERT(m_Rarity != DISTORTION_RARITY__NONE);
  // set state
  m_State = STATE__WAIT;
  // set collision location
  m_CollisionLocation = collisionLocation;
  LUNA_SOL_DIVE_ASSERT(m_CollisionLocation.IsValid());
  // initialize Collision
  m_Collision.Initialize(
              m_CollisionLocation.Point(),
              - m_CollisionLocation.Rail().Forward(),
              m_CollisionLocation.Rail().Vertical(),
              Parameter::Get().DistortionCollisionRadius());
  // initialize Rarity Effect
  m_pRarityEffect = pRarityEffect;
  LUNA_SOL_DIVE_ASSERT(m_Rarity == DISTORTION_RARITY__0 || m_pRarityEffect != NULL);
}
// Terminate
void Distortion::Terminate()
{
  // non-visualize the model
  if(IsVisible())
  {
    NonVisualize();
  }
  // unset Model pointer
  if(m_pModel != NULL)
  {
    UnsetModel();
  }
  // delete Effect
  GFL_SAFE_DELETE(m_pRarityEffect);
  // reset Parameter
  m_DistortionType = DISTORTION_TYPE__SIZE;
  m_Rarity = DISTORTION_RARITY__NONE;
  m_State = STATE__NONE;
  m_CollisionLocation.Terminate();
  m_Collision.Terminate();
  // unset pointer
  m_pStorage = NULL;
  m_pRenderingPipeline = NULL;
  // terminate Base
  Object::Terminate();
}
// Gimmick Type (override)
GimmickType Distortion::Type() const
{
  return GIMMICK_TYPE__DISTORTION;
}
// Distortion Type
const DistortionType& Distortion::SubType() const
{
  LUNA_SOL_DIVE_ASSERT(m_DistortionType != DISTORTION_TYPE__SIZE);
  return m_DistortionType;
}
// Distortion Rarity
const DistortionRarity& Distortion::Rarity() const
{
  LUNA_SOL_DIVE_ASSERT(m_Rarity != DISTORTION_RARITY__NONE);
  return m_Rarity;
}
// check if this Energy is valid (override)
bool Distortion::IsValid() const
{
  return m_pStorage != NULL;
}
// Update
void Distortion::Update()
{
  LUNA_SOL_DIVE_ASSERT(IsValid());
  // update base
  Object::Update();
  // update motion
  if(IsVisible())
  {
    m_pModel->UpdateAnimation();
  }
  // collapse
  if(m_State == STATE__COLLAPSE)
  {
    UpdateCollapse();
  }
}
// Update
DistortionAction Distortion::Update(
            const Rail::Location& location,
            const Rail::Directions& presentCollisionPoint,
            const Rail::Directions& previousCollisionPoint)
{
  LUNA_SOL_DIVE_ASSERT(IsValid());
  // update Base
  Object::Update(location);
  // update motion
  if(IsVisible())
  {
    m_pModel->UpdateAnimation();
  }
  // action
  DistortionAction action;
  action.is_valid = false;
  action.is_caught = false;
  action.type = m_DistortionType;
  action.rarity = m_Rarity;
  action.distortion_location = Location();
  action.collision_location = m_CollisionLocation;
  action.gravity_strength = 0.0f;
  LUNA_SOL_DIVE_ASSERT(m_State != STATE__AFTER_COLLISION);
  if(IsActive())
  {
    action.is_valid = true;
    if(m_State == STATE__WAIT
            && m_Collision.IsIntersect(
                          previousCollisionPoint.Point(),
                          presentCollisionPoint.Point()))
    {// collide
      action.is_caught = true;
      Inactivate();
      m_State = STATE__AFTER_COLLISION;
    }
    else
    {
      // gravity
      action.gravity_strength = GravityStrength(
                  m_CollisionLocation,
                  location);
      // vanish
      if(m_State != STATE__VANISH
              && Location().TravelDistance() - location.TravelDistance()
                 < Parameter::Get().DistortionVanishDistance())
      {
        Vanish();
      }
    }
#if PM_DEBUG
    Debug::Parameter::Get().pDrawUtil->Data().GimmickDistortionCollision(m_Collision);
#endif// PM_DEBUG
  }
  return action;
}
// Change the State to Vanish
void Distortion::Vanish()
{
  LUNA_SOL_DIVE_ASSERT(m_State == STATE__WAIT);
  // change state
  m_State = STATE__VANISH;
  // change motion
  m_pStorage->SetVanishingMotion(
              m_DistortionType,
              m_pModel);
  // fade out rarity effect
  if(m_pRarityEffect != NULL)
  {
    m_pRarityEffect->End();
  }
}
// Change the State to Collapse
void Distortion::Collapse()
{
  LUNA_SOL_DIVE_ASSERT(m_State != STATE__NONE);
  // ignore except for After Collision state
  if(m_State != STATE__AFTER_COLLISION) return;
  // change state
  m_State = STATE__COLLAPSE;
  // change motion
  m_pStorage->SetVanishingMotion(
              m_DistortionType,
              m_pModel);
  // fade out rarity effect
  if(m_pRarityEffect != NULL)
  {
    m_pRarityEffect->End();
  }
}
// Update in Collapse state
void Distortion::UpdateCollapse()
{
  LUNA_SOL_DIVE_ASSERT(m_State == STATE__COLLAPSE);
  LUNA_SOL_DIVE_ASSERT(IsValid());
  LUNA_SOL_DIVE_ASSERT(IsVisible());
  const f32& scaleChangeRate = Parameter::Get().DistortionCollapseScaleChangeRate();
  const f32 scale = m_pModel->GetScale().GetX() * scaleChangeRate;
  if(scale < 0.01f)
  {
    NonVisualize();
    Terminate();
  }
  else
  {
    m_pModel->SetScale(scale, scale, scale);
  }
}
// Translation (override)
void Distortion::Translation(
            const gfl2::math::Vector3& displacement)
{
  Object::Translation(displacement);
  m_Collision.Translation(displacement);
  if(m_pModel != NULL)
  {
    m_pModel->SetPosition(Location().Point());
  }
  // translation of effects is executed by Effect::Manager
}
// Set Model
void Distortion::SetModel()
{
  LUNA_SOL_DIVE_ASSERT(m_pModel == NULL);
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
  poke_3d::model::BaseModel* pModel = m_pStorage->GetModelPtr(
              m_DistortionType);
  LUNA_SOL_DIVE_ASSERT(pModel != NULL);
  if(pModel != NULL)
  {
    m_pModel = pModel;
    m_pModel->SetPosition(
                Location().Point());
    m_pModel->SetRotationQuat(
                gfl2::math::Quaternion(Location().Vertical(), PI)
                        * Location().RotationQuaternion());
  }
}
// Unset Model
void Distortion::UnsetModel()
{
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
  m_pStorage->ReleaseModelPtr(m_pModel);
  m_pModel = NULL;
}
// Visualize this Distortion
void Distortion::Visualize()
{
  if(! IsVisible())
  {
    if(m_pModel != NULL)
    {
      // update flag
      Object::Visualize();
      // start effect
      if(m_pRarityEffect != NULL)
      {
        m_pRarityEffect->Start();
        const f32& offset = Parameter::Get().DistortionRarityEffectOffset();
        m_pRarityEffect->Update(
                    Location().Point()
                            - Location().Forward() * offset,
                    Location().RotationQuaternion(),
                    1.0f);
      }
#if PM_DEBUG
      if(! Debug::Parameter::Get().modelVisualizeDistortion) return;
#endif// PM_DEBUG
      // visualize model
      m_pRenderingPipeline->AddObject(
                  m_pModel->GetModelInstanceNode(),
                  true);
    }
  }
}
// Non-Visualize this Distortion
void Distortion::NonVisualize()
{
  if(IsVisible())
  {
    if(m_pModel != NULL)
    {
      // update flag
      Object::NonVisualize();
      // end effect
      if(m_pRarityEffect != NULL)
      {
        m_pRarityEffect->End();
      }
      // non-visualize model
      m_pRenderingPipeline->RemoveObject(
                  m_pModel->GetModelInstanceNode());
    }
  }
}
// Storage (override)
const ObjectStorage& Distortion::Storage() const
{
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
  return *m_pStorage;
}
// Whether the Model is set for this Distortion (override)
bool Distortion::IsModelSet() const
{
  return m_pModel != NULL;
}
// static function
// Strength of Gravity
static f32 GravityStrength(
            const Rail::Location& distortionLocation,
            const Rail::Location& playerLocation)
{
  // displacement
  const f32 distance =
          distortionLocation.TravelDistance()
          - playerLocation.TravelDistance();
  const f32 sideDistance =
          (playerLocation.NormalizedCrossSectionPoint()
                  - distortionLocation.NormalizedCrossSectionPoint())
          .Length() * playerLocation.RailRadius();
  LUNA_SOL_DIVE_ASSERT(distance > - Parameter::Get().DistortionVanishDistance() * (1.0f + EPSILON));
  LUNA_SOL_DIVE_ASSERT(sideDistance >= 0.0f);
  // distance weight
  const f32 distanceWeight =
          (Parameter::Get().DistortionGravityForwardRange() <= 0.0f
                  || Parameter::Get().DistortionGravityBackwardRange() <= 0.0f)
          ? 0.0f
          : (distance >= 0.0f)
          ? std::exp(- distance / Parameter::Get().DistortionGravityForwardRange())
          : std::exp(distance / Parameter::Get().DistortionGravityBackwardRange());
  LUNA_SOL_DIVE_ASSERT(0.0f <= distanceWeight && distanceWeight <= 1.0f);
  // side weight
  const f32 sideWeight =
          (Parameter::Get().DistortionGravitySideRange() > 0.0f)
          ? std::exp(- std::pow(sideDistance / Parameter::Get().DistortionActivationDistance(), 2))
          : 1.0f;
  LUNA_SOL_DIVE_ASSERT(0.0f <= sideWeight && sideWeight <= 1.0f);
  // result
  return Parameter::Get().DistortionGravityStrength()
          * distanceWeight
          * sideWeight;
}
}// namespace Gimmick
}// namespace LunaSolDive
