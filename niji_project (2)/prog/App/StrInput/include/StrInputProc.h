#ifndef NIJI_PROJECT_APP_STRINPUT_PROC_H_INCLUDED
#define NIJI_PROJECT_APP_STRINPUT_PROC_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file    StrInputProc.h
 * @date    2015/12/21 15:56:25
 * @author  fukushima_yuya
 * @brief   文字入力：PROC
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/Frame/AppFrameListener.h>
#include <GameSys/include/GameProc.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(StrInput)
class StrInputFrame;
struct StrInputParam;
GFL_NAMESPACE_END(StrInput)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class AppRenderingManager;
class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(StrInput)

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
  virtual ~Proc( void ) { ; }

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
   * @brief   PROCの描画処理(下準備)
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
  
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
   * @brief   EndFuncが完了した直後に呼び出される関数
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

private:
  //------------------------------------------------------------------
  /**
   * @brief   フレームの生成
   */
  //------------------------------------------------------------------
  void CreateFrame( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   外部設定パラメータの設定
   * @param   pParam    外部設定パラメータ
   */
  //------------------------------------------------------------------
  void SetAppParam( StrInputParam* pParam );

private:
  app::util::Heap*                  m_pAppHeap;

  app::util::AppRenderingManager*   m_pAppRenderingManager;
  
  applib::frame::Manager*           m_pFrameManager;

  gfl2::heap::HeapBase*             m_pManagerHeap;

  GameSys::GameManager*             m_pGameManager;

  u32                               m_FrameCount;

  StrInputFrame*                    m_pFrame;

  StrInputParam*                    m_pAppParam;
};

GFL_NAMESPACE_END(StrInput)
GFL_NAMESPACE_END(App)

#endif // NIJI_PROJECT_APP_STRINPUT_PROC_H_INCLUDED