//======================================================================
/**
 * @file GimmickSamehader.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief サメハダー障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __GIMMICK_SAMEHADER_H_INCLUDED__
#define __GIMMICK_SAMEHADER_H_INCLUDED__
#pragma once

#include "GimmickBaseMove.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

class GimmickSamehader : public GimmickBaseMove
{
  GFL_FORBID_COPY_AND_ASSIGN(GimmickSamehader);

private:

  // コリジョン定義
  enum
  {
    COLLIDER_MAX = 1,
  };
  static const f32   COLLIDER_RADIUS;
  static const char* COLLIDER_JOINT_NAME;

  // アクション定義
  static const f32 ACTION_TRIGGER_DISTANCE;
  static const f32 ACTION_JUMP_START_FRAME;

public:

  enum AnimeId
  {
    ANIME_WAIT,
    ANIME_ACTION,
    ANIME_MAX,
  };

  enum EffectId
  {
    EFT_RUN,
    EFT_WATER_HIT1,
    EFT_WATER_HIT2,
    EFT_GLIDE,
    EFT_MAX,
  };

public:

  GimmickSamehader();
  virtual ~GimmickSamehader();

  virtual gfl2::math::Vector3 GetBodyPosition();

protected:

  virtual void CreateCollider();

  virtual s32  GetAnimationResId(s32 animeId);
  virtual s32  GetEffectResId(s32 eftId);
  virtual gfl2::math::Vector3 GetEffectPosition(s32 eftId);

  virtual void OnStateWaitInitialize(State prevState);
  virtual void OnStateWaitExecute();
  virtual void OnStateActionInitialize(State prevState);
  virtual void OnStateActionExecute();
  virtual bool CheckActionTrigger();

private:

  void ChasePlayerPosition(f32 rate);

private:

  s32 m_stateSeq;
  f32 m_actionFrame;

  bool m_isChasePlayer;

};

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __GIMMICK_SAMEHADER_H_INCLUDED__
