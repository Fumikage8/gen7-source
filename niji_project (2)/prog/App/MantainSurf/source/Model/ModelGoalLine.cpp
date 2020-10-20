//======================================================================
/**
 * @file ModelGoalLine.cpp
 * @date 2017/03/07
 * @author fang_yicheng
 * @brief ゴールラインモデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "ModelGoalLine.h"
#include <niji_conv_header/app/mantain_surf/field/end_line.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

ModelGoalLine::ModelGoalLine()
{
  m_SetupParam.m_resourceId        = GARC_mantain_field_end_line_BIN;
  m_SetupParam.m_modelResourceId   = BL_IDX_END_LINE_END_LINE_CTR_GFBMDLP;
  m_SetupParam.m_animationListSize = ANIME_MAX;
}

ModelGoalLine::~ModelGoalLine()
{
}

s32 ModelGoalLine::GetAnimationResId(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_NORMAL_LOOP:
    return BL_IDX_END_LINE_END_LINE_001_NORMAL_LOOP_GFBMOT;
  case ANIME_FADE_IN:
    return BL_IDX_END_LINE_END_LINE_002_FADEIN_GFBMOT;
  default:
    return -1;
  }
}

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
