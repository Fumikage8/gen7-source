//======================================================================
/**
 * @file ModelSea.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief 海モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __MODEL_SEA_H_INCLUDED__
#define __MODEL_SEA_H_INCLUDED__
#pragma once

#include "ModelBase.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

class ModelSea : public ModelBase
{
  GFL_FORBID_COPY_AND_ASSIGN(ModelSea);

public:

  enum AnimeId
  {
    ANIME_NORMAL_LOOP = 0,
    ANIME_TIME,
    ANIME_MAX,
  };

  enum AnimeSlot
  {
    ANIME_SLOT_NORMAL_LOOP,
    ANIME_SLOT_TIME,
  };
  
public:

  ModelSea();
  virtual ~ModelSea();

  void StartAnimation();
  void UpdateTimeZone(s32 timeZone);

protected:

  virtual s32 GetAnimationResId(s32 animeId);

private:

};

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __MODEL_SEA_H_INCLUDED__
