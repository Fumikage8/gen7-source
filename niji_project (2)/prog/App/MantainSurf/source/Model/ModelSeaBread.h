//======================================================================
/**
 * @file ModelSeaBread.h
 * @date 2017/03/02 14:36:00
 * @author fang_yicheng
 * @brief 海パン野郎モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __MODEL_SEA_BREAD_H_INCLUDED__
#define __MODEL_SEA_BREAD_H_INCLUDED__
#pragma once

#include "ModelBaseChara.h"

#include <model/include/gfl2_DressUpModel.h>
#include <model/include/gfl2_DressUpParam.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

class ModelSeaBread : public ModelBaseChara
{
  GFL_FORBID_COPY_AND_ASSIGN(ModelSeaBread);

public:

  enum AnimeId
  {
    ANIME_SEABREAD_RIDE,
    ANIME_MAX,
  };

public:

  ModelSeaBread();
  virtual ~ModelSeaBread();

  virtual void Terminate();

protected:

  virtual poke_3d::model::BaseModel* CreateModel(s32 resId);

  virtual s32 GetAnimationResId(s32 animeId);

protected:

  s32   m_charaId;

  poke_3d::model::CharaModelFactory           m_CharaModelFactory;
};

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __MODEL_SEA_BREAD_H_INCLUDED__
