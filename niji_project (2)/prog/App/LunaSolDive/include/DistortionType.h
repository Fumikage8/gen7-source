//======================================================================
/**
 * @file DistortionType.h
 * @date 2016/12/01 16:08:19
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__DISTORTION_TYPE_H_INCLUDED__
#define __LUNA_SOL_DIVE__DISTORTION_TYPE_H_INCLUDED__
#pragma once

namespace LunaSolDive{
enum DistortionType
{
  DISTORTION_TYPE__UB,
  DISTORTION_TYPE__CLIFF,
  DISTORTION_TYPE__WATER,
  DISTORTION_TYPE__CAVE,
  DISTORTION_TYPE__PLAINS,
  DISTORTION_TYPE__SIZE,
};

enum DistortionRarity
{
  DISTORTION_RARITY__0,
  DISTORTION_RARITY__1,
  DISTORTION_RARITY__2,
  DISTORTION_RARITY__3,
  DISTORTION_RARITY__NONE,
};
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__DISTORTION_TYPE_H_INCLUDED__
