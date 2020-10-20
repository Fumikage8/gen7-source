//======================================================================
/**
 * @file GimmickMenokurage.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief メノクラゲ障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __GIMMICK_MENOKURAGE_H_INCLUDED__
#define __GIMMICK_MENOKURAGE_H_INCLUDED__
#pragma once

#include "GimmickBaseStatic.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

class GimmickMenokurage : public GimmickBaseStatic
{
  GFL_FORBID_COPY_AND_ASSIGN(GimmickMenokurage);

private:

  // コリジョン定義
  enum
  {
    COLLIDER_MAX = 1,
  };
  static const f32   COLLIDER_RADIUS;
  static const f32   COLLIDER_HEIGHT;
  static const char* COLLIDER_JOINT_NAME;
  // アクション定義
  static const f32   MOVE_SPEED_MAX;
  static const f32   MOVE_SPEED_MIN;

public:

  enum AnimeId
  {
    ANIME_WAVE_LOOP,
    ANIME_PM_WAIT,
    ANIME_PM_ACTION,
    ANIME_MAX,
  };

  enum EffectId
  {
    EFT_WATER_HIT1,
    EFT_WATER_HIT2,
    EFT_MAX,
  };

public:

  GimmickMenokurage();
  virtual ~GimmickMenokurage();

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

  struct ActionParam
  {
    gfl2::math::Vector3 m_moveDir;
    f32                 m_moveSpeed;
  };
  ActionParam m_ActionParam;

  s32 m_stateSeq;

};

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __GIMMICK_MENOKURAGE_H_INCLUDED__
