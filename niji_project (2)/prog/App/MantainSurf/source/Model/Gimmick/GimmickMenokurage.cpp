//======================================================================
/**
 * @file GimmickMenokurage.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief メノクラゲ障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <niji_conv_header/app/mantain_surf/field/gmk_pm01.h>
#include "GimmickMenokurage.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

const f32   GimmickMenokurage::COLLIDER_RADIUS      = 21.0f;
const f32   GimmickMenokurage::COLLIDER_HEIGHT      = 125.0f;
const char* GimmickMenokurage::COLLIDER_JOINT_NAME  = "gmk_pm01_p01";
const f32   GimmickMenokurage::MOVE_SPEED_MIN       = 500.0f;   // プレイヤーと衝突した時の最小移動速度
const f32   GimmickMenokurage::MOVE_SPEED_MAX       = 1000.0f;  // プレイヤーと衝突した時の最大移動速度

const f32   DELTA_TIME = 1.0f / 30.0f;

GimmickMenokurage::GimmickMenokurage()
{
  m_type                            = TYPE_MENOKURAGE;
  m_SetupParam.m_resourceId         = GARC_mantain_field_gmk_pm01_BIN;
  m_SetupParam.m_modelResourceId    = BL_IDX_GMK_PM01_GMK_PM01_CTR_GFBMDLP;
  m_SetupParam.m_animationListSize  = ANIME_MAX;
  m_SetupParam.m_effectListSize     = EFT_MAX;
  m_SetupParam.m_colliderListSize   = COLLIDER_MAX;
  m_isSyncWithWave                  = true;
  m_isRandomDirection               = true;
  m_stateSeq                        = 0;
}

GimmickMenokurage::~GimmickMenokurage()
{
}

void GimmickMenokurage::CreateCollider()
{
  //AddCylinderCollider(COLLIDER_RADIUS, COLLIDER_HEIGHT, m_pBaseModel->GetJointIndex(COLLIDER_JOINT_NAME));
  AddSphereCollider(COLLIDER_RADIUS, m_pBaseModel->GetJointIndex(COLLIDER_JOINT_NAME));
}

s32 GimmickMenokurage::GetAnimationResId(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_WAVE_LOOP:
    return BL_IDX_GMK_PM01_GMK_PM01_001_WAVE_LOOP_GFBMOT;
  case ANIME_PM_WAIT:
    return BL_IDX_GMK_PM01_GMK_PM01_002_NORMAL_LOOP_GFBMOT;
  case ANIME_PM_ACTION:
    return BL_IDX_GMK_PM01_GMK_PM01_003_ACTION_GFBMOT;
  default:
    return GimmickBaseStatic::GetAnimationResId(animeId);
  }
}

gfl2::math::Vector3 GimmickMenokurage::GetEffectPosition(s32 eftId)
{
  switch(eftId)
  {
  case EFT_WATER_HIT1:
  case EFT_WATER_HIT2:
    return m_pEffectPosArray[eftId];
  default:
    return GimmickBaseStatic::GetEffectPosition(eftId);
  }
}

s32 GimmickMenokurage::GetEffectResId(s32 eftId)
{
  switch(eftId)
  {
  case EFT_WATER_HIT1:
  case EFT_WATER_HIT2:
    return GARC_mantain_effect_ea_manta_water_hit_m_lz_BPTCL;
  default:
    return GimmickBaseStatic::GetEffectResId(eftId);
  }
}

void GimmickMenokurage::OnStateWaitInitialize(State prevState)
{
  ChangeAnimation(ANIME_WAVE_LOOP, false, ANIME_LOOP_ON, 1);
  ChangeAnimation(ANIME_PM_WAIT);
}

void GimmickMenokurage::OnStateWaitExecute()
{
}

bool GimmickMenokurage::CheckActionTrigger()
{
  if(GetPlayerCollidedModel() == this)
  {
    // プレイヤーが自身と衝突
    // 当たり判定を一旦無効に
    SetColliderEnabled(false);
    return true;
  }
  return false;
}

void GimmickMenokurage::OnStateActionInitialize(State prevState)
{
  // SE再生
  gfl2::math::Vector3 bodyPos = GetBodyPosition();
  SurfSound::SurfSoundManager::PlaySE(SurfSound::SurfSoundManager::SE_GIMMICK_ACTION_1);
  UpdateSEPlayEnv(SurfSound::SurfSoundManager::SE_GIMMICK_ACTION_1, bodyPos);
  ChangeAnimation(ANIME_PM_ACTION);
  StartEffect(EFT_WATER_HIT1);
  SetEffectPosition(EFT_WATER_HIT1, m_pNamiController->GetNamiPointInfo(bodyPos).m_point);
  m_ActionParam.m_moveDir   = -m_pPlayerController->GetPlayerModelSet()->GetMainCollisionInfo()->m_normal;
  m_ActionParam.m_moveSpeed = gfl2::math::Lerp(MOVE_SPEED_MIN, MOVE_SPEED_MAX, GetPlayerSpeedRate());
  m_stateSeq = 0;
}

void GimmickMenokurage::OnStateActionExecute()
{
  // 位置を移動する
  f32 speed = m_ActionParam.m_moveSpeed * (1.0f - GetAnimationProgress()) * DELTA_TIME;
  gfl2::math::Vector3 newPos = GetPosition() + m_ActionParam.m_moveDir * speed;

  // 波上の位置調整
  Nami::NamiController::NamiPointInfo point = m_pNamiController->GetNamiPointInfo(newPos);
  newPos = point.m_point;
  SetPosition(newPos);
  ChangeInclination(point.m_normal, 1.0f);

  // 着水エフェクトを更新
  gfl2::math::Vector3 realPos = GetBodyPosition();
  switch(m_stateSeq)
  {
  case 0:
    if(GetAnimationProgress() > 0.5f && realPos.y <= point.m_point.y)
    {
      // 着水
      SurfSound::SurfSoundManager::PlaySE(SurfSound::SurfSoundManager::SE_GIMMICK_ACTION_3);
      UpdateSEPlayEnv(SurfSound::SurfSoundManager::SE_GIMMICK_ACTION_3, GetBodyPosition());
      StartEffect(EFT_WATER_HIT2);
      SetEffectPosition(EFT_WATER_HIT2, m_pNamiController->GetNamiPointInfo(realPos).m_point);
      m_stateSeq++;
    }
    break;
  }

  // アニメーション終了のチェック
  if(IsAnimationEnd())
  {
    SetVisible(false);
    // @fix GFMMCat[150] 完全着水後に描画状態が変更されないように非活動状態に変更する
    m_stateNow = ST_INACTIVE;
  }
}

gfl2::math::Vector3 GimmickMenokurage::GetBodyPosition()
{
  return GetJointPosition(COLLIDER_JOINT_NAME);
}

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
