#if PM_DEBUG
//=============================================================================
/**
 * @file    TrainerPassDebugWin.cpp
 * @brief   トレーナーパス画面デバッグウィンドウ処理
 * @author  Hiroyuki Nakamura
 * @date    2016.03.01
 */
//=============================================================================
#if !defined(TRAINERPASS_DEBUGWIN_H_INCLUDED)
#define TRAINERPASS_DEBUGWIN_H_INCLUDED

#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)

GFL_NAMESPACE_BEGIN(debug)
class DebugWinGroup;
GFL_NAMESPACE_END(debug)

GFL_NAMESPACE_BEGIN(str)
class MsgData;
GFL_NAMESPACE_END(str)

GFL_NAMESPACE_END(gfl2)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TrainerPass)

class TrainerPassDebugWinImpl;


class TrainerPassDebugWin
{
  GFL_FORBID_COPY_AND_ASSIGN(TrainerPassDebugWin);

public:
  TrainerPassDebugWin( gfl2::heap::HeapBase * heap, gfl2::debug::DebugWinGroup * parent_group );
  virtual ~TrainerPassDebugWin();

private:
  gfl2::heap::HeapBase * m_heap;
  gfl2::debug::DebugWinGroup * m_parentGroup;
  gfl2::str::MsgData * m_pMsgData;

  TrainerPassDebugWinImpl * m_impl;
};


GFL_NAMESPACE_END(TrainerPass)
GFL_NAMESPACE_END(App)

#endif  // TRAINERPASS_DEBUGWIN_H_INCLUDED

#endif  // PM_DEBUG

