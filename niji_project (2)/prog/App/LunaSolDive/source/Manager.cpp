//======================================================================
/**
 * @file Manager.cpp
 * @date 2016/09/07 12:57:48
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <debug/include/gfl2_DebugPrint.h>
#include <Fade/include/gfl2_FadeManager.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include "App/AppEvent/include/LunaSolDive/LunaSolDiveParam.h"
#include "Applib/include/Util/AppRenderingManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/GameManager.h"
#include "System/include/nijiAllocator.h"
#include "./Assert.h"
#include "./Light.h"
#include "./Manager.h"
#include "./RenderingPipeline.h"
#include "./Archive/Chara.h"
#include "./Archive/Effect.h"
#include "./Archive/Model.h"
#include "./Camera/Base.h"
#include "./Camera/Parameter.h"
#include "./Effect/Manager.h"
#include "./Gimmick/Action.h"
#include "./Gimmick/Manager.h"
#include "./Gimmick/Parameter.h"
#include "./Gimmick/Placement/Parameter.h"
#include "./Player/Manager.h"
#include "./Player/Parameter.h"
#include "./Rail/Manager.h"
#include "./Rail/Parameter.h"
#include "./Run/Manager.h"
#include "./Run/Parameter.h"
#include "./Run/Result.h"
#include "./Sound/Manager.h"
#include "./Sound/Parameter.h"
#include "./UI/DiveUI.h"
#if PM_DEBUG
#include "./Debug/DrawUtil.h"
#include "./Debug/Menu.h"
#include "./Debug/Parameter.h"
#endif// PM_DEBUG

namespace LunaSolDive{
// parameter
static const u32 MAIN_HEAP_SIZE = 0x01800000;// 24Mi byte
static const u32 EFFECT_HEAP_SIZE = 0x00700000;// 7Mi byte
#if PM_DEBUG
// static function
// show heap status
static void ShowHeapStatus(
            const char* name,
            gfl2::heap::HeapBase* pHeap);
// get default dress up parameter
static poke_3d::model::DressUpParam DefaultDressUpParameter(
            const HeroSex& sex);
#endif// PM_DEBUG
// constructor
Manager::Manager()
    : m_Version(VERSION__NONE),
      m_Sex(HERO_SEX__NONE),
      m_Mode(MODE__NONE),
      m_Maneuver(MANEUVER__NONE),
      m_pHeap(NULL),
      m_pEffectHeap(NULL),
      m_pGameManager(NULL),
      m_pAppRenderingManager(NULL),
      m_PresentStep(STEP__NONE),
      m_pAllocator(NULL),
      m_pRenderingPipeline(NULL),
      m_pLight(NULL),
      m_pRunManager(NULL),
      m_pEffectManager(NULL),
      m_pPlayerManager(NULL),
      m_pRailManager(NULL),
      m_pGimmickManager(NULL),
      m_pUI(NULL),
      m_ProcessCounter(0),
      m_SubProcessCounter(0),
      m_pIOParameter(NULL)
{
#if PM_DEBUG
  m_HeapInitialFreeSize = 0;
  m_EffectHeapInitialFreeSize = 0;
  m_pDebugMenu = NULL;
  m_IsDebugMode = false;
#endif// PM_DEBUG
}
// Initialize
void Manager::Initialize(
            gfl2::heap::HeapBase* pHeap,
            GameSys::GameManager* pGameManager,
            app::util::AppRenderingManager* pAppRenderingManager)
{
  // create Local Heap
  LUNA_SOL_DIVE_ASSERT(pHeap != NULL);
  m_pHeap = GFL_CREATE_LOCAL_HEAP(
              pHeap,
              MAIN_HEAP_SIZE,
              gfl2::heap::HEAP_TYPE_EXP,
              false);
  LUNA_SOL_DIVE_ASSERT(m_pHeap != NULL);
  m_pEffectHeap = GFL_CREATE_LOCAL_HEAP(
              pHeap,
              EFFECT_HEAP_SIZE,
              gfl2::heap::HEAP_TYPE_EXP,
              false);
  LUNA_SOL_DIVE_ASSERT(m_pEffectHeap != NULL);
#if PM_DEBUG
  ShowHeapStatus("Main", m_pHeap);
  ShowHeapStatus("Effect", m_pEffectHeap);
  m_HeapInitialFreeSize = m_pHeap->GetTotalFreeSize();
  m_EffectHeapInitialFreeSize = m_pEffectHeap->GetTotalFreeSize();
#endif// PM_DEBUG
  // set pointer
  m_pGameManager = pGameManager;
  LUNA_SOL_DIVE_ASSERT(m_pGameManager != NULL);
  m_pAppRenderingManager = pAppRenderingManager;
  LUNA_SOL_DIVE_ASSERT(pAppRenderingManager);
  // load game data
  const GameSys::GameData* const pGameData = m_pGameManager->GetGameData();
  LUNA_SOL_DIVE_ASSERT(pGameData != NULL);
  // version
  // @fix 社外要望: ソルガレオ/ルナアーラをversion固定からflagによる切替に変更
  m_Version =
          pGameData->GetEventWork()->CheckEventFlag(SYS_FLAG_LSDIVE_RIDE_SOL)
          ? VERSION__SOL
          : VERSION__LUNA;
  // load player status
  const Savedata::MyStatus* const pStatus = pGameData->GetPlayerStatusConst();
  LUNA_SOL_DIVE_ASSERT(pStatus != NULL);
  m_Sex = (pStatus->GetSex() == PM_MALE)
          ? HERO_SEX__MALE
          : HERO_SEX__FEMALE;
  // Mode
  m_Mode =
          (m_pIOParameter != NULL && m_pIOParameter->in_first_mode)
          ? MODE__FIRST
          : MODE__NORMAL;
  // maneuver
  m_Maneuver =
          (pGameData->GetEventWork()->CheckEventFlag(SYS_FLAG_LSDIVE_STICK_ENABLED) == 0)
          ? MANEUVER__GYROSCOPE
          : MANEUVER__STICK;
#if PM_DEBUG
  if(m_IsDebugMode)
  {// overwrite version, sex, mode, maneuver
    const gfl2::ui::Button* const pButton =
            m_pGameManager->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
    m_Version =
            pButton->IsHold(gfl2::ui::BUTTON_L)
            ? VERSION__SOL
            : VERSION__LUNA;
    m_Sex =
            pButton->IsHold(gfl2::ui::BUTTON_R)
            ? HERO_SEX__MALE
            : HERO_SEX__FEMALE;
    m_Mode =
            pButton->IsHold(gfl2::ui::BUTTON_X)
            ? MODE__FIRST
            : MODE__NORMAL;
    m_Maneuver = MANEUVER__GYROSCOPE;
  }
  // Initialize Debug Parameter
  Debug::Parameter::Initialize();
  Debug::Parameter::Get().isDebugMode = m_IsDebugMode;
  if(m_IsDebugMode)
  {
    Debug::Parameter::Get().isManuallyEnergyUpDownEnabled = true;
    Debug::Parameter::Get().isRandomDressUp = true;
  }
#endif// PM_DEBUG
  // initialize Parameter
  Camera::Parameter::Initialize(m_pHeap);
  Gimmick::Parameter::Initialize(m_pHeap);
  Gimmick::Placement::Parameter::Initialize(m_pHeap);
  Player::Parameter::Initialize(m_pHeap);
  Rail::Parameter::Initialize(m_pHeap);
  Run::Parameter::Initialize(m_pHeap);
  Sound::Parameter::Initialize(m_pHeap);
#if PM_DEBUG
  // Initialize Debug Menu
  gfl2::heap::HeapBase* const pDebugHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);
  LUNA_SOL_DIVE_ASSERT(pDebugHeap != NULL);
  m_pDebugMenu = GFL_NEW(pDebugHeap) Debug::Menu();
  m_pDebugMenu->Initialize();
#endif// PM_DEBUG
  // initialize Archive
  Archive::Effect::Initialize(
              m_pHeap,
              m_pGameManager->GetAsyncFileManager());
  Archive::Chara::Initialize(
              m_pHeap,
              m_pGameManager->GetAsyncFileManager());
  Archive::Model::Initialize(
              m_pHeap,
              m_pGameManager->GetAsyncFileManager());
  // initialize Allocator
  m_pAllocator = GFL_NEW(m_pHeap) System::nijiAllocator(m_pHeap);
  // initialize RnderingPipeline
  m_pRenderingPipeline = GFL_NEW(m_pHeap) RenderingPipeline();
  m_pRenderingPipeline->Initialize(
              m_pGameManager->GetAsyncFileManager(),
              m_pHeap,
              m_pAllocator);
  // initialize Light
  m_pLight = GFL_NEW(m_pHeap) Light();
  m_pLight->Initialize(
              m_pAllocator,
              m_pRenderingPipeline);
  // initialize Effect Manager
  m_pEffectManager = GFL_NEW(m_pHeap) Effect::Manager();
  m_pEffectManager->Initialize(
              m_pHeap,
              m_pEffectHeap,
              m_pRenderingPipeline);
  // initialize RailManager
  m_pRailManager = GFL_NEW(m_pHeap) Rail::Manager();
  LUNA_SOL_DIVE_ASSERT(m_pRailManager != NULL);
  m_pRailManager->Initialize(
              m_pHeap,
              m_pRenderingPipeline,
              m_pEffectManager);
  // initialize Gimmick Manager
  m_pGimmickManager = GFL_NEW(m_pHeap) Gimmick::Manager();
  LUNA_SOL_DIVE_ASSERT(m_pGimmickManager != NULL);
  m_pGimmickManager->Initialize(
              m_pHeap,
              m_Mode,
              m_pEffectManager,
              m_pRailManager,
              m_pRenderingPipeline);
  // initialize Player Manager
  m_pPlayerManager =GFL_NEW(m_pHeap) Player::Manager();
  LUNA_SOL_DIVE_ASSERT(m_pPlayerManager != NULL);
  m_pPlayerManager->Initialize(
              m_Version,
              m_Sex,
              m_pHeap,
              m_pRenderingPipeline,
              m_pEffectManager);
  // initialize Run Manager
  m_pRunManager = GFL_NEW(m_pHeap) Run::Manager();
  m_pRunManager->Initialize(
              m_Version,
              m_Maneuver,
              m_pHeap,
              this,
              m_pGameManager,
              m_pRailManager,
              m_pEffectManager);
  // initialize Sound Manager
  Sound::Manager::Initialize(
              pHeap,
              m_Version);
  // initialize UI
  m_pUI = GFL_NEW(m_pHeap) UI::DiveUI(
              m_pHeap,
              m_pAppRenderingManager);
  // change Step
  m_PresentStep = STEP__INITIALIZING;
}
// progress the Initializing Process
bool Manager::ProgressInitializingProcess()
{
  ARAI_PRINT("Initializing Process: %d\n", m_ProcessCounter);
  switch(m_ProcessCounter)
  {
  case 0:
#if PM_DEBUG
    if(m_IsDebugMode)
    {
      const gfl2::ui::Button* const pButton =
              GFL_SINGLETON_INSTANCE(GameSys::GameManager)
              ->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
      if(! pButton->IsTrigger(gfl2::ui::BUTTON_START))
      {
        break;
      }
    }
    // update debug menu
    m_pDebugMenu->Update();
#endif// PM_DEBUG
    // open archive
    Archive::Effect::Get().Open();
    Archive::Chara::Get().Open();
    Archive::Model::Get().Open();
    // increment counter
    ++m_ProcessCounter;
  case 1:
    // check Whether archives have been opened or not
    if(! Archive::Effect::Get().IsOpened()
            || ! Archive::Chara::Get().IsOpened()
            || ! Archive::Model::Get().IsOpened()) break;
    // load archive
    Archive::Effect::Get().LoadAll();
    Archive::Chara::Get().LoadAll();
    Archive::Model::Get().LoadAll();
    // increment counter
    ++m_ProcessCounter;
  case 2:
    // check Whether archive date have been opened or not
    if(! Archive::Effect::Get().IsAllLoaded()
            || ! Archive::Chara::Get().IsAllLoaded()
            || ! Archive::Model::Get().IsAllLoaded()) break;
    // close archive
    Archive::Effect::Get().Close();
    Archive::Chara::Get().Close();
    Archive::Model::Get().Close();
    // increment counter
    ++m_ProcessCounter;
  case 3:
    // check Whether archives have been closed or not
    if(! Archive::Effect::Get().IsClosed()
            || ! Archive::Chara::Get().IsClosed()
            ||! Archive::Model::Get().IsClosed()) break;
    // increment counter
    ++m_ProcessCounter;
  case 4:
    if(! m_pUI->ObserveSetup()) break;
    // increment counter
    ++m_ProcessCounter;
  case 5:
    // set up
    m_pEffectManager->SetUp();
    m_pRailManager->Create(
                m_pHeap,
                m_pAllocator);
    m_pGimmickManager->Create(
                m_pHeap,
                m_pAllocator);
    m_pPlayerManager->Create(
                m_pHeap,
                m_pAllocator,
                m_pGameManager->GetAsyncFileManager(),
#if PM_DEBUG
                m_IsDebugMode
                    ? DefaultDressUpParameter(m_Sex)
                    : m_pGameManager->GetGameData()->GetPlayerStatusConst()->GetDressup());
#else
                m_pGameManager->GetGameData()->GetPlayerStatusConst()->GetDressup());
#endif// PM_DEBUG
    m_pRunManager->Create(
                m_pHeap);
    // initial Travarse Scene
    gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();
    // set up
    m_pRunManager->SetUp();
    // finish
    m_ProcessCounter = 0;
    ARAI_PRINT("Initialize Process: end\n");
    return true;
  }
  return false;
}
// progress the Fade In Process
bool Manager::ProgressFadeInProcess()
{
  ARAI_PRINT("Fade In Process: %d\n", m_ProcessCounter);
#if PM_DEBUG
  if(m_IsDebugMode)
  {
    Sound::Manager::Get().StartBGM();
    return true;
  }
#endif// PM_DEBUG
  const u32 fadeInFrames = 8;
  gfl2::Fade::FadeManager* const pFadeManager =
          GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
  switch(m_ProcessCounter)
  {
  case 0:
    // start BGM
    Sound::Manager::Get().StartBGM();
    // request Fade In
    pFadeManager->RequestIn(
                gfl2::Fade::DISP_DOUBLE,
                gfl2::Fade::FADE_TYPE_ALPHA,
                fadeInFrames);
    // increment counter
    ++m_ProcessCounter;
    break;
  case 1:
    // check whether fade in has been finished
    if(! pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE)) break;
    // finish
    m_ProcessCounter = 0;
    return true;
  }
  return false;
}
// progress the Fade Out Process
bool Manager::ProgressFadeOutProcess()
{
  ARAI_PRINT("Fade Out Process: %d\n", m_ProcessCounter);
#if PM_DEBUG
  if(m_IsDebugMode)
  {
    Sound::Manager::Get().FadeOutBGM();
    return true;
  }
#endif// PM_DEBUG
  const u32 whiteFadeOutFrames = 20;
  const u32 blackFadeOutFrames = 8;// @fix MMCat[652]: 4->8に変更
  const u32 whiteWaitFrames = 3;
  gfl2::Fade::FadeManager* const pFadeManager =
          GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
  static const gfl2::math::Vector4 whiteBegin(255.0f, 255.0f, 255.0f, 0.0f);
  static const gfl2::math::Vector4 whiteEnd(255.0f, 255.0f, 255.0f, 255.0f);
  static const gfl2::math::Vector4 blackEnd(0.0f, 0.0f, 0.0f, 255.0f);
  switch(m_ProcessCounter)
  {
  case 0:
    // fade out BGM
    Sound::Manager::Get().FadeOutBGM();
    // request white fade out
    pFadeManager->RequestOut(
                gfl2::Fade::DISP_DOUBLE,
                gfl2::Fade::FADE_TYPE_ALPHA,
                &whiteBegin,
                &whiteEnd,
                whiteFadeOutFrames);
    // increment counter
    ++m_ProcessCounter;
    break;
  case 1:
    // check whether fade out has been finished
    if(! pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE)) break;
    // increment counter
    ++m_ProcessCounter;
    // reset sub process counter
    m_SubProcessCounter = 0;
    // fall through
  case 2:
    ++m_SubProcessCounter;
    if(m_SubProcessCounter > whiteWaitFrames)
    {
      // increment counter
      ++m_ProcessCounter;
      m_SubProcessCounter = 0;
    }
    break;
  case 3:
    // request black fade out
    pFadeManager->RequestOut(
                gfl2::Fade::DISP_DOUBLE,
                gfl2::Fade::FADE_TYPE_ALPHA,
                &whiteEnd,
                &blackEnd,
                blackFadeOutFrames);
    // increment counter
    ++m_ProcessCounter;
    break;
  case 4:
    // check whether black fade out has been finished
    if(! pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE)) break;
    // finish
    m_ProcessCounter = 0;
    return true;
  }
  return false;
}
// progress the Terminate Process
bool Manager::ProgressTerminateProcess()
{
  ARAI_PRINT("Terminate Process: %d\n", m_ProcessCounter);
  switch(m_ProcessCounter)
  {
  case 0:
    // wait 1 Frame
    // incremet counter
    ++m_ProcessCounter;
    break;
  case 1:
    if(m_pUI->IsDrawing()) break;
#if PM_DEBUG
    // delete Debug Parameter
    Debug::Parameter::Terminate();
    // delete Debug Menu
    if(m_pDebugMenu != NULL)
    {
      m_pDebugMenu->Terminate();
      GFL_DELETE(m_pDebugMenu);
      m_pDebugMenu = NULL;
    }
#endif// PM_DEBUG
    // delete Light
    if(m_pLight != NULL)
    {
      m_pLight->Terminate();
      GFL_DELETE(m_pLight);
      m_pLight = NULL;
    }
    // delete UI
    GFL_SAFE_DELETE(m_pUI);
    // delete Sound Manager
    Sound::Manager::Terminate();
    // delete Gimmick Manager
    if(m_pGimmickManager != NULL)
    {
      m_pGimmickManager->Terminate();
      GFL_DELETE(m_pGimmickManager);
      m_pGimmickManager = NULL;
    }
    // delete RailManager
    if(m_pRailManager != NULL)
    {
      m_pRailManager->Terminate();
      GFL_DELETE(m_pRailManager);
      m_pRailManager = NULL;
    }
    // delete Player Manager
    if(m_pPlayerManager != NULL)
    {
      m_pPlayerManager->Terminate();
      GFL_DELETE(m_pPlayerManager);
      m_pPlayerManager = NULL;
    }
    // delete Run Manager
    if(m_pRunManager != NULL)
    {
      m_pRunManager->Terminate();
      GFL_DELETE(m_pRunManager);
      m_pRunManager = NULL;
    }
    // delete EffectManager
    if(m_pEffectManager != NULL)
    {
      m_pEffectManager->Terminate();
      GFL_DELETE(m_pEffectManager);
      m_pEffectManager = NULL;
    }
    // delete RenderingPipeline
    if(m_pRenderingPipeline != NULL)
    {
      m_pRenderingPipeline->Finalize();
      GFL_DELETE(m_pRenderingPipeline);
      m_pRenderingPipeline = NULL;
    }
    // delete Archive
    Archive::Chara::Terminate();
    Archive::Effect::Terminate();
    Archive::Model::Terminate();
    // terminate Parameter
    Camera::Parameter::Terminate();
    Gimmick::Parameter::Terminate();
    Gimmick::Placement::Parameter::Terminate();
    Player::Parameter::Terminate();
    Rail::Parameter::Terminate();
    Run::Parameter::Terminate();
    Sound::Parameter::Terminate();
    // delete Allocator
    GFL_SAFE_DELETE(m_pAllocator);
#if PM_DEBUG
    // memory leak check
    if(m_pEffectHeap != NULL)
    {
      if(m_HeapInitialFreeSize != m_pHeap->GetTotalFreeSize())
      {
        m_pHeap->Dump();
      }
      LUNA_SOL_DIVE_ASSERT(m_HeapInitialFreeSize == m_pHeap->GetTotalFreeSize());
    }
    if(m_pHeap)
    {
      if(m_EffectHeapInitialFreeSize != m_pEffectHeap->GetTotalFreeSize())
      {
        m_pEffectHeap->Dump();
      }
      LUNA_SOL_DIVE_ASSERT(m_EffectHeapInitialFreeSize == m_pEffectHeap->GetTotalFreeSize());
    }
    m_HeapInitialFreeSize = 0;
    m_EffectHeapInitialFreeSize = 0;
#endif// PM_DEBUG
    // delete Heap
    if(m_pEffectHeap != NULL)
    {
      GFL_DELETE_HEAP(m_pEffectHeap);
      m_pEffectHeap = NULL;
    }
    if(m_pHeap != NULL)
    {
      GFL_DELETE_HEAP(m_pHeap);
      m_pHeap = NULL;
    }
    // reset parameter
    m_Version = VERSION__NONE;
    m_Sex = HERO_SEX__NONE;
    m_Mode = MODE__NONE;
    m_Maneuver = MANEUVER__NONE;
    m_pGameManager = NULL;
    m_pAppRenderingManager = NULL;
    // Terminate Process Finish
    m_ProcessCounter = 0;
    m_SubProcessCounter = 0;
    return true;
  default:
    break;
  }
  return false;
}
// Update
void Manager::Update()
{
  LUNA_SOL_DIVE_ASSERT(m_PresentStep != STEP__NONE);
  // update UI
  m_pUI->Update();
  // update Main Loop
  if(m_PresentStep == STEP__WORKING
          || m_PresentStep == STEP__FADE_IN
          || m_PresentStep == STEP__FADE_OUT)
  {
    LUNA_SOL_DIVE_ASSERT(m_pRunManager != NULL);
    m_pRunManager->Update();
    Sound::Manager::Get().Update();
    // Travarse Scene
    gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();
    // Game Over check
#if PM_DEBUG
    if(! m_IsDebugMode && m_PresentStep == STEP__WORKING && m_pRunManager->IsGameOver())
#else
    if(m_PresentStep == STEP__WORKING && m_pRunManager->IsGameOver())
#endif// PM_DEBUG
    {
      m_PresentStep = STEP__FADE_OUT;
      // output Result
      const u32 distance = m_pRunManager->Result().Distance();
      const DistortionType& type = m_pRunManager->Result().Type();
      const DistortionRarity& rarity = m_pRunManager->Result().Rarity();
      if(m_pIOParameter != NULL)
      {
        // Travel Distance[m] (Rounded Up)
        m_pIOParameter->out_distance = distance;
        // output Distortion Type
        m_pIOParameter->out_distortion_type = type;
        LUNA_SOL_DIVE_ASSERT(m_pIOParameter->out_distortion_type != DISTORTION_TYPE__SIZE);
        // output Distortion Rarity
        m_pIOParameter->out_rarelity = rarity;
        LUNA_SOL_DIVE_ASSERT(m_pIOParameter->out_rarelity != DISTORTION_RARITY__NONE);
      }
    }
#if PM_DEBUG
    // update debug menu
    m_pDebugMenu->Update();
    // Quit
    if(m_IsDebugMode)
    {
      const gfl2::ui::Button* const pButton =
              GFL_SINGLETON_INSTANCE(GameSys::GameManager)
              ->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
      if(pButton->IsHold(gfl2::ui::BUTTON_L)
              && pButton->IsHold(gfl2::ui::BUTTON_START))
      {
        ARAI_PRINT("Quit LunaSolDive\n");
        m_PresentStep = STEP__FADE_OUT;
      }
    }
#endif// PM_DEBUG
  }
  if(m_PresentStep == STEP__INITIALIZING)
  {
    if(ProgressInitializingProcess())
    {
      m_PresentStep = STEP__FADE_IN;
    }
  }
  else if(m_PresentStep == STEP__FADE_IN)
  {
    if(ProgressFadeInProcess())
    {
      m_PresentStep = STEP__WORKING;
    }
  }
  else if(m_PresentStep == STEP__FADE_OUT)
  {
    if(ProgressFadeOutProcess())
    {
      m_PresentStep = STEP__TERMINATING;
    }
  }
  else if(m_PresentStep == STEP__TERMINATING)
  {
    if(ProgressTerminateProcess())
    {
      m_PresentStep = STEP__TERMINATED;
    }
  }
}
// Draw
void Manager::Draw(
            const gfl2::gfx::CtrDisplayNo& displayNo)
{
  if(m_PresentStep != STEP__WORKING
          && m_PresentStep != STEP__FADE_IN
          && m_PresentStep != STEP__FADE_OUT)
  {
    return;
  }
  if(displayNo == gfl2::gfx::CtrDisplayNo::LEFT)
  {
    const gfl2::gfx::DisplayDeviceHandle& displayHandle = System::GflUse::m_HandleList[displayNo];
    // Projection Matrix (perspective)
    m_pRenderingPipeline->SetProjectionMatrix(
                m_pRunManager->Camera().ProjectionMatrix(displayNo));
    // View Transformation Matrix
    m_pRenderingPipeline->SetViewMatrix(
                m_pRunManager->Camera().ViewMatrix());
    // Render Target
    m_pRenderingPipeline->SetRenderTarget(
                gfl2::gfx::GFGL::GetBackBuffer(displayHandle));
    // Depth Stencil
    m_pRenderingPipeline->SetDepthStencil(
                gfl2::gfx::GFGL::GetDepthStencilBuffer(displayHandle));
    // Display Size
    m_pRenderingPipeline->SetDisplaySize(
                gfl2::gfx::GFGL::GetDisplayWidth(displayHandle),
                gfl2::gfx::GFGL::GetDisplayHeight(displayHandle));
    // Draw: Display
    m_pRenderingPipeline->Update();
    m_pRenderingPipeline->Execute();
#if PM_DEBUG
    Debug::Parameter::Get().pDrawUtil->Draw(
                displayNo,
                m_pRunManager->Camera());
#endif// PM_DEBUG
  }
  else if(displayNo == gfl2::gfx::CtrDisplayNo::DOWN)
  {
    m_pUI->Draw(displayNo);
    m_pAppRenderingManager->Draw(displayNo);
  }
}
// GameManager
GameSys::GameManager& Manager::GameManager() const
{
  LUNA_SOL_DIVE_ASSERT(m_pGameManager != NULL);
  return *m_pGameManager;
}
// Allocator pointer
System::nijiAllocator* Manager::AllocatorPtr() const
{
  LUNA_SOL_DIVE_ASSERT(m_pAllocator != NULL);
  return m_pAllocator;
}
// RenderingPipeline pointer
RenderingPipeline* Manager::RenderingPipelinePtr() const
{
  LUNA_SOL_DIVE_ASSERT(m_pRenderingPipeline != NULL);
  return m_pRenderingPipeline;
}
// EffectManager
Effect::Manager& Manager::EffectManager() const
{
  LUNA_SOL_DIVE_ASSERT(m_pEffectManager != NULL);
  return *m_pEffectManager;
}
// Player Manager
Player::Manager& Manager::PlayerManager() const
{
  LUNA_SOL_DIVE_ASSERT(m_pPlayerManager != NULL);
  return *m_pPlayerManager;
}
// RailManager
Rail::Manager& Manager::RailManager() const
{
  LUNA_SOL_DIVE_ASSERT(m_pRailManager != NULL);
  return *m_pRailManager;
}
// Gimmick Manager
Gimmick::Manager& Manager::GimmickManager() const
{
  LUNA_SOL_DIVE_ASSERT(m_pGimmickManager != NULL);
  return *m_pGimmickManager;
}
// check if LunaSolDive has been Terminated
bool Manager::IsTerminated() const
{
  return m_PresentStep == STEP__TERMINATED;
}
// Set pointer to input Parameter & output the Result
void Manager::SetIOParameter(
            App::Event::LUNASOL_DIVE_PARAM* pIOParameter)
{
  m_pIOParameter = pIOParameter;
  LUNA_SOL_DIVE_ASSERT(m_pIOParameter != NULL);
}
#if PM_DEBUG
// Switch to Debug Mode
void Manager::SwitchToDebugMode()
{
  m_IsDebugMode = true;
}
// static function
// show heap status
static void ShowHeapStatus(
            const char* name,
            gfl2::heap::HeapBase* pHeap)
{
  GFL_PRINT("Heap: %s\n", name);
  GFL_PRINT("  ID: %d\n", pHeap->GetHeapId());
  GFL_PRINT("  Size: 0x%lx\n", pHeap->GetTotalSize());
  GFL_PRINT("  Free Size: 0x%lx\n", pHeap->GetTotalFreeSize());
  GFL_PRINT("  Allocatable Size: 0x%lx\n", pHeap->GetTotalAllocatableSize());
  GFL_PRINT("  Fragment : %s\n", pHeap->IsFragment()? "True": "False");
}
// get default dress up parameter
static poke_3d::model::DressUpParam DefaultDressUpParameter(
            const HeroSex& sex)
{
  poke_3d::model::DressUpParam dressUpParameter;
  System::DressUp::GetDressUpParamDefault(
              (sex == HERO_SEX__MALE)? PM_MALE: PM_FEMALE,
              &dressUpParameter);
  return dressUpParameter;
}
#endif// PM_DEBUG
}// namespace LunaSolDive
