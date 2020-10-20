//======================================================================
/**
 * @file DrawUtil.h
 * @date 2017/02/28 12:09:14
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG
#if !defined __LUNA_SOL_DIVE__DEBUG__DRAW_UTIL_H_INCLUDED__
#define __LUNA_SOL_DIVE__DEBUG__DRAW_UTIL_H_INCLUDED__
#pragma once

#include "./DrawUtilData.h"

// forward declaration
namespace gfl2{
namespace gfx{
class CtrDisplayNo;
}// namespace gfx
}// namespace gfl2

namespace LunaSolDive{
// forward declaration
namespace Camera{
class Base;
}// namespace Camera

namespace Debug{
// forward declaration
class Menu;

// DrawUtil Setting
struct DrawUtilSetting
{
  bool active;
  bool input_point;
  bool present_point;
  bool collision_point;
  bool rail_section;
  bool gimmick_energy_collision;
  bool gimmick_obstacle_collision;
  bool gimmick_distortion_collision;
};

class DrawUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(DrawUtil);
public:
  // constructor
  DrawUtil();
  // destructor
  ~DrawUtil();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap);
  // Terminate
  void Terminate();
  // Draw
  void Draw(
              const gfl2::gfx::CtrDisplayNo& displayNo,
              const Camera::Base& camera);
  // Data
  DrawUtilData& Data();
private:
  // Draw Implement
  void DrawImpl(
              const Camera::Base& camera);
  // Setting
  DrawUtilSetting m_Setting;
  // Data
  DrawUtilData m_Data;
  // friend
  friend class Menu;
};
}// namespace Debug
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__DEBUG__DRAW_UTIL_H_INCLUDED__
#endif// PM_DEBUG
