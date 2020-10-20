//======================================================================
/**
 * @file Manager.h
 * @date 2016/11/25 19:16:56
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__EFFECT__MANAGER_H_INCLUDED__
#define __LUNA_SOL_DIVE__EFFECT__MANAGER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_List.h>

// forward declaration
namespace gfl2{
namespace Effect{
class GFHeap;
class System;
}// namespace Effect
}// namespace gfl2

namespace LunaSolDive{
// forward declaration
class RenderingPipeline;
namespace Effect{
// forward declaration
class Particle;

class Manager
{
  GFL_FORBID_COPY_AND_ASSIGN(Manager);
public:
  // constructor
  Manager();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap,
              gfl2::heap::HeapBase* pEffectHeap,
              RenderingPipeline* pRenderingPipeline);
  // Terminate
  void Terminate();
  // Register Resource
  void SetUp();
  // Update
  void Update();
  // Create
  Particle* Create(
              const u32& id);
  // Prepare for translation
  void PrepareTranslation(
              const gfl2::math::Vector3& displacement);
  // Execute translation
  void ExecuteTranslation();
  // Number of registerd particles
  u32 RegisteredNumber() const;
  // Register pointer to the Particle
  void RegisterParticlePtr(
              Particle* pParticle);
  // Unregister pointer to the Particle
  void UnregisterParticlePtr(
              const Particle* pParticle);
private:
  // RenderingPipeline pointer
  RenderingPipeline* m_pRenderingPipeline;
  // Effect System
  gfl2::Effect::System* m_pEffectSystem;
  // Effect Heap
  gfl2::Effect::GFHeap* m_pEffectHeap;
  // Effect Group ID List pointer
  gfl2::util::List<u32>* m_pEffectGroupIDList;
  // List of pointer to Particle
  Particle** m_ParticlePtrList;
  // Whether this is in process for the translation or not
  bool m_IsInTranslationProcess;
  // Displacement for translation
  gfl2::math::Vector3 m_Displacement;
};
}// namespace Effect
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__EFFECT__MANAGER_H_INCLUDED__
