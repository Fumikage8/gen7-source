//======================================================================
/**
 * @file InitialPart.h
 * @date 2017/01/05 18:37:48
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RAIL__INITIAL_PART_H_INCLUDED__
#define __LUNA_SOL_DIVE__RAIL__INITIAL_PART_H_INCLUDED__
#pragma once

#include "../Base/SimpleModel.h"

namespace LunaSolDive{
// forward declaration
namespace Effect{
class Manager;
class Particle;
}// namespace Effect

namespace Rail{
// forward declaration
class Directions;

class InitialPart: public Base::SimpleModel
{
  GFL_FORBID_COPY_AND_ASSIGN(InitialPart);
public:
  // constructor
  InitialPart();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap,
              RenderingPipeline* pRenderingPipeline,
              Effect::Manager* pEffectManager);
  // Terminate
  void Terminate();
  // Create
  void Create(
              gfl2::heap::HeapBase* pHeap,
              System::nijiAllocator* pAllocator,
              const Directions& directions);
  // Update
  void Update();
  // Visualize (override)
  virtual void Visualize();
  // Non-Visualize (override)
  virtual void NonVisualize();
private:
  // Model (override)
  virtual poke_3d::model::BaseModel& Model() const;
  // Model pointer
  poke_3d::model::BaseModel* m_pModel;
  // Motion ResourceNode pointer
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pMotionNode;
  // pointer to Effect Manager
  Effect::Manager* m_pEffectManager;
  // pointer to effect
  Effect::Particle* m_pEffect;
};
}// namespace Rail
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RAIL__INITIAL_PART_H_INCLUDED__
