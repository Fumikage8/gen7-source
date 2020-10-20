#if !defined(MUTOTEST_TESTPROC_H_INCLUDED)
#define MUTOTEST_TESTPROC_H_INCLUDED
#pragma once
#if PM_DEBUG
// ============================================================================
/**
 *  @file    MutoTest_TestProc.h
 *  @brief   Test Process
 *  @date    2015.05.11
 */
// ============================================================================


// ============================================================================
/*  includes  */
// ============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  <GameSys/include/GameProcManager.h>
#include  <AppLib/include/Frame/AppFrameManager.h>
#include  <AppLib/include/Util/app_util_heap.h>

#include  "../source/Common/TextDrawer.h"

// ============================================================================
/**/
// ============================================================================


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
 *  @class  TestProc
 *  @brief  武藤用TestProcess
 *  @date   2015.05.12
 */
//==============================================================================
class TestProc : public GameSys::GameProc	{
  GFL_FORBID_COPY_AND_ASSIGN(TestProc);

public:
  /** ctor  */
  TestProc(void);

  /** dtor  */
  virtual ~TestProc();


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
  void  CreateHeap(void);
  void  DeleteHeap(void);

  app::util::Heap           m_Heap;
  TextDrawer                m_TextDrawer;
};	/*	class TestProc	*/









} /*  namespace MutoTest  */
} /*  namespace Test  */
#endif  /*  #if PM_DEBUG  */
#endif  /*  #if !defined(MUTOTEST_TESTPROC_H_INCLUDED)  */

