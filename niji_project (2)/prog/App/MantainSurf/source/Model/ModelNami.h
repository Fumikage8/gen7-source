//======================================================================
/**
 * @file ModelNami.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief 波モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __MODEL_NAMI_H_INCLUDED__
#define __MODEL_NAMI_H_INCLUDED__
#pragma once

#include "ModelBase.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

class ModelNami : public ModelBase
{
  GFL_FORBID_COPY_AND_ASSIGN(ModelNami);

public:

  enum EffectId
  {
    EFT_WATER_WALL = 0,     // 波頂上の水柱
    EFT_MAX,
  };

  enum AnimeId
  {
    ANIME_NORMAL_LOOP = 0,
    ANIME_HEIGHT_CHANGE,
    ANIME_TIME,
    ANIME_MAX,
  };

  enum AnimeSlot
  {
    ANIME_SLOT_NORMAL_LOOP,
    ANIME_SLOT_TIME,
    ANIME_SLOT_HEIGHT_CHANGE,    // 高さ変化のアニメーションスロットID
  };
  
public:

  ModelNami();
  ModelNami(bool isUseLodModel);
  virtual ~ModelNami();

  void StartAnimation();

  void SetHeightAnimationFrame(f32 heightRate);
  void UpdateTimeZone(s32 timeZone);
  gfl2::math::Vector3 GetTopPosition();

  void SetPlayerPosition(const gfl2::math::Vector3 &position);

protected:

  virtual s32 GetAnimationResId(s32 animeId);

  virtual s32 GetEffectResId(s32 eftId);
  virtual gfl2::math::Vector3 GetEffectPosition(s32 eftId);
  virtual gfl2::math::Vector3 GetEffectOffset(s32 eftId);

private:

  gfl2::math::Vector3 m_playerPosition;
};

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __MODEL_NAMI_H_INCLUDED__
