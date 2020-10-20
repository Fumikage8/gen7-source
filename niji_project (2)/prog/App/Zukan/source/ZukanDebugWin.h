#if PM_DEBUG


#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANDEBUGWIN_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANDEBUGWIN_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanDebugWin.h
 @author  kawada_koji@gamefreak.co.jp
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
  
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanDebugWinImpl;
  class ZukanDebugWinPrismImpl;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanDebugWin
 @brief     図鑑のデバッグウィンドウ
 */
//==============================================================================
class ZukanDebugWin
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanDebugWin);

public:
  //! @brief  コンストラクタ
  //! @param[in]  heap         デバッグウィンドウ作成に使用するヒープ(デバッグ用のヒープ)
  //! @param[in]  parentGroup  親グループ(NULLならTopグループ)
  ZukanDebugWin(gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parentGroup);
  virtual ~ZukanDebugWin();

public:
  // 分布チェックmode
  static s32 GetMapIconCheckMode(void);
  // ポケモンモデルSRT調整mode
  static s32 GetPokeModelSRTModifyMode(void);
  // QR範囲外チェック
  static s32 GetQROutOfRangeVal(void);
  // QR全ポケモン表示
  static s32 GetQRAllDispFlag(void);

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
  ZukanDebugWinImpl*       m_impl;
  ZukanDebugWinPrismImpl*  m_prismImpl;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEBUGWIN_H_INCLUDED


#endif  // PM_DEBUG

