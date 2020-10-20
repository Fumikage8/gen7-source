//======================================================================
/**
 * @file Action.h
 * @date 2016/11/15 11:09:35
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__ACTION_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__ACTION_H_INCLUDED__
#pragma once

#include <math/include/gfl2_Vector2.h>
#include "./GimmickType.h"
#include "./Parameter.h"
#include "../Rail/Location.h"

namespace LunaSolDive{
namespace Gimmick{
// forward declaration
struct DistortionAction;
struct EnergyAction;
struct ObstacleAction;

struct Gravity
{
  gfl2::math::Vector2 center;
  f32 strength;
};

class Action
{
public:
  // constructor
  Action();
  // Initialize
  void Initialize(
              const f32& presentDistance);
  // Terminate
  void Terminate();
  // Add Energy Action
  void Add(
              const EnergyAction& action);
  // Add Obstacle Action
  void Add(
              const ObstacleAction& action);
  // Add Distortion Action
  void Add(
              const DistortionAction& action);
  // Energy Difference
  f32 EnergyDifference() const;
  // Energy Gain
  const f32& EnergyGain() const;
  // Energy Loss
  const f32& EnergyLoss() const;
  // Whether it collided with an energy or not
  bool IsCollidedEnergy() const;
  // Whether it collided with an obstacle or not
  bool IsCollidedObstacle() const;
  // Whether it collided with a distortion or not
  bool IsCollidedDistortion() const;
  // Distortion Type
  const DistortionType& DistortionType() const;
  // Distortion Rarity
  const DistortionRarity& DistortionRarity() const;
  // Collided Distortion Location
  const Rail::Location& CollidedDistortionLocation() const;
  // Nearest Distortion Location
  const Rail::Location& NearestDistortionLocation() const;
  // The number of gravity
  const u32& GravityNumber() const;
  // Gravity
  const Gravity& Gravity(
              const u32& index) const;
  // Translation
  void Translation(
              const gfl2::math::Vector3& displacement);
private:
  // Present Distance
  f32 m_PresentDistance;
  // Enegry Gain
  f32 m_EnergyGain;
  // Energy Loss
  f32 m_EnergyLoss;
  // Distortion Type
  LunaSolDive::DistortionType m_DistortionType;
  // Distortion Rarity
  LunaSolDive::DistortionRarity m_DistortionRarity;
  // Collided Distortion Location
  Rail::Location m_CollidedDistortionLocation;
  // Nearest Distortion Location
  Rail::Location m_NearestDistortionLocation;
  // The number of gravity
  u32 m_GravityNumber;
  // Gravity by the Distortion
  Gimmick::Gravity m_GravityList[Parameter::DISTORTION_NUMBER];
};
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__ACTION_H_INCLUDED__
