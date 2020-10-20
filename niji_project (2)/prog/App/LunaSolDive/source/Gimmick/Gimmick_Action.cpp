//======================================================================
/**
 * @file Gimmick_Action.cpp
 * @date 2016/11/15 11:21:19
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "./Action.h"
#include "./Distortion.h"
#include "./Energy.h"
#include "./Obstacle.h"
#include "../Assert.h"

namespace LunaSolDive{
namespace Gimmick{
// constructor
Action::Action()
    : m_PresentDistance(0.0f),
      m_EnergyGain(0.0f),
      m_EnergyLoss(0.0f),
      m_DistortionType(DISTORTION_TYPE__SIZE),
      m_DistortionRarity(DISTORTION_RARITY__NONE),
      m_GravityNumber(0){}
// Initialize
void Action::Initialize(
            const f32& presentDistance)
{
  m_PresentDistance = presentDistance;
}
// Terminate
void Action::Terminate()
{
  m_PresentDistance = 0.0f;
  m_EnergyGain = 0.0f;
  m_EnergyLoss = 0.0f;
  m_CollidedDistortionLocation.Terminate();
  m_NearestDistortionLocation.Terminate();
  m_DistortionType = DISTORTION_TYPE__SIZE;
  m_DistortionRarity = DISTORTION_RARITY__NONE;
  for(u32 i = 0; i < m_GravityNumber; ++i)
  {
    m_GravityList[i].center = gfl2::math::Vector2::GetZero();
    m_GravityList[i].strength = 0.0f;
  }
  m_GravityNumber = 0;
}
// Add Energy Action
void Action::Add(
            const EnergyAction& action)
{
  if(action.is_valid)
  {
    m_EnergyGain += action.energy_gain;
  }
}
// Add Obstacle Action
void Action::Add(
            const ObstacleAction& action)
{
  if(action.is_valid)
  {
    m_EnergyLoss += action.energy_loss;
  }
}
// Add Distortion Action
void Action::Add(
            const DistortionAction& action)
{
  if(action.is_valid)
  {
    if(action.is_caught)
    {
      LUNA_SOL_DIVE_ASSERT(m_DistortionType == DISTORTION_TYPE__SIZE);
      LUNA_SOL_DIVE_ASSERT(action.type != DISTORTION_TYPE__SIZE);
      m_DistortionType = action.type;
      m_DistortionRarity = action.rarity;
      m_CollidedDistortionLocation = action.distortion_location;
    }
    else
    {
      const u32 index = m_GravityNumber;
      LUNA_SOL_DIVE_ASSERT(index < Parameter::DISTORTION_NUMBER);
      m_GravityList[index].center = action.collision_location.NormalizedCrossSectionPoint();
      m_GravityList[index].strength = action.gravity_strength;
      ++m_GravityNumber;
    }
    // update nearest distortion locatin
    if(m_PresentDistance < action.collision_location.TravelDistance()
            && (! m_NearestDistortionLocation.IsValid()
                || action.collision_location.TravelDistance() < m_NearestDistortionLocation.TravelDistance()))
    {
      m_NearestDistortionLocation = action.collision_location;
    }
  }
}
// Energy Difference
f32 Action::EnergyDifference() const
{
  return m_EnergyGain - m_EnergyLoss;
}
// Energy Gain
const f32& Action::EnergyGain() const
{
  return m_EnergyGain;
}
// Energy Loss
const f32& Action::EnergyLoss() const
{
  return m_EnergyLoss;
}
// Whether it collided with an energy or not
bool Action::IsCollidedEnergy() const
{
  return m_EnergyGain > 0.0f;
}
// Whether it collided with an obstacle or not
bool Action::IsCollidedObstacle() const
{
  return m_EnergyLoss > 0.0f;
}
// Whether it collided with a distortion or not
bool Action::IsCollidedDistortion() const
{
  return m_DistortionType != DISTORTION_TYPE__SIZE;
}
// Distortion Type
const DistortionType& Action::DistortionType() const
{
  return m_DistortionType;
}
// Distortion Rarity
const DistortionRarity& Action::DistortionRarity() const
{
  return m_DistortionRarity;
}
// Collided Distortion Location
const Rail::Location& Action::CollidedDistortionLocation() const
{
  return m_CollidedDistortionLocation;
}
// Nearest Distortion Location
const Rail::Location& Action::NearestDistortionLocation() const
{
  return m_NearestDistortionLocation;
}
// The number of gravity
const u32& Action::GravityNumber() const
{
  LUNA_SOL_DIVE_ASSERT(m_GravityNumber <= Parameter::DISTORTION_NUMBER);
  return m_GravityNumber;
}
// Gravity
const Gravity& Action::Gravity(
            const u32& index) const
{
  LUNA_SOL_DIVE_ASSERT(index < m_GravityNumber);
  LUNA_SOL_DIVE_ASSERT(index < Parameter::DISTORTION_NUMBER);
  return m_GravityList[index];
}
// Translation
void Action::Translation(
            const gfl2::math::Vector3& displacement)
{
  m_CollidedDistortionLocation.Translation(displacement);
  m_NearestDistortionLocation.Translation(displacement);
}
}// namespace Gimmick
}// namespace LunaSolDive
