//======================================================================
/**
 * @file Obstacle.h
 * @date 2016/10/26 15:06:46
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__OBSTACLE_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__OBSTACLE_H_INCLUDED__
#pragma once

#include "./Object.h"
#include "./Collision/Cylinder.h"

// forward declaration
namespace poke_3d{
namespace model{
class BaseModel;
}// namespace model
}// namespace poke_3d

namespace LunaSolDive{
// forward declaration
class RenderingPipeline;
namespace Gimmick{
// forward declaration
class ObstacleStorage;

struct ObstacleAction
{
  bool is_valid;
  f32 energy_loss;
};

class Obstacle: public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(Obstacle);
public:
  // constructor
  Obstacle();
  // initialize
  void Initialize(
              ObstacleStorage* pStorage,
              RenderingPipeline* pRenderingPipeline,
              const Rail::Location& location);
  // Terminate (override)
  virtual void Terminate();
  // Gimmick Type (override)
  virtual GimmickType Type() const;
  // check if this Obstacle is valid (override)
  virtual bool IsValid() const;
  // Update
  void Update();
  // Update
  ObstacleAction Update(
              const Rail::Location& location,
              const Rail::Directions& previousCollisionPoint,
              const Rail::Directions& presentCollisionPoint);
  // Translation (override)
  virtual void Translation(
              const gfl2::math::Vector3& displacement);
  // Set the Model for this Obstacle (override)
  virtual void SetModel();
  // Unset Model
  void UnsetModel();
private:
  // State
  enum State
  {
    STATE__WAIT,
    STATE__VANISH,
  };
  // Vanish
  void Vanish();
  // Visualize this Obstacle (override)
  virtual void Visualize();
  // Non-Visualize this Obstacle (override)
  virtual void NonVisualize();
  // Storage (override)
  virtual const ObjectStorage& Storage() const;
  // Whether the Model is set for this Obstacle (override)
  virtual bool IsModelSet() const;
  // Storage pointer
  ObstacleStorage* m_pStorage;
  // RenderingPipeline pointer
  RenderingPipeline* m_pRenderingPipeline;
  // Model pointer
  poke_3d::model::BaseModel* m_pModel;
  // Collision
  Collision::Cylinder m_Collision;
  // State
  State m_State;
};
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__OBSTACLE_H_INCLUDED__
