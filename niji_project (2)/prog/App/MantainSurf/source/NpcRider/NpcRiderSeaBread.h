//======================================================================
/**
 * @file NpcRiderSeaBread.h
 * @date 2017/03/02 14:36:00
 * @author fang_yicheng
 * @brief 海パン野郎ライダー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __NPC_RIDER_SEA_BREAD_H_INCLUDED__
#define __NPC_RIDER_SEA_BREAD_H_INCLUDED__
#pragma once

#include "../Model/ModelSeaBread.h"
#include "../Model/ModelMantain.h"
#include "../Model/ModelGoalLine.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
class MantainSurfAppManager;
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

using namespace App::MantainSurf::Model;

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(NpcRider)

class NpcRiderSeaBread
{
  GFL_FORBID_COPY_AND_ASSIGN(NpcRiderSeaBread);

public:

  // 状態
  enum State
  {
    ST_NONE = -1,
    ST_INIT,      // 初期状態
    ST_APPEAR,    // 出現状態
    ST_STANDBY,   // 待機状態
    ST_AVOID,     // プレイヤー避ける
  };

public:

  NpcRiderSeaBread();
  virtual ~NpcRiderSeaBread();

  void Terminate();

  void Create(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pHeap, MantainSurfAppManager* pAppManager, Resource::ResourceManager *pCharaResourceManager, Resource::ResourceManager *pFieldResourceManager, Effect::EffectManager* pEffectManager);
    
  void Update();
  void UpdateAfterTraverse();

  void AddToRenderingPipeLine(MantainSurfRenderingPipeline* pRenderingPipeLine, bool edgeFlag);

  const gfl2::math::Vector3 GetPosition();
  const gfl2::math::Vector3 GetGoalLinePosition();
  void SetPosition(const gfl2::math::Vector3 &position);
  void SetVisible(bool visible);

  void ChangeAnimation(ModelMantain::AnimeId id, bool isForce = false, ModelBase::ANIME_LOOP_FLAG loopFlag = ModelBase::ANIME_LOOP_DEFAULT);

  void StartEffect(ModelMantain::EffectId id, const gfl2::math::Vector3 &startScale = gfl2::math::Vector3(1.0f, 1.0f, 1.0f));
  void EndEffect(ModelMantain::EffectId id);
  void EndAllEffect();

  void ChangeRotationX(f32 angle, f32 changeRate);
  void ChangeRotationY(f32 angle, f32 changeRate);
  void ChangeRotationZ(f32 angle, f32 changeRate);
  void ChangeRotation(f32 angleX, f32 angleY, f32 angleZ, f32 changeRate);

  f32  GetRotationX();
  f32  GetRotationY();
  f32  GetRotationZ();

  void ChangeLookDirection(const gfl2::math::Vector3 &vecZX, f32 changeRate, f32 maxAngle = PI);

  void ResetZPosition(const f32 resetLength);
  
  void SetAppear();
  bool IsStandby() { return m_stateNow >= ST_STANDBY; }

private:

  void UpdateState();
  void UpdateVisible();
  void UpdateModel();
  void UpdateLookDirection();

  void ExecuteAppear();
  void ExecuteStandby();
  void ExecuteAvoid();

  bool CheckAvoidTrigger();

  f32  CalcMoveToPosX();

private:

  MantainSurfAppManager* m_pAppManager;

  ModelSeaBread  m_SeaBreadModel;
  ModelMantain   m_MantainModel;
  ModelGoalLine  m_GoalLineModel;

  State m_stateNow;
  State m_stateNext;

  f32   m_procTimer;

  struct AvoidParam
  {
    f32 pos_x_start;
    f32 pos_x_end;
    f32 start_z_diff;

    AvoidParam()
    {
      pos_x_start = 0.0f;
      pos_x_end = 0.0f;
      start_z_diff = 0.0f;
    }
  };
  AvoidParam m_AvoidParam;

};

GFL_NAMESPACE_END(NpcRider)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __NPC_RIDER_SEA_BREAD_H_INCLUDED__
