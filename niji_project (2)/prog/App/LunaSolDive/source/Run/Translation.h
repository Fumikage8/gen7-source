//======================================================================
/**
 * @file Translation.h
 * @date 2017/01/19 18:14:22
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LUNA_SOL_DIVE__RUN__TRANSLATION_H_INCLUDED__
#define __LUNA_SOL_DIVE__RUN__TRANSLATION_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>

namespace LunaSolDive{
// forward declaration
class Manager;
namespace Camera{
class Main;
}// namespace Camera

namespace Run{
class Translation
{
  GFL_FORBID_COPY_AND_ASSIGN(Translation);
public:
  // constructor
  Translation();
  // Initialize
  void Initialize(
              LunaSolDive::Manager* pManager,
              Camera::Main* pCamera);
  // Terminate
  void Terminate();
  // Update
  void Update();
private:
  // Pointer to root Manager
  LunaSolDive::Manager* m_pManager;
  // Pointer to Camera
  Camera::Main* m_pCamera;
  // Frame Count
  u32 m_FrameCount;
  // Displacement
  gfl2::math::Vector3 m_Displacement;
  // Whether the translation operation is in progress
  bool m_IsInProgress;
};
}// namespace Run
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__RUN__TRANSLATION_H_INCLUDED__
