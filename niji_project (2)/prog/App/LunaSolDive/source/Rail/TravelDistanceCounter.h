//======================================================================
/**
 * @file TravelDistanceCounter.h
 * @date 2016/11/04 18:57:31
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RAIL__TRAVEL_DISTANCE_COUNTER_H_INCLUDED__
#define __LUNA_SOL_DIVE__RAIL__TRAVEL_DISTANCE_COUNTER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

namespace LunaSolDive{
namespace Rail{
class TravelDistanceCounter
{
  GFL_FORBID_COPY_AND_ASSIGN(TravelDistanceCounter);
public:
  // constructor
  TravelDistanceCounter();
  // Initialize
  void Initialize(
              const f32& offset);
  // Terminate
  void Terminate();
  // Travel Distance: Total
  f32 Total() const;
  // Travel Distance: Present Part
  f32 PresentPart() const;
  // Add Part Length
  void AddPartLength(
              const f32& partLength);
  // Add Section Length
  void AddSectionLength(
              const f32& sectionLength);
  // Update Present Section Length
  void UpdatePresentSection(
              const f32& sectionLength);
private:
  // Offset Distance
  f32 m_Offset;
  // Travel Distance: from Initial Part Begin to Previous Part End
  f32 m_FromInitialPartToPresentPart;
  // Travel Distance: from Present Part Begin to Previous Section End
  f32 m_FromPresentPartToPresentSection;
  // Travel Distance: from Present Section Begin to Present Point
  f32 m_FromPresentSectionToPresentPoint;
};
}// namespace Rail
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RAIL__TRAVEL_DISTANCE_COUNTER_H_INCLUDED__
