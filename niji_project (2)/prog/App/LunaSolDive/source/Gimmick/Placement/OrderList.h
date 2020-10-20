//======================================================================
/**
 * @file OrderList.h
 * @date 2017/03/02 17:27:37
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__PLACEMENT__ORDER_LIST_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__PLACEMENT__ORDER_LIST_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "./Order.h"

namespace LunaSolDive{
namespace Gimmick{
namespace Placement{
class OrderList
{
public:
  // constructor
  OrderList();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap);
  // Terminate
  void Terminate();
  // Get
  const Order& Get(
              const f32& distance);
private:
  // Size
  u32 m_Size;
  // Order List
  Order* m_List;
};
}// namespace Placement
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__PLACEMENT__ORDER_LIST_H_INCLUDED__
