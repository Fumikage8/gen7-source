//======================================================================
/**
 * @file Manager.h
 * @date 2016/11/02 15:53:57
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__MANAGER_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__MANAGER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "../Version.h"

// forward declaration
namespace gfl2{
namespace math{
class Vector3;
}// namespace math
}// namespace gfl2
namespace System{
class nijiAllocator;
}// namespace System

namespace LunaSolDive{
// forward declarations
class RenderingPipeline;
namespace Effect{
class Manager;
}// namespace Effect
namespace Rail{
class Directions;
class Location;
class Manager;
}// namespace Rail
namespace Gimmick{
// forward declarations
class Action;
class ObjectList;
class Storage;
namespace Placement{
class Manager;
}// namespace Placement

class Manager
{
  GFL_FORBID_COPY_AND_ASSIGN(Manager);
public:
  // constrcutor
  Manager();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap,
              const Mode& mode,
              Effect::Manager* pEffectManager,
              const Rail::Manager* pRailManager,
              RenderingPipeline* pRenderingPipeline);
  // Terminate
  void Terminate();
  // Create
  void Create(
              gfl2::heap::HeapBase* pHeap,
              System::nijiAllocator* pAllocator);
  // Upadte
  void Update();
  // Update
  void Update(
              const Rail::Location& location,
              const Rail::Directions& presentCollisionPoint,
              const Rail::Directions& previouCollisionPoint);
  // Action
  const Action& Action() const;
  // Start the collapse of the distortion after collision
  void StartDistortionCollapse();
  // Translation
  void Translation(
              const gfl2::math::Vector3& displacement);
private:
  // pointer to Rail Manager
  const Rail::Manager* m_pRailManager;
  // Storage
  Storage* m_pStorage;
  // ObjectList
  ObjectList* m_pObjectList;
  // Action
  Gimmick::Action* m_pAction;
  // Placement
  Placement::Manager* m_pPlacement;
};
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__MANAGER_H_INCLUDED__
