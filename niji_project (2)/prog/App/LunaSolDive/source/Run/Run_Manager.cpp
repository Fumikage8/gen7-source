//======================================================================
/**
 * @file Run_Manager.cpp
 * @date 2016/09/21 11:55:13
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <math/include/gfl2_Quaternion.h>
#include <GameSys/include/GameManager.h>
#include "./Aim.h"
#include "./Beginning.h"
#include "./Ending.h"
#include "./Energy.h"
#include "./Manager.h"
#include "./Parameter.h"
#include "./Result.h"
#include "./Translation.h"
#include "../Assert.h"
#include "../Manager.h"
#include "../Camera/Main.h"
#include "../Effect/Manager.h"
#include "../Gimmick/Action.h"
#include "../Gimmick/Manager.h"
#include "../Player/Manager.h"
#include "../Rail/Manager.h"
#ifdef GF_PLATFORM_CTR
#include "./InputCTR.h"
#endif // GF_PLATFORM_CTR
#ifdef GF_PLATFORM_WIN32
#include "./InputWin32.h"
#endif // GF_PLATFORM_WIN32
#if PM_DEBUG
#include "../Debug/DrawUtil.h"
#include "../Debug/ManualCamera.h"
#include "../Debug/Parameter.h"
#include "../Debug/Timer.h"
#endif// PM_DEBUG

namespace LunaSolDive{
namespace Run{
// static function
static u32 DriftingMovementFrames = 80;
static u32 DriftingArriveFrame = 10;
static f32 DriftingMaxMovement = 0.03f;
struct ProgressParameter
{
  bool is_drifting;
  f32 forward;
  gfl2::math::Vector2 cross_section_point;
};
// Progress
static ProgressParameter Progress(
            const Energy& energy,
            const Aim& aim,
            const Rail::Location& nearestDistortionLocation,
            const Rail::Location& playerLocation);
// Camera Location
static Rail::Location CameraLocation(
            const LunaSolDive::Manager& manager);
// constructor
Manager::Manager()
    : m_pManager(NULL),
      m_State(STATE__NONE),
      m_pInput(NULL),
      m_pAim(NULL),
      m_pMainCamera(NULL),
      m_pEnergy(NULL),
      m_pResult(NULL),
      m_pBeginningStage(NULL),
      m_pEndingStage(NULL),
      m_pTranslationManager(NULL){}
// Initilaize
void Manager::Initialize(
            const Version& version,
            const Maneuver& maneuver,
            gfl2::heap::HeapBase* pHeap,
            LunaSolDive::Manager* pManager,
            GameSys::GameManager* pGameManager,
            const Rail::Manager* pRailManager,
            Effect::Manager* pEffectManager)
{
  LUNA_SOL_DIVE_ASSERT(pManager != NULL);
  LUNA_SOL_DIVE_ASSERT(pGameManager != NULL);
  LUNA_SOL_DIVE_ASSERT(m_pManager == NULL);
  // set Manager pointer
  m_pManager = pManager;
  // Initialize InputManager
#ifdef GF_PLATFORM_CTR
  m_pInput = GFL_NEW(pHeap) InputCTR();
#endif // GF_PLATFORM_CTR
#ifdef GF_PLATFORM_WIN32
  m_pInput = GFL_NEW(pHeap) InputWin32();
#endif // GF_PLATFORM_WIN32
  m_pInput->Initialize(
              maneuver,
              pHeap,
              pGameManager->GetUiDeviceManager());
  // initialize Aim
  m_pAim = GFL_NEW(pHeap) Aim();
  m_pAim->Initialize();
  // initialize Camera
  m_pMainCamera = GFL_NEW(pHeap) Camera::Main();
  LUNA_SOL_DIVE_ASSERT(m_pMainCamera != NULL);
  m_pMainCamera->Initialize(
              pRailManager,
              pEffectManager);
  // initialize Energy
  m_pEnergy = GFL_NEW(pHeap) Energy();
  // initialize Result
  m_pResult = GFL_NEW(pHeap) Run::Result();
  // initialize Beginning Stage
  m_pBeginningStage = GFL_NEW(pHeap) Beginning();
  m_pBeginningStage->Initialize(
              version,
              pHeap,
              pManager);
  // initialize Ending Stage
  m_pEndingStage = GFL_NEW(pHeap) Ending();
  m_pEndingStage->Initialize(
              version,
              pHeap,
              pManager,
              m_pMainCamera);
  // initialize Translation Manager
  m_pTranslationManager = GFL_NEW(pHeap) Translation();
  m_pTranslationManager->Initialize(
              pManager,
              m_pMainCamera);
  // set State
  m_State = STATE__BEGINNING;
}
// Terminate
void Manager::Terminate()
{
  // unset pointer
  m_pManager = NULL;
  // delete Input
  if(m_pInput != NULL)
  {
    m_pInput->Terminate();
    GFL_DELETE(m_pInput);
    m_pInput = NULL;
  }
  // delete Aim
  if(m_pAim != NULL)
  {
    m_pAim->Terminate();
    GFL_DELETE(m_pAim);
    m_pAim = NULL;
  }
  // delete Delay Trace Camera
  if(m_pMainCamera != NULL)
  {
    m_pMainCamera->Terminate();
    GFL_DELETE(m_pMainCamera);
    m_pMainCamera = NULL;
  }
  // delete Energy
  if(m_pEnergy != NULL)
  {
    m_pEnergy->Terminate();
    GFL_DELETE(m_pEnergy);
    m_pEnergy = NULL;
  }
  // delete Result
  if(m_pResult != NULL)
  {
    m_pResult->Terminate();
    GFL_DELETE(m_pResult);
    m_pResult = NULL;
  }
  // delete Beginning Stage
  if(m_pBeginningStage != NULL)
  {
    m_pBeginningStage->Terminate();
    GFL_DELETE(m_pBeginningStage);
    m_pBeginningStage = NULL;
  }
  // delete Ending Stage
  if(m_pEndingStage != NULL)
  {
    m_pEndingStage->Terminate();
    GFL_DELETE(m_pEndingStage);
    m_pEndingStage = NULL;
  }
  // delete Translation Manager
  if(m_pTranslationManager != NULL)
  {
    m_pTranslationManager->Terminate();
    GFL_DELETE(m_pTranslationManager);
    m_pTranslationManager = NULL;
  }
  // reset parameter
  m_State = STATE__NONE;
}
// Create
void Manager::Create(
            gfl2::heap::HeapBase* pHeap)
{
  m_pEndingStage->Create(pHeap);
}
// Set Up
void Manager::SetUp()
{
  m_pEnergy->Initialize(
              Parameter::Get().EnergyInitialRate());
  m_pBeginningStage->SetUp();
}
// Update
void Manager::Update()
{
  LUNA_SOL_DIVE_ASSERT(m_State != STATE__NONE);
#if PM_DEBUG
  // manual camera
  if(Debug::Parameter::Get().isManualCameraEnable
          && m_pInput->Button().IsTrigger(gfl2::ui::BUTTON_L))
  {
    Debug::Parameter::Get().isManualCameraBeingUsed =
            ! Debug::Parameter::Get().isManualCameraBeingUsed;
    if(Debug::Parameter::Get().isManualCameraBeingUsed)
    {
      Debug::Parameter::Get().pManualCamera->Initialize(
                  m_pManager->PlayerManager().Location());
    }
    else
    {
      Debug::Parameter::Get().pManualCamera->Terminate();
    }
  }
  if(Debug::Parameter::Get().isManualCameraEnable
          && Debug::Parameter::Get().isManualCameraBeingUsed)
  {
    Debug::Parameter::Get().pManualCamera->Update(
                m_pInput->Button(),
                gfl2::math::Vector2(
                            m_pInput->Stick().GetX(),
                            m_pInput->Stick().GetY()));
    if(! m_pInput->Button().IsTrigger(gfl2::ui::BUTTON_R)) return;
  }
  // reset DrawUtilData
  Debug::Parameter::Get().pDrawUtil->Data().Reset();
#endif// OPM_DEBUG
  if(m_State == STATE__BEGINNING)
  {// Beginning Stage
    m_pBeginningStage->Update();
    if(m_pBeginningStage->IsFinished())
    {
      ARAI_PRINT("Beginning Stage End\n");
      m_State = STATE__MAIN;
      // set up
      m_pInput->SetInitialDirection();
      m_pMainCamera->SetUp(
                  CameraLocation(*m_pManager));
    }
    return;
  }
  else if(m_State == STATE__ENDING || m_State == STATE__GAME_OVER)
  {// Ending Stage
    m_pEndingStage->Update();
    if(m_pEndingStage->IsFinished()){
      m_State = STATE__GAME_OVER;
    }
#if PM_DEBUG
    // restart
    if(IsGameOver() && Debug::Parameter::Get().isDebugMode)
    {
      if(m_pInput->Button().IsHold(gfl2::ui::BUTTON_B))
      {
        m_State = STATE__MAIN;
        m_pEndingStage->Quit();
        m_pManager->PlayerManager().Restart();
        m_pResult->Terminate();
      }
    }
#endif// PM_DEBUG
    return;
  }
  // update InputManger
  m_pInput->Update();
  // previous frameData
  const Rail::Location previousLocation =
          m_pManager->RailManager().Location();
  const Rail::Directions previousHeroSpine2 =
          m_pManager->PlayerManager().HeroSpine2();
  // update Aim
  m_pAim->Update(
              m_pInput->Aim(),
              m_pEnergy->VerticalMovingRate(),
              m_pManager->GimmickManager().Action());
  // progress Rail
  const ProgressParameter progressParameter = Progress(
              *m_pEnergy,
              *m_pAim,
              m_pManager->GimmickManager().Action().NearestDistortionLocation(),
              previousLocation);
  if(progressParameter.is_drifting)
  {
    m_pAim->OverwritePoint(progressParameter.cross_section_point);
    m_pMainCamera->StartGravityVibration();
  }
  else
  {
    m_pMainCamera->StopGravityVibration();
  }
  m_pManager->RailManager().Progress(
              progressParameter.forward,
              progressParameter.cross_section_point);
  const Rail::Location presentLocation =
          m_pManager->RailManager().Location();
  // update Rail
  m_pManager->RailManager().Update();
  // update Player location
  m_pManager->PlayerManager().UpdateMain(
              presentLocation.Directions(),
              *m_pEnergy,
              *m_pAim,
              m_pInput->ResetSignal());
  // update Gimmick
  m_pManager->GimmickManager().Update(
              presentLocation,
              m_pManager->PlayerManager().HeroSpine2(),
              previousHeroSpine2);
  // update Energy
  m_pEnergy->Update(
              m_pManager->GimmickManager().Action().EnergyGain(),
              m_pManager->PlayerManager().IsDamaged()
                  ? 0.0f
                  : m_pManager->GimmickManager().Action().EnergyLoss());
  // get energy
  if(m_pManager->GimmickManager().Action().IsCollidedEnergy())
  {
    if(! m_pManager->PlayerManager().IsDamaged())
    {
      m_pManager->PlayerManager().PowerUp(
                  m_pEnergy->IsOverLimit());
    }
    m_pMainCamera->Accelerate();
  }
  // damage
  if(m_pManager->GimmickManager().Action().IsCollidedObstacle())
  {
    if(! m_pManager->PlayerManager().IsDamaged())
    {
      m_pManager->PlayerManager().Damage();
      m_pMainCamera->Damage();
    }
    m_pMainCamera->Decelerate();
  }
  // Update camera
  m_pMainCamera->Update(
              CameraLocation(*m_pManager));
  // update Player::EnergyAura
  m_pManager->PlayerManager().UpdateEnergy(
              m_pEnergy->Rate());
  m_pManager->PlayerManager().UpdateEnergyAura(
              m_pMainCamera->Directions().Point());
  // translation
  m_pTranslationManager->Update();
  // game over check
  if(m_pManager->GimmickManager().Action().IsCollidedDistortion())
  {
    m_State = STATE__ENDING;
    // result
    m_pResult->Initialize(
                m_pManager->GimmickManager().Action().DistortionType(),
                m_pManager->GimmickManager().Action().DistortionRarity(),
                m_pManager->GimmickManager().Action().CollidedDistortionLocation().TravelDistance());
    // set up ending stage
    m_pEndingStage->SetUp(
                m_pEnergy->Speed(),
                m_pManager->GimmickManager().Action().CollidedDistortionLocation());
    m_pManager->PlayerManager().ChangeMainToEnding();
  }
  // update effect manager
  m_pManager->EffectManager().Update();
#if PM_DEBUG
  // update Debug Parameter
  Debug::Parameter::Get().travelDistance = presentLocation.TravelDistance();
  Debug::Parameter::Get().energy = m_pEnergy->Rate();
  Debug::Parameter::Get().energyLimit = m_pEnergy->Limit();
  Debug::Parameter::Get().forwardSpeed = m_pEnergy->Speed();
  Debug::Parameter::Get().verticalSpeed = m_pEnergy->VerticalMovingRate();
  Debug::Parameter::Get().aimInput = m_pAim->Input();
  Debug::Parameter::Get().aimPoint = m_pAim->Point();
  // update Debug::Timer
  Debug::Parameter::Get().pTimer->Update();
  // update Debug::DrawUtilData
  Debug::Parameter::Get().pDrawUtil->Data().PresentLocation(
              presentLocation);
  Debug::Parameter::Get().pDrawUtil->Data().InputPoint(
              m_pManager->RailManager().Location(
                          0.0f,
                          m_pAim->Input()).Point());
  Debug::Parameter::Get().pDrawUtil->Data().CollisionPoint(
              m_pManager->PlayerManager().HeroSpine2().Point());
#endif// PM_DEBUG
}
// Camera
const Camera::Base& Manager::Camera() const
{
#if PM_DEBUG
  if(Debug::Parameter::Get().isManualCameraEnable
          && Debug::Parameter::Get().isManualCameraBeingUsed)
  {
    LUNA_SOL_DIVE_ASSERT(Debug::Parameter::Get().pManualCamera != NULL);
    return *Debug::Parameter::Get().pManualCamera;
  }
#endif// PM_DEBUG
  LUNA_SOL_DIVE_ASSERT(m_pMainCamera != NULL);
  LUNA_SOL_DIVE_ASSERT(m_State != STATE__NONE);
  return (m_State == STATE__BEGINNING)
          ? m_pBeginningStage->Camera()
          : *m_pMainCamera;
}
// Result
const Result& Manager::Result() const
{
  LUNA_SOL_DIVE_ASSERT(m_pResult != NULL);
  return *m_pResult;
}
// Whether the game is over or not
bool Manager::IsGameOver() const
{
  return m_State == STATE__GAME_OVER;
}
// static function
// Progress
static ProgressParameter Progress(
            const Energy& energy,
            const Aim& aim,
            const Rail::Location& nearestDistortionLocation,
            const Rail::Location& playerLocation)
{
  LUNA_SOL_DIVE_ASSERT(energy.Speed() > 0.0f);
  // whether energy is empty or not
  // whether the location of nearest distortion is valid
  if(energy.IsEmpty()
          && nearestDistortionLocation.IsValid())
  {
    // distance between player and nearest distortion
    const f32 forwardDistance =
            nearestDistortionLocation.TravelDistance()
            - playerLocation.TravelDistance();
    LUNA_SOL_DIVE_ASSERT(forwardDistance >= 0.0f);
    // number of frames required to reach
    const u32 remainingFrames =
            (energy.Speed() > 0.0f)
            ? std::ceil(forwardDistance / energy.Speed())
            : 0xffffffff;
    // point on the cross section
    const gfl2::math::Vector2 destinationPoint =
            nearestDistortionLocation.NormalizedCrossSectionPoint();
    const gfl2::math::Vector2 presentPoint =
            playerLocation.NormalizedCrossSectionPoint();
    // calculate movement on the cross section
    const gfl2::math::Vector2 pointMovement =
            (remainingFrames > DriftingArriveFrame)
            ? (destinationPoint - presentPoint)
                    / (remainingFrames - DriftingArriveFrame)
            : (destinationPoint - presentPoint);
    if(remainingFrames <= DriftingMovementFrames
            && pointMovement.Length() < DriftingMaxMovement)
    {
      const ProgressParameter result = {
                  true,
                  energy.Speed(),
                  presentPoint + pointMovement};
      return result;
    }
  }
  const ProgressParameter result ={
              false,
              energy.Speed(),
              aim.Point()};
  return result;
}
// Camera Location
static Rail::Location CameraLocation(
            const LunaSolDive::Manager& manager)
{
  return Rail::Location(
              manager.RailManager().Location().TravelDistance(),
              manager.PlayerManager().OriginLocation(),
              manager.RailManager().Location().Rail(),
              manager.RailManager().Location().RailRadius());
}
}// namespace Run
}// namespace LunaSolDive
