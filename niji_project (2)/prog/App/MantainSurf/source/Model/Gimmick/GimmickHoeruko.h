//======================================================================
/**
 * @file GimmickHoeruko.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief ホエルコ障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __GIMMICK_HOERUKO_H_INCLUDED__
#define __GIMMICK_HOERUKO_H_INCLUDED__
#pragma once

#include "GimmickBaseStatic.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

class GimmickHoeruko : public GimmickBaseStatic
{
  GFL_FORBID_COPY_AND_ASSIGN(GimmickHoeruko);

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
    ANIME_SHADOW_LOOP,
    ANIME_PM_ACTION,
    ANIME_PM_WAIT,
    ANIME_TIME,
    ANIME_MAX,
  };

  enum AnimeSlot
  {
    ANIME_SLOT_DEFAULT, // 待機／アクション用
    ANIME_SLOT_SHADOW,  // 影／水波紋アニメーション
    ANIME_SLOT_TIME,    // 時間変化用
  };

  enum EffectId
  {
    EFT_WATER_HIT1,
    EFT_MAX,
  };

public:

  GimmickHoeruko();
  virtual ~GimmickHoeruko();

  virtual void UpdateTimeZone(s32 timeZone);

protected:

  virtual void CreateCollider();

  virtual s32  GetAnimationResId(s32 animeId);
  virtual s32  GetEffectResId(s32 eftId);

  virtual void OnStateWaitInitialize(State prevState);
  virtual void OnStateWaitExecute();
  virtual void OnStateActionInitialize(State prevState);
  virtual void OnStateActionExecute();
  virtual bool CheckActionTrigger();

private:

};

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __GIMMICK_HOERUKO_H_INCLUDED__
