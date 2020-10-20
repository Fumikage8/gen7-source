//======================================================================
/**
 * @file Sound_Manager.cpp
 * @date 2017/03/07 18:37:07
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <debug/include/gfl2_DebugPrint.h>
#include "Sound/include/Sound.h"
#include "./Manager.h"
#include "./Parameter.h"
#include "../Assert.h"

namespace LunaSolDive{
namespace Sound{
// parameter
static const u8 POKEMON_VOICE_INDEX = 0;
static const SoundMiddleID BGM_ID = STRM_BGM_E13;
// static functtion
// SE -> SoundMiddleID
static SoundMiddleID ToSoundMiddleID(
            const SE& id);
// Tnitialize
void Manager::Initialize(
            gfl2::heap::HeapBase* pHeap,
            const Version& version)
{
  LUNA_SOL_DIVE_ASSERT(pInstance == NULL);
  pInstance = GFL_NEW(pHeap) Manager(
              version);
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
}
// Terminate
void Manager::Terminate()
{
  GFL_SAFE_DELETE(pInstance);
}
// getter
Manager& Manager::Get()
{
  LUNA_SOL_DIVE_ASSERT(pInstance != NULL);
  return *pInstance;
}
// instance
Manager* Manager::pInstance = NULL;
// constructor
Manager::Manager(
            const Version& version)
    : m_Version(version),
      m_VolumeState(VOLUME_STATE__NONE),
      m_VolumeDownFrames(0),
      m_VolumeKeepFrames(0),
      m_VolumeUpFrames(0),
      m_VolumeFrameCount(0)
{
  LUNA_SOL_DIVE_ASSERT(m_Version != VERSION__NONE);
}
// Start BGM
void Manager::StartBGM()
{
  ::Sound::StartBGMReq(BGM_ID);
}
// Fade Out BGM
void Manager::FadeOutBGM()
{
  ::Sound::FadeOutBGMReq(::Sound::BGM_FADE_FAST);
}
// Play SE
void Manager::PlaySE(
            const SE& id)
{
  ::Sound::PlaySE(ToSoundMiddleID(id));
}
// Play Pokemon Voice
void Manager::PlayPokemonVoice()
{
  const MonsNo monsNo =
          (m_Version == VERSION__SOL)
          ? MONSNO_NIKKOU
          : MONSNO_GEKKOU;
  ::Sound::PlayVoice(
              POKEMON_VOICE_INDEX,
              monsNo,
              0,
              ::Sound::VOICE_TYPE_DEFAULT);
  ::Sound::ChangeVoiceVolume(
              POKEMON_VOICE_INDEX,
              Parameter::Get().PokemonVoiceVolume(),
              0);
}
// Register Volume Control for Z ring
void Manager::RegisterVolumeControl(
            const f32& bgmVolume,
            const f32& voiceVolume,
            const u32& downFrames,
            const u32& keepFrames,
            const u32& upFrames)
{
  LUNA_SOL_DIVE_ASSERT(m_VolumeState == VOLUME_STATE__NONE);
  ::Sound::ChangeBGMVolume(
              bgmVolume,
              downFrames);
  ::Sound::ChangeVoiceVolume(
              POKEMON_VOICE_INDEX,
              voiceVolume,
              downFrames);
  m_VolumeState = VOLUME_STATE__DOWN;
  m_VolumeDownFrames = downFrames;
  m_VolumeKeepFrames = keepFrames;
  m_VolumeUpFrames = upFrames;
  ARAI_PRINT("register volume control\n");
  ARAI_PRINT("  BGM volume  : %.3f\n", bgmVolume);
  ARAI_PRINT("  voice volume: %.3f\n", voiceVolume);
  ARAI_PRINT("  down frames : %d\n", m_VolumeDownFrames);
  ARAI_PRINT("  keep frames : %d\n", m_VolumeKeepFrames);
  ARAI_PRINT("  up frames   : %d\n", m_VolumeUpFrames);
}
// Register BGM Volume Control for Z ring
void Manager::RegisterVolumeControl(
            const VolumeControlParameter& parameter)
{
  RegisterVolumeControl(
              parameter.bgm_volume,
              parameter.voice_volume,
              parameter.down_frames,
              parameter.keep_frames,
              parameter.up_frames);
}
// Update
void Manager::Update()
{
  // BGM Volume Control
  if(m_VolumeState != VOLUME_STATE__NONE)
  {
    ARAI_PRINT("volume control: %s, %d\n",
                (m_VolumeState == VOLUME_STATE__DOWN)? "Down":
                (m_VolumeState == VOLUME_STATE__KEEP)? "Keep":
                (m_VolumeState == VOLUME_STATE__UP)? "Up":
                "Error",
                m_VolumeFrameCount);
    if(m_VolumeState == VOLUME_STATE__DOWN
            && m_VolumeFrameCount >= m_VolumeDownFrames)
    {// Down -> Keep
      m_VolumeState = VOLUME_STATE__KEEP;
      m_VolumeFrameCount = 0;
    }
    if(m_VolumeState == VOLUME_STATE__KEEP
            && m_VolumeFrameCount >= m_VolumeKeepFrames)
    {// Keep -> Up
      ::Sound::ChangeBGMVolume(
                  1.0f,
                  m_VolumeUpFrames);
      ::Sound::ChangeVoiceVolume(
                  POKEMON_VOICE_INDEX,
                  Parameter::Get().PokemonVoiceVolume(),
                  m_VolumeUpFrames);
      m_VolumeState = VOLUME_STATE__UP;
      m_VolumeFrameCount = 0;
    }
    if(m_VolumeState == VOLUME_STATE__UP
            && m_VolumeFrameCount >= m_VolumeUpFrames)
    {// Up -> None
      m_VolumeState = VOLUME_STATE__NONE;
      m_VolumeDownFrames = 0;
      m_VolumeKeepFrames = 0;
      m_VolumeUpFrames = 0;
      m_VolumeFrameCount = 0;
      return;
    }
    // increment frame count
    ++m_VolumeFrameCount;
  }
}
// static functtion
// SE -> SoundMiddleID
static SoundMiddleID ToSoundMiddleID(
            const SE& id)
{
  return (id == SE__ACTIVATE_ENERGY_AURA)
          ? SEQ_SE_MJ_SYS_038
          : (id == SE__HIT_ENERGY)
          ? SEQ_SE_MJ_SYS_040
          : (id == SE__HIT_OBSTACLE)
          ? SEQ_SE_MJ_SYS_039
          : SEQ_SE_MJ_SYS_037;
}
}// namespace Sound
}// namespace LunaSoLDive
