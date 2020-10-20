//======================================================================
/**
 * @file GimmickReefMedium.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief 岩礁(中)障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <niji_conv_header/app/mantain_surf/field/gmk_obj02.h>
#include "GimmickReefMedium.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

const f32   GimmickReefMedium::COLLIDER_RADIUS      = 125.0f;
const f32   GimmickReefMedium::COLLIDER_HEIGHT      = 270.0f;
const char* GimmickReefMedium::COLLIDER_JOINT_NAME  = "gmk_obj02_rock01";

GimmickReefMedium::GimmickReefMedium()
{
  m_type                            = TYPE_REEF_M;
  m_SetupParam.m_resourceId         = GARC_mantain_field_gmk_obj02_BIN;
  m_SetupParam.m_modelResourceId    = BL_IDX_GMK_OBJ02_GMK_OBJ02_CTR_GFBMDLP;
  m_SetupParam.m_animationListSize  = ANIME_MAX;
  m_SetupParam.m_colliderListSize   = COLLIDER_MAX;
  m_isSyncWithWave                  = false;
  m_isRandomDirection               = true;
}

GimmickReefMedium::~GimmickReefMedium()
{
}

void GimmickReefMedium::CreateCollider()
{
  //AddCylinderCollider(COLLIDER_RADIUS, COLLIDER_HEIGHT, m_pBaseModel->GetJointIndex(COLLIDER_JOINT_NAME));
  AddSphereCollider(COLLIDER_RADIUS, m_pBaseModel->GetJointIndex(COLLIDER_JOINT_NAME));
}

s32 GimmickReefMedium::GetAnimationResId(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_WAVE_LOOP:
    return BL_IDX_GMK_OBJ02_GMK_OBJ02_001_WAVE_LOOP_GFBMOT;
  case ANIME_PM_WAIT:
    return BL_IDX_GMK_OBJ02_GMK_OBJ02_002_NORMAL_LOOP_GFBMOT;
  case ANIME_PM_ACTION:
    return BL_IDX_GMK_OBJ02_GMK_OBJ02_003_ACTION_GFBMOT;
  default:
    return GimmickBaseStatic::GetAnimationResId(animeId);
  }
}

void GimmickReefMedium::OnStateWaitInitialize(State prevState)
{
  ChangeAnimation(ANIME_WAVE_LOOP, false, ANIME_LOOP_ON, 1);
  ChangeAnimation(ANIME_PM_WAIT);
}

bool GimmickReefMedium::CheckActionTrigger()
{
  if(GetPlayerCollidedModel() == this)
  {
    // プレイヤーが自身と衝突
    return true;
  }
  return false;
}

void GimmickReefMedium::OnStateActionInitialize(State prevState)
{
  ChangeAnimation(ANIME_PM_ACTION);
}

void GimmickReefMedium::OnStateActionExecute()
{
  // アニメーション終了のチェック
  if(IsAnimationEnd())
  {
    m_stateNext = ST_WAIT;
  }
}

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
