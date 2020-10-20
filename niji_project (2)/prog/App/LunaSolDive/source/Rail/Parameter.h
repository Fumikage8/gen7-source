//======================================================================
/**
 * @file Parameter.h
 * @date 2017/02/22 18:50:43
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RAIL__PARAMETER_H_INCLUDED__
#define __LUNA_SOL_DIVE__RAIL__PARAMETER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>

namespace LunaSolDive{
#if PM_DEBUG
// forward declarations
namespace Debug{
class Menu;
}// namespace Debug
#endif// PM_DEBUG

namespace Rail{
class Parameter
{
  GFL_FORBID_COPY_AND_ASSIGN(Parameter);
public:
  // initialize
  static void Initialize(
              gfl2::heap::HeapBase* pHeap);
  // Terminate
  static void Terminate();
  // getter
  static const Parameter& Get();
  // Reset
  void Reset();
  // Radius
  const f32& Radius() const;
  // Number of initial straight rail
  const u32& InitialStraightNumber() const;
  // Curve Forbidden Angle: Upper Range
  const f32& CurveForbiddenAngleUpperRange() const;
  // Curve Forbidden Angle: Lower Range
  const f32& CurveForbiddenAngleLowerRange() const;
private:
  // Instance
  static Parameter* pInstance;
  // constructor
  Parameter();
  // Radius
  f32 m_Radius;
  // Number of initial straight rail
  u32 m_InitialStraightNumber;
  // Curve Forbidden Angle: Upper Range
  f32 m_CurveForbiddenAngleUpperRange;
  // Curve Forbidden Angle: Lower Range
  f32 m_CurveForbiddenAngleLowerRange;
#if PM_DEBUG
  friend class Debug::Menu;
#endif// PM_DEBUG
};
}// namespace Rail
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RAIL__PARAMETER_H_INCLUDED__
