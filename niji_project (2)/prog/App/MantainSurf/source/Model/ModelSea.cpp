//======================================================================
/**
 * @file ModelSea.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief 波モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ModelSea.h"
#include <niji_conv_header/app/mantain_surf/field/sea.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

ModelSea::ModelSea()
{
  m_SetupParam.m_resourceId        = GARC_mantain_field_sea_BIN;
  m_SetupParam.m_modelResourceId   = BL_IDX_SEA_SEA_CTR_GFBMDLP;
  m_SetupParam.m_animationListSize = ANIME_MAX;
}

ModelSea::~ModelSea()
{
}

void ModelSea::StartAnimation()
{
  ChangeAnimation(ANIME_NORMAL_LOOP, false, ANIME_LOOP_ON, ANIME_SLOT_NORMAL_LOOP);
  ChangeAnimation(ANIME_TIME, false, ANIME_LOOP_OFF, ANIME_SLOT_TIME);
}

void ModelSea::UpdateTimeZone(s32 timeZone)
{
  m_pBaseModel->SetAnimationFrame(timeZone, ANIME_SLOT_TIME);
}

s32 ModelSea::GetAnimationResId(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_NORMAL_LOOP:
    return BL_IDX_SEA_SEA_001_NORMAL_LOOP_GFBMOT;
  case ANIME_TIME:
    return BL_IDX_SEA_SEA_002_TIME_GFBMOT;
  default:
    return -1;
  }
}

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
