//======================================================================
/**
 * @file GimmickBaseStatic.h
 * @date 2016/09/12 16:39:59
 * @author fang_yicheng
 * @brief ベース設置型障害物モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __GIMMICK_BASE_STATIC_H_INCLUDED__
#define __GIMMICK_BASE_STATIC_H_INCLUDED__
#pragma once

#include "GimmickBase.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Model)
GFL_NAMESPACE_BEGIN(Gimmick)

class GimmickBaseStatic : public GimmickBase
{
  GFL_FORBID_COPY_AND_ASSIGN(GimmickBaseStatic);

public:

  GimmickBaseStatic();
  virtual ~GimmickBaseStatic();

private:

};

GFL_NAMESPACE_END(Gimmick)
GFL_NAMESPACE_END(Model)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __GIMMICK_BASE_STATIC_H_INCLUDED__
