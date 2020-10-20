//======================================================================
/**
 * @file GimmickSamehader.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief ベース移動型障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <niji_conv_header/app/mantain_surf/field/gmk_mover01.h>
#include "GimmickSamehader.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

const f32   GimmickSamehader::COLLIDER_RADIUS         = 105.0f;
const char* GimmickSamehader::COLLIDER_JOINT_NAME     = "gmk_mover01_p01";
const f32   GimmickSamehader::ACTION_TRIGGER_DISTANCE = 2250.0f;  // アクション開始判定のプレイヤーとの距離
const f32   GimmickSamehader::ACTION_JUMP_START_FRAME = 18.0f;    // ジャンプ開始時のフレーム

const f32   CHASE_RATE = 0.1f;    // サメハダーの追尾度

GimmickSamehader::GimmickSamehader()
{
  m_type                            = TYPE_SAMEHADER;
  m_SetupParam.m_resourceId         = GARC_mantain_field_gmk_mover01_BIN;
  m_SetupParam.m_modelResourceId    = BL_IDX_GMK_MOVER01_GMK_MOVER01_CTR_GFBMDLP;
  m_SetupParam.m_animationListSize  = ANIME_MAX;
  m_SetupParam.m_effectListSize     = EFT_MAX;
  m_SetupParam.m_colliderListSize   = COLLIDER_MAX;
  m_isSyncWithWave                  = true;
  m_stateSeq                        = 0;
  m_isChasePlayer                   = true;

  m_actionFrame = 0.0f;
}

GimmickSamehader::~GimmickSamehader()
{
}

void GimmickSamehader::CreateCollider()
{
  AddSphereCollider(COLLIDER_RADIUS, m_pBaseModel->GetJointIndex(COLLIDER_JOINT_NAME));
}

s32 GimmickSamehader::GetAnimationResId(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_WAIT:
    return BL_IDX_GMK_MOVER01_GMK_MOVER01_001_NORMAL_LOOP_GFBMOT;
  case ANIME_ACTION:
    return BL_IDX_GMK_MOVER01_GMK_MOVER01_002_ACTION_GFBMOT;
  default:
    return GimmickBaseMove::GetAnimationResId(animeId);
  }
}

s32 GimmickSamehader::GetEffectResId(s32 eftId)
{
  switch(eftId)
  {
  case EFT_RUN:
    return GARC_mantain_effect_ea_manta_run_lo_lz_BPTCL;
  case EFT_WATER_HIT1:
  case EFT_WATER_HIT2:
    return GARC_mantain_effect_ea_manta_water_hit_m_lz_BPTCL;
  case EFT_GLIDE:
    return GARC_mantain_effect_ea_manta_glide_lo_lz_BPTCL;
  default:
    return GimmickBaseMove::GetEffectResId(eftId);
  }
}

gfl2::math::Vector3 GimmickSamehader::GetEffectPosition(s32 eftId)
{
  switch(eftId)
  {
  case EFT_RUN:
  case EFT_GLIDE:
    return GetBodyPosition();
  case EFT_WATER_HIT1:
  case EFT_WATER_HIT2:
    return m_pEffectPosArray[eftId];
  default:
    return GimmickBaseMove::GetEffectPosition(eftId);
  }
}

void GimmickSamehader::OnStateWaitInitialize(State prevState)
{
  ChangeAnimation(ANIME_WAIT);
}

void GimmickSamehader::OnStateWaitExecute()
{
  ChasePlayerPosition(0.5f);
}

bool GimmickSamehader::CheckActionTrigger()
{
  if(!IsVisible())
  {
    // 不可視の場合は動き出さない
    return false;
  }
  if(!m_pPlayerController->IsSurfingState())
  {
    // サーフ状態ではない場合は動き出さない
    return false;
  }
  if((GetPlayerPosition() - GetPosition()).Length() <= ACTION_TRIGGER_DISTANCE + ACTION_TRIGGER_DISTANCE * 1.25f * GetPlayerSpeedRate())
  {
    return true;
  }
  return false;
}

void GimmickSamehader::OnStateActionInitialize(State prevState)
{
  ChangeAnimation(ANIME_ACTION);
  StartEffect(EFT_RUN);
  m_stateSeq = 0;
  m_isChasePlayer = true;
  m_actionFrame = 0.0f;
}

void GimmickSamehader::OnStateActionExecute()
{
  m_actionFrame++;
  if(GetPlayerCollidedModel() == this)
  {
    // プレイヤーが自身と衝突したら追跡しないように
    m_isChasePlayer = false;
    // 当たり判定を一旦無効に
    SetColliderEnabled(false);
  }

  if(m_isChasePlayer)
  {
    // プレイヤーを追跡
    ChasePlayerPosition(CHASE_RATE + CHASE_RATE * GetPlayerSpeedRate());
  }

  gfl2::math::Vector3 realPos = GetBodyPosition();
  Nami::NamiController::NamiPointInfo point = m_pNamiController->GetNamiPointInfo(realPos);
  switch(m_stateSeq)
  {
  case 0:
    // 傾きを更新
    ChangeInclination(point.m_normal, 1.0f);
    if(m_actionFrame >= ACTION_JUMP_START_FRAME && realPos.y > point.m_point.y)
    {
      // SE再生
      SurfSound::SurfSoundManager::PlaySE(SurfSound::SurfSoundManager::SE_GIMMICK_ACTION_1);
      UpdateSEPlayEnv(SurfSound::SurfSoundManager::SE_GIMMICK_ACTION_1, GetBodyPosition());
      // ジャンプ
      EndEffect(EFT_RUN);
      StartEffect(EFT_WATER_HIT1);
      StartEffect(EFT_GLIDE);
      SetEffectPosition(EFT_WATER_HIT1, point.m_point);
      m_stateSeq++;
    }
    break;

  case 1:
    if(realPos.y <= point.m_point.y)
    {
      // 着水
      SurfSound::SurfSoundManager::PlaySE(SurfSound::SurfSoundManager::SE_GIMMICK_ACTION_3);
      UpdateSEPlayEnv(SurfSound::SurfSoundManager::SE_GIMMICK_ACTION_3, GetBodyPosition());
      StartEffect(EFT_WATER_HIT2);
      SetEffectPosition(EFT_WATER_HIT2, point.m_point);
      m_stateSeq++;
    }
    break;
  }
  // アニメーション終了のチェック
  if(IsAnimationEnd())
  {
    EndEffect(EFT_GLIDE);
    SetVisible(false);
    // @fix GFMMCat[150] 完全着水後に描画状態が変更されないように非活動状態に変更する
    m_stateNow = ST_INACTIVE;
  }
}

void GimmickSamehader::ChasePlayerPosition(f32 rate)
{
  gfl2::math::Vector3 direction = GetPlayerPosition() - GetPosition();
  direction.y = 0.0f;
  if(direction.Length() > 0.0f)
  {
    ChangeLookDirection(direction.Normalize(), rate);
  }
}

gfl2::math::Vector3 GimmickSamehader::GetBodyPosition()
{
  return GetJointPosition(COLLIDER_JOINT_NAME);
}

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
