//======================================================================
/**
 * @file Run_Input.cpp
 * @date 2016/09/29 17:43:00
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <GameSys/include/GameManager.h>
#include "./Input.h"
#include "../Assert.h"

namespace LunaSolDive{
namespace Run{
// constructor
Input::Input()
    : m_pDeviceManager(NULL){}
// destructor
Input::~Input(){}
// Initialize
void Input::Initialize(
            const Maneuver& /*maneuver*/,
            gfl2::heap::HeapBase* /*pHeap*/,
            gfl2::ui::DeviceManager* pDeviceManager)
{
  LUNA_SOL_DIVE_ASSERT(pDeviceManager != NULL);
  // set DeviceManager pointer
  m_pDeviceManager = pDeviceManager;
}
// Terminate
void Input::Terminate()
{
  // unset DeviceManager pointer
  m_pDeviceManager = NULL;
}
// Update
void Input::Update(){}
// Button
const gfl2::ui::Button& Input::Button() const
{
  return *DeviceManager().GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
}
// Stick
const gfl2::ui::VectorDevice& Input::Stick() const
{
  return *DeviceManager().GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
}
// Set Initial Direction
void Input::SetInitialDirection(){}
// DeviceManager
gfl2::ui::DeviceManager& Input::DeviceManager() const{
  LUNA_SOL_DIVE_ASSERT(m_pDeviceManager != NULL);
  return *m_pDeviceManager;
}
}// namespace Run
}// namespace LunaSolDive
