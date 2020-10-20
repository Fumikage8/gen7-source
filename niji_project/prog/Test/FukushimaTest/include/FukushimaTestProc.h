#if PM_DEBUG

#ifndef __TEST_FUKUSHSIMA_TEST_PROC_H_INCLUDED__
#define __TEST_FUKUSHSIMA_TEST_PROC_H_INCLUDED__
#pragma once
//==============================================================================
/**
 * @file    FukushimaTestProc.h
 * @author  fukushima_yuya
 * @date    2015.06.19
 * @brief   テスト用プロック
 */
//==============================================================================

// niji
#include <App/NumberInput/include/NumberInputAppParam.h>
#include <App/PokeList/include/PokeListAppParam.h>
#include <App/PokeList/include/PokeListSetupParam.h>
#include <App/Status/include/StatusAppParam.h>
#include <App/StrInput/include/StrInputAppParam.h>
#include <AppLib/include/Frame/AppFrameListener.h>
#include <Debug/Launcher/include/LauncherProc.h>
#include <GameSys/include/GameProc.h>
#include <System/include/nijiAllocator.h>

// gflib2
#include <heap/include/gfl2_Heap.h>
#include <macro/include/gfl2_Macros.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( NumberInput )
class NumberInputFrame;
GFL_NAMESPACE_END( NumberInput )
GFL_NAMESPACE_BEGIN( StrInput )
class StrInputFrame;
GFL_NAMESPACE_END( StrInput )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )
class TestPokeListFieldFrame;
class TestStatusFrame;
class IconTestFrame;
class PokeModelTestFrame;
class DynamicHeaderTestFrame;
GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

GFL_NAMESPACE_BEGIN( applib )
GFL_NAMESPACE_BEGIN( frame )
class Manager;
GFL_NAMESPACE_END( frame )
GFL_NAMESPACE_END( applib )

GFL_NAMESPACE_BEGIN( GameSys )
class GameManager;
GFL_NAMESPACE_END( GameSys )


//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
enum {
  TEST_POKELIST,
  TEST_STATUS,
  TEST_STRINPUT,
  TEST_ICON,
  TEST_STRINPUT_KOREA,
  TEST_POKE_MODEL,
  TEST_NUM,
};


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   PROCクラス
//------------------------------------------------------------------------------
class Proc
  : public applib::frame::AppFrameListener
  , public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( Proc ); //!< コピーコンストラクタ＋代入禁止

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  Proc( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~Proc( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   PROCの初期化を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン更新を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  
  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::proc::Manager* pManager,gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメインを行う関数
   *  1/30フレームの②フレーム目の処理です。
   *  1/60フレーム動作時には、使われません。
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result NextFrameMainFunc( gfl2::proc::Manager* pManager );

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

  //------------------------------------------------------------------
  /**
   * @brief	  EndFuncが完了した直後に呼び出される関数
   */
  //------------------------------------------------------------------
  virtual void EndFuncFinish( applib::frame::CellSuper* pEndCell );

public:
  //------------------------------------------------------------------
  /**
   * @brief   フレームマネージャの取得
   */
  //------------------------------------------------------------------
  applib::frame::Manager* GetFrameManager( void ) { return m_pFrameManager; }

  //------------------------------------------------------------------
  /**
   * @brief   アプリケーションレンダリングマネージャの取得
   */
  //------------------------------------------------------------------
  app::util::AppRenderingManager* GetAppRenderingManager( void ) { return m_pAppRenderingManager; }

  //------------------------------------------------------------------
  /**
   * @brief   アプリケーションヒープの取得
   */
  //------------------------------------------------------------------
  app::util::Heap* GetAppHeap( void ) { return m_pAppHeap; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   フレームのインデックス
   */
  //------------------------------------------------------------------
  void SetCreateFrame( u32 index ) { m_CreateFrameIndex = index; }

  //------------------------------------------------------------------
  /**
   * @brief   文字入力画面のモードの設定
   * @param   mode    文字入力モード
   */
  //------------------------------------------------------------------
  void SetStrInputMode( u32 mode ) { m_StrInputMode = mode; }

private:
  //------------------------------------------------------------------
  /**
   * @brief   フレームの生成
   */
  //------------------------------------------------------------------
  void CreateFrame( void );

private:
  app::util::Heap*    m_pAppHeap;

  app::util::AppRenderingManager*   m_pAppRenderingManager;
  
  applib::frame::Manager*   m_pFrameManager;

  gfl2::heap::HeapBase*     m_pManagerHeap;

  System::nijiAllocator*    m_pNijiAllocator;

  GameSys::GameManager*     m_pGameManager;

  debug::launcher::LauncherProc::CallParam    m_LauncherParam;

  u32   m_CreateFrameIndex;

  u32   m_FrameCount;

  u32   m_StrInputMode;

private:
  TestPokeListFieldFrame*           m_pPokeListFieldFrame;

  App::PokeList::APP_PARAM          m_PokeListAppParam;

  App::PokeList::CONTEXT_PARAM      m_PokeListContext;

  TestStatusFrame*                  m_pStatusFrame;

  App::Status::APP_PARAM            m_StatusAppParam;

  App::StrInput::StrInputFrame*     m_pStrInputFrame;

  App::StrInput::APP_PARAM          m_StrInputAppParam;

  IconTestFrame*                    m_pIconTestFrame;

  PokeModelTestFrame*               m_pPokeModelTestFrame;

  DynamicHeaderTestFrame*           m_pDynamicHeaderTestFrame;

  App::NumberInput::APP_PARAM       m_NumberInputAppParam;

  App::NumberInput::NumberInputFrame*   m_pNumberInputFrame;
};


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // __TEST_FUKUSHSIMA_TEST_PROC_H_INCLUDED__

#endif // PM_DEBUG