//======================================================================
/**
 * @file Beginning.h
 * @date 2017/01/06 15:03:13
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RUN__BEGINNIG_H_INCLUDED__
#define __LUNA_SOL_DIVE__RUN__BEGINNIG_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include "../Version.h"
#include "../Rail/Directions.h"

namespace LunaSolDive{
// forward declarations
class Manager;
namespace Camera{
class Base;
class Beginning;
}// namespace Camera

namespace Run{
class Beginning
{
  GFL_FORBID_COPY_AND_ASSIGN(Beginning);
public:
  // constructor
  Beginning();
  // Initialize
  void Initialize(
              const Version& version,
              gfl2::heap::HeapBase* pHeap,
              LunaSolDive::Manager* pManager);
  // Terminate
  void Terminate();
  // Set Up
  void SetUp();
  // Update
  void Update();
  // Whether this stage has been finished
  bool IsFinished() const;
  // Camera
  const Camera::Base& Camera() const;
private:
  // Root Manager
  LunaSolDive::Manager* m_pManager;
  // Camera
  Camera::Beginning* m_pCamera;
  // Player Location
  Rail::Directions m_PlayerLocation;
  // Whether the enrgy aura has been activated or not
  bool m_IsEnergyAuraActivated;
};
}// namespace Run
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RUN__BEGINNIG_H_INCLUDED__
