//======================================================================
/**
 * @file GimmickCommon.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief 通用型障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __GIMMICK_COMMON_H_INCLUDED__
#define __GIMMICK_COMMON_H_INCLUDED__
#pragma once

#include "GimmickBaseStatic.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

class GimmickCommon : public GimmickBaseStatic
{
  GFL_FORBID_COPY_AND_ASSIGN(GimmickCommon);

private:

  // コリジョン定義
  enum
  {
    COLLIDER_MAX = 1,
  };
  static const f32 COLLIDER_RADIUS;

public:

  GimmickCommon();
  virtual ~GimmickCommon();

  virtual void CreateCollider();
};

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __GIMMICK_COMMON_H_INCLUDED__
