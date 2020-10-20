//======================================================================
/**
 * @file Proc.h
 * @date 2016/09/05 19:49:13
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__PROC_H_INCLUDED__
#define __LUNA_SOL_DIVE__PROC_H_INCLUDED__
#pragma once

#include <GameSys/include/GameProc.h>

// forward declaration
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

namespace LunaSolDive{
// forward declaration
class Manager;

class Proc: public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(Proc);
public:
  // constructor
  Proc();
  // destructor
  virtual ~Proc();
  // Initialize (override)
  virtual gfl2::proc::Result InitFunc(
              gfl2::proc::Manager* pManager);
  // Terminate (override)
  virtual gfl2::proc::Result EndFunc(
              gfl2::proc::Manager* pManager);
  // Update (override)
  virtual gfl2::proc::Result UpdateFunc(
              gfl2::proc::Manager* pManager);
  // Draw (override)
  virtual void DrawFunc(
              gfl2::proc::Manager* pManager,
              gfl2::gfx::CtrDisplayNo displayNo);
  // Set pointer to input Parameter & output the Result
  void SetIOParameter(
              App::Event::LUNASOL_DIVE_PARAM* pIOParameter);
private:
  // Heap
  gfl2::heap::HeapBase* m_pHeap;
  // Application Heap
  app::util::Heap* m_pAppHeap;
  // App Rendering Manager
  app::util::AppRenderingManager* m_pRenderingManager;
  // LunaSolDive::Manager
  Manager* m_pLunaSolDiveManager;
  // Process Counter
  u32 m_ProcessCounter;
  // Pointer to input Parameter & output the Result
  App::Event::LUNASOL_DIVE_PARAM* m_pIOParameter;
#if PM_DEBUG
public:
  // Switch to Debug Mode
  void SwitchToDebugMode();
private:
  // Whether it is in debug mode or not
  bool m_IsDebugMode;
#endif// PM_DEBUG
};
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__PROC_H_INCLUDED__
