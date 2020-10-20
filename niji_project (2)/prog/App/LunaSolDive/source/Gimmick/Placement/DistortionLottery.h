//======================================================================
/**
 * @file DistortionLottery.h
 * @date 2016/12/09 20:59:42
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__PLACEMENT__DISTORTION_LOTTERY_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__PLACEMENT__DISTORTION_LOTTERY_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <App/LunaSolDive/include/DistortionType.h>
#include "../../Version.h"

namespace LunaSolDive{
namespace Gimmick{
namespace Placement{
struct SelectedDistortion
{
  DistortionType type;
  DistortionRarity rarity;
};

class DistortionLottery
{
  GFL_FORBID_COPY_AND_ASSIGN(DistortionLottery);
public:
  // constructor
  DistortionLottery();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap,
              const Mode& mode);
  // Terminate
  void Terminate();
  // Select
  SelectedDistortion Select(
              const f32& distance) const;
private:
  // Mode: normal or first
  Mode m_Mode;
};
}// namespace Placement
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__PLACEMENT__DISTORTION_LOTTERY_H_INCLUDED__
