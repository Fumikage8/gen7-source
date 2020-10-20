//======================================================================
/**
 * @file Energy.h
 * @date 2016/11/10 17:58:12
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RUN__ENERGY_H_INCLUDED__
#define __LUNA_SOL_DIVE__RUN__ENERGY_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

namespace LunaSolDive{
namespace Run{
class Energy
{
  GFL_FORBID_COPY_AND_ASSIGN(Energy);
public:
  // constructor
  Energy();
  // Initialize
  void Initialize(
              const f32& percentage);
  // Terminate
  void Terminate();
  // Update
  void Update(
              const f32& gain,
              const f32& loss);
  // check if the Energy is empty
  bool IsEmpty() const;
  // Speed
  f32 Speed() const;
  // Moving Rate on the Vertical Plane
  f32 VerticalMovingRate() const;
  // Limit of Rate
  const f32& Limit() const;
  // Rate
  const f32& Rate() const;
  // Whether energy is over the limit
  const bool& IsOverLimit() const;
private:
  // Limit of Rate
  f32 m_Limit;
  // Rate
  f32 m_Rate;
  // Whether energy is over the limit
  bool m_IsOverLimit;
};
}// namespace Run
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RUN__ENERGY_H_INCLUDED__
