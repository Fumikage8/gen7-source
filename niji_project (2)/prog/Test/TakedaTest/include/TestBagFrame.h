//==============================================================================
/**
 * @file        TestBagFrame.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/12/24(木) 16:41:31
 */
//==============================================================================
#if PM_DEBUG

#if !defined(__TESTBAGFRAME_H__)  // 重複定義防止
#define __TESTBAGFRAME_H__  // 重複定義防止マクロ
#pragma once

#if PM_DEBUG

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// pmlib
#include <pml/include/pmlib.h>
// niji
#include "AppLib/include/Frame/AppFrameCellSuper.h"
// module
//#include "TestBagAppParam.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class MenuWindow;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( FieldWindow )
class FieldTalkWindow;
class FieldYesNoWindow;
GFL_NAMESPACE_END( FieldWindow )
GFL_NAMESPACE_END( Field )


GFL_NAMESPACE_BEGIN(takedaTest_app)

//=============================================================================
/**
 * @class BagFrame
 * @brief バッグ画面フレームクラス
 * @date  2015.03.04
 */
//=============================================================================
class BagFrame
  : public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(BagFrame);   // コピーコンストラクタ＋代入禁止

public:
  BagFrame(void);
  virtual ~BagFrame();

  //-----------------------------------------------------------------------------
  /**
   * @func    Setup
   * @brief   外部設定パラメータをセット
   * @date    2015.03.04
   *
   * @param   param             外部設定パラメータ
   * @parma   heap              ヒープ
   */
  //-----------------------------------------------------------------------------
  void Setup( app::util::Heap * heap, s32 idx );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFunc
   * @brief   フレーム初期化
   * @date    2015.03.04
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result InitFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFunc
   * @brief   フレーム終了
   * @date    2015.03.04
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result EndFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateFunc
   * @brief   フレーム更新
   * @date    2015.03.04
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   フレーム描画
   * @date    2015.03.04
   *
   * @param   no  描画先のディスプレイ番号
   */
  //-----------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );

private:
  bool DeleteBagDraw(void);

private:
  app::util::Heap * m_heap;

  Field::FieldWindow::FieldTalkWindow   *m_pTalkWindow;
  Field::FieldWindow::FieldYesNoWindow  *m_pYesNoWindow;

  u16 m_mainSeq;
  u16 m_subSeq;
  s32 m_iIdx;
};

GFL_NAMESPACE_END(takedaTest_app)

#endif

#endif // __TESTBAGFRAME_H__ 重複定義防止

#endif // PM_DEBUG
