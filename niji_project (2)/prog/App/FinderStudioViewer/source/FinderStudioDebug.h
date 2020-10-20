//======================================================================
/**
 * @file FinderStudioMain.h
 * @date 2016/09/05
 * @author harada_takumi
 * @brief ファインダースタジオデバッグ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FINDER_STUDIO_DEBUG_H_INCLUDED__
#define __FINDER_STUDIO_DEBUG_H_INCLUDED__
#pragma once

// System
#include "System/include/GflUse.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)
  class FinderStudioViewer;

#if PM_DEBUG

// デバッグライト使用フラグ取得
bool IsUseDebugLight();

// 初期化
void InitializeDebug(gfl2::heap::HeapBase* pHeap,FinderStudioViewer* pFinderStudioMain);

// 更新
void UpdateDebug();

// 解放処理
void TerminateDebug();

#endif

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)

#endif // __FINDER_STUDIO_DEBUG_H_INCLUDED__