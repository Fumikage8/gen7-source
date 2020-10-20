//======================================================================
/**
 * @file Gimmick_Placement_Order.cpp
 * @date 2017/03/02 16:08:48
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <algorithm>
#include "./Order.h"
#include "../../Assert.h"
#if PM_DEBUG
#include "../../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Gimmick{
namespace Placement{
// static function
// constructor
Order::Order()
    : m_DistanceBegin(0.0f),
      m_DistanceEnd(0.0f),
      m_IsLast(false)
{
  m_Energy.trial_number = 0;
  m_Energy.probability_begin = 0.0f;
  m_Energy.probability_end = 0.0f;
  m_Obstacle.trial_number = 0;
  m_Obstacle.probability_begin = 0.0f;
  m_Obstacle.probability_end = 0.0f;
  m_Distortion.trial_number = 0;
  m_Distortion.probability_begin = 0.0f;
  m_Distortion.probability_end = 0.0f;
}
// Initialize
void Order::Initialize(
            const f32& distanceBegin,
            const f32& distanceEnd,
            const bool& isLast,
            const u32& energyTrialNumber,
            const f32& energyProbabilityBegin,
            const f32& energyProbabilityEnd,
            const u32& obstacleTrialNumber,
            const f32& obstacleProbabilityBegin,
            const f32& obstacleProbabilityEnd,
            const f32& distortionTrialNumber,
            const f32& distortionProbabilityBegin,
            const f32& distortionProbabilityEnd)
{
  // distance
  m_DistanceBegin = distanceBegin;
  m_DistanceEnd = distanceEnd;
  m_IsLast = isLast;
  // enrgy
  m_Energy.trial_number = energyTrialNumber;
  m_Energy.probability_begin = energyProbabilityBegin;
  m_Energy.probability_end = energyProbabilityEnd;
  // obstacle
  m_Obstacle.trial_number = obstacleTrialNumber;
  m_Obstacle.probability_begin = obstacleProbabilityBegin;
  m_Obstacle.probability_end = obstacleProbabilityEnd;
  // distortion
  m_Distortion.trial_number = distortionTrialNumber;
  m_Distortion.probability_begin = distortionProbabilityBegin;
  m_Distortion.probability_end = distortionProbabilityEnd;
  // check distance
  LUNA_SOL_DIVE_ASSERT(m_DistanceBegin >= 0.0f);
  LUNA_SOL_DIVE_ASSERT(m_IsLast || m_DistanceBegin < m_DistanceEnd);
  // check prpbaility
  LUNA_SOL_DIVE_ASSERT(0.0f <= m_Energy.probability_begin);
  LUNA_SOL_DIVE_ASSERT(m_Energy.probability_begin <= 1.0f);
  LUNA_SOL_DIVE_ASSERT(0.0f <= m_Energy.probability_end);
  LUNA_SOL_DIVE_ASSERT(m_Energy.probability_end <= 1.0f);
  LUNA_SOL_DIVE_ASSERT(0.0f <= m_Obstacle.probability_begin);
  LUNA_SOL_DIVE_ASSERT(m_Obstacle.probability_begin <= 1.0f);
  LUNA_SOL_DIVE_ASSERT(0.0f <= m_Obstacle.probability_end);
  LUNA_SOL_DIVE_ASSERT(m_Obstacle.probability_end <= 1.0f);
  LUNA_SOL_DIVE_ASSERT(0.0f <= m_Distortion.probability_begin);
  LUNA_SOL_DIVE_ASSERT(m_Distortion.probability_begin <= 1.0f);
  LUNA_SOL_DIVE_ASSERT(0.0f <= m_Distortion.probability_end);
  LUNA_SOL_DIVE_ASSERT(m_Distortion.probability_end <= 1.0f);
  // check last
  LUNA_SOL_DIVE_ASSERT(! m_IsLast || m_Energy.trial_number == 0);
  LUNA_SOL_DIVE_ASSERT(
              ! m_IsLast
              || (m_Distortion.trial_number != 0
                      && m_Distortion.probability_begin > 0.0f));
}
// Terminate
void Order::Terminate()
{
  *this = Order();
}
// Whether this distance is in the target range or not
bool Order::IsInRange(
            const f32& distance) const
{
  return m_IsLast
          ? m_DistanceBegin <= distance
          : m_DistanceBegin <= distance && distance < m_DistanceEnd;
}
// Distance: Begin
const f32& Order::DistanceBegin() const
{
  return m_DistanceBegin;
}
// Distance: End
const f32& Order::DistanceEnd() const
{
  return m_DistanceEnd;
}
// Whether this is the first order
bool Order::IsFirst() const
{
  return m_DistanceBegin == 0.0f;
}
// Whether this is the last order
bool Order::IsLast() const
{
  return m_IsLast;
}
// Energy: Number of trials
const u32& Order::EnergyTrialNumber() const
{
  return m_Energy.trial_number;
}
// Energy: Probability
f32 Order::EnergyProbability(
            const f32& distance) const
{
  return Probability(
              distance,
              m_Energy);
}
// Obstacle: Number of trials
const u32& Order::ObstacleTrialNumber() const
{
  return m_Obstacle.trial_number;
}
// Obstacle: Probability
f32 Order::ObstacleProbability(
            const f32& distance) const
{
  return Probability(
              distance,
              m_Obstacle);
}
// Distortion: Number of trials
const u32& Order::DistortionTrialNumber() const
{
  return m_Distortion.trial_number;
}
// Distortion: Probability
f32 Order::DistortionProbability(
            const f32& distance) const
{
  return Probability(
              distance,
              m_Distortion);
}
// Plobability
f32 Order::Probability(
            const f32& distance,
            const LotteryOrder& lotteryOrder) const
{
  LUNA_SOL_DIVE_ASSERT(IsInRange(distance));
#if PM_DEBUG
  if(Debug::Parameter::Get().isPlacementProbabilitySet0)
  {
    return 0.0f;
  }
  else if(Debug::Parameter::Get().isPlacementProbabilitySet1)
  {
    return 1.0f;
  }
#endif// PM_DEBUG
  if(m_IsLast)
  {
    return lotteryOrder.probability_begin;
  }
  else
  {
    const f32 range = m_DistanceEnd - m_DistanceBegin;
    const f32 progress = distance - m_DistanceBegin;
    const f32 progressRate =
              (range <= 0.0f || progress < 0.0f)
              ? 0.0f
              : std::min(progress / range, 1.0f);
    return lotteryOrder.probability_begin
           + (lotteryOrder.probability_end - lotteryOrder.probability_begin) * progressRate;
  }
}
}// namespace Placement
}// namespace Gimmick
}// namespace LunaSolDive
