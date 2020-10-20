//======================================================================
/**
 * @file ModelBackground.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief 背景モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ModelBackground.h"
#include <niji_conv_header/app/mantain_surf/field/bg_01.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

ModelBackground::ModelBackground()
{
  m_SetupParam.m_resourceId        = GARC_mantain_field_bg_01_BIN;
  m_SetupParam.m_modelResourceId   = BL_IDX_BG_01_BG_01_CTR_GFBMDLP;
  m_SetupParam.m_animationListSize = ANIME_MAX;
}

ModelBackground::~ModelBackground()
{
}

void ModelBackground::StartAnimation()
{
  ChangeAnimation(ANIME_WAVE, false, ANIME_LOOP_ON, ANIME_SLOT_WAVE);
  ChangeAnimation(ANIME_TIME, false, ANIME_LOOP_OFF, ANIME_SLOT_TIME);
}

void ModelBackground::UpdateTimeZone(s32 timeZone)
{
  m_pBaseModel->SetAnimationFrame(timeZone, ANIME_SLOT_TIME);
}

s32 ModelBackground::GetAnimationResId(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_NORMAL:
    return BL_IDX_BG_01_BG_01_001_NORMAL_GFBMOT;
  case ANIME_WAVE:
    return BL_IDX_BG_01_BG_01_002_WAVE_GFBMOT;
  case ANIME_TIME:
    return BL_IDX_BG_01_BG_01_003_TIME_GFBMOT;
  default:
    return -1;
  }
}

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
