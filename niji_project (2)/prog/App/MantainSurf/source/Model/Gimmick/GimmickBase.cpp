//======================================================================
/**
 * @file GimmickBase.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief ベース障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "GimmickBase.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

const f32 APPEAR_TIME = 8.0f;   // 障害物出現処理所要時間(フレーム)
const gfl2::math::Vector3 INIT_POSITION = gfl2::math::Vector3(0.0f, 0.0f, -50000.0f);

GimmickBase::GimmickBase()
{
  m_type = TYPE_NONE;
  m_pNamiController = NULL;
  m_pPlayerController = NULL;
  m_pCamera = NULL;
  m_stateNow = ST_NONE;
  m_stateNext = ST_NONE;
  m_isSyncWithWave = false;
  m_isRandomDirection = false;
  m_stateSeq = 0;
  m_initPosition = INIT_POSITION;
}

GimmickBase::~GimmickBase()
{
}

void GimmickBase::Update()
{
  if(!IsDead())
  {
    // 処理負荷軽減対策として障害物が活動中の場合のみUpdateを回すように
    ModelBaseChara::Update();
    UpdateState();
    UpdateDrawableState();
  }
}

GimmickBase::Type GimmickBase::GetType()
{
  return m_type;
}

void GimmickBase::Reset(const gfl2::math::Vector3 &position)
{
  m_initPosition = position;
  m_stateNow = ST_NONE;
  m_stateNext = ST_APPEAR;
  m_drawableState.Initialize();
  EndAnimation();
  EndAllEffect();
  SetVisible(true);
  SetEffectVisible(true);
  SetColliderEnabled(true);

  // 波との同期
  gfl2::math::Vector3 pos = position;
  if(m_isSyncWithWave)
  {
    Nami::NamiController::NamiPointInfo point = m_pNamiController->GetNamiPointInfo(position);
    pos = point.m_point;
    ChangeInclination(point.m_normal, 1.0f);
  }

  // 向きを設定
  f32 angle = m_isRandomDirection ? PI * MakeRandRate() : PI;
  ChangeRotationY(angle, 1.0f);

  // 位置を配置する
  SetPosition(pos);
}

void GimmickBase::Reset()
{
  Reset(m_initPosition);
}

void GimmickBase::SetDrawEnable(bool isDrawEnable, u32 delay)
{
  if(IsVisible() == isDrawEnable)
  {
    return;
  }

  if(m_stateNow == ST_INACTIVE)
  {
    // @fix GFMMCat[150] 障害物が非活動状態の場合は描画状態を変更する必要がない
    return;
  }

  if(delay == 0)
  {
    // 即変更
    m_drawableState.Initialize();
    SetVisible(isDrawEnable);
    SetEffectVisible(isDrawEnable);
  }
  else
  {
    // 遅延変更
    s32 stateNew = isDrawEnable ? 1 : 0;
    m_drawableState.isStateWillChange = true;
    if(m_drawableState.stateNext != stateNew)
    {
      m_drawableState.stateNext = stateNew;
      m_drawableState.delayTimer = delay;
    }
  }
}

void GimmickBase::SetDead()
{
  if(m_type == TYPE_SAMEHADER && IsActionState())
  {
    // サメハダーアクション中は無視
    return;
  }
  if(m_stateNow != ST_DEAD)
  {
    m_stateNow = ST_DEAD;
    m_initPosition = INIT_POSITION;
    SetVisible(false);
    SetEffectVisible(false);
    EndAllEffect();
    OnSetDead();
  }
}

bool GimmickBase::IsDead()
{
  return m_stateNow == ST_DEAD;
}

void GimmickBase::SetState(const State state)
{
  m_stateNext = state;
}

bool GimmickBase::IsActionState()
{
  return m_stateNow == ST_ACTION;
}

bool GimmickBase::IsTurnOver()
{
  switch(GetType())
  {
  case TYPE_HOERUOH:
    return true;
  default:
    return false;
  }
}

// ペナルティ係数を取得
f32 GimmickBase::GetPenaltyRatio()
{
  switch(GetType())
  {
  case TYPE_COMMON:
  case TYPE_REEF_S:
  case TYPE_MENOKURAGE:
    return 0.8f;
  case TYPE_REEF_M:
  case TYPE_HOERUKO:
    return 1.0f;
  default:
    return 1.2f;
  }
}

void GimmickBase::SetController(Nami::NamiController* pNamiController, Player::PlayerController* pPlayerController)
{
  m_pNamiController = pNamiController;
  m_pPlayerController = pPlayerController;
}

void GimmickBase::SetCamera(Camera::Camera* pCamera)
{
  m_pCamera = pCamera;
}

gfl2::math::Vector3 GimmickBase::GetPlayerPosition()
{
  return m_pPlayerController->GetPlayerModelSet()->GetPosition();
}

f32 GimmickBase::GetPlayerSpeedRate()
{
  return m_pPlayerController->GetSpeedRate();
}

ModelBase* GimmickBase::GetPlayerCollidedModel()
{
  return m_pPlayerController->GetPlayerModelSet()->GetMainCollisionInfo()->m_pTargetModel;
}

void GimmickBase::UpdateSEPlayEnv(SurfSound::SurfSoundManager::SEIndex seIndex, const gfl2::math::Vector3 &position)
{
  // SE音量を更新
  {
    f32 distance = (m_pCamera->GetPosition() - position).Length();
    f32 volume = 1.0f - gfl2::math::Clamp(distance / m_pCamera->GetSoundAudibleMaxDistance(), 0.0f, 1.0f);
    SurfSound::SurfSoundManager::ChangeSEVolume(seIndex, volume);
  }

  // SEパンを更新
  {
    f32 pan = m_pCamera->JudgeLeftRightSide(position) * 0.9f;
    SurfSound::SurfSoundManager::ChangeSEPan(seIndex, pan);
  }
}

void GimmickBase::UpdateState()
{
  StateCheckChange();
  StateInitialize();
  StateExecute();
}

void GimmickBase::UpdateDrawableState()
{
  if(m_drawableState.isStateWillChange && m_drawableState.delayTimer > 0)
  {
    m_drawableState.delayTimer--;
    if(m_drawableState.delayTimer == 0)
    {
      bool isVisible = m_drawableState.stateNext > 0 ? true : false;
      SetVisible(isVisible);
      SetEffectVisible(isVisible);
      m_drawableState.Initialize();
    }
  }
}

void GimmickBase::StateCheckChange()
{
  switch(m_stateNow)
  {
  case ST_APPEAR:
    if(m_stateSeq == SEQ_END)
    {
      m_stateNext = ST_WAIT;
    }
    break;
  case ST_WAIT:
    if(CheckActionTrigger())
    {
      m_stateNext = ST_ACTION;
    }
    break;
  }
}

void GimmickBase::StateInitialize()
{
  while(m_stateNext != ST_NONE)
  {
    State prevState = m_stateNow;
    m_stateNow = m_stateNext;
    m_stateNext = ST_NONE;

    switch(m_stateNow)
    {
    case ST_APPEAR:
      OnStateAppearInitialize(prevState);
      break;
    case ST_WAIT:
      OnStateWaitInitialize(prevState);
      break;
    case ST_ACTION:
      OnStateActionInitialize(prevState);
      break;
    }
  }
}

void GimmickBase::StateExecute()
{
  switch(m_stateNow)
  {
  case ST_APPEAR:
    OnStateAppearExecute();
    break;
  case ST_WAIT:
    OnStateWaitExecute();
    break;
  case ST_ACTION:
    OnStateActionExecute();
    break;
  }
}

void GimmickBase::OnStateAppearInitialize(State prevState)
{
  m_stateSeq = 0;
  m_stateParam.m_procTimer = 0;
}

void GimmickBase::OnStateAppearExecute()
{
  m_stateParam.m_procTimer++;
  f32 scale = gfl2::math::Lerp(0.0f, 1.0f, m_stateParam.m_procTimer / APPEAR_TIME);
  SetScale(gfl2::math::Vector3(1.0f, 1.0f, 1.0f) * scale);
  if(m_stateParam.m_procTimer == APPEAR_TIME)
  {
    m_stateSeq = SEQ_END;
  }
}

// [-1.0～1.0]内の乱数を生成する
f32 GimmickBase::MakeRandRate()
{
  f32 base = static_cast<f32>(System::GflUse::GetPublicRand(201)) - 100.0f;
  f32 val = base / 100.0f;
  return val;
}

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
