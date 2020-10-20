#if PM_DEBUG


#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANSAVEDATADEBUGWIN_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANSAVEDATADEBUGWIN_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanSaveDataDebugWin.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.05.25
 @brief   図鑑セーブデータのデバッグウィンドウ
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

GFL_NAMESPACE_BEGIN(Savedata)
  class ZukanData;
GFL_NAMESPACE_END(Savedata)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanSaveDataDebugWinImpl;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanSaveDataDebugWin
 @brief     図鑑セーブデータのデバッグウィンドウ
 */
//==============================================================================
class ZukanSaveDataDebugWin
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanSaveDataDebugWin);

public:
  //! @brief  コンストラクタ
  //! @param[in]  heap         デバッグウィンドウ作成に使用するヒープ(デバッグ用のヒープ)
  //! @param[in]  parentGroup  親グループ(NULLならTopグループ)
  ZukanSaveDataDebugWin(gfl2::heap::HeapBase* heap, gfl2::debug::DebugWinGroup* parentGroup);
  virtual ~ZukanSaveDataDebugWin();

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの、グローバルインスタンスから受け取ったもの
  ////////////////////////////////////////
  gfl2::heap::HeapBase*       m_heap;
  gfl2::debug::DebugWinGroup* m_parentGroup;
  gfl2::str::MsgData*         m_pMsgData;
  Savedata::ZukanData*        m_svZukanData;

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  ZukanSaveDataDebugWinImpl*  m_impl;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANSAVEDATADEBUGWIN_H_INCLUDED


#endif  // PM_DEBUG

