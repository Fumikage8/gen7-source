//======================================================================
/**
 * @file Run_InputWin32.cpp
 * @date 2016/09/23 19:30:01
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#ifdef GF_PLATFORM_WIN32
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_MathCommon.h>
#include <ui/include/gfl2_UI_Button.h>
#include <ui/include/gfl2_UI_DeviceManager.h>
#include <ui/include/gfl2_UI_VectorDevice.h>
#include "./InputWin32.h"
#include "../Assert.h"

namespace LunaSolDive{
namespace Run{
// constructor
InputWin32::InputWin32()
    : Input(),
      m_Aim(gfl2::math::Vector2::GetZero()){}
// Initialize (override)
void InputWin32::Initialize(
            const Maneuver& maneuver,
            gfl2::heap::HeapBase* pHeap,
            gfl2::ui::DeviceManager* pDeviceManager)
{
  Input::Initialize(
              maneuver,
              pHeap,
              pDeviceManager);
}
// Terminate (override)
void InputWin32::Terminate()
{
  Input::Terminate();
}
// Update (override)
void InputWin32::Update()
{
  // Update Aim
  m_Aim = gfl2::math::Vector2(
              Stick().GetX(),
              Stick().GetY());
  if(m_Aim.Length() > 1.0f)
  {
    m_Aim = m_Aim.Normalize();
  }
  LUNA_SOL_DIVE_ASSERT(m_Aim.Length() < 1.0f + EPSILON);
}
// Aim (override)
const gfl2::math::Vector2& InputWin32::Aim() const
{
  return m_Aim;
}
// Reset Signal (override)
ResetSignal InputWin32::ResetSignal() const
{
  if(Button().IsTrigger(gfl2::ui::BUTTON_ZL))
  {
    return RESET_SIGNAL__PULL;
  }
  else if(Button().IsTrigger(gfl2::ui::BUTTON_ZR))
  {
    return RESET_SIGNAL__PUSH;
  }
  else
  {
    return RESET_SIGNAL__NONE;
  }
}
}// namespace Run
}// namespace LunaSolDive
#endif // GF_PLATFORM_WIN32
