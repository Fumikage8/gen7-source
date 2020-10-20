//======================================================================
/**
 * @file Input.h
 * @date 2016/09/29 17:35:57
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RUN__INPUT_H_INCLUDED__
#define __LUNA_SOL_DIVE__RUN__INPUT_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <types/include/gfl2_Typedef.h>
#include "./Signal.h"
#include "../Version.h"

// forward declaration
namespace gfl2{
namespace math{
class Vector2;
}// neamspace math
namespace ui{
class Button;
class DeviceManager;
class VectorDevice;
}// namespace ui
}// namespace gfl2

namespace LunaSolDive{
namespace Run{
class Input
{
  GFL_FORBID_COPY_AND_ASSIGN(Input);
public:
  // constructor
  Input();
  // destructor
  virtual ~Input() = 0;
  // Initialize
  virtual void Initialize(
              const Maneuver& maneuver,
              gfl2::heap::HeapBase* pHeap,
              gfl2::ui::DeviceManager* pDeviceManager);
  // Terminate
  virtual void Terminate();
  // Update
  virtual void Update();
  // Button
  const gfl2::ui::Button& Button() const;
  // Stick
  const gfl2::ui::VectorDevice& Stick() const;
  // Aim
  virtual const gfl2::math::Vector2& Aim() const = 0;
  // Set Initial Direction
  virtual void SetInitialDirection();
  // Reset Signal
  virtual Run::ResetSignal ResetSignal() const = 0;
protected:
  // DeviceManager
  gfl2::ui::DeviceManager& DeviceManager() const;
private:
  // DeviceManager pointer
  gfl2::ui::DeviceManager* m_pDeviceManager;
};
}// namespace Run
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RUN__INPUT_H_INCLUDED__
