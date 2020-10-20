//======================================================================
/**
 * @file GimmickHoeruoh.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief ホエルオー障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <niji_conv_header/app/mantain_surf/field/gmk_mover02.h>
#include "GimmickHoeruoh.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

const f32   GimmickHoeruoh::COLLIDER_RADIUS[COLLIDER_MAX]      = { 317.0f, 355.0f, 274.0f };  // 調整値
const char* GimmickHoeruoh::COLLIDER_JOINT_NAMES[COLLIDER_MAX] = { "gmk_mover02_p01" /* 胴体 */, "gmk_mover02_p02" /* 頭 */, "gmk_mover02_p03" /* 尻 */ };
const f32   GimmickHoeruoh::ACTION_TRIGGER_DISTANCE            = 3500.0f;
const f32   GimmickHoeruoh::BODY_LENGTH                        = 1350.0f; // ホエルオーの身長
const f32   GimmickHoeruoh::ACTION_EFT_START_FRAME             = 22.0f;   // 出現エフェクト再生開始のフレーム数
const f32   GimmickHoeruoh::ACTION_EFT_START_DURATION          = 30.0f;   // 出現エフェクト再生開始所要フレーム数
const f32   GimmickHoeruoh::ACTION_EFT_END_FRAME               = 124.0f;  // 出現エフェクト再生終了のフレーム数
const f32   GimmickHoeruoh::ACTION_EFT_END_DURATION            = 30.0f;   // 出現エフェクト再生終了所要フレーム数
const f32   GimmickHoeruoh::ACTION_EFT_MAX_SCALE               = 2.0f;    // 出現エフェクトの最大スケール

GimmickHoeruoh* GimmickHoeruoh::m_pLastestActiveHoeruoh = NULL;

GimmickHoeruoh::GimmickHoeruoh()
{
  m_type                            = TYPE_HOERUOH;
  m_SetupParam.m_resourceId         = GARC_mantain_field_gmk_mover02_BIN;
  m_SetupParam.m_modelResourceId    = BL_IDX_GMK_MOVER02_GMK_MOVER02_CTR_GFBMDLP;
  m_SetupParam.m_animationListSize  = ANIME_MAX;
  m_SetupParam.m_effectListSize     = EFT_MAX;
  m_SetupParam.m_colliderListSize   = COLLIDER_MAX;

  m_actionFrame = 0.0f;
}

GimmickHoeruoh::~GimmickHoeruoh()
{
}

void GimmickHoeruoh::CreateCollider()
{
  for(u32 i = 0; i < COLLIDER_MAX; i++)
  {
    AddSphereCollider(COLLIDER_RADIUS[i], m_pBaseModel->GetJointIndex(COLLIDER_JOINT_NAMES[i]));
  }
}

void GimmickHoeruoh::UpdateTimeZone(s32 timeZone)
{
  m_pBaseModel->SetAnimationFrame(timeZone, ANIME_SLOT_TIME);
}

s32 GimmickHoeruoh::GetAnimationResId(s32 animeId)
{
  switch(animeId)
  {
  case ANIME_SHADOW:
    return BL_IDX_GMK_MOVER02_GMK_MOVER02_001_SHADOW_LOOP_GFBMOT;
  case ANIME_WAIT:
    return BL_IDX_GMK_MOVER02_GMK_MOVER02_002_NORMAL_LOOP_GFBMOT;
  case ANIME_ACTION:
    return BL_IDX_GMK_MOVER02_GMK_MOVER02_003_ACTION_GFBMOT;
  case ANIME_TIME:
    return BL_IDX_GMK_MOVER02_GMK_MOVER02_004_TIME_GFBMOT;
  default:
    return GimmickBaseMove::GetAnimationResId(animeId);
  }
}

s32 GimmickHoeruoh::GetEffectResId(s32 eftId)
{
  switch(eftId)
  {
  case EFT_WATER:
    return GARC_mantain_effect_ea_manta_water_whale_lo_lz_BPTCL;
  default:
    return GimmickBaseMove::GetEffectResId(eftId);
  }
}

gfl2::math::Vector3 GimmickHoeruoh::GetEffectPosition(s32 eftId)
{
  switch(eftId)
  {
  case EFT_WATER:
  default:
    return GimmickBaseMove::GetEffectPosition(eftId);
  }
}

gfl2::math::Vector3 GimmickHoeruoh::GetEffectOffset(s32 eftId)
{
  switch(eftId)
  {
  case EFT_WATER:
    return gfl2::math::Vector3(-1335.0f, 220.0f, 0.0f);
  default:
    return GimmickBaseMove::GetEffectOffset(eftId);
  }
}

gfl2::math::Quaternion GimmickHoeruoh::GetEffectRotation(s32 eftId)
{
  switch(eftId)
  {
  case EFT_WATER:
    return gfl2::math::Quaternion(0.0f, 0.0f, -0.58f, 1.0f);
  default:
    return GimmickBaseMove::GetEffectRotation(eftId);
  }
}

void GimmickHoeruoh::OnStateWaitInitialize(State prevState)
{
  ChangeAnimation(ANIME_WAIT, false, ANIME_LOOP_ON, ANIME_SLOT_DEFAULT);
  ChangeAnimation(ANIME_SHADOW, true, ANIME_LOOP_ON, ANIME_SLOT_SHADOW);
  ChangeAnimation(ANIME_TIME, false, ANIME_LOOP_OFF, ANIME_SLOT_TIME);
}

void GimmickHoeruoh::OnStateWaitExecute()
{

}

bool GimmickHoeruoh::CheckActionTrigger()
{
  if(!IsVisible())
  {
    return false;
  }
  if(!m_pPlayerController->IsSurfingState())
  {
    // サーフ状態ではない場合は動き出さない
    return false;
  }
  if((GetPlayerPosition() - GetPosition()).Length() <= ACTION_TRIGGER_DISTANCE + ACTION_TRIGGER_DISTANCE * 2.0f * GetPlayerSpeedRate())
  {
    return true;
  }
  return false;
}

void GimmickHoeruoh::OnStateActionInitialize(State prevState)
{
  // SE再生
  PlayActionSE();
  ChangeAnimation(ANIME_ACTION, false, ANIME_LOOP_OFF, ANIME_SLOT_DEFAULT);
  StartEffect(EFT_WATER);
}

void GimmickHoeruoh::OnStateActionExecute()
{
  // フレーム更新
  m_actionFrame = GetAnimationFrame();
  gfl2::math::Vector3 scale = gfl2::math::Vector3(1.0f, 1.0f, 1.0f) * ACTION_EFT_MAX_SCALE;
  if(m_actionFrame < ACTION_EFT_START_FRAME)
  {
    // 開始段階(scale 0 → 0)
    scale *= 0.0f;
  }
  else if(m_actionFrame >= ACTION_EFT_START_FRAME && m_actionFrame < ACTION_EFT_END_FRAME)
  {
    // 出現段階(scale 0 → max)
    f32 t = (m_actionFrame - ACTION_EFT_START_FRAME) / ACTION_EFT_START_DURATION;
    scale *= gfl2::math::Clamp(t, 0.0f, 1.0f);
  }
  else if(m_actionFrame >= ACTION_EFT_END_FRAME)
  {
    // 終了段階(scale max → 0)
    f32 t = 1.0f - (m_actionFrame - ACTION_EFT_END_FRAME) / ACTION_EFT_END_DURATION;
    scale *= gfl2::math::Clamp(t, 0.0f, 1.0f);
  }
  SetEffectScale(EFT_WATER, scale);

  // SE再生環境を更新
  if(m_pLastestActiveHoeruoh != NULL)
  {
    // @fix GFMMCat[151] あとから出たホエルオーのポジションによりSE再環境を更新する
    UpdateSEPlayEnv(SurfSound::SurfSoundManager::SE_GIMMICK_ACTION_2, m_pLastestActiveHoeruoh->GetBodyPosition());
  }

#if PM_DEBUG
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(pButton->IsHold(gfl2::ui::BUTTON_ZR))
  {
    // アニメーション停止
    if(m_actionFrame > 0.0f)
    {
      m_pBaseModel->SetAnimationFrame(m_actionFrame - 1.0f);
    }
  }
#endif

  if(IsAnimationEnd(ANIME_SLOT_DEFAULT))
  {
    // SE停止
    StopActionSE();
    EndEffect(EFT_WATER);
    SetVisible(false);
    // @fix GFMMCat[150] 完全着水後に描画状態が変更されないように非活動状態に変更する
    m_stateNow = ST_INACTIVE;
  }
}

void GimmickHoeruoh::OnSetDead()
{
  // SE停止
  StopActionSE();
  // @fix GFMMCat[161] アニメーションをリセットするために一旦待機モーションに戻す
  ChangeAnimation(ANIME_WAIT, false, ANIME_LOOP_ON, ANIME_SLOT_DEFAULT);
  // Dead状態ではメインのUpdateが回さないため、アニメーションを反映するために1フレームを更新しておく
  UpdateAnimation();
}

gfl2::math::Vector3 GimmickHoeruoh::GetBodyPosition()
{
  return GetJointPosition(COLLIDER_JOINT_NAMES[0]);
}

gfl2::math::Vector3 GimmickHoeruoh::GetHeadPosition()
{
  return GetJointPosition(COLLIDER_JOINT_NAMES[1]);
}

gfl2::math::Vector3 GimmickHoeruoh::GetHipPosition()
{
  return GetJointPosition(COLLIDER_JOINT_NAMES[2]);
}

f32 GimmickHoeruoh::GetBodyRadius()
{
  return COLLIDER_RADIUS[1];
}

void GimmickHoeruoh::PlayActionSE()
{
  SurfSound::SurfSoundManager::PlaySE(SurfSound::SurfSoundManager::SE_GIMMICK_ACTION_2);
  m_pLastestActiveHoeruoh = this;
}

void GimmickHoeruoh::StopActionSE()
{
  if(m_pLastestActiveHoeruoh == this)
  {
    // @fix GFMMCat[151] 一番最近に出ているホエルオーが自分自身ならSEを停止する
    // そうでなければ自分よりあとから出ているホエルオーが存在するため、SEを停止しないように
    SurfSound::SurfSoundManager::StopSE(SurfSound::SurfSoundManager::SE_GIMMICK_ACTION_2);
  }
}

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
