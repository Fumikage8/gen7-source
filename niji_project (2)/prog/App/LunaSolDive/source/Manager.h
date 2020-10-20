//======================================================================
/**
 * @file Manager.h
 * @date 2016/09/07 12:57:14
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__LUNA_SOL_DIVE_MANAGER_H_INCLUDED__
#define __LUNA_SOL_DIVE__LUNA_SOL_DIVE_MANAGER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "./Version.h"

// forward declaration
namespace gfl2{
namespace gfx{
class CtrDisplayNo;
}// namespace gfx
}// namespace gfl2
namespace app{
namespace util{
class Heap;
class AppRenderingManager;
}// namespace util
}// namespace app
namespace App{
namespace Event{
struct LUNASOL_DIVE_PARAM;
}// namespace Event
}// namespace App
namespace GameSys{
class GameManager;
}// namespace GameSys
namespace System{
class nijiAllocator;
}// namespace System

namespace LunaSolDive{
// forward declaration
class Light;
class RenderingPipeline;
namespace Effect{
class Manager;
}// namespace Effect
namespace Gimmick{
class Manager;
}// namespace Gimmick
namespace Player{
class Manager;
}// namespace Player
namespace Rail{
class Manager;
}// namespace Rail
namespace Run{
class Manager;
}// namespace Run
namespace UI{
class DiveUI;
}// namespace
#if PM_DEBUG
namespace Debug{
class Menu;
}// namespace Debug
#endif// PM_DEBUG

class Manager
{
  GFL_FORBID_COPY_AND_ASSIGN(Manager);
public:
  // constructor
  Manager();
  // Initialize
  void Initialize(
              gfl2::heap::HeapBase* pHeap,
              GameSys::GameManager* pGameManager,
              app::util::AppRenderingManager* pAppRenderingManager);
  // Update
  void Update();
  // Draw
  void Draw(const gfl2::gfx::CtrDisplayNo& displayNo);
  // GameManager
  GameSys::GameManager& GameManager() const;
  // Allocator pointer
  System::nijiAllocator* AllocatorPtr() const;
  // RenderingPipeline pointer
  RenderingPipeline* RenderingPipelinePtr() const;
  // EffectManager
  Effect::Manager& EffectManager() const;
  // Player Manager
  Player::Manager& PlayerManager() const;
  // Rail Manager
  Rail::Manager& RailManager() const;
  // Gimmick Manager
  Gimmick::Manager& GimmickManager() const;
  // check if LunaSolDive has been Terminated
  bool IsTerminated() const;
  // Set pointer to input Parameter & output the Result
  void SetIOParameter(
              App::Event::LUNASOL_DIVE_PARAM* pIOParameter);
private:
  // Step
  enum Step
  {
    STEP__NONE,
    STEP__INITIALIZING,
    STEP__FADE_IN,
    STEP__WORKING,
    STEP__FADE_OUT,
    STEP__TERMINATING,
    STEP__TERMINATED,
  };
  // progress the Initializing Process
  bool ProgressInitializingProcess();
  // progress the Fade In Process
  bool ProgressFadeInProcess();
  // progress the Fade Out Process
  bool ProgressFadeOutProcess();
  // progress the Terminate Process
  bool ProgressTerminateProcess();
  // Version: Sol or Luna
  Version m_Version;
  // Sex of the hero: Male or Female
  HeroSex m_Sex;
  // Mode normal or first
  Mode m_Mode;
  // Maneuver (Gyroscope or Stick)
  Maneuver m_Maneuver;
  // Heap
  gfl2::heap::HeapBase* m_pHeap;
  // Effect Heap
  gfl2::heap::HeapBase* m_pEffectHeap;
  // pointer to GameManager
  GameSys::GameManager* m_pGameManager;
  // pointer to AppRenderingManager
  app::util::AppRenderingManager* m_pAppRenderingManager;
  // Present Step
  Step m_PresentStep;
  // Allocator pointer
  System::nijiAllocator* m_pAllocator;
  // RenderingPipeline pointer
  RenderingPipeline* m_pRenderingPipeline;
  // Light
  Light* m_pLight;
  // Run Manager
  Run::Manager* m_pRunManager;
  // Effect Manager
  Effect::Manager* m_pEffectManager;
  // Player Manager
  Player::Manager* m_pPlayerManager;
  // Rail Manager
  Rail::Manager* m_pRailManager;
  // Gimmick Manager
  Gimmick::Manager* m_pGimmickManager;
  // UI
  UI::DiveUI* m_pUI;
  // Process Counter
  u32 m_ProcessCounter;
  // Sub Process Counter
  u32 m_SubProcessCounter;
  // Pointer to input Parameter & output the Result
  App::Event::LUNASOL_DIVE_PARAM* m_pIOParameter;
#if PM_DEBUG
public:
  // Switch to Debug Mode
  void SwitchToDebugMode();
private:
  // Heap Initial Free Size
  s32 m_HeapInitialFreeSize;
  // Effect Heap Initial Free Size
  s32 m_EffectHeapInitialFreeSize;
  // Debug Menu
  Debug::Menu* m_pDebugMenu;
  // Whether it is in debug mode or not
  bool m_IsDebugMode;
#endif// PM_DEBUG
};
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__LUNA_SOL_DIVE_MANAGER_H_INCLUDED__
