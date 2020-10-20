//======================================================================
/**
 * @file Chara.h
 * @date 2016/11/29 21:40:38
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__ARCHIVE__CHARA_H_INCLUDED__
#define __LUNA_SOL_DIVE__ARCHIVE__CHARA_H_INCLUDED__
#pragma once

#include "./Base.h"
#include "../Version.h"

namespace LunaSolDive{
namespace Archive{

struct MotionIndex
{
  u32 walk_straight;
  u32 walk_left;
  u32 walk_right;
  u32 walk_turn_left;
  u32 walk_turn_right;
  u32 walk_reset;
  u32 run_straight;
  u32 run_left;
  u32 run_right;
  u32 run_turn_left;
  u32 run_turn_right;
  u32 run_reset;
  u32 glide_straight;
  u32 glide_left;
  u32 glide_right;
  u32 glide_turn_left;
  u32 glide_turn_right;
  u32 glide_reset;
};

class Chara: public Base
{
  GFL_FORBID_COPY_AND_ASSIGN(Chara);
public:
  // Initialize
  static void Initialize(
              gfl2::heap::HeapBase* pHeap,
              gfl2::fs::AsyncFileManager* pFileManager);
  // Terminate
  static void Terminate();
  // getter
  static const Chara& Get();
private:
  // constructor
  Chara();
  // Instance
  static Chara* pInstance;
};
// Index of hero's motion
MotionIndex HeroMotionIndex(
            const Version& version,
            const HeroSex& sex);
// Index of pokemon's motion
MotionIndex PokemonMotionIndex(
            const Version& version);
}// namespace Archive
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__ARCHIVE__CHARA_H_INCLUDED__
