#if PM_DEBUG
#if !defined(MUTOTEST_TESTPROC_GIFTDEMO_H_INCLUDED)
#define MUTOTEST_TESTPROC_GIFTDEMO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   MutoTest_TestProc_GiftDemo.h
 * @date   2016/03/14 Mon. 12:24:19
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

#include  "../source/Common/TextDrawer.h"

// ============================================================================
/**/
// ============================================================================
namespace pml
{
  namespace pokepara
  {
    class PokemonParam;
  }

}



/*
  Test::MutoTest
*/
namespace Test      {
namespace MutoTest  {
  class TextDrawer;
// ============================================================================
/**/
// ============================================================================



//==============================================================================
/**
 *  @class  TestProc_GiftDemo
 *  @brief  
 */
//==============================================================================
class TestProc_GiftDemo : public GameSys::GameProc	{
  GFL_FORBID_COPY_AND_ASSIGN(TestProc_GiftDemo);

public:
  /** ctor  */
  TestProc_GiftDemo(void);

  /** dtor  */
  virtual ~TestProc_GiftDemo();


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

  app::util::Heap               m_Heap;
  TextDrawer                    m_TextDrawer;

  GameSys::GameProc*            m_pDemoProc;
  u32                           m_MonsNo;
  bool                          m_IsEgg;
  u32                           m_DemoType;
  pml::pokepara::PokemonParam*  m_pPokepara;
  pml::pokepara::PokemonParam*  m_pParentPokepara[2];
  gfl2::str::StrBuf*            m_pStrBuf;
};	/*	class TestProc_GiftDemo	*/





} /*  namespace MutoTest  */
} /*  namespace Test  */
#endif  /*  #if PM_DEBUG  */
#endif  /*  #if !defined(MUTOTEST_TESTPROC_GIFTDEMO_H_INCLUDED)  */

