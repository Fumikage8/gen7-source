//======================================================================
/**
 * @file Particle.h
 * @date 2016/11/25 22:39:10
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__EFFECT__PARTICLE_H_INCLUDED__
#define __LUNA_SOL_DIVE__EFFECT__PARTICLE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>
#include <math/include/gfl2_Quaternion.h>

// forward declaration
namespace gfl2{
namespace Effect{
class GFHeap;
class Handle;
class System;
}// namespace Effect
}// namespace gfl2

namespace LunaSolDive{
namespace Effect{
// forward declaration
class Manager;
class ParticleImpl;

class Particle
{
  GFL_FORBID_COPY_AND_ASSIGN(Particle);
public:
  // constructor
  Particle();
  // destructor
  ~Particle();
  // Initialize
  void Initialize(
              Effect::Manager* pManager,
              gfl2::Effect::System* pSystem,
              gfl2::Effect::GFHeap* pEffectHeap,
              const u32& id);
  // Terminate
  void Terminate();
  // Start
  void Start();
  // End
  void End();
  // Kill
  void Kill();
  // Whether this particle is alive or not
  bool IsAlive() const;
  // Update location & rotation & scale
  void Update(
              const gfl2::math::Vector3& location,
              const gfl2::math::Quaternion& rotation,
              const f32& scale);
  // Effedct ID
  u32 ID() const;
  // Prepare for translation
  void PrepareTranslation(
              const gfl2::math::Vector3& displacement);
  // Whether this is in process for the translation or not
  bool IsInTranslationProcess() const;
  // Execute translation
  void ExecuteTranslation();
private:
  enum TranslationMethod
  {
    TRANSLATION_METHOD__NONE,
    TRANSLATION_METHOD__RELOCATE,
    TRANSLATION_METHOD__SWAP,
  };
  // Create sub particle
  void CreateSub();
  // pointer to Effect Manager
  Manager* m_pManager;
  // pointer to Effect System
  gfl2::Effect::System* m_pSystem;
  // pointer to Effect Heap
  gfl2::Effect::GFHeap* m_pEffectHeap;
  // Translation Method
  TranslationMethod m_TranslationMethod;
  // pointer to main particle
  ParticleImpl* m_pMain;
  // pointer to sub particle (for translation)
  ParticleImpl* m_pSub;
  // Location
  gfl2::math::Vector3 m_Location;
  // Rotation
  gfl2::math::Quaternion m_Rotation;
  // Scale
  f32 m_Scale;
  // Displacement for translation
  gfl2::math::Vector3 m_Displacement;
  // Whether this is in process for the translation or not
  bool m_IsInTranslationProcess;
};
}// namespace Effect
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__EFFECT__PARTICLE_H_INCLUDED__
