#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMESCENEBASE_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMESCENEBASE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanFrameSceneBase.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.27
 @brief   図鑑のシーンのフレームの基底
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Frame/AppFrameListener.h>
#include <AppLib/include/Frame/AppFrameCellSuper.h>

// 図鑑のインクルード
#include "ZukanSceneParam.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanFrameSceneBaseCell
 @brief     図鑑のシーンのフレームのセルの基底クラス
 */
//==============================================================================
class ZukanFrameSceneBaseCell : public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanFrameSceneBaseCell);

public:
  ZukanFrameSceneBaseCell(void);
  virtual ~ZukanFrameSceneBaseCell();

  virtual applib::frame::Result InitFunc(void);
  virtual applib::frame::Result UpdateFunc(void);
  virtual void                  DrawFunc(gfl2::gfx::CtrDisplayNo no);
  virtual applib::frame::Result EndFunc(void);

public:
  //! @brief  引数を渡す
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  void Setup(const ZukanSceneParam& param);

  //! @brief  ビューから呼び出されるリクエスト
  virtual void Req(s32 reqId);

protected:
  //! @brief  シーケンスを変更する
  //! @param[in]  seq  このシーケンスに変更する
  void changeSeq(int seq);

protected:
  ZukanSceneParam  m_param;
  u32              m_seqCount;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMESCENEBASE_H_INCLUDED

