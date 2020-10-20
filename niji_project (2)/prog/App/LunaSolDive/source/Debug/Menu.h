//======================================================================
/**
 * @file Menu.h
 * @date 2016/12/22 20:05:22
 * @author arai_takashi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG
#if !defined __LUNA_SOL_DIVE__DEBUG__MENU_H_INCLUDED__
#define __LUNA_SOL_DIVE__DEBUG__MENU_H_INCLUDED__
#pragma once

#include <heap/include/gfl2_Heap.h>

// forward declaration
namespace gfl2{
namespace debug{
class DebugWinGroup;
class DebugWinItem;
}// namespace debug
}// namespace LunaSolDive

namespace LunaSolDive{
namespace Debug{
class Menu
{
  GFL_FORBID_COPY_AND_ASSIGN(Menu);
public:
  // constructor
  Menu();
  // Initialize
  void Initialize();
  // Terminate
  void Terminate();
  // Update
  void Update();
private:
  // Pointer to Heap
  gfl2::heap::HeapBase* m_pHeap;
  // Root Menu
  gfl2::debug::DebugWinGroup* m_pRoot;
  // static function
  // Add Page: Info
  static void AddPageInfo(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: Mode
  static void AddPageMode(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: Gyroscope
  static void AddPageGyroscope(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: Parameter
  static void AddPageParameter(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: Parameter/Run
  static void AddPageParameterRun(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: Parameter/Gyroscope
  static void AddPageParameterGyroscope(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: Parameter/Camera
  static void AddPageParameterCamera(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: Parameter/Rail
  static void AddPageParameterRail(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: Parameter/Gimmick
  static void AddPageParameterGimmick(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: Parameter/Gimmick Placement
  static void AddPageParameterGimmickPlacement(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: Parameter/Player
  static void AddPageParameterPlayer(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: Parameter/Motion Brend Frames
  static void AddPageParameterMotionBrendFrames(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: Parameter/Beginning
  static void AddPageParameterBeginning(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: Parameter/Ending
  static void AddPageParameterEnding(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: Parameter/Sound
  static void AddPageParameterSound(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: Parameter/Translation
  static void AddPageParameterTranslation(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: Model
  static void AddPageModel(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
  // Add Page: DrawUtil
  static void AddPageDrawUtil(
              gfl2::debug::DebugWinGroup* pParentPage,
              gfl2::heap::HeapBase* pHeap);
};
}// namespace Debug
}// namespace LunaSolDive
#endif // __LUNA_SOL_DIVE__DEBUG__MENU_H_INCLUDED__
#endif// PM_DEBUG
