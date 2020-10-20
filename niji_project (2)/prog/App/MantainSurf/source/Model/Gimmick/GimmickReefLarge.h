//======================================================================
/**
 * @file GimmickReefLarge.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief 岩礁(大)障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __GIMMICK_REEF_LARGE_H_INCLUDED__
#define __GIMMICK_REEF_LARGE_H_INCLUDED__
#pragma once

#include "GimmickBaseStatic.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

class GimmickReefLarge : public GimmickBaseStatic
{
  GFL_FORBID_COPY_AND_ASSIGN(GimmickReefLarge);

private:

  // コリジョン定義
  enum
  {
    COLLIDER_MAX = 1,
  };
  static const f32   COLLIDER_RADIUS;
  static const f32   COLLIDER_HEIGHT;
  static const char* COLLIDER_JOINT_NAME;

public:

  enum AnimeId
  {
    ANIME_WAVE_LOOP,
    ANIME_PM_WAIT,
    ANIME_PM_ACTION,
    ANIME_MAX,
  };

public:

  GimmickReefLarge();
  virtual ~GimmickReefLarge();

protected:

  virtual void CreateCollider();

  virtual s32  GetAnimationResId(s32 animeId);

  virtual void OnStateWaitInitialize(State prevState);
  virtual void OnStateActionInitialize(State prevState);
  virtual void OnStateActionExecute();
  virtual bool CheckActionTrigger();

private:

};

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __GIMMICK_REEF_LARGE_H_INCLUDED__
