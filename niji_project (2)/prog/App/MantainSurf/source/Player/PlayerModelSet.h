//======================================================================
/**
 * @file PlayerModelSet.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief プレイヤーモデルセット
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __PLAYER_MODELSET_H_INCLUDED__
#define __PLAYER_MODELSET_H_INCLUDED__
#pragma once

#include "../Model/ModelPlayer.h"
#include "../Model/ModelMantain.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
class MantainSurfAppManager;
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

using namespace App::MantainSurf::Model;

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Player)

class PlayerModelSet
{
  GFL_FORBID_COPY_AND_ASSIGN(PlayerModelSet);

public:

  PlayerModelSet();
  virtual ~PlayerModelSet();

  void Terminate();

  ModelPlayer*  GetPlayerModel();
  ModelMantain* GetMantainModel();

  void CreateModelSet(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap, MantainSurfAppManager* pAppManager, Resource::ResourceManager *pResourceManager, Effect::EffectManager* pEffectManager);
    
  void Update();
  void UpdateAfterTraverse();

  void AddToRenderingPipeLine(MantainSurfRenderingPipeline* pRenderingPipeLine, bool edgeFlag);

  const gfl2::math::Vector3 GetPosition();
  void SetPosition(const gfl2::math::Vector3 &position);

  void SetVisible(bool visible);

  void SetColliderEnabled(bool enable);

  void ChangeAnimation(ModelPlayer::AnimeId id, bool isForce = false, ModelBase::ANIME_LOOP_FLAG loopFlag = ModelBase::ANIME_LOOP_DEFAULT);
  void ChangeSurfMotion(ModelPlayer::AnimeId animeId, f32 t);
  bool IsAnimationEnd(bool isOneFrameAdvance = false);
  bool IsAnimationEnd(s32 animeId, bool isOneFrameAdvance = false);
  void EndAnimation();
  f32  GetAnimationEndFrame();
  f32  GetAnimationFrame();
  f32  GetAnimationProgress();
  void SetAnimationProgress(f32 progress);

  void StartEffect(ModelMantain::EffectId id, const gfl2::math::Vector3 &startScale = gfl2::math::Vector3(1.0f, 1.0f, 1.0f));
  void EndEffect(ModelMantain::EffectId id);
  void EndAllEffect();

  gfl2::math::Vector3 GetLookDirection();
  
  //gfl2::math::Vector3 ChangeLookDirection(gfl2::math::Vector3 direction, f32 changeRate, f32 maxAngle = PI);
  //gfl2::math::Vector3 ChangeUpDirection(gfl2::math::Vector3 direction, f32 changeRate, f32 maxAngle = PI);
  //void ChangeTurnOverAngle(f32 angle);
  //void ChangeInclination(gfl2::math::Vector3 normal, f32 changeRate = 0.3f);

  void ChangeRotationX(f32 angle, f32 changeRate);
  void ChangeRotationY(f32 angle, f32 changeRate);
  void ChangeRotationZ(f32 angle, f32 changeRate);
  void ChangeRotation(f32 angleX, f32 angleY, f32 angleZ, f32 changeRate);

  f32  GetRotationX();
  f32  GetRotationY();
  f32  GetRotationZ();

  void ChangeLookDirection(const gfl2::math::Vector3 &vecZX, f32 changeRate, f32 maxAngle = PI);
  void ChangeUpDirection(const gfl2::math::Vector3 &vecYZ, f32 changeRate, f32 maxAngle = PI);
  void ChangeInclination(const gfl2::math::Vector3 &vecYX, f32 changeRate, f32 maxAngle = PI);

  bool IsHitObstacle();

  Collision::ColliderBase::CollisionInfo* GetMainCollisionInfo();

  void ResetZPosition(const f32 resetLength);

  void StopEffect();
  void ResumeEffect();

private:

  MantainSurfAppManager* m_pAppManager;
  gfl2::math::Vector3 m_playerOffset;

  ModelPlayer  m_PlayerModel;
  ModelMantain m_MantainModel;

  gfl2::math::Vector3 m_prevValidPos;

};

GFL_NAMESPACE_END(Player)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __PLAYERMODEL_H_INCLUDED__
