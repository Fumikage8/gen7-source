//======================================================================
/**
 * @file Obstacle.cpp
 * @date 2016/10/26 15:16:10
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <model/include/gfl2_BaseModel.h>
#include "./Obstacle.h"
#include "./ObstacleStorage.h"
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
Obstacle::Obstacle()
      : Object(),
        m_pStorage(NULL),
        m_pRenderingPipeline(NULL),
        m_pModel(NULL),
        m_State(STATE__WAIT){}
// Initialize
void Obstacle::Initialize(
            ObstacleStorage* pStorage,
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
              Parameter::Get().ObstacleCollisionRadius(),
              Parameter::Get().ObstacleCollisionHeight());
  // initialize parameter
  m_pModel = NULL;
  m_State = STATE__WAIT;
}
// Terminate
void Obstacle::Terminate()
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
  // unset pointer
  m_pModel = NULL;
  m_pRenderingPipeline = NULL;
  m_pStorage = NULL;
  // reset parameter
  m_Collision.Terminate();
  m_State = STATE__WAIT;
  // terminate Base
  Object::Terminate();
}
// Gimmick Type (override)
GimmickType Obstacle::Type() const
{
  return GIMMICK_TYPE__OBSTACLE;
}
// check if this Obstacle is valid (override)
bool Obstacle::IsValid() const
{
  return m_pStorage != NULL;
}
// Update
void Obstacle::Update()
{
  LUNA_SOL_DIVE_ASSERT(IsValid());
  // update base
  Object::Update();
  // update motion
  if(IsVisible())
  {
    m_pModel->UpdateAnimation();
  }
}
// Update
ObstacleAction Obstacle::Update(
            const Rail::Location& location,
            const Rail::Directions& presentCollisionPoint,
            const Rail::Directions& previousCollisionPoint)
{
  LUNA_SOL_DIVE_ASSERT(IsValid());
  // Update Base
  Object::Update(location);
  // update motion
  if(IsVisible())
  {
    if(m_State == STATE__WAIT
        && location.TravelDistance() > Location().TravelDistance())
    {
      Vanish();
    }
    m_pModel->UpdateAnimation();
  }
  if(IsActive())
  {
    if(m_Collision.IsIntersect(
                previousCollisionPoint.Point(),
                presentCollisionPoint.Point()))
    {
      Inactivate();
      // call SE
      Sound::Manager::Get().PlaySE(Sound::SE__HIT_OBSTACLE);
      const ObstacleAction result = {
                  true,
                  Parameter::Get().ObstacleEnergyLoss()};
      return result;
    }
#if PM_DEBUG
    Debug::Parameter::Get().pDrawUtil->Data().GimmickObstacleCollision(m_Collision);
#endif// PM_DEBUG
  }
  const ObstacleAction result = {
              false,
              0.0f};
  return result;
}
// Translation (override)
void Obstacle::Translation(
            const gfl2::math::Vector3& displacement)
{
  Object::Translation(displacement);
  m_Collision.Translation(displacement);
  if(m_pModel != NULL)
  {
    m_pModel->SetPosition(Location().Point());
  }
}
// Set Model
void Obstacle::SetModel()
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
void Obstacle::UnsetModel()
{
  LUNA_SOL_DIVE_ASSERT(m_pModel != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
  m_pStorage->ReleaseModelPtr(m_pModel);
  m_pModel = NULL;
}
// Vanish
void Obstacle::Vanish()
{
  LUNA_SOL_DIVE_ASSERT(m_State == STATE__WAIT);
  // change state: wait -> vanish
  m_State = STATE__VANISH;
  // set vanish motion
  m_pStorage->SetVanishingMotion(m_pModel);
}
// Visualize this Obstacle (override)
void Obstacle::Visualize()
{
  if(m_pModel != NULL)
  {
    Object::Visualize();
#if PM_DEBUG
    if(! Debug::Parameter::Get().modelVisualizeObstacle) return;
#endif// PM_DEBUG
    m_pRenderingPipeline->AddObject(
                m_pModel->GetModelInstanceNode(),
                true);
  }
}
// Non-Visualize this Obstacle
void Obstacle::NonVisualize()
{
  if(m_pModel != NULL)
  {
    Object::NonVisualize();
    m_pRenderingPipeline->RemoveObject(
                m_pModel->GetModelInstanceNode());
  }
}
// Storage (override)
const ObjectStorage& Obstacle::Storage() const
{
  LUNA_SOL_DIVE_ASSERT(m_pStorage != NULL);
  return *m_pStorage;
}
// Whether the Model is set for this Obstacle (override)
bool Obstacle::IsModelSet() const
{
  return m_pModel != NULL;
}
}// namespace Gimmick
}// namespace LunaSolDive
