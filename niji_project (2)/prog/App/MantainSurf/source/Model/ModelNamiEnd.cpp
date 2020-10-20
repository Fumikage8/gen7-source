//======================================================================
/**
 * @file ModelNamiEnd.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief 波終了部分モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ModelNamiEnd.h"
#include <niji_conv_header/app/mantain_surf/field/wave_end.h>
#include <niji_conv_header/app/mantain_surf/field/wave.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

ModelNamiEnd::ModelNamiEnd()
{
#if 1
  m_SetupParam.m_resourceId        = GARC_mantain_field_wave_end_BIN;
  m_SetupParam.m_modelResourceId   = BL_IDX_WAVE_END_WAVE_END_CTR_GFBMDLP;
  m_SetupParam.m_animationListSize = ANIME_MAX;
#else
  m_SetupParam.m_resourceId = GARC_mantain_field_wave_BIN;
  m_SetupParam.m_modelResourceId = BL_IDX_WAVE_WAVE_CTR_GFBMDLP;
  m_SetupParam.m_animationListSize = ANIME_MAX;
#endif
}

ModelNamiEnd::~ModelNamiEnd()
{
}

s32 ModelNamiEnd::GetAnimationResId(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_NORMAL_LOOP:
    return BL_IDX_WAVE_END_WAVE_END_001_NORMAL_LOOP_GFBMOT;
  case ANIME_HEIGHT_CHANGE:
    return BL_IDX_WAVE_END_WAVE_END_002_TAKASA_LOOP_GFBMOT;
  case ANIME_TIME:
    return BL_IDX_WAVE_END_WAVE_END_003_TIME_GFBMOT;
  default:
    return -1;
  }
}

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
