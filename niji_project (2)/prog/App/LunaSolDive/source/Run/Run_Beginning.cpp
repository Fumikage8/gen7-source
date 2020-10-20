//======================================================================
/**
 * @file Run_Beginning..cpp
 * @date 2017/01/06 15:12:35
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <algorithm>
#include "./Beginning.h"
#include "./Parameter.h"
#include "../Manager.h"
#include "../Assert.h"
#include "../Camera/Beginning.h"
#include "../Effect/Manager.h"
#include "../Player/Manager.h"
#include "../Rail/Manager.h"

namespace LunaSolDive{
namespace Run{
// constructor
Beginning::Beginning()
    : m_pManager(NULL),
      m_pCamera(NULL),
      m_IsEnergyAuraActivated(false){}
// Initialize
void Beginning::Initialize(
            const Version& version,
            gfl2::heap::HeapBase* pHeap,
            LunaSolDive::Manager* pManager)
{
  // set pointer
  m_pManager = pManager;
  LUNA_SOL_DIVE_ASSERT(m_pManager != NULL);
  // initialize Camera
  m_pCamera = GFL_NEW(pHeap) Camera::Beginning();
  LUNA_SOL_DIVE_ASSERT(m_pCamera != NULL);
  m_pCamera->Initialize(
              version);
}
// Terminate
void Beginning::Terminate()
{
  // delete Camera
  if(m_pCamera != NULL)
  {
    m_pCamera->Terminate();
    GFL_DELETE(m_pCamera);
    m_pCamera = NULL;
  }
  // unset pointer
  m_pManager = NULL;
  // reset paramter
  m_PlayerLocation.Terminate();
  m_IsEnergyAuraActivated = false;
}
// Set Up
void Beginning::SetUp()
{
  // initialize location
  m_PlayerLocation = m_pManager->RailManager().InitialDirections();
  m_PlayerLocation.Translation(
              m_PlayerLocation.Forward() * - Parameter::Get().BeginningDistance());
  m_pManager->PlayerManager().UpdateBeginning(
              m_PlayerLocation);
  // set up camera
  m_pCamera->Update(
              m_pManager->PlayerManager().OriginLocation());
}
// Update
void Beginning::Update()
{
  // speed
  const f32 speed = Parameter::Get().BeginningDistance() / m_pCamera->TotalFrameNumber();
  // update location
  m_PlayerLocation.Translation(
              m_PlayerLocation.Forward() * speed);
  // update rail
  m_pManager->RailManager().Update();
  // update player
  m_pManager->PlayerManager().UpdateBeginning(
              m_PlayerLocation);
  // update camera
  m_pCamera->Update(
              m_pManager->PlayerManager().OriginLocation());
  // update energy aura
  if(m_pCamera->IsZoomIn())
  {
    if(! m_IsEnergyAuraActivated)
    {
      m_pManager->PlayerManager().ActivateEnergyAura();
      m_IsEnergyAuraActivated = true;
    }
    m_pManager->PlayerManager().UpdateEnergyAura(
                m_pCamera->Directions().Point());
  }
  // update effect manager
  m_pManager->EffectManager().Update();
}
// Whether this stage has been finished
bool Beginning::IsFinished() const
{
  LUNA_SOL_DIVE_ASSERT(m_pCamera != NULL);
  return m_pCamera->IsFinished();
}
// Camera
const Camera::Base& Beginning::Camera() const
{
  LUNA_SOL_DIVE_ASSERT(m_pCamera != NULL);
  return *m_pCamera;
}
}// namespace Run
}// namespace LunaSolDive
