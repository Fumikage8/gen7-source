//======================================================================
/**
 * @file MotionType.h
 * @date 2016/12/01 19:00:24
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__PLAYER__MOTION_TYPE_H_INCLUDED__
#define __LUNA_SOL_DIVE__PLAYER__MOTION_TYPE_H_INCLUDED__
#pragma once

namespace LunaSolDive{
namespace Player{

enum MotionSpeed
{
  MOTION_SPEED__WALK,
  MOTION_SPEED__RUN,
  MOTION_SPEED__GLIDE,
  MOTION_SPEED__SIZE,
};

enum MotionDirection
{
  MOTION_DIRECTION__STRAIGHT,
  MOTION_DIRECTION__LEFT,
  MOTION_DIRECTION__RIGHT,
  MOTION_DIRECTION__SIZE,
};
}// namespace Player
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__PLAYER__MOTION_TYPE_H_INCLUDED__
