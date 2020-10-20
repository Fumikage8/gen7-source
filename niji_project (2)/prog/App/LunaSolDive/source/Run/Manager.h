//======================================================================
/**
 * @file Manager.h
 * @date 2016/09/20 20:09:44
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RUN__MANAGER_H_INCLUDED__
#define __LUNA_SOL_DIVE__RUN__MANAGER_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "../Version.h"

// forward declaration
namespace GameSys{
class GameManager;
}// namespace GameSys

namespace LunaSolDive{
// forward declaration
class Manager;
namespace Camera{
class Base;
class Main;
}// namespace Camera
namespace Effect{
class Manager;
}// namespace Effect
namespace Rail{
class Manager;
}// namespace Rail

namespace Run{
// forward declaration
class Aim;
class Beginning;
class Ending;
class Energy;
class Input;
class Result;
class Translation;

class Manager
{
  GFL_FORBID_COPY_AND_ASSIGN(Manager);
public:
  // constructor
  Manager();
  // Initialize
  void Initialize(
              const Version& version,
              const Maneuver& maneuver,
              gfl2::heap::HeapBase* pHeap,
              LunaSolDive::Manager* pManager,
              GameSys::GameManager* pGameManager,
              const Rail::Manager* pRailManager,
              Effect::Manager* pEffectManager);
  // Terminate
  void Terminate();
  // Crate
  void Create(
              gfl2::heap::HeapBase* pHeap);
  // Set Up
  void SetUp();
  // Update
  void Update();
  // Camera
  const Camera::Base& Camera() const;
  // Result
  const Result& Result() const;
  // Whether the game is over or not
  bool IsGameOver() const;
private:
  // State
  enum State
  {
    STATE__NONE,
    STATE__BEGINNING,
    STATE__MAIN,
    STATE__ENDING,
    STATE__GAME_OVER,
  };
  // Manager pointer
  LunaSolDive::Manager* m_pManager;
  // State
  State m_State;
  // Input
  Input* m_pInput;
  // Aim
  Aim* m_pAim;
  // Camera
  Camera::Main* m_pMainCamera;
  // Energy
  Energy* m_pEnergy;
  // Result
  Run::Result* m_pResult;
  // Beginning Stage
  Beginning* m_pBeginningStage;
  // Ending Stage
  Ending* m_pEndingStage;
  // Translation Manager
  Translation* m_pTranslationManager;
};
}// namespace Run
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RUN__MANAGER_H_INCLUDED__
