//======================================================================
/**
 * @file ModelNami.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief 波モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ModelNami.h"
#include <niji_conv_header/app/mantain_surf/field/wave.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

const char* NAMI_TOP_JOINT_NAME = "joint_takasa";

ModelNami::ModelNami()
{
}

ModelNami::ModelNami(bool isUseLodModel)
{
  m_SetupParam.m_resourceId        = GARC_mantain_field_wave_BIN;
  m_SetupParam.m_modelResourceId   = BL_IDX_WAVE_WAVE_CTR_GFBMDLP;
  m_SetupParam.m_animationListSize = ANIME_MAX;
  m_SetupParam.m_effectListSize    = EFT_MAX;
}

ModelNami::~ModelNami()
{
}

void ModelNami::StartAnimation()
{
  ChangeAnimation(ANIME_NORMAL_LOOP, false, ANIME_LOOP_ON, ANIME_SLOT_NORMAL_LOOP);
  ChangeAnimation(ANIME_TIME, false, ANIME_LOOP_OFF, ANIME_SLOT_TIME);
  ChangeAnimation(ANIME_HEIGHT_CHANGE, false, ANIME_LOOP_OFF, ANIME_SLOT_HEIGHT_CHANGE);
}

void ModelNami::SetHeightAnimationFrame(f32 heightRate)
{
  f32 frame = m_pBaseModel->GetAnimationEndFrame(ANIME_SLOT_HEIGHT_CHANGE) * heightRate;
  m_pBaseModel->SetAnimationFrame(frame, ANIME_SLOT_HEIGHT_CHANGE);
  // 水柱の大きさを調整
  SetEffectScale(EFT_WATER_WALL, gfl2::math::Vector3(1.0f, 1.0f, 1.0f) * (0.7f + 0.3f * heightRate));
}

void ModelNami::UpdateTimeZone(s32 timeZone)
{
  m_pBaseModel->SetAnimationFrame(timeZone, ANIME_SLOT_TIME);
}

gfl2::math::Vector3 ModelNami::GetTopPosition()
{
  return GetJointPosition(NAMI_TOP_JOINT_NAME);
}

void ModelNami::SetPlayerPosition(const gfl2::math::Vector3 &position)
{
  m_playerPosition = position;
}

s32 ModelNami::GetAnimationResId(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_NORMAL_LOOP:
    return BL_IDX_WAVE_WAVE_001_NORMAL_LOOP_GFBMOT;
  case ANIME_HEIGHT_CHANGE:
    return BL_IDX_WAVE_WAVE_002_TAKASA_LOOP_GFBMOT;
  case ANIME_TIME:
    return BL_IDX_WAVE_WAVE_003_TIME_GFBMOT;
  default:
    return -1;
  }
}

s32 ModelNami::GetEffectResId(s32 eftId)
{
  switch(eftId)
  {
  case EFT_WATER_WALL:
    return GARC_mantain_effect_ea_manta_bg_waterwall01_lo_lz_BPTCL;
  default:
    return -1;
  }
}

gfl2::math::Vector3 ModelNami::GetEffectPosition(s32 eftId)
{
  gfl2::math::Vector3 pos = GetPosition();
  switch(eftId)
  {
  case EFT_WATER_WALL:
    pos = GetTopPosition();
    pos.z = m_playerPosition.z;
    return pos;
  default:
    return GetEffectPosition(eftId);
  }
}

gfl2::math::Vector3 ModelNami::GetEffectOffset(s32 eftId)
{
  switch(eftId)
  {
  case EFT_WATER_WALL:
    return gfl2::math::Vector3(0.0f, 0.0f, 0.0f);
  default:
    return ModelBase::GetEffectOffset(eftId);
  }
}

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
