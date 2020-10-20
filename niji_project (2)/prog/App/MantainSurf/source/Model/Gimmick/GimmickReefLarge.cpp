//======================================================================
/**
 * @file GimmickReefLarge.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief 岩礁(大)障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <niji_conv_header/app/mantain_surf/field/gmk_obj03.h>
#include "GimmickReefLarge.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

const f32   GimmickReefLarge::COLLIDER_RADIUS       = 142.5;
const f32   GimmickReefLarge::COLLIDER_HEIGHT       = 280.0f;
const char* GimmickReefLarge::COLLIDER_JOINT_NAME   = "gmk_obj03_rock01";

GimmickReefLarge::GimmickReefLarge()
{
  m_type                            = TYPE_REEF_L;
  m_SetupParam.m_resourceId         = GARC_mantain_field_gmk_obj03_BIN;
  m_SetupParam.m_modelResourceId    = BL_IDX_GMK_OBJ03_GMK_OBJ03_CTR_GFBMDLP;
  m_SetupParam.m_animationListSize  = ANIME_MAX;
  m_SetupParam.m_colliderListSize   = COLLIDER_MAX;
  m_isSyncWithWave                  = false;
  m_isRandomDirection               = true;
}

GimmickReefLarge::~GimmickReefLarge()
{
}

void GimmickReefLarge::CreateCollider()
{
  //AddCylinderCollider(COLLIDER_RADIUS, COLLIDER_HEIGHT, m_pBaseModel->GetJointIndex(COLLIDER_JOINT_NAME));
  AddSphereCollider(COLLIDER_RADIUS, m_pBaseModel->GetJointIndex(COLLIDER_JOINT_NAME));
}

s32 GimmickReefLarge::GetAnimationResId(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_WAVE_LOOP:
    return BL_IDX_GMK_OBJ03_GMK_OBJ03_001_WAVE_LOOP_GFBMOT;
  case ANIME_PM_WAIT:
    return BL_IDX_GMK_OBJ03_GMK_OBJ03_002_NORMAL_LOOP_GFBMOT;
  case ANIME_PM_ACTION:
    return BL_IDX_GMK_OBJ03_GMK_OBJ03_003_ACTION_GFBMOT;
  default:
    return GimmickBaseStatic::GetAnimationResId(animeId);
  }
}

void GimmickReefLarge::OnStateWaitInitialize(State prevState)
{
  ChangeAnimation(ANIME_WAVE_LOOP, false, ANIME_LOOP_ON, 1);
  ChangeAnimation(ANIME_PM_WAIT);
}

bool GimmickReefLarge::CheckActionTrigger()
{
  if(GetPlayerCollidedModel() == this)
  {
    // プレイヤーが自身と衝突
    return true;
  }
  return false;
}

void GimmickReefLarge::OnStateActionInitialize(State prevState)
{
  ChangeAnimation(ANIME_PM_ACTION);
}

void GimmickReefLarge::OnStateActionExecute()
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
