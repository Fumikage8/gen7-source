//======================================================================
/**
 * @file ModelNamiEnd.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief 波終了部分モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __MODEL_NAMI_END_H_INCLUDED__
#define __MODEL_NAMI_END_H_INCLUDED__
#pragma once

#include "ModelNami.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)

class ModelNamiEnd : public ModelNami
{
  GFL_FORBID_COPY_AND_ASSIGN(ModelNamiEnd);

public:

  ModelNamiEnd();
  virtual ~ModelNamiEnd();

protected:

  virtual s32 GetAnimationResId(s32 animeId);

};

GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __MODEL_NAMI_END_H_INCLUDED__
