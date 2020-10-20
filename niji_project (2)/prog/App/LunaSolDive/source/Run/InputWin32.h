//======================================================================
/**
 * @file InputWin32.h
 * @date 2016/09/23 19:26:49
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#ifdef GF_PLATFORM_WIN32
#if !defined __LUNA_SOL_DIVE__RUN__INPUT_WIN32_H_INCLUDED__
#define __LUNA_SOL_DIVE__RUN__INPUT_WIN32_H_INCLUDED__
#pragma once

#include <math/include/gfl2_Vector2.h>
#include "./Input.h"

// forward declaration
namespace gfl2{
namespace ui{
class DeviceManager;
}// namespace ui
}// namespace gfl2

namespace LunaSolDive{
namespace Run{
class InputWin32: public Input
{
  GFL_FORBID_COPY_AND_ASSIGN(InputWin32);
public:
  // constructor
  InputWin32();
  // Initialize (ovverride)
  virtual void Initialize(
              const Maneuver& maneuver,
              gfl2::heap::HeapBase* pHeap,
              gfl2::ui::DeviceManager* pDeviceManager);
  // Terminate (override)
  virtual void Terminate();
  // Update (override)
  virtual void Update();
  // Aim (override)
  virtual const gfl2::math::Vector2& Aim() const;
  // Reset Signal (override)
  virtual Run::ResetSignal ResetSignal() const;
private:
  //  Aim
  gfl2::math::Vector2 m_Aim;
};
}// namespace Run
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RUN__INPUT_WIN32_H_INCLUDED__
#endif // GF_PLATFORM_WIN32
