//======================================================================
/**
 * @file Main.h
 * @date 2016/12/13 15:38:14
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__CAMERA__MAIN_H_INCLUDED__
#define __LUNA_SOL_DIVE__CAMERA__MAIN_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>
#include "./Base.h"
#include "../Rail/Directions.h"

namespace LunaSolDive{
// forward declaration
namespace Effect{
class Manager;
class Particle;
}// namespace Effect
namespace Rail{
class Location;
class Manager;
}// namespace Rail

namespace Camera{
class Main: public Base
{
  GFL_FORBID_COPY_AND_ASSIGN(Main);
public:
  // constructor
  Main();
  // Initialize
  void Initialize(
              const Rail::Manager* pRailManager,
              Effect::Manager* pEffectManager);
  // Terminate
  void Terminate();
  // Set Up
  void SetUp(
              const Rail::Location& location);
  // Update
  void Update(
              const Rail::Location& location);
  // Damage
  void Damage();
  // Start Gravity Vibration
  void StartGravityVibration();
  // Stop Gravity Vibration
  void StopGravityVibration();
  // Accelerate
  void Accelerate();
  // Decelerate
  void Decelerate();
  // Projection Matrix (override)
  virtual gfl2::math::Matrix44 ProjectionMatrix(
              const gfl2::gfx::CtrDisplayNo& displayNo) const;
  // View Matrix (override)
  virtual gfl2::math::Matrix34 ViewMatrix() const;
  // Directions (override)
  virtual const Rail::Directions& Directions() const;
  // Translation
  void Translation(
              const gfl2::math::Vector3& displacement);
private:
  // Acceleration & Deceleration
  enum Acceleration{
    ACCELERATION__NONE,
    ACCELERATION__ACCELERATION,
    ACCELERATION__DECELERATION,
  };
  // Update: Implement
  void UpdateImpl(
              const Rail::Location& location,
              const f32& maxMovingRate);
  // Rail Manager
  const Rail::Manager* m_pRailManager;
  // Effect Manager
  Effect::Manager* m_pEffectManager;
  // Camera Point on the Cross-Section
  gfl2::math::Vector2 m_CrossSectionPoint;
  // Directions
  Rail::Directions m_Directions;
  // Effect
  Effect::Particle* m_pEffect;
  // Whether vibration due to damage continues or not
  bool m_IsDamageVibrationContinue;
  // Frame counter for vibration due to damage
  u32 m_DamageVibrationCount;
  // Whether vibration due to gravity continues or not
  bool m_IsGravityVibrationContinue;
  // Frame count for vibration due to gravity
  u32 m_GravityVibrationCount;
  // Acceleration
  Acceleration m_Acceleration;
  // Displacement of distance by acceleration
  f32 m_AccelerationDisplacement;
};
}// namespace Camera
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__CAMERA__MAIN_H_INCLUDED__
