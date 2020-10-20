//======================================================================
/**
 * @file Parameter.h
 * @date 2016/11/18 18:35:20
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__PARAMETER_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__PARAMETER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>

namespace LunaSolDive{
#if PM_DEBUG
// forward declaration
namespace Debug{
class Menu;
}// namespace Debug
#endif// PM_DEBUG

namespace Gimmick{
class Parameter
{
  GFL_FORBID_COPY_AND_ASSIGN(Parameter);
public:
  // Size of the Array
  enum
  {
    ENERGY_NUMBER = 32,
    OBSTACLE_NUMBER = 32,
    DISTORTION_NUMBER = 10,
  };
  // initialize
  static void Initialize(
              gfl2::heap::HeapBase* pHeap);
  // Terminate
  static void Terminate();
  // getter
  static const Parameter& Get();
  // Reset
  void Reset();
  // Object: the Distance to appear
  const f32& ObjectAppearanceDistance() const;
  // Object: the Distance to disappear
  const f32& ObjectDisappearanceDistance() const;
  // Energy: Energy gain by collision
  const f32& EnergyEnergyGain() const;
  // Energy: Distance to be activated
  const f32& EnergyActivationDistance() const;
  // Energy: Distance to be inactivated
  const f32& EnergyInactivationDistance() const;
  // Energy: Radius of the cylinder used for collision determination
  const f32& EnergyCollisionRadius() const;
  // Energy: Height of the cylinder used for collision determination
  const f32& EnergyCollisionHeight() const;
  // Obstacle: Energy loss by collision
  const f32& ObstacleEnergyLoss() const;
  // Obstacle: Distance to be activated
  const f32& ObstacleActivationDistance() const;
  // Obstacle: Distance to be inactivated
  const f32& ObstacleInactivationDistance() const;
  // Obstacle: Radius of the cylinder used for collision determination
  const f32& ObstacleCollisionRadius() const;
  // Obstacle: Height of the cylinder used for collision determination
  const f32& ObstacleCollisionHeight() const;
  // Distortion: Distance to be activated
  const f32& DistortionActivationDistance() const;
  // Distortion: Distance to be inactivated
  const f32& DistortionInactivationDistance() const;
  // Distortion: Radius of the hemisphere used for collision determination
  const f32& DistortionCollisionRadius() const;
  // Distortion: Distance the distortion vanish
  const f32& DistortionVanishDistance() const;
  // Distortion: Strength of gravity
  const f32& DistortionGravityStrength() const;
  // Distortion: Forward range of gravity
  const f32& DistortionGravityForwardRange() const;
  // Distortion: Backward range of gravity
  const f32& DistortionGravityBackwardRange() const;
  // Distortion: Vertical & Horizontal range of gravity
  const f32& DistortionGravitySideRange() const;
  // Distortion: Scale change rate in collapse
  const f32& DistortionCollapseScaleChangeRate() const;
  // Distortion: Offset distance of rarity effect
  const f32& DistortionRarityEffectOffset() const;
private:
  // Instance
  static Parameter* pInstance;
  // constructor
  Parameter();
  // Object: the Distance to appear
  f32 m_ObjectAppearanceDistance;
  // Object: the Distance to disappear
  f32 m_ObjectDisappearanceDistance;
  // Energy: Energy gain by collision
  f32 m_EnergyEnergyGain;
  // Energy: Distance to be activated
  f32 m_EnergyActivationDistance;
  // Energy: Distance to be inactivated
  f32 m_EnergyInactivationDistance;
  // Energy: Radius of the cylinder used for collision determination
  f32 m_EnergyCollisionRadius;
  // Energy: Height of the cylinder used for collision determination
  f32 m_EnergyCollisionHeight;
  // Obstacle: Energy loss by collision
  f32 m_ObstacleEnergyLoss;
  // Obstacle: Distance to be activated
  f32 m_ObstacleActivationDistance;
  // Obstacle: Distance to be inactivated
  f32 m_ObstacleInactivationDistance;
  // Obstacle: Radius of the cylinder used for collision determination
  f32 m_ObstacleCollisionRadius;
  // Obstacle: Height of the cylinder used for collision determination
  f32 m_ObstacleCollisionHeight;
  // Distortion: Distance to be activated
  f32 m_DistortionActivationDistance;
  // Distortion: Distance to be inactivated
  f32 m_DistortionInactivationDistance;
  // Distortion: Radius of the hemisphere used for collision determination
  f32 m_DistortionCollisionRadius;
  // Distortion: Distance the distortion vanish
  f32 m_DistortionVanishDistance;
  // Distortion: Strength of gravity
  f32 m_DistortionGravityStrength;
  // Distortion: Forward range of gravity
  f32 m_DistortionGravityForwardRange;
  // Distortion: Backward range of gravity
  f32 m_DistortionGravityBackwardRange;
  // Distortion: Vertical & Horizontal range of gravity
  f32 m_DistortionGravitySideRange;
  // Distortion: Scale change rate in collapse
  f32 m_DistortionCollapseScaleChangeRate;
  // Distortion: Offset distance of rarity effect
  f32 m_DistortionRarityEffectOffset;
#if PM_DEBUG
  friend class Debug::Menu;
#endif// PM_DEBUG
};
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__PARAMETER_H_INCLUDED__
