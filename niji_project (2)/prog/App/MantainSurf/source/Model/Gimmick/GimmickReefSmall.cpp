//======================================================================
/**
 * @file GimmickReefSmall.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief 岩礁(小)障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <niji_conv_header/app/mantain_surf/field/gmk_obj01.h>
#include "GimmickReefSmall.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

const f32 GimmickReefSmall::COLLIDER_RADIUS = 72.5f;
const f32 GimmickReefSmall::COLLIDER_HEIGHT = 168.0f;

GimmickReefSmall::GimmickReefSmall()
{
  m_type                            = TYPE_REEF_S;
  m_SetupParam.m_resourceId         = GARC_mantain_field_gmk_obj01_BIN;
  m_SetupParam.m_modelResourceId    = BL_IDX_GMK_OBJ01_GMK_OBJ01_CTR_GFBMDLP;
  m_SetupParam.m_animationListSize  = ANIME_MAX;
  m_SetupParam.m_colliderListSize   = COLLIDER_MAX;
  m_isSyncWithWave                  = false;
  m_isRandomDirection               = true;
}

GimmickReefSmall::~GimmickReefSmall()
{
}


void GimmickReefSmall::CreateCollider()
{
  //AddCylinderCollider(COLLIDER_RADIUS, COLLIDER_HEIGHT);
  AddSphereCollider(COLLIDER_RADIUS);
}

s32 GimmickReefSmall::GetAnimationResId(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_WAVE_LOOP:
    return BL_IDX_GMK_OBJ01_GMK_OBJ01_001_NORMAL_LOOP_GFBMOT;
  default:
    return GimmickBaseStatic::GetAnimationResId(animeId);
  }
}

void GimmickReefSmall::OnStateWaitInitialize(State prevState)
{
  ChangeAnimation(ANIME_WAVE_LOOP);
}

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
