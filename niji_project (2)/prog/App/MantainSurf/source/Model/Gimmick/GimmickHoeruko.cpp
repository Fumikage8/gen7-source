//======================================================================
/**
 * @file GimmickHoeruko.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief ホエルコ障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <niji_conv_header/app/mantain_surf/field/gmk_pm02.h>
#include "GimmickHoeruko.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

const f32   GimmickHoeruko::COLLIDER_RADIUS       = 147.0f;
const f32   GimmickHoeruko::COLLIDER_HEIGHT       = 360.0f;
const char* GimmickHoeruko::COLLIDER_JOINT_NAME   = "gmk_pm02_hamon";

const f32   DELTA_TIME = 1.0f / 30.0f;

GimmickHoeruko::GimmickHoeruko()
{
  m_type = TYPE_HOERUKO;
  m_SetupParam.m_resourceId         = GARC_mantain_field_gmk_pm02_BIN;
  m_SetupParam.m_modelResourceId    = BL_IDX_GMK_PM02_GMK_PM02_CTR_GFBMDLP;
  m_SetupParam.m_animationListSize  = ANIME_MAX;
  m_SetupParam.m_effectListSize     = EFT_MAX;
  m_SetupParam.m_colliderListSize   = COLLIDER_MAX;
  m_isSyncWithWave                  = true;
  m_isRandomDirection               = true;
}

GimmickHoeruko::~GimmickHoeruko()
{
}

void GimmickHoeruko::CreateCollider()
{
  //AddCylinderCollider(COLLIDER_RADIUS, COLLIDER_HEIGHT, m_pBaseModel->GetJointIndex(COLLIDER_JOINT_NAME));
  AddSphereCollider(COLLIDER_RADIUS, m_pBaseModel->GetJointIndex(COLLIDER_JOINT_NAME));
}

void GimmickHoeruko::UpdateTimeZone(s32 timeZone)
{
  m_pBaseModel->SetAnimationFrame(timeZone, ANIME_SLOT_TIME);
}

s32 GimmickHoeruko::GetAnimationResId(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_SHADOW_LOOP:
    return BL_IDX_GMK_PM02_GMK_PM02_001_SHADOW_LOOP_GFBMOT;
  case ANIME_PM_ACTION:
    return BL_IDX_GMK_PM02_GMK_PM02_002_ACTION_GFBMOT;
  case ANIME_PM_WAIT:
    return BL_IDX_GMK_PM02_GMK_PM02_003_NORMAL_LOOP_GFBMOT;
  case ANIME_TIME:
    return BL_IDX_GMK_PM02_GMK_PM02_004_TIME_GFBMOT;
  default:
    return GimmickBaseStatic::GetAnimationResId(animeId);
  }
}

s32 GimmickHoeruko::GetEffectResId(s32 eftId)
{
  switch(eftId)
  {
  case EFT_WATER_HIT1:
    return GARC_mantain_effect_ea_manta_water_hit_m_lz_BPTCL;
  default:
    return GimmickBaseStatic::GetEffectResId(eftId);
  }
}

void GimmickHoeruko::OnStateWaitInitialize(State prevState)
{
  ChangeAnimation(ANIME_PM_ACTION, false, ANIME_LOOP_DEFAULT, ANIME_SLOT_DEFAULT);
  ChangeAnimation(ANIME_SHADOW_LOOP, false, ANIME_LOOP_ON, ANIME_SLOT_SHADOW);
  ChangeAnimation(ANIME_TIME, false, ANIME_LOOP_OFF, ANIME_SLOT_TIME);
}

void GimmickHoeruko::OnStateWaitExecute()
{
  SetAnimationProgress(0.0f, ANIME_SLOT_DEFAULT);
}

bool GimmickHoeruko::CheckActionTrigger()
{
  if(GetPlayerCollidedModel() == this)
  {
    // プレイヤーが自身と衝突
    return true;
  }
  return false;
}

void GimmickHoeruko::OnStateActionInitialize(State prevState)
{
  // SE再生
  SurfSound::SurfSoundManager::PlaySE(SurfSound::SurfSoundManager::SE_GIMMICK_ACTION_1);
  UpdateSEPlayEnv(SurfSound::SurfSoundManager::SE_GIMMICK_ACTION_1, GetPosition());
  // エフェクト再生
  StartEffect(EFT_WATER_HIT1);
}

void GimmickHoeruko::OnStateActionExecute()
{
  // アニメーション終了のチェック
  if(IsAnimationEnd(ANIME_SLOT_DEFAULT))
  {
    ChangeAnimation(ANIME_PM_WAIT, false, ANIME_LOOP_ON, ANIME_SLOT_DEFAULT);
    m_stateNow = ST_NONE;
  }
}

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
