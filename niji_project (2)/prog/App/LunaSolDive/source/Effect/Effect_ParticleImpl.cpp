//======================================================================
/**
 * @file Effect_ParticleImpl.cpp
 * @date 2017/01/19 10:59:55
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <Effect/include/gfl2_EffectHandle.h>
#include <Effect/include/gfl2_EffectHeap.h>
#include <Effect/include/gfl2_EffectSystem.h>
#include <math/include/gfl2_Quaternion.h>
#include "./ParticleImpl.h"
#include "../Assert.h"

namespace LunaSolDive{
namespace Effect{
// constructor
ParticleImpl::ParticleImpl()
    : m_pSystem(NULL),
      m_pHandle(NULL),
      m_ID(0xffffffff){}
// destructor
ParticleImpl::~ParticleImpl()
{
  Terminate();
}
// Initialize
void ParticleImpl::Initialize(
            gfl2::Effect::System* pSystem,
            gfl2::Effect::GFHeap* pEffectHeap,
            const u32& id)
{
  m_pSystem = pSystem;
  LUNA_SOL_DIVE_ASSERT(m_pSystem);
  m_pHandle = GFL_NEW(pEffectHeap->GetGFHeapBase()) gfl2::Effect::Handle();
  LUNA_SOL_DIVE_ASSERT(m_pHandle);
  m_ID = id;
}
// Terminate
void ParticleImpl::Terminate()
{
  Kill();
  GFL_SAFE_DELETE(m_pHandle);
  // reset parameter
  m_pSystem = NULL;
  m_ID = 0xffffffff;
}
// Start
void ParticleImpl::Start()
{
  nw::eft::EmitterSet* const pEmitterSet = m_pHandle->GetEmitterSet();
  bool create = true;
  if(pEmitterSet != NULL)
  {
    if(! pEmitterSet->IsAlive() || pEmitterSet->IsFadeRequest())
    {
      pEmitterSet->Kill();
    }
    else
    {
      create = false;
    }
  }
  if(create)
  {
    m_pSystem->CreateEmitterSetID(
                m_pHandle,
                gfl2::math::Vector3::GetZero(),
                0,
                m_ID);
  }
}
// End
void ParticleImpl::End()
{
  nw::eft::EmitterSet* const pEmitterSet = m_pHandle->GetEmitterSet();
  if(pEmitterSet != NULL
              && pEmitterSet->IsAlive()
              && ! pEmitterSet->IsFadeRequest())
  {
    pEmitterSet->Fade();
  }
}
// Kill
void ParticleImpl::Kill()
{
  if(m_pHandle != NULL
              && m_pHandle->GetEmitterSet() != NULL)
  {
    m_pHandle->GetEmitterSet()->Kill();
  }
}
// Whether this particle is alive or not
bool ParticleImpl::IsAlive() const
{
  return m_pHandle != NULL
          && m_pHandle->GetEmitterSet() != NULL
          && m_pHandle->GetEmitterSet()->IsAlive();;
}
// Update location & rotation & scale
void ParticleImpl::Update(
            const gfl2::math::Vector3& location,
            const gfl2::math::Quaternion& rotation,
            const f32& scale)
{
  if(m_pHandle != NULL
              && m_pHandle->GetEmitterSet() != NULL)
  {
    const nw::math::VEC3 nwLocation(
                location.GetX(),
                location.GetY(),
                location.GetZ());
    const gfl2::math::Vector rotationVector = rotation.QuaternionToRadian();
    const nw::math::VEC3 nwRotation(
                rotationVector.x,
                rotationVector.y,
                rotationVector.z);
    const nw::math::VEC3 nwScale(
                scale,
                scale,
                scale);
    nw::math::MTX34 srt;
#if defined(GF_PLATFORM_CTR)
    nw::eft::MTX34MakeSRT(
#elif defined(GF_PLATFORM_WIN32)
    nw::math::MTX34MakeSRT(
#endif
                &srt,
                nwScale,
                nwRotation,
                nwLocation);
    m_pHandle->GetEmitterSet()->SetMtx(srt);
  }
}
// Effedct ID
const u32& ParticleImpl::ID() const
{
  return m_ID;
}
}// namespace Effect
}// namespace LunaSolDive
