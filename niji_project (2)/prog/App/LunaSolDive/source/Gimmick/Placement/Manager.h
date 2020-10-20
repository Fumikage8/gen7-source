//======================================================================
/**
 * @file Manager.h
 * @date 2016/12/08 17:25:18
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__PLACEMENT__MANAGER_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__PLACEMENT__MANAGER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include "../../Version.h"

namespace LunaSolDive{
// forward declaration
namespace Rail{
class Manager;
}// namespace Rail

namespace Gimmick{
// forward Declaration
class ObjectList;
class Storage;

namespace Placement{
// forward Declaration
class Order;
class OrderList;
class DistortionLottery;

class Manager
{
  GFL_FORBID_COPY_AND_ASSIGN(Manager);
public:
  // constructor
  Manager();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap,
              const Mode& mode,
              const Rail::Manager* pRailManager,
              Storage* pStorage,
              const ObjectList* pObjectList);
  // Terminate
  void Terminate();
  // Initial Placement
  void InitialPlacement();
  // Update
  void Update();
private:
  // Execute Placement
  void Execute();
  // Execute placement as ordered
  void ExecuteOrder(
              const Order& order,
              const f32& distance);
  // Place Energy at random
  void PlaceEnergyRandom(
              const f32& distance) const;
  // Place Obstacle at random
  void PlaceObstacleRandom(
              const f32& distance) const;
  // Place Distortion at random
  void PlaceDistortionRandom(
              const f32& distance) const;
  // Unit Length
  const f32& UnitLength() const;
  // Block Size
  const u32& BlockSize() const;
  // Mode
  Mode m_Mode;
  // Pointer to Rail Manager
  const Rail::Manager* m_pRailManager;
  // Pointer to Storage
  Storage* m_pStorage;
  // Pointer to ObjectList
  const ObjectList*  m_pObjectList;
  // DistortionLottery
  DistortionLottery* m_pDistortionLottery;
  // Order List
  OrderList* m_pOrderList;
  // Trigger Point
  f32 m_TriggerPoint;
};
}// namespace Placement
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__PLACEMENT__MANAGER_H_INCLUDED__
