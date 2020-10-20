//======================================================================
/**
 * @file Parameter.h
 * @date 2017/01/31 13:21:14
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__PLACEMENT__PARAMETER_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__PLACEMENT__PARAMETER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>

namespace LunaSolDive{
#if PM_DEBUG
// forward declarations
namespace Debug{
class Menu;
}// namespace Debug
#endif// PM_DEBUG

namespace Gimmick{
namespace Placement{
class Parameter
{
  GFL_FORBID_COPY_AND_ASSIGN(Parameter);
public:
  // Initialize
  static void Initialize(
              gfl2::heap::HeapBase* pHeap);
  // Terminate
  static void Terminate();
  // getter
  static const Parameter& Get();
  // Reset
  void Reset();
  // Record Count
  const u32& RecordCount() const;
  // Unit Length
  const f32& UnitLength() const;
  // Block Size
  const u32& BlockSize() const;
  // Number of units ahead should be placed
  const u32& AheadUnits() const;
  // Initial Placement: Range
  const f32& InitialPlacementRange() const;
  // Initial Placement: Forbidden Range
  const f32& InitialPlacementForbiddenRange() const;
private:
  // instance
  static Parameter* pInstance;
  // constructor
  Parameter();
  // Record Count
  u32 m_RecordCount;
  // Unit Length
  f32 m_UnitLength;
  // Block Size
  u32 m_BlockSize;
  // Number of units ahead should be placed
  u32 m_AheadUnits;
  // Initial Placement: Range
  f32 m_InitialPlacementRange;
  // Initial Placement: Forbidden Range
  f32 m_InitialPlacementForbiddenRange;
#if PM_DEBUG
  friend class Debug::Menu;
#endif// PM_DEBUG
};
}// namespace Placement
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__PLACEMENT__PARAMETER_H_INCLUDED__
