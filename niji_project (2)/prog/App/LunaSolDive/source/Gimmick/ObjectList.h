//======================================================================
/**
 * @file ObjectList.h
 * @date 2016/11/28 22:18:42
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__OBJECT_LIST_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__OBJECT_LIST_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "./GimmickType.h"

// forward declaration
namespace gfl2{
namespace math{
class Vector3;
}// namespace math
}// namespace gfl2

namespace LunaSolDive{
namespace Gimmick{
// forward declaration
class Distortion;
class Energy;
class Obstacle;

class ObjectList
{
  GFL_FORBID_COPY_AND_ASSIGN(ObjectList);
public:
  // constructor
  ObjectList();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap,
              const u32& energyNumber,
              const u32& obstacleNumber,
              const u32& distortionNumber);
  // Termiante
  void Terminate();
  // The number of energy
  const u32& EnergyNumber() const;
  // Energy
  Energy& Energy(
              const u32& index) const;
  // The number of obstacle
  const u32& ObstacleNumber() const;
  // Obstacle
  Obstacle& Obstacle(
              const u32& index) const;
  // The number of distortion
  const u32& DistortionNumber() const;
  // Distortion List
  Distortion& Distortion(
              const u32& index) const;
  // Free Energy pointer
  Gimmick::Energy* FreeEnergyPtr() const;
  // Free Obstacle pointer
  Gimmick::Obstacle* FreeObstaclePtr() const;
  // Free Distortion pointer
  Gimmick::Distortion* FreeDistortionPtr() const;
  // Translation
  void Translation(
              const gfl2::math::Vector3& displacement) const;
  // Whether the object can be placed or not
  bool IsPlacementPossible(
              const GimmickType& type,
              const Rail::Location& location) const;
private:
  // The number of energy
  u32 m_EnergyNumber;
  // Energy List
  Gimmick::Energy* m_EnergyList;
  // The number of obstacle
  u32 m_ObstacleNumber;
  // Obstacle List
  Gimmick::Obstacle* m_ObstacleList;
  // The number of distortion
  u32 m_DistortionNumber;
  // Distortion List
  Gimmick::Distortion* m_DistortionList;
};
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__OBJECT_LIST_H_INCLUDED__
