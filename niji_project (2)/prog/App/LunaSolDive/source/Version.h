//======================================================================
/**
 * @file Version.h
 * @date 2016/11/29 11:47:32
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__VERSION_H_INCLUDED__
#define __LUNA_SOL_DIVE__VERSION_H_INCLUDED__
#pragma once

namespace LunaSolDive{
enum Version
{
  VERSION__NONE,
  VERSION__SOL,
  VERSION__LUNA,
};

enum HeroSex
{
  HERO_SEX__MALE,
  HERO_SEX__FEMALE,
  HERO_SEX__NONE,
};

enum Mode
{
  MODE__NORMAL,
  MODE__FIRST,
  MODE__NONE,
};

enum Maneuver
{
  MANEUVER__GYROSCOPE,
  MANEUVER__STICK,
  MANEUVER__NONE,
};
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__VERSION_H_INCLUDED__
