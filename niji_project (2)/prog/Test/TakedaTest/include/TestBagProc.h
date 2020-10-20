//==============================================================================
/**
 * @file        TestBagProc.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/12/24(木) 16:41:19
 */
//==============================================================================
#if PM_DEBUG

#if !defined(__TESTBAGPROC_H__) // 重複定義防止
#define __TESTBAGPROC_H__ // 重複定義防止マクロ
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

#include <GameSys/include/GameProc.h>
#include <AppLib/include/Util/app_util_heap.h>
#include "TestBagAppParam.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(takedaTest_app)
using namespace app;

// 前方宣言
class BagFrame;

//=============================================================================
/**
 * @class BagProc
 * @brief バッグ画面Procクラス
 * @date  2015.03.04
 */
//=============================================================================
class BagProc
  : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(BagProc);  // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  BagProc(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~BagProc();

  //-----------------------------------------------------------------------------
  /**
   * @func    Setup
   * @brief   外部設定パラメータをセット
   * @date    2015.03.04
   *
   * @param   param   外部設定パラメータ
   */
  //-----------------------------------------------------------------------------
  void Setup( s32 idx );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFunc
   * @brief   Proc初期化処理
   * @date    2015.03.04
   *
   * @param   pManager    プロセス管理マネージャー
   *
   * @return  gfl2::proc::Result
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFunc
   * @brief   Proc終了処理
   * @date    2015.03.04
   *
   * @param   pManager    プロセス管理マネージャー
   *
   * @return  gfl2::proc::Result
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateFunc
   * @brief   Procアップデート処理
   * @date    2015.03.04
   *
   * @param   pManager    プロセス管理マネージャー
   *
   * @return  gfl2::proc::Result
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   Proc描画処理
   * @date    2015.03.04
   *
   * @param   pManager    プロセス管理マネージャー
   * @param   displayNo   描画対象画面
   */
  //-----------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::proc::Manager * pManager, gfl2::gfx::CtrDisplayNo displayNo );


private:
  gfl2::heap::HeapBase * m_pManagerHeap;

  app::util::Heap m_heap;

  app::util::AppRenderingManager * m_pRenderingManager;
  applib::frame::Manager * m_pFrameManager;
  BagFrame * m_pBagFrame;
  
  s32       m_iIdx;
};

GFL_NAMESPACE_END(takedaTest_app)

#endif // __TESTBAGPROC_H__ 重複定義防止

#endif // PM_DEBUG
