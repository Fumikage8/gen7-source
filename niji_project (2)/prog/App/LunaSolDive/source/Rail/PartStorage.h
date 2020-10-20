//======================================================================
/**
 * @file PartStorage.h
 * @date 2016/10/24 11:24:17
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RAIL__PART_STORAGE_H_INCLUDED__
#define __LUNA_SOL_DIVE__RAIL__PART_STORAGE_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Vector3.h>
#include "./PartShape.h"

// forward declaration
namespace gfl2{
namespace renderingengine{
namespace scenegraph{
namespace resource{
class ResourceNode;
}// resource
}// scenegraph
}// renderingengine
}// namespace gfl2
namespace poke_3d{
namespace model{
class BaseModel;
}// namespace model
}// namespace poke_3d
namespace System{
class nijiAllocator;
}// namespace System

namespace LunaSolDive{
namespace Rail{
class PartStorage{
  GFL_FORBID_COPY_AND_ASSIGN(PartStorage);
public:
  // constructor
  PartStorage();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap,
              const PartShape& shape,
              const u32& modelNumber,
              const u32& pointNumber,
              const gfl2::math::Vector3* pointList,
              const f32& radius,
              const gfl2::math::Vector3& beginVerticalDirection);
  // Terminate
  void Terminate();
  // Create
  void Create(
              gfl2::heap::HeapBase* pHeap,
              System::nijiAllocator* pAllocator);
  // Shape
  const PartShape& Shape() const;
  // Point Number
  const u32& PointNumber() const;
  // Point
  const gfl2::math::Vector3& Point(
              const u32& index) const;
  // Radius
  const f32& Radius() const;
  // Begin Vertical Direction
  const gfl2::math::Vector3& BeginVerticalDirection() const;
  // Get Model pointer
  poke_3d::model::BaseModel* GetModelPtr();
  // Release Model pointer
  void ReleaseModelPtr(
              poke_3d::model::BaseModel* pModel);
private:
  // Get whether the resource are used or not
  bool GetUsageStatus(
              const u32& index) const;
  // Set whether the resource are used or not
  void SetUsageStatus(
              const u32& index,
              const bool& isUsed);
  // Shape
  PartShape m_Shape;
  // Point Number
  u32 m_PointNumber;
  // Point List
  const gfl2::math::Vector3* m_PointList;
  // Radius
  f32 m_Radius;
  // Begin Vertical Direction
  gfl2::math::Vector3 m_BeginVerticalDirection;
  // Model Number
  u32 m_ModelNumber;
  // Model List
  poke_3d::model::BaseModel* m_ModelList;
  // Model ResourceNode
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pModelResource;
  // Appearing Motion ResourceNode
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pAppearingMotionResource;
  // Loop Motion ResourceNode
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pLoopMotionResource;
  // Bits representing whether resources are used or not
  u32 m_UsageStatus;
};
}// namespace Rail
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RAIL__PART_STORAGE_H_INCLUDED__
