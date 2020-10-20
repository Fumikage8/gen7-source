//======================================================================
/**
 * @file PlayerModelSet.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief プレイヤーモデルセット
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "PlayerModelSet.h"
#include "../MantainSurfAppManager.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Player)

static const f32 VALID_POS_MAX = 999999999.0f;

PlayerModelSet::PlayerModelSet() :
  m_pAppManager(NULL),
  m_playerOffset(gfl2::math::Vector3(0.0f, 0.0f, 0.0f)),
  m_prevValidPos(gfl2::math::Vector3(0.0f, 0.0f, 0.0f))
{
}

PlayerModelSet::~PlayerModelSet()
{
}

void PlayerModelSet::Terminate()
{
  m_PlayerModel.Terminate();
  m_MantainModel.Terminate();
}

ModelPlayer* PlayerModelSet::GetPlayerModel()
{
  return &m_PlayerModel;
}

ModelMantain* PlayerModelSet::GetMantainModel()
{
  return &m_MantainModel;
}

void PlayerModelSet::CreateModelSet(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap, MantainSurfAppManager* pAppManager, Resource::ResourceManager *pResourceManager, Effect::EffectManager* pEffectManager)
{
  m_pAppManager = pAppManager;
  
  // セーブデータを取得
  GameSys::GameData* pGameData = m_pAppManager->GetGameData();
  s32 sex = pGameData->GetPlayerStatusConst()->GetSex();
  poke_3d::model::DressUpParam dressUpParam = pGameData->GetPlayerStatusConst()->GetDressup();

#if PM_DEBUG
  GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
  if(pButton->IsHold(gfl2::ui::BUTTON_R))
  {
    // 性別切り替え
    sex = (sex + 1) % 2;
    dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX] = sex;
  }
#endif

  // 主人公情報設定
  m_PlayerModel.SetupHeroInfo(sex, dressUpParam);
  
  // モデル初期化
  m_PlayerModel.Initialize(pGfxAllocator, pHeap, pResourceManager, pEffectManager);
  m_MantainModel.Initialize(pGfxAllocator, pHeap, pResourceManager, pEffectManager);
  m_MantainModel.AddChildModel(&m_PlayerModel);
}

void PlayerModelSet::Update()
{
  m_MantainModel.Update();
  m_PlayerModel.Update();
}

void PlayerModelSet::UpdateAfterTraverse()
{
  m_MantainModel.UpdateAfterTraverse();
  m_PlayerModel.UpdateAfterTraverse();
}

void PlayerModelSet::AddToRenderingPipeLine(MantainSurfRenderingPipeline* pRenderingPipeLine, bool edgeFlag)
{
  m_MantainModel.AddToRenderingPipeLine(pRenderingPipeLine, edgeFlag);
  m_PlayerModel.AddToRenderingPipeLine(pRenderingPipeLine, edgeFlag);
}

const gfl2::math::Vector3 PlayerModelSet::GetPosition()
{
  return m_MantainModel.GetPosition();
}

void PlayerModelSet::SetPosition(const gfl2::math::Vector3 &position)
{
  if(gfl2::math::FAbs(position.x) <= VALID_POS_MAX && gfl2::math::FAbs(position.y) <= VALID_POS_MAX && gfl2::math::FAbs(position.z) <= VALID_POS_MAX)
  {
    m_prevValidPos = position;
  }
  else
  {
    // 値が大きい過ぎる
    GFL_ASSERT(0);
  }
  m_MantainModel.SetPosition(m_prevValidPos);
  m_PlayerModel.SetPosition(m_playerOffset);
}

void PlayerModelSet::SetVisible(bool visible)
{
  m_PlayerModel.SetVisible(visible);
  m_MantainModel.SetVisible(visible);
}

void PlayerModelSet::SetColliderEnabled(bool enable)
{
  m_PlayerModel.SetColliderEnabled(enable);
  m_MantainModel.SetColliderEnabled(enable);
}

void PlayerModelSet::ChangeAnimation(ModelPlayer::AnimeId id, bool isForce, ModelBase::ANIME_LOOP_FLAG loopFlag)
{
  m_PlayerModel.ChangeAnimation(id, isForce, loopFlag);
  m_MantainModel.ChangeAnimation(id, isForce, loopFlag);
}

void PlayerModelSet::ChangeSurfMotion(ModelPlayer::AnimeId animeId, f32 t)
{
  m_PlayerModel.ChangeSurfMotion(animeId, t);
  m_MantainModel.ChangeSurfMotion(animeId, t);
}

bool PlayerModelSet::IsAnimationEnd(bool isOneFrameAdvance)
{
  return m_MantainModel.IsAnimationEnd(0, -1, isOneFrameAdvance);
}

bool PlayerModelSet::IsAnimationEnd(s32 animeId, bool isOneFrameAdvance)
{
  return m_MantainModel.IsAnimationEnd(0, animeId, isOneFrameAdvance);
}

void PlayerModelSet::EndAnimation()
{
  m_PlayerModel.EndAnimation();
  m_MantainModel.EndAnimation();
}

f32 PlayerModelSet::GetAnimationEndFrame()
{
  return m_MantainModel.GetAnimationEndFrame();
}

f32 PlayerModelSet::GetAnimationFrame()
{
  return m_MantainModel.GetAnimationFrame();
}

f32 PlayerModelSet::GetAnimationProgress()
{
  return m_MantainModel.GetAnimationProgress();
}

void PlayerModelSet::SetAnimationProgress(f32 progress)
{
  m_PlayerModel.SetAnimationProgress(progress);
  m_MantainModel.SetAnimationProgress(progress);
}

void PlayerModelSet::StartEffect(ModelMantain::EffectId id, const gfl2::math::Vector3 &startScale)
{
#if 0
  m_PlayerModel.StartEffect(id);
#endif
  m_MantainModel.StartEffect(id, startScale);
}

void PlayerModelSet::EndEffect(ModelMantain::EffectId id)
{
#if 0
  m_PlayerModel.EndEffect(id);
#endif
  m_MantainModel.EndEffect(id);
}

void PlayerModelSet::EndAllEffect()
{
#if 0
  m_PlayerModel.EndAllEffect();
#endif
  m_MantainModel.EndAllEffect();
}

// 現姿勢の向きベクトルを取得
gfl2::math::Vector3 PlayerModelSet::GetLookDirection()
{
  return m_MantainModel.GetDirection(ModelBaseChara::DIR_FRONT);
}

void PlayerModelSet::ChangeRotationX(f32 angle, f32 changeRate)
{
  m_MantainModel.ChangeRotationX(angle, changeRate);
}

void PlayerModelSet::ChangeRotationY(f32 angle, f32 changeRate)
{
  m_MantainModel.ChangeRotationY(angle, changeRate);
}

void PlayerModelSet::ChangeRotationZ(f32 angle, f32 changeRate)
{
  m_MantainModel.ChangeRotationZ(angle, changeRate);
}

void PlayerModelSet::ChangeRotation(f32 angleX, f32 angleY, f32 angleZ, f32 changeRate)
{
  m_MantainModel.ChangeRoation(angleX, angleY, angleZ, changeRate);
}

f32 PlayerModelSet::GetRotationX()
{
  return m_MantainModel.GetRotationX();
}

f32 PlayerModelSet::GetRotationY()
{
  return m_MantainModel.GetRotationY();
}

f32 PlayerModelSet::GetRotationZ()
{
  return m_MantainModel.GetRotationZ();
}

void PlayerModelSet::ChangeLookDirection(const gfl2::math::Vector3 &vecZX, f32 changeRate, f32 maxAngle)
{
  m_MantainModel.ChangeLookDirection(vecZX, changeRate, maxAngle);
}

void PlayerModelSet::ChangeUpDirection(const gfl2::math::Vector3 &vecYZ, f32 changeRate, f32 maxAngle)
{
  m_MantainModel.ChangeUpDirection(vecYZ, changeRate, maxAngle);
}

void PlayerModelSet::ChangeInclination(const gfl2::math::Vector3 &vecYX, f32 changeRate, f32 maxAngle)
{
  m_MantainModel.ChangeInclination(vecYX, changeRate, maxAngle);
}

bool PlayerModelSet::IsHitObstacle()
{
  return m_MantainModel.GetMainCollionInfo()->m_isCollided;
}

// 衝突情報を取得
Collision::ColliderBase::CollisionInfo* PlayerModelSet::GetMainCollisionInfo()
{
  return m_MantainModel.GetMainCollionInfo();
}

void PlayerModelSet::ResetZPosition(const f32 resetLength)
{
  m_MantainModel.ResetZPosition(resetLength);
}

void PlayerModelSet::StopEffect()
{
  m_PlayerModel.StopEffect();
  m_MantainModel.StopEffect();
}

void PlayerModelSet::ResumeEffect()
{
  m_PlayerModel.ResumeEffect();
  m_MantainModel.ResumeEffect();
}

GFL_NAMESPACE_END(Player)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
