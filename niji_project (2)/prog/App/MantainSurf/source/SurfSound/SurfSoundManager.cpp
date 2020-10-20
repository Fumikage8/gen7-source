//======================================================================
/**
 * @file SurfSoundManager.cpp
 * @date 2017/2/6 20:42:00
 * @author fang_yicheng
 * @brief サウンドマネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "SurfSoundManager.h"
#include "../MantainSurfAppManager.h"
#include <Sound/include/Sound.h>
#include <Sound/include/HioSoundEditor.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(SurfSound)

const u32 SURF_SE_FADE_FRAME_LONG  = 30;     // サーフSEの切り替えフレーム数(長)
const u32 SURF_SE_FADE_FRAME_SHORT = 6;      // サーフSEの切り替えフレーム数(短)

const f32 SURF_SE_PITCH_MIN = 1.0f;   // サーフSEの最小ピッチ
const f32 SURF_SE_PITCH_MAX = 1.6f;   // サーフSEの最大ピッチ

SurfSoundManager::SurfSEIndex         SurfSoundManager::m_SurfSENow = SurfSoundManager::SURF_SE_NONE;
SurfSoundManager::SEInfo              SurfSoundManager::m_SEInfoList[SurfSoundManager::SE_MAX];
SurfSoundManager::SEIndex             SurfSoundManager::m_resumingSEList[SE_MAX];
SurfSoundManager::MasterSEVolumeParam SurfSoundManager::m_MasterVolumeParam;
u32                                   SurfSoundManager::m_resumingSEListSize = 0;
f32                                   SurfSoundManager::m_masterSEVolume = 1.0;


SurfSoundManager::SurfSoundManager() :
  m_pAppManager(NULL)
{
}

SurfSoundManager::~SurfSoundManager()
{
}

void SurfSoundManager::Initialize(MantainSurfAppManager* pAppManager)
{
  m_pAppManager = pAppManager;
  m_SurfSENow = SurfSoundManager::SURF_SE_NONE;
  m_resumingSEListSize = 0;
  m_masterSEVolume = 1.0f;
  m_MasterVolumeParam.Initialize();
  for(u32 i = 0; i < SE_MAX; i++)
  {
    m_SEInfoList[i].sound_id = GetSESoundId(static_cast<SEIndex>(i));
    m_SEInfoList[i].volume = 1.0f;
    Sound::ChangeSEVolume(m_SEInfoList[i].sound_id, m_SEInfoList[i].volume * m_masterSEVolume);
  }
}

void SurfSoundManager::ChangeSurfSE(SurfSEIndex se)
{
  if(se == SURF_SE_NONE)
  {
    // SE停止
    if(m_SurfSENow != SURF_SE_NONE)
    {
      Sound::StopSE(GetSESoundId(m_SurfSENow), SURF_SE_FADE_FRAME_SHORT);
    }
  }
  else if(m_SurfSENow != se)
  {
    // SE再生
    if(m_SurfSENow != SURF_SE_NONE)
    {
      // 現在再生中のSEをフェードアウト
      Sound::StopSE(GetSESoundId(m_SurfSENow), SURF_SE_FADE_FRAME_LONG);
    }
    Sound::PlaySE(GetSESoundId(se), SURF_SE_FADE_FRAME_LONG);
  }
  m_SurfSENow = se;
}

void SurfSoundManager::ChangeSurfSEVolume(f32 volume)
{
  SEIndex seIndex = SE_NONE;
  switch(m_SurfSENow)
  {
  case SURF_SE_0:
    seIndex = SE_SURF_LV_0;
    break;
  case SURF_SE_1:
    seIndex = SE_SURF_LV_1;
    break;
  case SURF_SE_2:
    seIndex = SE_SURF_LV_2;
    break;
  default:
    return;
  }
  m_SEInfoList[seIndex].volume = volume;
  Sound::ChangeSEVolume(m_SEInfoList[seIndex].sound_id, volume * m_masterSEVolume);
}

void SurfSoundManager::ChangeMasterSEVolume(f32 volume, u32 changeFrame)
{
  m_MasterVolumeParam.volumeStart = m_masterSEVolume;
  m_MasterVolumeParam.volumeEnd = volume;
  m_MasterVolumeParam.changeFrame = changeFrame + 1;
  m_MasterVolumeParam.curFrame = 0;
}

void SurfSoundManager::PlaySE(SEIndex se, u32 fadein_frame, f32 pitch)
{
  Sound::PlaySE(m_SEInfoList[se].sound_id, fadein_frame);
  Sound::ChangeSEPitch(m_SEInfoList[se].sound_id, pitch);
  Sound::ChangeSEVolume(m_SEInfoList[se].sound_id, m_SEInfoList[se].volume * m_masterSEVolume);
}

void SurfSoundManager::StopSE(SEIndex se, u32 fadeout_frame)
{
  Sound::StopSE(m_SEInfoList[se].sound_id, fadeout_frame);
}

void SurfSoundManager::ChangeSEPitch(SEIndex se, f32 pitch)
{
  Sound::ChangeSEPitch(m_SEInfoList[se].sound_id, pitch);
}

void SurfSoundManager::ChangeSEVolume(SEIndex se, f32 volume)
{
  m_SEInfoList[se].volume = volume;
  Sound::ChangeSEVolume(m_SEInfoList[se].sound_id, volume * m_masterSEVolume);
}

void SurfSoundManager::ChangeSEPan(SEIndex se, f32 pan)
{
  Sound::ChangeSEPan(m_SEInfoList[se].sound_id, pan);
}

void SurfSoundManager::StopAllSE(u32 fadeFrame)
{
  for(u32 i = 0; i < SE_MAX; i++)
  {
    StopSE(static_cast<SEIndex>(i), fadeFrame);
  }
  m_SurfSENow = SURF_SE_NONE;
}

bool SurfSoundManager::IsAllSEStopped()
{
  for(u32 i = 0; i < SE_MAX; i++)
  {
    if(Sound::IsSEPlaying(m_SEInfoList[i].sound_id))
    {
      return false;
    }
  }
  return true;
}

void SurfSoundManager::Update()
{
  UpdateSurfSE();
  UpdateMasterSEVolume();
}

void SurfSoundManager::UpdateMasterSEVolume()
{
  if(m_MasterVolumeParam.changeFrame == m_MasterVolumeParam.curFrame)
  {
    return;
  }

  m_MasterVolumeParam.curFrame++;
  f32 t = static_cast<f32>(m_MasterVolumeParam.curFrame) / static_cast<f32>(m_MasterVolumeParam.changeFrame);
  m_masterSEVolume = gfl2::math::Lerp(m_MasterVolumeParam.volumeStart, m_MasterVolumeParam.volumeEnd, t);

  for(u32 i = 0; i < SE_MAX; i++)
  {
    Sound::ChangeSEVolume(m_SEInfoList[i].sound_id, m_SEInfoList[i].volume * m_masterSEVolume);
  }
}

void SurfSoundManager::UpdateSurfSE()
{
  if(m_SurfSENow == SURF_SE_NONE) return;

  // ピッチ更新
  f32 t = m_pAppManager->GetPlayerController()->CalcSurfSEPitchLevel();
  t = gfl2::math::Clamp(t, 0.0f, 1.0f);
  f32 pitch = gfl2::math::Lerp(SURF_SE_PITCH_MIN, SURF_SE_PITCH_MAX, t);
  Sound::ChangeSEPitch(GetSESoundId(m_SurfSENow), pitch);
  // パン更新
  f32 pan = gfl2::math::Lerp(-0.2f, 0.5f, m_pAppManager->GetPlayerController()->GetAccelParam()->m_turnRate);
  Sound::ChangeSEPan(GetSESoundId(m_SurfSENow), pan);
}

void SurfSoundManager::StartBGM()
{
  Sound::StartBGMReq(STRM_BGM_MJ_SYS02);
}

void SurfSoundManager::EndBGM()
{
  Sound::FadeOutBGMReq(Sound::BGM_FADE_APP);
}

u32 SurfSoundManager::GetSESoundId(SEIndex se)
{
  switch(se)
  {
  case SE_SURF_ENV:
    return SEQ_SE_MJ_SYS_021;
  case SE_SURF_LV_0:
    return SEQ_SE_MJ_SYS_005;
  case SE_SURF_LV_1:
    return SEQ_SE_MJ_SYS_002;
  case SE_SURF_LV_2:
    return SEQ_SE_MJ_SYS_003;
  case SE_ACCEL:
    return SEQ_SE_MJ_SYS_004;
  case SE_HIT_GIMMICK:
    return SEQ_SE_MJ_SYS_006;
  case SE_GIMMICK_ACTION_3:
    return SEQ_SE_MJ_SYS_022;
  case SE_TURN_OVER:
    return SEQ_SE_MJ_SYS_007;
  case SE_AERIAL_START:
    return SEQ_SE_MJ_SYS_008;
  case SE_AERIAL_NORMAL:
    return SEQ_SE_MJ_SYS_009;
  case SE_LANDING:
    return SEQ_SE_MJ_SYS_016;
  case SE_RECOVERING:
    return SEQ_SE_MJ_SYS_017;
  case SE_RECOVERED:
    return SEQ_SE_MJ_SYS_018;
  case SE_GIMMICK_ACTION_1:
    return SEQ_SE_MJ_SYS_019;
  case SE_GIMMICK_ACTION_2:
    return SEQ_SE_MJ_SYS_020;
  case SE_PADDLING_END:
    return SEQ_SE_MJ_SYS_023;
  case SE_SKILL_EX_01:
    return SEQ_SE_MJ_SYS_011;
  case SE_SKILL_EX_02:
    return SEQ_SE_MJ_SYS_012;
  case SE_SKILL_EX_03:
    return SEQ_SE_MJ_SYS_015;
  case SE_SKILL_EX_04:
    return SEQ_SE_MJ_SYS_014;
  case SE_SKILL_EX_05:
    return SEQ_SE_MJ_SYS_013;
  case SE_SKILL_EX_06:
    return SEQ_SE_MJ_SYS_010;
  case SE_SKILL_EX_07:
    return SEQ_SE_MJ_SYS_062;
  case SE_SKILL_NORMAL_01:
    return SEQ_SE_MJ_SYS_042;
  case SE_SKILL_NORMAL_02:
    return SEQ_SE_MJ_SYS_043;
  case SE_SUNSHINE:
    return SEQ_SE_MJ_SYS_025;
  case SE_SHOOTING_START:
    return SEQ_SE_MJ_SYS_028;
  case SE_UI_SCORE_LOW:
    return SEQ_SE_MJ_SYS_026;
  case SE_UI_SCORE_HIGH:
    return SEQ_SE_MJ_SYS_027;
  case SE_SKILL_END:
    return SEQ_SE_MJ_SYS_044;
  case SE_SKILL_START:
    return SEQ_SE_MJ_SYS_045;
  case SE_UI_RESULT_INOUT:           // (UI)Result In/Out
    return SEQ_SE_MJ_SYS_029;
  case SE_UI_RESULTITEM_IN:          // (UI)Result 項目 in
    return SEQ_SE_MJ_SYS_066;
  case SE_UI_RESULT_COUNTUP_LOOP:    // (UI)Result カウントアップ（ループ）
    return SEQ_SE_MJ_SYS_064;
  case SE_UI_RESULT_COUNTUP_FINISH:  // (UI)Result カウントアップ 終了
    return SEQ_SE_MJ_SYS_065;
  default:
    return 0;
  }
}

u32 SurfSoundManager::GetSESoundId(SurfSEIndex se)
{
  switch(se)
  {
  case SURF_SE_0:
    return GetSESoundId(SE_SURF_LV_0);
  case SURF_SE_1:
    return GetSESoundId(SE_SURF_LV_1);
  case SURF_SE_2:
    return GetSESoundId(SE_SURF_LV_2);
  default:
    return 0;
  }
}

bool SurfSoundManager::StopSoundForTutorial(bool isForce)
{
  for(u32 i = 0; i < SE_MAX; i++)
  {
    if(IsLoopSE(static_cast<SEIndex>(i))) continue;
    if(Sound::IsSEPlaying(m_SEInfoList[i].sound_id))
    {
      // 非ループSEが再生中
      if(isForce)
      {
        // 強制停止
        StopSE(static_cast<SEIndex>(i));
      }
      else
      {
        return false;
      }
    }
  }
  // ループSE停止
  m_resumingSEListSize = 0;
  for(u32 i = 0; i < SE_MAX; i++)
  {
    if(IsLoopSE(static_cast<SEIndex>(i)) && Sound::IsSEPlaying(m_SEInfoList[i].sound_id))
    {
      StopSE(static_cast<SEIndex>(i));
      m_resumingSEList[m_resumingSEListSize] = static_cast<SEIndex>(i);
      m_resumingSEListSize++;
    }
  }
  return true;
}

void SurfSoundManager::ResumeSoundForTutorial()
{
  for(u32 i = 0; i < m_resumingSEListSize; i++)
  {
    PlaySE(m_resumingSEList[i]);
  }
}

bool SurfSoundManager::IsLoopSE(SEIndex se)
{
  switch(se)
  {
  case SE_SURF_ENV:
  case SE_SURF_LV_0:
  case SE_SURF_LV_1:
  case SE_SURF_LV_2:
  case SE_AERIAL_NORMAL:
  case SE_RECOVERING:
  case SE_GIMMICK_ACTION_2:
  case SE_UI_RESULT_COUNTUP_LOOP:
    return true;
  default:
    return false;
  }
}

GFL_NAMESPACE_END(SurfSound)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
