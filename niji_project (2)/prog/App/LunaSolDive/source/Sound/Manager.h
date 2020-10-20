//======================================================================
/**
 * @file Manager.h
 * @date 2017/03/07 17:46:34
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__SOUND__MANAGER_H_INCLUDED__
#define __LUNA_SOL_DIVE__SOUND__MANAGER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <macro/include/gfl2_Macros.h>
#include "../Version.h"

namespace LunaSolDive{
namespace Sound{
// forward declaration
struct VolumeControlParameter;

enum SE
{
  SE__ACTIVATE_ENERGY_AURA,
  SE__HIT_ENERGY,
  SE__HIT_OBSTACLE,
  SE__HIT_DISTORTION,
};

class Manager{
  GFL_FORBID_COPY_AND_ASSIGN(Manager);
public:
  // Initialize
  static void Initialize(
              gfl2::heap::HeapBase* pHeap,
              const Version& version);
  // Terminate
  static void Terminate();
  // getter
  static Manager& Get();
  // Start BGM
  void StartBGM();
  // Fade Out BGM
  void FadeOutBGM();
  // Play SE
  void PlaySE(
              const SE& id);
  // Play Pokemon Voice
  void PlayPokemonVoice();
  // Register Volume Control for Z ring
  void RegisterVolumeControl(
              const f32& bgmVolume,
              const f32& voiceVolume,
              const u32& downFrames,
              const u32& keepFrames,
              const u32& upFrames);
  // Register Volume Control for Z ring
  void RegisterVolumeControl(
              const VolumeControlParameter& parameter);
  // Update
  void Update();
private:
  // instance
  static Manager* pInstance;
  // Vlume Control State
  enum VolumeState
  {
    VOLUME_STATE__NONE,
    VOLUME_STATE__DOWN,
    VOLUME_STATE__KEEP,
    VOLUME_STATE__UP,
  };
  // constructor
  Manager(
          const Version& version);
  // Version
  Version m_Version;
  // Volume State
  VolumeState m_VolumeState;
  // Number of frames to lower the volume
  u32 m_VolumeDownFrames;
  // Number of frames to keep the volume lowered
  u32 m_VolumeKeepFrames;
  // Number of frames to raise the volume
  u32 m_VolumeUpFrames;
  // Frame count for volume control
  u32 m_VolumeFrameCount;
};
}// namespace Sound
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__SOUND__MANAGER_H_INCLUDED__
