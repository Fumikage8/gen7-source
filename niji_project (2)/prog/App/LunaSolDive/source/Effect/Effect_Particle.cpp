//======================================================================
/**
 * @file Effect_Particle.cpp
 * @date 2016/11/25 22:42:45
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <Effect/include/gfl2_EffectHandle.h>
#include <Effect/include/gfl2_EffectHeap.h>
#include <Effect/include/gfl2_EffectSystem.h>
#include "./Manager.h"
#include "./Particle.h"
#include "./ParticleImpl.h"
#include "../Assert.h"
#include "arc_index/LunaSolDiveEffect.gaix"

namespace LunaSolDive{
namespace Effect{
// constructor
Particle::Particle()
    : m_pManager(NULL),
      m_pSystem(NULL),
      m_pEffectHeap(NULL),
      m_TranslationMethod(TRANSLATION_METHOD__NONE),
      m_pMain(NULL),
      m_pSub(NULL),
      m_Scale(0.0f),
      m_IsInTranslationProcess(false){}
// destructor
Particle::~Particle()
{
  Terminate();
}
// Initialize
void Particle::Initialize(
            Effect::Manager* pManager,
            gfl2::Effect::System* pSystem,
            gfl2::Effect::GFHeap* pEffectHeap,
            const u32& id)
{
  // set pointer
  m_pManager = pManager;
  LUNA_SOL_DIVE_ASSERT(m_pManager != NULL);
  m_pSystem = pSystem;
  LUNA_SOL_DIVE_ASSERT(m_pSystem != NULL);
  m_pEffectHeap = pEffectHeap;
  LUNA_SOL_DIVE_ASSERT(m_pEffectHeap != NULL);
  // create main implement
  m_pMain = GFL_NEW(m_pEffectHeap->GetGFHeapBase()) ParticleImpl();
  LUNA_SOL_DIVE_ASSERT(m_pMain != NULL);
  m_pMain->Initialize(
              m_pSystem,
              m_pEffectHeap,
              id);
  // set translation method
  switch(id)
  {
    case GARC_LunaSolDiveEffect_ea_dive_run_fol_lo_lz_BPTCL:
    case GARC_LunaSolDiveEffect_ea_dive_run_fol_max_lo_lz_BPTCL:
    case GARC_LunaSolDiveEffect_ea_dive_run_root_lo_lz_BPTCL:
      m_TranslationMethod = TRANSLATION_METHOD__SWAP;
      break;
    default:
      m_TranslationMethod = TRANSLATION_METHOD__RELOCATE;
  }
  // register to manger
  m_pManager->RegisterParticlePtr(this);
}
// Terminate
void Particle::Terminate()
{
  // unregister to manger
  m_pManager->UnregisterParticlePtr(this);
  // delete particle
  GFL_SAFE_DELETE(m_pMain);
  GFL_SAFE_DELETE(m_pSub);
  // unset pointer
  m_pManager = NULL;
  m_pSystem = NULL;
  m_pEffectHeap = NULL;
  // reset parameter
  m_TranslationMethod = TRANSLATION_METHOD__NONE;
  m_Location = gfl2::math::Vector3::GetZero();
  m_Rotation = gfl2::math::Quaternion();
  m_Scale = 0.0f;
  m_Displacement = gfl2::math::Vector3::GetZero();
  m_IsInTranslationProcess = false;
}
// Start
void Particle::Start()
{
  LUNA_SOL_DIVE_ASSERT(m_pMain != NULL);
  if(m_pMain != NULL)
  {
    m_pMain->Start();
    if(m_TranslationMethod == TRANSLATION_METHOD__SWAP
            && m_IsInTranslationProcess)
    {
      if(m_pSub == NULL)
      {
        CreateSub();
      }
      if(m_pSub != NULL)
      {
        m_pSub->Start();
      }
    }
  }
}
// End
void Particle::End()
{
  LUNA_SOL_DIVE_ASSERT(m_pMain != NULL);
  if(m_pMain != NULL)
  {
    m_pMain->End();
    if(m_pSub != NULL)
    {
      m_pSub->End();
    }
  }
}
// Kill
void Particle::Kill()
{
  LUNA_SOL_DIVE_ASSERT(m_pMain != NULL);
  if(m_pMain != NULL)
  {
    m_pMain->Kill();
    if(m_pSub != NULL)
    {
      m_pSub->Kill();
    }
  }
}
// Whether this particle is alive or not
bool Particle::IsAlive() const
{
  LUNA_SOL_DIVE_ASSERT(m_pMain != NULL);
  return (m_pMain != NULL)
          ? m_pMain->IsAlive()
          : false;
}
// Update location & rotation & scale
void Particle::Update(
            const gfl2::math::Vector3& location,
            const gfl2::math::Quaternion& rotation,
            const f32& scale)
{
  LUNA_SOL_DIVE_ASSERT(m_pMain != NULL);
  if(m_pMain != NULL)
  {
    m_Location = location;
    m_Rotation = rotation;
    m_Scale = scale;
    m_pMain->Update(
                m_Location,
                m_Rotation,
                m_Scale);
    if(m_pSub != NULL)
    {
      m_pSub->Update(
                  m_Location + m_Displacement,
                  m_Rotation,
                  m_Scale);
    }
  }
}
// Effedct ID
u32 Particle::ID() const
{
  return (m_pMain != NULL)
          ? m_pMain->ID()
          : 0xffffffff;
}
// Prepare for translation
void Particle::PrepareTranslation(
            const gfl2::math::Vector3& displacement)
{
  LUNA_SOL_DIVE_ASSERT(m_pMain != NULL);
  LUNA_SOL_DIVE_ASSERT(! gfl2::math::IsAlmostZero(displacement.Length()));
  LUNA_SOL_DIVE_ASSERT(! m_IsInTranslationProcess);
  LUNA_SOL_DIVE_ASSERT(m_TranslationMethod != TRANSLATION_METHOD__NONE);
  m_IsInTranslationProcess = true;
  m_Displacement = displacement;
  if(m_TranslationMethod == TRANSLATION_METHOD__SWAP)
  {
    if(m_pMain != NULL && m_pMain->IsAlive())
    {
      CreateSub();
      if(m_pSub != NULL)
      {
        m_pSub->Start();
        m_pSub->Update(
                    m_Location + m_Displacement,
                    m_Rotation,
                    m_Scale);
      }
    }
  }
}
// Whether this is in process for the translation or not
bool Particle::IsInTranslationProcess() const
{
  return m_IsInTranslationProcess;
}
// Execute translation
void Particle::ExecuteTranslation()
{
  LUNA_SOL_DIVE_ASSERT(m_pMain != NULL);
  LUNA_SOL_DIVE_ASSERT(m_IsInTranslationProcess);
  LUNA_SOL_DIVE_ASSERT(m_TranslationMethod != TRANSLATION_METHOD__NONE);
  m_IsInTranslationProcess = false;
  m_Location += m_Displacement;
  m_Displacement = gfl2::math::Vector3::GetZero();
  if(m_TranslationMethod == TRANSLATION_METHOD__RELOCATE
          && m_pMain != NULL)
  {
    m_pMain->Update(
                m_Location,
                m_Rotation,
                m_Scale);
  }
  else if(m_TranslationMethod == TRANSLATION_METHOD__SWAP
      && m_pSub != NULL)
  {
    std::swap(m_pMain, m_pSub);
    GFL_SAFE_DELETE(m_pSub);
  }
}
// Create sub particle
void Particle::CreateSub()
{
  LUNA_SOL_DIVE_ASSERT(m_pMain != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pMain->IsAlive());
  LUNA_SOL_DIVE_ASSERT(m_pSub == NULL);
  m_pSub = GFL_NEW(m_pEffectHeap->GetGFHeapBase()) ParticleImpl();
  LUNA_SOL_DIVE_ASSERT(m_pSub != NULL);
  if(m_pSub != NULL)
  {
    m_pSub->Initialize(
                m_pSystem,
                m_pEffectHeap,
                m_pMain->ID());
  }
}
}// namespace Effect
}// namespace LunaSolDive
