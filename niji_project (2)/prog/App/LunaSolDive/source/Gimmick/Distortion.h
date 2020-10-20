//======================================================================
/**
 * @file Distortion.h
 * @date 2016/11/16 17:00:47
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__DISTORTION_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__DISTORTION_H_INCLUDED__
#pragma once

#include "./Object.h"
#include "./Collision/Hemisphere.h"

// forward declaration
namespace poke_3d{
namespace model{
class BaseModel;
}// namespace model
}// namespace poke_3d

namespace LunaSolDive{
// forward declaration
class RenderingPipeline;
namespace Effect{
class Particle;
}// namespace Effect
namespace Gimmick{
// forward declaration
class DistortionStorage;

struct DistortionAction
{
  bool is_valid;
  bool is_caught;
  DistortionType type;
  DistortionRarity rarity;
  Rail::Location distortion_location;
  Rail::Location collision_location;
  f32 gravity_strength;
};

class Distortion: public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(Distortion);
public:
  // constructor
  Distortion();
  // Initialize
  void Initialize(
              DistortionStorage* pStorage,
              RenderingPipeline* pRenderingPipeline,
              const DistortionType& distortionType,
              const DistortionRarity& rarity,
              const Rail::Location& location,
              const Rail::Location& collisionLocation,
              Effect::Particle* pRarityEffect);
  // Terminate (override)
  virtual void Terminate();
  // Gimmick Type (override)
  virtual GimmickType Type() const;
  // Distortion Type
  const DistortionType& SubType() const;
  // Distortion Rarity
  const DistortionRarity& Rarity() const;
  // check if this Energy is valid (override)
  virtual bool IsValid() const;
  // Update
  void Update();
  // Update
  DistortionAction Update(
              const Rail::Location& location,
              const Rail::Directions& presentCollisionPoint,
              const Rail::Directions& previousCollisionPoint);
  // Change the state to Collapse
  void Collapse();
  // Translation (override)
  virtual void Translation(
              const gfl2::math::Vector3& displacement);
  // Set the Model for this Distortion (override)
  virtual void SetModel();
  // Unset Model
  void UnsetModel();
private:
  // State
  enum State
  {
    STATE__NONE,
    STATE__WAIT,
    STATE__VANISH,
    STATE__AFTER_COLLISION,
    STATE__COLLAPSE,
  };
  // Change the state to Vanish
  void Vanish();
  // Update in Collapse state
  void UpdateCollapse();
  // Visualize this Distortion (override)
  virtual void Visualize();
  // Non-Visualize this Distortion (override)
  virtual void NonVisualize();
  // Storage (override)
  virtual const ObjectStorage& Storage() const;
  // Whether the Model is set for this Distortion (override)
  virtual bool IsModelSet() const;
  // Storage pointer
  DistortionStorage* m_pStorage;
  // RenderingPipeline pointer
  RenderingPipeline* m_pRenderingPipeline;
  // Model pointer
  poke_3d::model::BaseModel* m_pModel;
  // Distortion Type
  DistortionType m_DistortionType;
  // Ristortion Rarity
  DistortionRarity m_Rarity;
  // State
  State m_State;
  // Collision Location
  Rail::Location m_CollisionLocation;
  // Collision
  Collision::Hemisphere m_Collision;
  // Effect: Rarity
  Effect::Particle* m_pRarityEffect;
};
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__DISTORTION_H_INCLUDED__
