//======================================================================
/**
 * @file ParticleImpl.h
 * @date 2017/01/19 10:52:41
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__EFFECT__PARTICLE_IMPL_H_INCLUDED__
#define __LUNA_SOL_DIVE__EFFECT__PARTICLE_IMPL_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

// forward declaration
namespace gfl2{
namespace Effect{
class GFHeap;
class Handle;
class System;
}// namespace Effect
namespace math{
class Vector3;
class Quaternion;
}// namespace math
}// namespace gfl2

namespace LunaSolDive{
namespace Effect{
class ParticleImpl
{
  GFL_FORBID_COPY_AND_ASSIGN(ParticleImpl);
public:
  // constructor
  ParticleImpl();
  // destructor
  ~ParticleImpl();
  // Initialize
  void Initialize(
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
  const u32& ID() const;
private:
  // Effect System
  gfl2::Effect::System* m_pSystem;
  // Effect Handle
  gfl2::Effect::Handle* m_pHandle;
  // Effect ID
  u32 m_ID;
};
}// namespace Effect
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__EFFECT__PARTICLE_IMPL_H_INCLUDED__
