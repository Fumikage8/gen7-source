//======================================================================
/**
 * @file NpcRiderSeaBread.cpp
 * @date 2017/03/02 14:36:00
 * @author fang_yicheng
 * @brief 海パン野郎ライダー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "NpcRiderSeaBread.h"
#include "../MantainSurfAppManager.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(NpcRider)

const f32 APPEAR_DURATION         = 8.0f;      // 出現処理所要時間(フレーム)
const f32 PLACEMENT_X_OFFSET      = 400.0f;    // 配置位置のX座標オフセット
const f32 PLACEMENT_Z_OFFSET      = 12500.0f;  // 配置位置のZ座標オフセット
const f32 VISIBLE_Z_OFFSET        = 2000.0f;   // 可視距離のZ座標オフセット
const f32 AVOID_CHECK_X_DIFF      = 250.0f;    // プレイヤー回避チェック開始時のX座標差分
const f32 AVOID_CHECK_TIME        = 4.0f;      // プレイヤー回避チェックの開始時間
const f32 POS_X_INIT              = 2250.0f;   // X座標の初期値
const f32 POS_X_MIN               = 2000.0f;   // X座標の最小値
const f32 POS_X_MAX               = 3700.0f;   // X座標の最大値
const f32 GOAL_LINE_POS_Z_OFFSET  = -300.0f;   // ゴールラインZ座標のオフセット

NpcRiderSeaBread::NpcRiderSeaBread() :
  m_pAppManager(NULL),
  m_stateNow(ST_NONE),
  m_stateNext(ST_NONE),
  m_procTimer(0.0f)
{
}

NpcRiderSeaBread::~NpcRiderSeaBread()
{
}

void NpcRiderSeaBread::Terminate()
{
  m_SeaBreadModel.Terminate();
  m_MantainModel.Terminate();
  m_GoalLineModel.Terminate();
}

void NpcRiderSeaBread::Create(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap, MantainSurfAppManager* pAppManager, Resource::ResourceManager *pCharaResourceManager, Resource::ResourceManager *pFieldResourceManager, Effect::EffectManager* pEffectManager)
{
  m_pAppManager = pAppManager;
  
  // モデル初期化
  m_SeaBreadModel.Initialize(pGfxAllocator, pHeap, pCharaResourceManager, pEffectManager);
  m_MantainModel.Initialize(pGfxAllocator, pHeap, pCharaResourceManager, pEffectManager);
  m_GoalLineModel.Initialize(pGfxAllocator, pHeap, pFieldResourceManager, pEffectManager);
  m_MantainModel.AddChildModel(&m_SeaBreadModel);
  m_MantainModel.SetSeaBreadRide(true);
  m_GoalLineModel.SetRotation(gfl2::math::Vector3(0.0f, PI, 0.0f));
  m_GoalLineModel.ChangeAnimation(Model::ModelGoalLine::ANIME_NORMAL_LOOP);

  // 状態初期化
  m_stateNext = ST_INIT;
}

void NpcRiderSeaBread::Update()
{
  UpdateState();
  UpdateVisible();
  UpdateModel();
}

void NpcRiderSeaBread::UpdateModel()
{
  if(m_stateNow > ST_INIT)
  {
    // 処理負荷軽減対策としてゴールラインが出現後にUpdateを回すように
    m_MantainModel.Update();
    m_SeaBreadModel.Update();
    m_GoalLineModel.Update();
  }
}

void NpcRiderSeaBread::UpdateState()
{
  // 状態変化のチェック
  switch(m_stateNow)
  {
  case ST_INIT:
    if(!m_pAppManager->IsTutorialMode() && m_pAppManager->GetNamiController()->IsWaveEnding())
    {
      // 波が終了段階に入った瞬間、海パンを出現させる
      m_stateNext = ST_APPEAR;
    }
    break;
  case ST_APPEAR:
    if(m_procTimer == APPEAR_DURATION)
    {
      // 待機状態へ
      m_stateNext = ST_STANDBY;
    }
    break;
  case ST_STANDBY:
    if(CheckAvoidTrigger())
    {
      // プレイヤーを避ける必要のため、状態変更
      m_stateNext = ST_AVOID;
    }
    break;
  case ST_AVOID:
    break;
  }

  // 状態変化時の初期化
  while(m_stateNext != ST_NONE)
  {
    m_stateNow = m_stateNext;
    m_stateNext = ST_NONE;
    switch(m_stateNow)
    {
    case ST_INIT:
      SetVisible(false);
      break;
    case ST_APPEAR:
      {
        m_procTimer = 0.0f;
        // 初期位置設定
        gfl2::math::Vector3 posPlayer = m_pAppManager->GetPlayerModelSet()->GetPosition();
        gfl2::math::Vector3 posSelf(0.0f, 0.0f, 0.0f);
        posSelf.x = POS_X_INIT;
        posSelf.z = posPlayer.z + PLACEMENT_Z_OFFSET;
        SetPosition(posSelf);
        // モーション/エフェクト設定
        ChangeAnimation(Model::ModelMantain::ANIME_SEABREAD_RIDE);
        m_GoalLineModel.ChangeAnimation(Model::ModelGoalLine::ANIME_FADE_IN, true, Model::ModelBase::ANIME_LOOP_DEFAULT, 1);
        m_GoalLineModel.GetModel()->SetAnimationStepFrame(m_GoalLineModel.GetAnimationEndFrame(1) / APPEAR_DURATION, 1);
      }
      break;
    case ST_STANDBY:
      break;
    case ST_AVOID:
      {
        m_procTimer = 0.0f;
        gfl2::math::Vector3 posSelf = GetPosition();
        gfl2::math::Vector3 posPlayer = m_pAppManager->GetPlayerModelSet()->GetPosition();
        m_AvoidParam.pos_x_start = posSelf.x;
        m_AvoidParam.pos_x_end = gfl2::math::Clamp(posPlayer.x + PLACEMENT_X_OFFSET, POS_X_MIN, POS_X_MAX);
        m_AvoidParam.start_z_diff = posSelf.z - posPlayer.z;
      }
      break;
    }
  }

  // 各状態の実行
  switch(m_stateNow)
  {
  case ST_INIT:
    break;
  case ST_APPEAR:
    UpdateLookDirection();
    ExecuteAppear();
    break;
  case ST_STANDBY:
    UpdateLookDirection();
    ExecuteStandby();
    break;
  case ST_AVOID:
    UpdateLookDirection();
    ExecuteAvoid();
    break;
  }
}

void NpcRiderSeaBread::UpdateVisible()
{
  if(m_stateNow > ST_INIT)
  {
    if(m_pAppManager->GetPlayerModelSet()->GetPosition().z - GetPosition().z >= VISIBLE_Z_OFFSET)
    {
      SetVisible(false);
    }
    else
    {
      SetVisible(true);
    }
  }
}

void NpcRiderSeaBread::UpdateLookDirection()
{
  gfl2::math::Vector3 direction = (m_pAppManager->GetPlayerModelSet()->GetPosition() - GetPosition());
  direction.y = 0.0f;
  if(direction.Length() > 0.0f)
  {
    ChangeLookDirection(direction.Normalize(), 0.2f);
  }
}

void NpcRiderSeaBread::ExecuteAppear()
{
  m_procTimer++;
  f32 t = m_procTimer / APPEAR_DURATION;
  m_SeaBreadModel.SetScale(gfl2::math::Vector3(1.0f, 1.0f, 1.0f) * t);
  m_MantainModel.SetScale(gfl2::math::Vector3(1.0f, 1.0f, 1.0f) * t);
  {
    // ゴールラインのスケール
    gfl2::math::Vector3 scale(1.0f, 1.0f, 1.0f);
    scale.y *= t;
    scale.z *= t;
    //m_GoalLineModel.SetScale(scale);
  }
}

void NpcRiderSeaBread::ExecuteStandby()
{
}

bool NpcRiderSeaBread::CheckAvoidTrigger()
{
  // 一定時間後プレイヤーの位置を予測
  gfl2::math::Vector3 posSelf = GetPosition();
  gfl2::math::Vector3 posPlayerStart = m_pAppManager->GetPlayerModelSet()->GetPosition();
  gfl2::math::Vector3 posPlayerEnd = posPlayerStart + m_pAppManager->GetPlayerController()->GetVelocity() * AVOID_CHECK_TIME;
  if(posPlayerEnd.z >= posSelf.z)
  {
    // プレイヤーがゴールしようとした場合は回避開始
    return true;
  }
  return false;
}

void NpcRiderSeaBread::ExecuteAvoid()
{
  gfl2::math::Vector3 posSelf = GetPosition();
  gfl2::math::Vector3 posPlayer = m_pAppManager->GetPlayerModelSet()->GetPosition();
  gfl2::math::Vector3 velocity = m_pAppManager->GetPlayerController()->GetVelocity();

  // 移動終了位置を計算
  f32 t = 1.0f;
  f32 pos_x_end = m_AvoidParam.pos_x_end;
  if(posSelf.z - posPlayer.z > 0.0f)
  {
    // プレイヤーが海パンを超えてない
    t = gfl2::math::Clamp(1.0f - (posSelf.z - posPlayer.z) / m_AvoidParam.start_z_diff, 0.0f, 1.0f);
    pos_x_end = gfl2::math::Clamp(posPlayer.x + PLACEMENT_X_OFFSET, POS_X_MIN, POS_X_MAX);
  }
  f32 move_t = (1.0f - gfl2::math::CosDeg(t * 180.0f)) / 2.0f;
  f32 scale_t = gfl2::math::SinDeg(t * 180.0f);

  if(pos_x_end > m_AvoidParam.pos_x_end)
  {
    m_AvoidParam.pos_x_end = pos_x_end;
  }

  // 位置更新
  f32 pos_x = gfl2::math::Lerp(m_AvoidParam.pos_x_start, m_AvoidParam.pos_x_end, move_t);
  posSelf.x = gfl2::math::Lerp(posSelf.x, pos_x, 0.15f);
  SetPosition(posSelf);
  // エフェクトスケール更新
  if(scale_t == 0.0f)
  {
    m_MantainModel.EndEffect(Model::ModelMantain::EFT_RUN);
  }
  else
  {
    m_MantainModel.StartEffect(Model::ModelMantain::EFT_RUN);
    m_MantainModel.SetEffectScale(Model::ModelMantain::EFT_RUN, gfl2::math::Vector3(1.0f, 1.0f, 1.0f) * scale_t);
  }
}

f32 NpcRiderSeaBread::CalcMoveToPosX()
{
  gfl2::math::Vector3 posSelf = GetPosition();
  gfl2::math::Vector3 posPlayer = m_pAppManager->GetPlayerModelSet()->GetPosition();
  gfl2::math::Vector3 velocity = m_pAppManager->GetPlayerController()->GetVelocity();
  f32 pos_x = posPlayer.x + PLACEMENT_X_OFFSET;
  // 移動終了位置を計算
  if(velocity.x != 0.0f && velocity.z != 0.0f)
  {
    pos_x = posPlayer.x + (posSelf.z - posPlayer.z) / (velocity.z / velocity.x) + PLACEMENT_X_OFFSET;
  }
  return gfl2::math::Clamp(pos_x, POS_X_MIN, POS_X_MAX);
}

void NpcRiderSeaBread::UpdateAfterTraverse()
{
  m_MantainModel.UpdateAfterTraverse();
  m_SeaBreadModel.UpdateAfterTraverse();
  m_GoalLineModel.UpdateAfterTraverse();
}

void NpcRiderSeaBread::AddToRenderingPipeLine(MantainSurfRenderingPipeline* pRenderingPipeLine, bool edgeFlag)
{
  m_MantainModel.AddToRenderingPipeLine(pRenderingPipeLine, edgeFlag);
  m_SeaBreadModel.AddToRenderingPipeLine(pRenderingPipeLine, edgeFlag);
  m_GoalLineModel.AddToRenderingPipeLine(pRenderingPipeLine, edgeFlag);
}

const gfl2::math::Vector3 NpcRiderSeaBread::GetPosition()
{
  return m_MantainModel.GetPosition();
}

const gfl2::math::Vector3 NpcRiderSeaBread::GetGoalLinePosition()
{
  return m_GoalLineModel.GetPosition();
}

void NpcRiderSeaBread::SetPosition(const gfl2::math::Vector3 &position)
{
  {
    // 海パンの位置設定
    m_MantainModel.SetPosition(position);
  }
  {
    // ゴールラインの位置設定
    gfl2::math::Vector3 pos = m_GoalLineModel.GetPosition();
    pos.z = position.z + GOAL_LINE_POS_Z_OFFSET;
    m_GoalLineModel.SetPosition(pos);
  }
}

void NpcRiderSeaBread::SetVisible(bool visible)
{
  m_SeaBreadModel.SetVisible(visible);
  m_MantainModel.SetVisible(visible);
  m_GoalLineModel.SetVisible(visible);
}

void NpcRiderSeaBread::ChangeAnimation(ModelMantain::AnimeId id, bool isForce, ModelBase::ANIME_LOOP_FLAG loopFlag)
{
  m_MantainModel.ChangeAnimation(id, isForce, loopFlag);
  switch(id)
  {
  case ModelMantain::ANIME_SEABREAD_RIDE:
    m_SeaBreadModel.ChangeAnimation(ModelSeaBread::ANIME_SEABREAD_RIDE, isForce, loopFlag);
    break;
  }
}

void NpcRiderSeaBread::StartEffect(ModelMantain::EffectId id, const gfl2::math::Vector3 &startScale)
{
  m_MantainModel.StartEffect(id, startScale);
}

void NpcRiderSeaBread::EndEffect(ModelMantain::EffectId id)
{
  m_MantainModel.EndEffect(id);
}

void NpcRiderSeaBread::EndAllEffect()
{
  m_MantainModel.EndAllEffect();
}

void NpcRiderSeaBread::ChangeRotationX(f32 angle, f32 changeRate)
{
  m_MantainModel.ChangeRotationX(angle, changeRate);
}

void NpcRiderSeaBread::ChangeRotationY(f32 angle, f32 changeRate)
{
  m_MantainModel.ChangeRotationY(angle, changeRate);
}

void NpcRiderSeaBread::ChangeRotationZ(f32 angle, f32 changeRate)
{
  m_MantainModel.ChangeRotationZ(angle, changeRate);
}

void NpcRiderSeaBread::ChangeRotation(f32 angleX, f32 angleY, f32 angleZ, f32 changeRate)
{
  m_MantainModel.ChangeRoation(angleX, angleY, angleZ, changeRate);
}

f32 NpcRiderSeaBread::GetRotationX()
{
  return m_MantainModel.GetRotationX();
}

f32 NpcRiderSeaBread::GetRotationY()
{
  return m_MantainModel.GetRotationY();
}

f32 NpcRiderSeaBread::GetRotationZ()
{
  return m_MantainModel.GetRotationZ();
}

void NpcRiderSeaBread::ChangeLookDirection(const gfl2::math::Vector3 &vecZX, f32 changeRate, f32 maxAngle)
{
  m_MantainModel.ChangeLookDirection(vecZX, changeRate, maxAngle);
}

void NpcRiderSeaBread::ResetZPosition(const f32 resetLength)
{
  m_MantainModel.ResetZPosition(resetLength);
  m_GoalLineModel.ResetZPosition(resetLength);
}

void NpcRiderSeaBread::SetAppear()
{
  m_stateNext = ST_APPEAR;
}

GFL_NAMESPACE_END(NpcRider)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
