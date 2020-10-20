//======================================================================
/**
 * @file ModelSkyBox.h
 * @date 2017/01/16 13:13:13
 * @author fang_yicheng
 * @brief 天球モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __MODEL_SKYBOX_H_INCLUDED__
#define __MODEL_SKYBOX_H_INCLUDED__
#pragma once

#include "ModelBase.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

class ModelSkyBox : public ModelBase
{
  GFL_FORBID_COPY_AND_ASSIGN(ModelSkyBox);

public:

  enum AnimeId
  {
    ANIME_NORMAL = 0,   // 常時アニメーション
    ANIME_MOON,         // 月満ち欠けアニメーション
    ANIME_TIME,         // 時間帯アニメーション
    ANIME_MAX,
  };

  enum AnimeSlot
  {
    ANIME_SLOT_NORMAL,
    ANIME_SLOT_MOON,
    ANIME_SLOT_TIME,
  };
  
public:

  ModelSkyBox();
  virtual ~ModelSkyBox();

  void StartAnimation();
  void UpdateTimeZone(s32 timeZone);
  void UpdateMoon(s32 moonPattern);

protected:

  virtual s32 GetAnimationResId(s32 animeId);

private:

};

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __MODEL_SKYBOX_H_INCLUDED__
