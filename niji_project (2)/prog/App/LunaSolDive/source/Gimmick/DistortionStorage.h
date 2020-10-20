//======================================================================
/**
 * @file DistortionStorage.h
 * @date 2016/11/16 17:26:11
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__DISTORTION_STORAGE_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__DISTORTION_STORAGE_H_INCLUDED__
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
namespace Effect{
class Manager;
class Particle;
}// namespace Effect
namespace Rail{
class Location;
class Manager;
}// namespace Rail
namespace Gimmick{
// forward declaration
class Distortion;

class DistortionStorage: public ObjectStorage
{
  GFL_FORBID_COPY_AND_ASSIGN(DistortionStorage);
public:
  // constructor
  DistortionStorage();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap,
              const Rail::Manager* pRailManager,
              Effect::Manager* pEffectManager,
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
              Distortion* pTarget,
              RenderingPipeline* pRenderingPipeline,
              const DistortionType& type,
              const DistortionRarity& rarity,
              const Rail::Location& location);
  // Get Model pointer
  poke_3d::model::BaseModel* GetModelPtr(
              const DistortionType& type);
  // Release Model pointer
  void ReleaseModelPtr(
              poke_3d::model::BaseModel* pModel);
  // Set Vanishing Motion to Model
  void SetVanishingMotion(
              const DistortionType& type,
              poke_3d::model::BaseModel* pModel);
  // Distance at the distortion activation (override)
  virtual const f32& ActivationDistance() const;
  // Distance at the distortion inactivation (override)
  virtual const f32& InactivationDistance() const;
private:
  // The number of model & resource node
  u32 ResourceNumber() const;
  // The index of model & resource node
  u32 ResourceIndex(
              const DistortionType& type,
              const u32& index) const;
  // Get whether the resource are used or not
  bool GetUsageStatus(
              const DistortionType& type,
              const u32& index) const;
  // Set whether the resource are used or not
  void SetUsageStatus(
              const DistortionType& type,
              const u32& index,
              const bool& isUsed);
  // Create Rarity Effect
  Effect::Particle* CreateRarityEffect(
              const DistortionRarity& rarity) const;
  // Rail Manager
  const Rail::Manager* m_pRailManager;
  // Effect Manager
  Effect::Manager* m_pEffectManager;
  // Model Number
  u32 m_ModelNumber;
  // Model List
  poke_3d::model::BaseModel* m_ModelList;
  // Model ResourceNode List
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_ModelResourceList;
  // Appearing Motion ResourceNode List
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_AppearingMotionResourceList;
  // Loop Motion ResourceNode List
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_LoopMotionResourceList;
  // Vanishing Motion ResourceNode List
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_VanishingMotionResourceList;
  // Bits representing whether resources are used or not
  u64 m_UsageStatus;
};
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__DISTORTION_STORAGE_H_INCLUDED__
