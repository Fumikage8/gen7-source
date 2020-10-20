//======================================================================
/**
 * @file GimmickHoeruoh.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief メノクラゲ障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __GIMMICK_HOERUOH_H_INCLUDED__
#define __GIMMICK_HOERUOH_H_INCLUDED__
#pragma once

#include "GimmickBaseMove.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

class GimmickHoeruoh : public GimmickBaseMove
{
  GFL_FORBID_COPY_AND_ASSIGN(GimmickHoeruoh);

private:

  // コリジョン定義
  enum
  {
    COLLIDER_MAX = 3,
  };
  static const f32   COLLIDER_RADIUS[COLLIDER_MAX];
  static const char* COLLIDER_JOINT_NAMES[COLLIDER_MAX];

  // アクション定義
  static const f32   ACTION_TRIGGER_DISTANCE;
  static const f32   BODY_LENGTH;
  static const f32   ACTION_EFT_START_FRAME;
  static const f32   ACTION_EFT_START_DURATION;
  static const f32   ACTION_EFT_END_FRAME;
  static const f32   ACTION_EFT_END_DURATION;
  static const f32   ACTION_EFT_MAX_SCALE;

public:

  enum AnimeId
  {
    ANIME_SHADOW,
    ANIME_WAIT,
    ANIME_ACTION,
    ANIME_TIME,
    ANIME_MAX,
  };

  enum AnimeSlot
  {
    ANIME_SLOT_DEFAULT, // 待機／アクション用
    ANIME_SLOT_SHADOW,  // 影用
    ANIME_SLOT_TIME,    // 時間変化用
  };

  enum EffectId
  {
    EFT_WATER,    // 出現時の水しぶき
    EFT_MAX,
  };

public:

  GimmickHoeruoh();
  virtual ~GimmickHoeruoh();

  virtual void CreateCollider();
  virtual void UpdateTimeZone(s32 timeZone);

  virtual gfl2::math::Vector3 GetBodyPosition();

  gfl2::math::Vector3 GetHeadPosition();
  gfl2::math::Vector3 GetHipPosition();
  f32                 GetBodyRadius();

protected:

  virtual s32  GetAnimationResId(s32 animeId);
  virtual s32  GetEffectResId(s32 eftId);
  virtual gfl2::math::Vector3 GetEffectPosition(s32 eftId);
  virtual gfl2::math::Vector3 GetEffectOffset(s32 eftId);
  virtual gfl2::math::Quaternion GetEffectRotation(s32 eftId);

  virtual void OnStateWaitInitialize(State prevState);
  virtual void OnStateWaitExecute();
  virtual void OnStateActionInitialize(State prevState);
  virtual void OnStateActionExecute();
  virtual bool CheckActionTrigger();
  virtual void OnSetDead();

private:

  void PlayActionSE();
  void StopActionSE();

private:

  // @fix GFMMCat[151] SE再生対処で一番最近に出ているホエルオーのインスタンスを保持するように
  static GimmickHoeruoh* m_pLastestActiveHoeruoh;

  f32 m_actionFrame;

};

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __GIMMICK_HOERUOH_H_INCLUDED__
