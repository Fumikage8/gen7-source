//======================================================================
/**
 * @file Energy.h
 * @date 2016/11/16 13:01:36
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__ENERGY_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__ENERGY_H_INCLUDED__
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
class EnergyStorage;

struct EnergyAction
{
  bool is_valid;
  f32 energy_gain;
};

class Energy: public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(Energy);
public:
  // constructor
  Energy();
  // initialize
  void Initialize(
              EnergyStorage* pStorage,
              RenderingPipeline* pRenderingPipeline,
              const Rail::Location& location);
  // Terminate (override)
  virtual void Terminate();
  // Gimmick Type (override)
  virtual GimmickType Type() const;
  // check if this Energy is valid (override)
  virtual bool IsValid() const;
  // Update
  void Update();
  // Update
  EnergyAction Update(
              const Rail::Location& location,
              const Rail::Directions& presentCollisionPoint,
              const Rail::Directions& previousollisionPoint);
  // Translation (override)
  virtual void Translation(
              const gfl2::math::Vector3& displacement);
  // Set the Model for this Energy (override)
  virtual void SetModel();
  // Unset Model
  void UnsetModel();
private:
  // State
  enum State
  {
    STATE__WAIT,
    STATE__TRACKING,
  };
  // Location Parameter in the Traking State
  struct TrackingParameter
  {
    f32 forward;
    f32 radius;
    f32 angle;
  };
  // Initialize Tracking Parameter
  void InitializeTrackingParameter(
              const Rail::Directions& collisionPoint);
  // Update in Tracking State
  void UpdateTracking(
              const Rail::Directions& collisionPoint);
  // Visualize this Energy (overide)
  virtual void Visualize();
  // Non-Visualize this Energy (override)
  virtual void NonVisualize();
  // Inactivate this Energy (override)
  virtual void Inactivate();
  // Storage (override)
  virtual const ObjectStorage& Storage() const;
  // Whether the Model is set for this Energy (override)
  virtual bool IsModelSet() const;
  // Storage pointer
  EnergyStorage* m_pStorage;
  // RenderingPipeline pointer
  RenderingPipeline* m_pRenderingPipeline;
  // Model pointer
  poke_3d::model::BaseModel* m_pModel;
  // Collision
  Collision::Cylinder m_Collision;
  // State
  State m_State;
  // Location Parameter in the Tracking State
  TrackingParameter m_TrackingParameter;
};
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__ENERGY_H_INCLUDED__