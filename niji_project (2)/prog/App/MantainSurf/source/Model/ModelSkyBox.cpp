//======================================================================
/**
 * @file ModelSkyBox.cpp
 * @date 2017/01/16 13:13:13
 * @author fang_yicheng
 * @brief 天球モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ModelSkyBox.h"
#include <niji_conv_header/app/mantain_surf/field/skybox.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

ModelSkyBox::ModelSkyBox()
{
  m_SetupParam.m_resourceId        = GARC_mantain_field_skybox_BIN;
  m_SetupParam.m_modelResourceId   = BL_IDX_SKYBOX_SKYBOX_CTR_GFBMDLP;
  m_SetupParam.m_animationListSize = ANIME_MAX;
}

ModelSkyBox::~ModelSkyBox()
{
}

void ModelSkyBox::StartAnimation()
{
  // 常時アニメーション
  ChangeAnimation(ANIME_NORMAL, false, ANIME_LOOP_ON, ANIME_SLOT_NORMAL);
  // 時間帯アニメーション
  ChangeAnimation(ANIME_TIME, false, ANIME_LOOP_DEFAULT, ANIME_SLOT_TIME);
  // 月満ち欠けアニメーション
  ChangeAnimation(ANIME_MOON, false, ANIME_LOOP_DEFAULT, ANIME_SLOT_MOON);
}

void ModelSkyBox::UpdateTimeZone(s32 timeZone)
{
  m_pBaseModel->SetAnimationFrame(timeZone, ANIME_SLOT_TIME);
}

void ModelSkyBox::UpdateMoon(s32 moonPattern)
{
  m_pBaseModel->SetAnimationFrame(moonPattern, ANIME_SLOT_MOON);
}

s32 ModelSkyBox::GetAnimationResId(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_NORMAL:
    return BL_IDX_SKYBOX_SKYBOX_01_NORMAL_GFBMOT;
  case ANIME_MOON:
    return BL_IDX_SKYBOX_SKYBOX_02_MOON_GFBMOT;
  case ANIME_TIME:
    return BL_IDX_SKYBOX_SKYBOX_03_TIME_GFBMOT;
  default:
    return -1;
  }
}

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
