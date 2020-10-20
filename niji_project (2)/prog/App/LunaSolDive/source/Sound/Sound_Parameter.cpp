//======================================================================
/**
 * @file Sound_Parameter.cpp
 * @date 2017/03/08 13:38:08
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "./Parameter.h"
#include "../Assert.h"
#if PM_DEBUG
#include "../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Sound{
// Initialize
void Parameter::Initialize(
            gfl2::heap::HeapBase* pHeap)
{
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  LUNA_SOL_DIVE_ASSERT(pInstance == NULL);
  pInstance = GFL_NEW(pHeap) Parameter();
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
#if PM_DEBUG
  Debug::Parameter::Get().pSoundParameter = pInstance;
#endif// PM_DEBUG
}
// Terminate
void Parameter::Terminate()
{
  GFL_SAFE_DELETE(pInstance);
}
// getter
const Parameter& Parameter::Get()
{
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
  return *pInstance;
}
// instance
Parameter* Parameter::pInstance = NULL;
// constructor
Parameter::Parameter()
{
  Reset();
}
// Reset
void Parameter::Reset()
{
  m_PokemonVoiceVolume = 1.0f;
  // Volume Control for Z ring: ActivateEnergyAura
  m_VolumeControlActivateEnergyAura.bgm_volume = 0.50f;
  m_VolumeControlActivateEnergyAura.voice_volume = 0.50f;
  m_VolumeControlActivateEnergyAura.down_frames = 10;
  m_VolumeControlActivateEnergyAura.keep_frames = 10;
  m_VolumeControlActivateEnergyAura.up_frames = 10;
  // Volume Control for Z ring: Hit Distortion
  m_VolumeControlHitDistortion.bgm_volume = 0.50f;
  m_VolumeControlHitDistortion.voice_volume = 0.50f;
  m_VolumeControlHitDistortion.down_frames = 10;
  m_VolumeControlHitDistortion.keep_frames = 10;
  m_VolumeControlHitDistortion.up_frames = 10;
}
// Volume of Pokemon Voice
const f32& Parameter::PokemonVoiceVolume() const
{
  return m_PokemonVoiceVolume;
}
// Volume Control for Z ring: ActivateEnergyAura
const VolumeControlParameter& Parameter::VolumeControlActivateEnergyAura() const
{
  return m_VolumeControlActivateEnergyAura;
}
// Volume Control for Z ring: Hit Distortion
const VolumeControlParameter& Parameter::VolumeControlHitDistortion() const
{
  return m_VolumeControlHitDistortion;
}
}// namespace Sound
}// namespace LunaSolDive
