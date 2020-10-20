//======================================================================
/**
 * @file Parameter.h
 * @date 2017/03/08 13:26:28
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__SOUND__PARAMETER_H_INCLUDED__
#define __LUNA_SOL_DIVE__SOUND__PARAMETER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>

namespace LunaSolDive{
#if PM_DEBUG
// forward declarations
namespace Debug{
class Menu;
}// namespace Debug
#endif// PM_DEBUG

namespace Sound{
struct VolumeControlParameter
{
  f32 bgm_volume;
  f32 voice_volume;
  u32 down_frames;
  u32 keep_frames;
  u32 up_frames;
};

class Parameter
{
  GFL_FORBID_COPY_AND_ASSIGN(Parameter);
public:
  // Initialize
  static void Initialize(
                gfl2::heap::HeapBase* pHeap);
  // Terminate
  static void Terminate();
  // getter
  static const Parameter& Get();
  // Reset
  void Reset();
  // Volume of Pokemon Voice
  const f32& PokemonVoiceVolume() const;
  // Volume Control for Z ring: ActivateEnergyAura
  const VolumeControlParameter& VolumeControlActivateEnergyAura() const;
  // Volume Control for Z ring: Hit Distortion
  const VolumeControlParameter& VolumeControlHitDistortion() const;
private:
  // instance
  static Parameter* pInstance;
  // constructor
  Parameter();
  // Volume of Pokemon Voice
  f32 m_PokemonVoiceVolume;
  // Volume Control for Z ring: Activate Energy Aura
  VolumeControlParameter m_VolumeControlActivateEnergyAura;
  // BGM Volume Control for Z ring: Hit Distortion
  VolumeControlParameter m_VolumeControlHitDistortion;
#if PM_DEBUG
  friend class Debug::Menu;
#endif// PM_DEBUG
};
}// namespace Sound
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__SOUND__PARAMETER_H_INCLUDED__
