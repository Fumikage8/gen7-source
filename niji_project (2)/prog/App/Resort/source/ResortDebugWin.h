#pragma once
#if PM_DEBUG
#if !defined(__RESORT_DEBUG_WIN_H__)
#define __RESORT_DEBUG_WIN_H__
//==============================================================================
/**
 @file    ResortDebugWin.h
 @author  k.ohno
 @date    2015.08.11
 @brief   図鑑のデバッグウィンドウ
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(debug)
class DebugWinGroup;
GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl2)
  


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

class ResortDebugWinImpl;

//==============================================================================
/**
 @class     ResortDebugWin
 @brief     図鑑のデバッグウィンドウ
 */
//==============================================================================
class ResortDebugWin
{
  GFL_FORBID_COPY_AND_ASSIGN(ResortDebugWin);

public:
  //! @brief  コンストラクタ
  //! @param[in]  heap         デバッグウィンドウ作成に使用するヒープ(デバッグ用のヒープ)
  //! @param[in]  parentGroup  親グループ(NULLならTopグループ)
  ResortDebugWin(gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parentGroup);
  virtual ~ResortDebugWin();


  int GetDebugCameraMode(void);
  bool GetDebugFullMode(void);

  int GetSpeedWork(void);

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  gfl2::heap::HeapBase*       m_heap;
  gfl2::debug::DebugWinGroup* m_parentGroup;
  gfl2::str::MsgData*         m_pMsgData;

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  ResortDebugWinImpl*  m_impl;
};


GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)


#endif  // __RESORT_DEBUG_WIN_H__


#endif  // PM_DEBUG

