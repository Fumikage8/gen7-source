//======================================================================
/**
 * @file Gimmick_Energy.cpp
 * @date 2016/11/16 13:08:45
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <model/include/gfl2_BaseModel.h>
#include "./Energy.h"
#include "./EnergyStorage.h"
#include "./Parameter.h"
#include "../Assert.h"
#include "../RenderingPipeline.h"
#include "../Sound/Manager.h"
#if PM_DEBUG
#include "../Debug/DrawUtil.h"
#include "../Debug/Parameter.h"
#endif//PM_DEBUG

namespace LunaSolDive{
namespace Gimmick{
// constructor
Energy::Energy()
    : Object(),
      m_pStorage(NULL),
      m_pRenderingPipeline(NULL),
      m_pModel(NULL),
      m_State(STATE__WAIT)
{
  m_TrackingParameter.forward = 0.0f;
  m_TrackingParameter.radius = 0.0f;
  m_TrackingParameter.angle = 0.0f;
}
// Initialize
void Energy::Initialize(
            EnergyStorage* pStorage,
            RenderingPipeline* pRenderingPipeline,
            const Rail::Location& location)
{
  // initialize Base
  Object::Initialize(
              location);
  // set Storage pointer
  m_pStorage = pStorage;
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
  // set RenderingPipeline pointer
  m_pRenderingPipeline = pRenderingPipeline;
  LUNA_SOL_DIVE_ASSERT(m_pRenderingPipeline != NULL);
  // initialize Collision Cylinder
  m_Collision.Initialize(
              Location().Point(),
              - Location().Rail().Forward(),
              Location().Rail().Vertical(),
              Parameter::Get().EnergyCollisionRadius(),
              Parameter::Get().EnergyCollisionHeight());
  // initialize pointer
  m_pModel = NULL;
}
// Terminate
void Energy::Terminate()
{
  // non-visualize
  if(IsVisible())
  {
    NonVisualize();
  }
  // unset Model
  if(m_pModel != NULL)
  {
    UnsetModel();
  }
  // terminate parameter
  m_Collision.Terminate();
  m_State = STATE__WAIT;
  m_TrackingParameter.forward = 0.0f;
  m_TrackingParameter.radius = 0.0f;
  m_TrackingParameter.angle = 0.0f;
  // unset pointer
  m_pModel = NULL;
  m_pRenderingPipeline = NULL;
  m_pStorage = NULL;
  // terminate Base
  Object::Terminate();
}
// Gimmick Type (override)
GimmickType Energy::Type() const
{
  return GIMMICK_TYPE__ENERGY;
}
// check if this Energy is valid (override)
bool Energy::IsValid() const
{
  return m_pStorage != NULL;
}
// Update
void Energy::Update()
{
  LUNA_SOL_DIVE_ASSERT(IsValid());
  // update Base
  Object::Update();
  // update motion
  if(IsVisible())
  {
    m_pModel->UpdateAnimation();
  }
  // tracking
  if(m_State == STATE__TRACKING)
  {
    UpdateTracking(Rail::Directions());
  }
}
// Update
EnergyAction Energy::Update(
            const Rail::Location& location,
            const Rail::Directions& presentCollisionPoint,
            const Rail::Directions& previousCollisionPoint)
{
  LUNA_SOL_DIVE_ASSERT(IsValid());
  if(m_State == STATE__WAIT)
  {
    // Update Base
    Object::Update(location);
    // update motion
    if(IsVisible())
    {
      m_pModel->UpdateAnimation();
    }
    if(IsActive())
    {
      if(m_Collision.IsIntersect(
                  previousCollisionPoint.Point(),
                  presentCollisionPoint.Point()))
      {
        Inactivate();
        // change state
        m_State = STATE__TRACKING;
        InitializeTrackingParameter(
                    presentCollisionPoint);
        // call SE
        Sound::Manager::Get().PlaySE(Sound::SE__HIT_ENERGY);
        const EnergyAction result = {
                    true,
                    Parameter::Get().EnergyEnergyGain()};
        return result;
      }
#if PM_DEBUG
      Debug::Parameter::Get().pDrawUtil->Data().GimmickEnergyCollision(m_Collision);
#endif// PM_DEBUG
    }
  }
  else
  {
    // update motion
    if(IsVisible())
    {
      m_pModel->UpdateAnimation();
    }
    UpdateTracking(presentCollisionPoint);
  }
  const EnergyAction result = {
              false,
              0.0f};
  return result;
}
// Initialize Tracking Parameter
void Energy::InitializeTrackingParameter(
            const Rail::Directions& collisionPoint)
{
  LUNA_SOL_DIVE_ASSERT(collisionPoint.IsValid());
  // displacement
  const gfl2::math::Vector3 displacement = Location().Point() - collisionPoint.Point();
  // projection & rejection to forward
  const f32 projection = collisionPoint.Forward().Dot(displacement);
  const gfl2::math::Vector3 rejection = displacement - collisionPoint.Forward() * projection;
  // radius, angle
  const f32 radius = rejection.Length();
  const f32 angle =
          ! gfl2::math::IsAlmostZero(radius)
          ? gfl2::math::Atan2Rad(
                      collisionPoint.Horizontal().Dot(rejection),
                      collisionPoint.Vertical().Dot(rejection))
          : 0.0f;
  // initialize location parameter in tracking state
  m_TrackingParameter.forward = projection;
  m_TrackingParameter.radius = radius;
  m_TrackingParameter.angle = angle;
}
// Update in Tracking State
void Energy::UpdateTracking(
            const Rail::Directions& collisionPoint)
{
  LUNA_SOL_DIVE_ASSERT(m_State == STATE__TRACKING);
  LUNA_SOL_DIVE_ASSERT(IsValid());
  LUNA_SOL_DIVE_ASSERT(IsVisible());
  // shrink scale
  const f32 scale = m_pModel->GetScale().GetX() * 0.9f;
  m_pModel->SetScale(scale, scale, scale);
  if(scale < 0.1f)
  {
    NonVisualize();
    Terminate();
    return;
  }
  // tarcking
  if(! collisionPoint.IsValid()) return;
  m_TrackingParameter.forward *= 0.80f;
  m_TrackingParameter.radius *= 0.80f;
  const gfl2::math::Vector3 nextLocation =
          collisionPoint.Point()
          + collisionPoint.Forward()
                  * m_TrackingParameter.forward
          + collisionPoint.Vertical()
                  * m_TrackingParameter.radius
                  * gfl2::math::FCos(m_TrackingParameter.angle)
          + collisionPoint.Horizontal()
                  * m_TrackingParameter.radius
                  * gfl2::math::FSin(m_TrackingParameter.angle);
  m_pModel->SetPosition(nextLocation);
}
// Translation (override)
void Energy::Translation(
            const gfl2::math::Vector3& displacement)
{
  Object::Translation(displacement);
  m_Collision.Translation(displacement);
  if(m_pModel != NULL)
  {
    m_pModel->SetPosition(
                m_pModel->GetPosition() + displacement);
  }
}
// Set the Model for this Energy (override)
void Energy::SetModel()
{
  LUNA_SOL_DIVE_ASSERT(m_pModel == NULL);
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
  poke_3d::model::BaseModel* const pModel = m_pStorage->GetModelPtr();
  if(pModel != NULL)
  {
    m_pModel = pModel;
    // set Position & Rotation
    m_pModel->SetPosition(Location().Point());
    m_pModel->SetRotationQuat(
                gfl2::math::Quaternion(Location().Vertical(), PI)
                        * Location().RotationQuaternion());
  }
}
// Unset Model
void Energy::UnsetModel()
{
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
  m_pStorage->ReleaseModelPtr(m_pModel);
  m_pModel = NULL;
}
// Visualize this Energy (override)
void Energy::Visualize()
{
  if(m_pModel != NULL)
  {
    Object::Visualize();
#if PM_DEBUG
    if(! Debug::Parameter::Get().modelVisualizeEnergy) return;
#endif// PM_DEBUG
    m_pRenderingPipeline->AddObject(
                m_pModel->GetModelInstanceNode(),
                true);
  }
}
// Non-Visualize this Energy (override)
void Energy::NonVisualize()
{
  if(m_pModel != NULL)
  {
    Object::NonVisualize();
    m_pRenderingPipeline->RemoveObject(
                m_pModel->GetModelInstanceNode());
  }
}
// Inactivate this Energy (override)
void Energy::Inactivate()
{
  // change state
  Object::Inactivate();
  // set vanishing motion
  m_pStorage->SetVanishingMotion(m_pModel);
}
// Storage (override)
const ObjectStorage& Energy::Storage() const
{
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
  return *m_pStorage;
}
// Whether the Model is set for this Energy (oaverride)
bool Energy::IsModelSet() const
{
  return m_pModel != NULL;
}
}// namespace Gimmick
}// namespace LunaSolDive
