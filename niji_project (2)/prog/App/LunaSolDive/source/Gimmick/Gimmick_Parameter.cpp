//======================================================================
/**
 * @file Gimmick_Parameter.cpp
 * @date 2016/11/18 18:47:08
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "./Parameter.h"
#include "../Assert.h"
#if PM_DEBUG
#include "../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Gimmick{
// initialize
void Parameter::Initialize(
            gfl2::heap::HeapBase* pHeap)
{
  LUNA_SOL_DIVE_ASSERT(pInstance == NULL);
  pInstance = GFL_NEW(pHeap) Parameter();
#if PM_DEBUG
  Debug::Parameter::Get().pGimmickParameter = pInstance;
#endif// PM_DEBUG
}
// Terminate
void Parameter::Terminate()
{
  GFL_SAFE_DELETE(pInstance);
}
// getter
const Parameter& Parameter::Get()
{
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
  return *pInstance;
}
// Instance
Parameter* Parameter::pInstance = NULL;
// constructor
Parameter::Parameter()
{
  Reset();
}
// Reset
void Parameter::Reset()
{
  // Object
  m_ObjectAppearanceDistance = 100.0e+2f;
  m_ObjectDisappearanceDistance = -10.0e+2f;
  // Energy
  m_EnergyEnergyGain = 10.0e-2f;
  m_EnergyActivationDistance = 500.0f;
  m_EnergyInactivationDistance = -200.0f;
  m_EnergyCollisionRadius = 180.0f;
  m_EnergyCollisionHeight = 150.0f;
  // Obstacle
  m_ObstacleEnergyLoss = 20.0e-2f;
  m_ObstacleActivationDistance = 500.0f;
  m_ObstacleInactivationDistance = -200.0f;
  m_ObstacleCollisionRadius = 130.0f;
  m_ObstacleCollisionHeight = 150.0f;
  // Distortion
  m_DistortionActivationDistance = 50.0e+2f;
  m_DistortionInactivationDistance = 0.0f;
  m_DistortionCollisionRadius = 170.0f;
  m_DistortionVanishDistance = 5.0e+2f;
  m_DistortionGravityStrength = 0.060f;
  m_DistortionGravityForwardRange = 5.0e+2f;
  m_DistortionGravityBackwardRange = 2.0e+2f;
  m_DistortionGravitySideRange = 120.0f;
  m_DistortionCollapseScaleChangeRate = 0.80f;
  m_DistortionRarityEffectOffset = 100.0f;
}
// Object: the Distance to appear
const f32& Parameter::ObjectAppearanceDistance() const
{
  return m_ObjectAppearanceDistance;
}
// Object: the Distance to disappear
const f32& Parameter::ObjectDisappearanceDistance() const
{
  return m_ObjectDisappearanceDistance;
}
// Energy: Energy gain by collision
const f32& Parameter::EnergyEnergyGain() const
{
  return m_EnergyEnergyGain;
}
// Energy: Distance to be activated
const f32& Parameter::EnergyActivationDistance() const
{
  return m_EnergyActivationDistance;
}
// Energy: Distance to be inactivated
const f32& Parameter::EnergyInactivationDistance() const
{
  return m_EnergyInactivationDistance;
}
// Energy: Radius of the cylinder used for collision determination
const f32& Parameter::EnergyCollisionRadius() const
{
  return m_EnergyCollisionRadius;
}
// Energy: Height of the cylinder used for collision determination
const f32& Parameter::EnergyCollisionHeight() const
{
  return m_EnergyCollisionHeight;
}
// Obstacle: Energy loss by collision
const f32& Parameter::ObstacleEnergyLoss() const
{
  return m_ObstacleEnergyLoss;
}
// Obstacle: Distance to be activated
const f32& Parameter::ObstacleActivationDistance() const
{
  return m_ObstacleActivationDistance;
}
// Obstacle: Distance to be inactivated
const f32& Parameter::ObstacleInactivationDistance() const
{
  return m_ObstacleInactivationDistance;
}
// Obstacle: Radius of the cylinder used for collision determination
const f32& Parameter::ObstacleCollisionRadius() const
{
  return m_ObstacleCollisionRadius;
}
// Obstacle: Height of the cylinder used for collision determination
const f32& Parameter::ObstacleCollisionHeight() const
{
  return m_ObstacleCollisionHeight;
}
// Distortion: Distance to be activated
const f32& Parameter::DistortionActivationDistance() const
{
  return m_DistortionActivationDistance;
}
  // Distortion: Distance to be inactivated
const f32& Parameter::DistortionInactivationDistance() const
{
  return m_DistortionInactivationDistance;
}
// Distortion: Radius of the hemisphere used for collision determination
const f32& Parameter::DistortionCollisionRadius() const
{
  return m_DistortionCollisionRadius;
}
// Distortion: Distance the distortion vanish
const f32& Parameter::DistortionVanishDistance() const
{
  return m_DistortionVanishDistance;
}
// Distortion: Strength of gravity
const f32& Parameter::DistortionGravityStrength() const
{
  return m_DistortionGravityStrength;
}
// Distortion: Forward range of gravity
const f32& Parameter::DistortionGravityForwardRange() const
{
  return m_DistortionGravityForwardRange;
}
// Distortion: Backward range of gravity
const f32& Parameter::DistortionGravityBackwardRange() const
{
  return m_DistortionGravityBackwardRange;
}
// Distortion: Vertical & Horizontal range of gravity
const f32& Parameter::DistortionGravitySideRange() const
{
  return m_DistortionGravitySideRange;
}
// Distortion: Scale change rate in collapse
const f32& Parameter::DistortionCollapseScaleChangeRate() const
{
  return m_DistortionCollapseScaleChangeRate;
}
// Distortion: Offset distance of rarity effect
const f32& Parameter::DistortionRarityEffectOffset() const
{
  return m_DistortionRarityEffectOffset;
}
}// namespace Gimmick
}// namespace LunaSolDive
