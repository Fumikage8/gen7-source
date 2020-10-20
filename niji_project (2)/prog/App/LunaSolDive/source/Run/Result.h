//======================================================================
/**
 * @file Result.h
 * @date 2017/01/17 11:24:25
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RUN__RESULT_H_INCLUDED__
#define __LUNA_SOL_DIVE__RUN__RESULT_H_INCLUDED__
#pragma once

#include <types/include/gfl2_Typedef.h>
#include "App/LunaSolDive/include/DistortionType.h"

namespace LunaSolDive{
namespace Run{
class Result
{
public:
  // constructor
  Result();
  // Initialize
  void Initialize(
              const DistortionType& type,
              const DistortionRarity& rarity,
              const f32& distance);
  // Terminate
  void Terminate();
  // Distortion Type
  const DistortionType& Type() const;
  // Distortion Rarity
  const DistortionRarity& Rarity() const;
  // Travel Distance (raw)
  const f32& RawDistance() const;
  // Travel Distance in 16bits integer
  u32 Distance() const;
private:
  // Distortion Type
  DistortionType m_Type;
  // Distortion Rarity
  DistortionRarity m_Rarity;
  // Travel Distance
  f32 m_Distance;
};
}// namespace Run
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RUN__RESULT_H_INCLUDED__
