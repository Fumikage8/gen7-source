//======================================================================
/**
 * @file Effect_Manager.cpp
 * @date 2016/11/25 19:21:17
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <Effect/include/gfl2_EffectSystem.h>
#include <Effect/include/gfl2_EffectRenderPath.h>
#include "./Manager.h"
#include "./Particle.h"
#include "../Assert.h"
#include "../RenderingPipeline.h"
#include "../Archive/Effect.h"
#include "arc_index/LunaSolDiveEffect.gaix"

namespace LunaSolDive{
namespace Effect{
// pearameter
static u32 PARTICLE_PTR_LIST_SIZE = 32;
// constructor
Manager::Manager()
    : m_pRenderingPipeline(NULL),
      m_pEffectSystem(NULL),
      m_pEffectHeap(NULL),
      m_pEffectGroupIDList(NULL),
      m_ParticlePtrList(NULL),
      m_IsInTranslationProcess(false){}
// Initialize
void Manager::Initialize(
            gfl2::heap::HeapBase* pHeap,
            gfl2::heap::HeapBase* pEffectHeap,
            RenderingPipeline* pRenderingPipeline)
{
  // set RenderingPipeline pointer
  m_pRenderingPipeline = pRenderingPipeline;
  LUNA_SOL_DIVE_ASSERT(m_pRenderingPipeline != NULL);
  // initialize EffectHeap
  m_pEffectHeap = GFL_NEW(pHeap) gfl2::Effect::GFHeap(pEffectHeap);
  LUNA_SOL_DIVE_ASSERT(m_pEffectHeap != NULL);
  // initialize EffectSystem
  const u32 EMITTER_SET_NUM = 48;
  const u32 EMITTER_NUM =  EMITTER_SET_NUM * 8;
  const u32 PARTICLE_NUM = 768;
  const u32 STRIPE_NUM  = 64;
  gfl2::Effect::Config configSystem;
  configSystem.SetEffectHeap(m_pEffectHeap);
#ifdef GF_PLATFORM_CTR
  configSystem.SetParticleMemType(nw::eft::BUFFER_LOCATION_FCRAM);
#endif// GF_PLATFORM_CTR
  configSystem.SetResourceNum(GARC_LunaSolDiveEffect_DATA_NUM);
  configSystem.SetEmitterNum(EMITTER_NUM);
  configSystem.SetEmitterSetNum(EMITTER_SET_NUM);
  configSystem.SetParticleNum(PARTICLE_NUM);
  configSystem.SetStripeNum(STRIPE_NUM);
  m_pEffectSystem = GFL_NEW(pHeap) gfl2::Effect::System(configSystem);
  // register RenderingPipeline
  m_pEffectGroupIDList = GFL_NEW(pHeap) gfl2::util::List<u32>(pHeap, 1);
  m_pEffectGroupIDList->PushFront(0);
  gfl2::Effect::EffectRenderPath::Config configRendering;
  configRendering.m_pSystem = m_pEffectSystem;
  configRendering.m_cpGroupIDList = m_pEffectGroupIDList;
  m_pRenderingPipeline->SetupEffectRenderPathConfig(configRendering);
  // initialize list of pointer to particle
  m_ParticlePtrList = GFL_NEW(pHeap) Particle*[PARTICLE_PTR_LIST_SIZE];
  for(u32 i = 0; i < PARTICLE_PTR_LIST_SIZE; ++i)
  {
    m_ParticlePtrList[i] = NULL;
  }
}
// Terminate
void Manager::Terminate()
{
  LUNA_SOL_DIVE_ASSERT(RegisteredNumber() == 0);
  // delete Effect System
  if(m_pEffectSystem != NULL)
  {
    for(u32 i = 0; i < Archive::Effect::Get().DataNumber(); ++i)
    {
      m_pEffectSystem->ClearResource(
                  m_pEffectHeap,
                  i);
    }
    GFL_DELETE(m_pEffectSystem);
    m_pEffectSystem = NULL;
  }
  GFL_SAFE_DELETE(m_pEffectHeap);
  GFL_SAFE_DELETE(m_pEffectGroupIDList);
  GFL_SAFE_DELETE(m_ParticlePtrList);
  // unset pointer
  m_pRenderingPipeline = NULL;
  // reset parameter
  m_IsInTranslationProcess = false;
  m_Displacement = gfl2::math::Vector3::GetZero();
}
// Register Resource
void Manager::SetUp()
{
  // register resource
  for(u32 i = 0; i < Archive::Effect::Get().DataNumber(); ++i)
  {
    m_pEffectSystem->EntryResource(
                m_pEffectHeap,
                Archive::Effect::Get().Data(i),
                i);
  }
}
// Update
void Manager::Update()
{
  m_pEffectSystem->BeginFrame();
  m_pEffectSystem->Calc(0);
}
// Create
Particle* Manager::Create(
            const u32& id)
{
  LUNA_SOL_DIVE_ASSERT(id < Archive::Effect::Get().DataNumber());
  Particle* const pParticle = GFL_NEW(m_pEffectHeap->GetGFHeapBase()) Particle();
  LUNA_SOL_DIVE_ASSERT(pParticle != NULL);
  LUNA_SOL_DIVE_ASSERT(RegisteredNumber() < PARTICLE_PTR_LIST_SIZE);
  pParticle->Initialize(
              this,
              m_pEffectSystem,
              m_pEffectHeap,
              id);
  ARAI_PRINT("Registerd Particle: %d\n", RegisteredNumber());
  if(m_IsInTranslationProcess)
  {
    pParticle->PrepareTranslation(m_Displacement);
  }
  return pParticle;
}
// Prepare for Translation
void Manager::PrepareTranslation(
            const gfl2::math::Vector3& displacement)
{
  LUNA_SOL_DIVE_ASSERT(! m_IsInTranslationProcess);
  m_IsInTranslationProcess = true;
  m_Displacement = displacement;
  for(u32 i = 0; i < PARTICLE_PTR_LIST_SIZE; ++i)
  {
    if(m_ParticlePtrList[i] != NULL)
    {
      m_ParticlePtrList[i]->PrepareTranslation(m_Displacement);
    }
  }
}
// Execute translation
void Manager::ExecuteTranslation()
{
  LUNA_SOL_DIVE_ASSERT(m_IsInTranslationProcess);
  m_IsInTranslationProcess = false;
  m_Displacement = gfl2::math::Vector3::GetZero();
  for(u32 i = 0; i < PARTICLE_PTR_LIST_SIZE; ++i)
  {
    if(m_ParticlePtrList[i] != NULL)
    {
      m_ParticlePtrList[i]->ExecuteTranslation();
    }
  }
}
// Number of registerd particles
u32 Manager::RegisteredNumber() const
{
  u32 value = 0;
  for(u32 i = 0; i < PARTICLE_PTR_LIST_SIZE; ++i)
  {
    if(m_ParticlePtrList[i] != NULL)
    {
      ++value;
    }
  }
  return value;
}
// Register pointer to Particle
void Manager::RegisterParticlePtr(
            Particle* pParticle)
{
  for(u32 i = 0; i < PARTICLE_PTR_LIST_SIZE; ++i)
  {
    if(m_ParticlePtrList[i] == NULL)
    {
      m_ParticlePtrList[i] = pParticle;
      return;
    }
  }
  LUNA_SOL_DIVE_ASSERT(false);
}
// Unregister pointer to Particle
void Manager::UnregisterParticlePtr(
            const Particle* pParticle)
{
  for(u32 i = 0; i < PARTICLE_PTR_LIST_SIZE; ++i)
  {
    if(m_ParticlePtrList[i] == pParticle)
    {
      m_ParticlePtrList[i] = NULL;
      return;
    }
  }
  LUNA_SOL_DIVE_ASSERT(false);
}
}// namespace Effect
}// namespace LunaSolDive
