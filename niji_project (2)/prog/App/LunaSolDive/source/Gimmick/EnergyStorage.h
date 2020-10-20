//======================================================================
/**
 * @file EnergyStorage.h
 * @date 2016/11/16 13:02:51
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__ENERGY_STORAGE_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__ENERGY_STORAGE_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "./ObjectStorage.h"

// forward declaration
namespace gfl2{
namespace renderingengine{
namespace scenegraph{
namespace resource{
class ResourceNode;
}// namespace resource
}// namespace scenegraph
}// namespace renderingengine
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
// forward declaration
class RenderingPipeline;
namespace Rail{
class Location;
}// namespace Rail
namespace Gimmick{
// forward declaration
class Energy;

class EnergyStorage: public ObjectStorage
{
  GFL_FORBID_COPY_AND_ASSIGN(EnergyStorage);
public:
  // constructor
  EnergyStorage();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap,
              const u32& modelNumber);
  // Terminate
  void Terminate();
  // Create Model
  void Create(
              gfl2::heap::HeapBase* pHeap,
              System::nijiAllocator* pAllocator);
  // Gimmick Type (override)
  virtual GimmickType Type() const;
  // Set Up
  void SetUp(
              Energy* pTarget,
              RenderingPipeline* pRenderingPipeline,
              const Rail::Location& location);
  // Get Model pointer
  poke_3d::model::BaseModel* GetModelPtr();
  // Release Model pointer
  void ReleaseModelPtr(
              poke_3d::model::BaseModel* pModel);
  // Set Vanishing Motion to Model
  void SetVanishingMotion(
              poke_3d::model::BaseModel* pModel);
  // Distance at the energy is activated (override)
  virtual const f32& ActivationDistance() const;
  // Distance at the energy is inactivated (override)
  virtual const f32& InactivationDistance() const;
private:
  // Get whether the resource are used or not
  bool GetUsageStatus(
              const u32& index) const;
  // Set whether the resource are used or not
  void SetUsageStatus(
              const u32& index,
              const bool& isUsed);
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
  // Vanishing Motion ResourceNode
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pVanishingMotionResource;
  // Bits representing whether resources are used or not
  u32 m_UsageStatus;
};
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__ENERGY_STORAGE_H_INCLUDED__
