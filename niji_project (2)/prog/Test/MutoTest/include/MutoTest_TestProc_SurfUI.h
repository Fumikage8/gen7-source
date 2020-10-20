﻿#if !defined(MUTOTEST_TESTPROC_SURFUI_H_INCLUDED)
#define MUTOTEST_TESTPROC_SURFUI_H_INCLUDED
#pragma once
#if PM_DEBUG
//==============================================================================
/**
 * @file   MutoTest_TestProc_SurfUI.h
 * @date   2016/11/14 Mon. 12:23:02
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// ============================================================================
/*  includes  */
// ============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  <GameSys/include/GameProcManager.h>
#include  <AppLib/include/Frame/AppFrameManager.h>
#include  <AppLib/include/Util/app_util_heap.h>
#include  <AppLib/include/Util/AppRenderingManager.h>
#include  <ro/include/gfl2_RoManager.h>

#include  <App/MantainSurf/source/UI/SurfUI.h>

#include  "../source/Common/TextDrawer.h"

// ============================================================================
/**/
// ============================================================================
namespace App {
  namespace MantainSurf {
    namespace UI  {
      class SurfUI;
    }
  }
}



/*
  Test::MutoTest
*/
namespace Test  {
namespace MutoTest  {
  class TextDrawer;
// ============================================================================
/**/
// ============================================================================



//==============================================================================
/**
 *  @class  TestProc_SurfUI
 *  @brief  武藤用TestProc_SurfUIess
 *  @date   2015.05.12
 */
//==============================================================================
class TestProc_SurfUI
  : public    GameSys::GameProc
  , protected App::MantainSurf::UI::SurfUI::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN(TestProc_SurfUI);

public:
  /** ctor  */
  TestProc_SurfUI(void);

  /** dtor  */
  virtual ~TestProc_SurfUI();


  /*
    GamSys::GameProc I/F
  */

  //------------------------------------------------------------------
  /**
   *  @func     GetFrameMode
   *  @brief    FrameModeの取得
   *  @retval   FrameMode
   */
  //------------------------------------------------------------------
  //    virtual GameSys::FrameMode GetFrameMode( void ) const { return GameSys::FRAMEMODE_DEFAULT; }

  //------------------------------------------------------------------
  /**
   * @brief   PROCの初期化を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
   *  @func     UpdateFunc
   *  @brief    PROCのメイン更新を行う関数
   *  @return   Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
   *  @func     PreDrawFunc
   *  @brief    PROCの描画処理(下準備)
   *            登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
   *            例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
   *  @param pManager   プロセス管理マネージャー
   */
  //------------------------------------------------------------------
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );

  //------------------------------------------------------------------
  /**
   *  @brief    PROCの描画処理
   *            登録したPROCによる、フレームバッファへの描画を実行します。
   *            ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
   *  @param pManager   プロセス管理マネージャー
   *  @param displayNo  描画対象画面
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメインを行う関数
   *  1/30フレームの②フレーム目の処理です。
   *  1/60フレーム動作時には、使われません。
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  //    virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);
  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);
  //------------------------------------------------------------------
  /**
   * @brief   PROCのイベント起動検知処理
   *
   *  以下のタイミングで行うべき、イベント起動処理
   *  　GameProc->EventCheckFunc
   *  　EventManager->Main
   *  　GameProc->Main
   */
  //------------------------------------------------------------------
  //    virtual void EventCheckFunc(GameManager* pManager);


private:
  virtual void  SurfUI_OnEvent(App::MantainSurf::UI::SurfUI* pSender, const App::MantainSurf::UI::SurfUI::IEventHandler::EventCode eventCode);

  void  CreateHeap(void);
  void  DeleteHeap(void);

  app::util::Heap                 m_Heap;
  app::util::AppRenderingManager* m_pRenderManager;
  gfl2::ro::Module**              m_ppROModules;
  u32                             m_State;
  TextDrawer                      m_TextDrawer;
  
  App::MantainSurf::UI::SurfUI*   m_pSurfUI;
  bool                            m_bTrickListVisibility;
  bool                            m_bExit;
  u32                             m_TutorialID;
};	/*	class TestProc_SurfUI	*/









} /*  namespace MutoTest  */
} /*  namespace Test  */
#endif  /*  #if PM_DEBUG  */
#endif  /*  #if !defined(MUTOTEST_TESTPROC_SURFUI_H_INCLUDED)  */

