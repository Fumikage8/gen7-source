//======================================================================
/**
 * @file Run_Translation.cpp
 * @date 2017/01/19 18:25:38
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <debug/include/gfl2_DebugPrint.h>
#include "./Parameter.h"
#include "./Translation.h"
#include "../Assert.h"
#include "../Manager.h"
#include "../Camera/Main.h"
#include "../Effect/Manager.h"
#include "../Gimmick/Manager.h"
#include "../Player/Manager.h"
#include "../Rail/Manager.h"
#if PM_DEBUG
#include "GameSys/include/GameManager.h"
#include "../Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Run{
// constructor
Translation::Translation()
    : m_pManager(NULL),
      m_pCamera(NULL),
      m_FrameCount(0),
      m_IsInProgress(false){}
// Initialize
void Translation::Initialize(
            LunaSolDive::Manager* pManager,
            Camera::Main* pCamera)
{
  m_pManager = pManager;
  LUNA_SOL_DIVE_ASSERT(m_pManager != NULL);
  m_pCamera = pCamera;
  LUNA_SOL_DIVE_ASSERT(m_pCamera != NULL);
}
// Terminate
void Translation::Terminate()
{
  m_pManager = NULL;
  m_FrameCount = 0;
  m_Displacement = gfl2::math::Vector3::GetZero();
  m_IsInProgress = false;
}
// Update
void Translation::Update()
{
  LUNA_SOL_DIVE_ASSERT(m_pManager != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pCamera != NULL);
  if(! m_IsInProgress)
  {// is in waiting
    const gfl2::math::Vector3 presentLocation =
                m_pManager->RailManager().Location().Point();
#if PM_DEBUG
    const gfl2::ui::Button* const pButton =
            GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()
            ->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
    if(presentLocation.Length() > Parameter::Get().TranslationThreshold()
            || (Debug::Parameter::Get().isTranslationTriggerEnable
                        && pButton->IsHold(gfl2::ui::BUTTON_L)))
#else
    if(presentLocation.Length() > Parameter::Get().TranslationThreshold())
#endif// PM_DEBUG
    {
      m_IsInProgress = true;
      m_FrameCount = 0;
      m_Displacement = - presentLocation;
      ARAI_PRINT("Prepare Translation: (%+.5f, %+.5f, %+.5f) Length=%.5f\n",
                  m_Displacement.GetX(),
                  m_Displacement.GetY(),
                  m_Displacement.GetZ(),
                  m_Displacement.Length());
      // prepare effect translation
      m_pManager->EffectManager().PrepareTranslation(
                  m_Displacement);
    }
  }
  else
  {// is in progress
    ++m_FrameCount;
    if(m_FrameCount > Parameter::Get().TranslationWaitFrame())
    {
      ARAI_PRINT("Execute Translation: (%+.5f, %+.5f, %+.5f) Length=%.5f\n",
                  m_Displacement.GetX(),
                  m_Displacement.GetY(),
                  m_Displacement.GetZ(),
                  m_Displacement.Length());
      m_pManager->RailManager().Translation(m_Displacement);
      m_pManager->GimmickManager().Translation(m_Displacement);
      m_pManager->PlayerManager().Translation(m_Displacement);
      m_pCamera->Translation(m_Displacement);
      m_pManager->EffectManager().ExecuteTranslation();
      // reset parameter
      m_IsInProgress = false;
      m_FrameCount = 0;
      m_Displacement = gfl2::math::Vector3::GetZero();
    }
  }
}
}// namespace Run
}// namespace LunaSolDive
