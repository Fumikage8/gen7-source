//======================================================================
/**
 * @file GimmickType.h
 * @date 2016/10/26 17:30:06
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__GIMMICK__GIMMICK_TYPE_H_INCLUDED__
#define __LUNA_SOL_DIVE__GIMMICK__GIMMICK_TYPE_H_INCLUDED__
#pragma once

#include "App/LunaSolDive/include/DistortionType.h"

namespace LunaSolDive{
namespace Gimmick{
enum GimmickType
{
  GIMMICK_TYPE__ENERGY,
  GIMMICK_TYPE__OBSTACLE,
  GIMMICK_TYPE__DISTORTION,
  GIMMICK_TYPE__SIZE,
};
}// namespace Gimmick
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__GIMMICK__GIMMICK_TYPE_H_INCLUDED__
