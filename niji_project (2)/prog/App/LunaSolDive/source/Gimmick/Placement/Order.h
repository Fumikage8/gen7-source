//======================================================================
/**
 * @file Order.h
 * @date 2017/03/02 15:33:25
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__PLACEMENT__ORDER_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__PLACEMENT__ORDER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

namespace LunaSolDive{
namespace Gimmick{
namespace Placement{
class Order
{
public:
  // constructor
  Order();
  // Initialize
  void Initialize(
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
              const f32& distrotionProbabilityEnd);
  // Terminate
  void Terminate();
  // Whether this distance is in the target range or not
  bool IsInRange(
              const f32& distance) const;
  // Distance: Begin
  const f32& DistanceBegin() const;
  // Distance: End
  const f32& DistanceEnd() const;
  // Whether this is the first order
  bool IsFirst() const;
  // Whether this is the last order
  bool IsLast() const;
  // Energy: Number of trials
  const u32& EnergyTrialNumber() const;
  // Energy: Probability
  f32 EnergyProbability(
              const f32& distance) const;
  // Obstacle: Number of trials
  const u32& ObstacleTrialNumber() const;
  // Obstacle: Probability
  f32 ObstacleProbability(
              const f32& distance) const;
  // Distortion: Number of trials
  const u32& DistortionTrialNumber() const;
  // Distortion: Probability
  f32 DistortionProbability(
              const f32& distance) const;
private:
  // Lottery Order
  struct LotteryOrder
  {
    u32 trial_number;
    f32 probability_begin;
    f32 probability_end;
  };
  // Probability
  f32 Probability(
              const f32& distance,
              const LotteryOrder& lotteryOrder) const;
  // Distance Begin
  f32 m_DistanceBegin;
  // Distance End
  f32 m_DistanceEnd;
  // Whether this is the last order
  bool m_IsLast;
  // Lottery Order: Energy
  LotteryOrder m_Energy;
  // Lottery Order: Obstacle
  LotteryOrder m_Obstacle;
  // Lottery Order: Distortion
  LotteryOrder m_Distortion;
};
}// namespace Placement
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__PLACEMENT__ORDER_H_INCLUDED__
