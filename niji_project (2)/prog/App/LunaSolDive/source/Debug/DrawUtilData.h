//======================================================================
/**
 * @file DrawUtilData.h
 * @date 2017/03/14 18:32:41
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG
#if !defined __LUNA_SOL_DIVE__DEBUG__DRAW_UTIL_DATA_H_INCLUDED__
#define __LUNA_SOL_DIVE__DEBUG__DRAW_UTIL_DATA_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "../Rail/Location.h"

namespace LunaSolDive{
// forward declaration
namespace Gimmick{
namespace Collision{
class Cylinder;
class Hemisphere;
}// namespace Collision
}// namespace Gimmick
namespace Rail{
class Section;
}// namespace Rail

namespace Debug{
class DrawUtilData
{
  GFL_FORBID_COPY_AND_ASSIGN(DrawUtilData);
public:
  // constructor
  DrawUtilData();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap);
  // Terminate
  void Terminate();
  // Reset
  void Reset();
  // Present Location: getter
  const Rail::Location& PresentLocation() const;
  // Present Location: setter
  void PresentLocation(
              const Rail::Location& value);
  // Input Point: getter
  const gfl2::math::Vector3& InputPoint() const;
  // Input Point: setter
  void InputPoint(
              const gfl2::math::Vector3& value);
  // Collision Point: getter
  const gfl2::math::Vector3& CollisionPoint() const;
  // Collision Point: setter
  void CollisionPoint(
              const gfl2::math::Vector3& value);
  // Rail::Section List Size
  const u32& RailSectionListSize() const;
  // Rail::Section: getter
  const Rail::Section& RailSection(
              const u32& index) const;
  // Rail::Section: setter
  void RailSection(
              const Rail::Section& value);
  // Gimmick::Energy Collision List Size
  const u32& GimmickEnergyCollisionListSize() const;
  // Gimmick::Energy Collision: getter
  const Gimmick::Collision::Cylinder& GimmickEnergyCollision(
              const u32& index) const;
  // Gimmick::Energy Collision: setter
  void GimmickEnergyCollision(
              const Gimmick::Collision::Cylinder& value);
  // Gimmick::Obstacle Collision List Size
  const u32& GimmickObstacleCollisionListSize() const;
  // Gimmick::Obstacle Collision: getter
  const Gimmick::Collision::Cylinder& GimmickObstacleCollision(
              const u32& index) const;
  // Gimmick::Obstacle Collision: setter
  void GimmickObstacleCollision(
              const Gimmick::Collision::Cylinder& value);
  // Gimmick::Distortion Collision List Size
  const u32& GimmickDistortionCollisionListSize() const;
  // Gimmick::Distortion Collision: getter
  const Gimmick::Collision::Hemisphere& GimmickDistortionCollision(
              const u32& index) const;
  // Gimmick::Distortion Collision: setter
  void GimmickDistortionCollision(
              const Gimmick::Collision::Hemisphere& value);
private:
  // Present Location
  Rail::Location m_PresentLocation;
  // Input Point
  gfl2::math::Vector3 m_InputPoint;
  // CollisionPoint
  gfl2::math::Vector3 m_CollisionPoint;
  // Rail::Section List Size
  u32 m_RailSectionListSize;
  // Rail::Section List
  Rail::Section* m_RailSectionList;
  // Gimmick::Energy Collision List Size
  u32 m_GimmickEnergyCollisionListSize;
  // Gimmick::Energy Collision List
  Gimmick::Collision::Cylinder* m_GimmickEnergyCollisionList;
  // Gimmick::Obstacle Collision List Size
  u32 m_GimmickObstacleCollisionListSize;
  // Gimmick::Obstacle Collision List
  Gimmick::Collision::Cylinder* m_GimmickObstacleCollisionList;
  // Gimmick::Distortion Collision List Size
  u32 m_GimmickDistortionCollisionListSize;
  // Gimmick::Distortion Collision List
  Gimmick::Collision::Hemisphere* m_GimmickDistortionCollisionList;
};
}// namespace Debug
}// namespace LunaSolDive
#endif// __LUNA_SOL_DIVE__DEBUG__DRAW_UTIL_DATA_H_INCLUDED__
#endif// PM_DEBUG
