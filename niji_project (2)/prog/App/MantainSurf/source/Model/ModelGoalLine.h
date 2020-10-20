//======================================================================
/**
 * @file ModelGoalLine.h
 * @date 2017/03/07
 * @author fang_yicheng
 * @brief ゴールラインモデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __MODEL_GOAL_LINE_H_INCLUDED__
#define __MODEL_GOAL_LINE_H_INCLUDED__
#pragma once

#include "ModelBase.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

class ModelGoalLine : public ModelBase
{
  GFL_FORBID_COPY_AND_ASSIGN(ModelGoalLine);

public:

  enum AnimeId
  {
    ANIME_NORMAL_LOOP,
    ANIME_FADE_IN,
    ANIME_MAX,
  };
  
public:

  ModelGoalLine();
  virtual ~ModelGoalLine();

protected:

  virtual s32 GetAnimationResId(s32 animeId);

private:

};

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __MODEL_GOAL_LINE_H_INCLUDED__
