#if PM_DEBUG

#ifndef __TEST_FUKUSHIMA_TEST_STATUS_FRAME_H__
#define __TEST_FUKUSHIMA_TEST_STATUS_FRAME_H__
#pragma once
//==============================================================================
/**
 * @file    StatusTestFrame.h
 * @author  fukushima_yuya
 * @date    2015.06.26
 * @brief   ステータス画面フレーム
 */
//==============================================================================

// niji
#include "App/Status/include/StatusAppParam.h"
#include "App/Status/source/StatusDrawListener.h"

#include <AppLib/include/Frame/AppFrameCellSuper.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )
class TestStatusLowerDraw;
GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN( print )
class WordSet;
GFL_NAMESPACE_END( print )

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( str )
class MsgData;
GFL_NAMESPACE_END( str )
GFL_NAMESPACE_END( gfl2 )


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   ステータスフレーム
//------------------------------------------------------------------------------
class TestStatusFrame
  : public applib::frame::CellSuper
  , public App::Status::StatusDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN( TestStatusFrame );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  TestStatusFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~TestStatusFrame( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result EndFunc( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  動作に必要な引数を登録
   */
  //------------------------------------------------------------------
  void Setup( App::Status::APP_PARAM* pAppParam );

public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   ステータス画面の終了通知
   */
  //------------------------------------------------------------------
  virtual void EndStatus( void );

  //------------------------------------------------------------------
  /**
   * @brief   キャンセルボタン押下通知
   */
  //------------------------------------------------------------------
  virtual void OnCancelButton( void ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   説明ウィンドウを開く通知
   * @param   mode    表示するモード
   * @param   value   表示に必要な値(技ならWazaNo、メモならリボンID)
   */
  //------------------------------------------------------------------
  virtual void OpenExplainWindow( App::Status::Mode mode, u32 value ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   説明ウィンドウを開く通知
   * @param   mode    表示するモード
   */
  //------------------------------------------------------------------
  virtual void CloseExplainWindow( App::Status::Mode mode ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   表示モードの変更通知
   * @param   mode  ステータスの表示モード
   */
  //------------------------------------------------------------------
  virtual void StatusModeChange( App::Status::Mode mode ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   表示ポケモンの変更通知
   * @param   poke_index  表示するポケモンのインデックス
   */
  //------------------------------------------------------------------
  virtual void PokemonChange( u8 poke_index ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   表示技の変更通知
   * @param   waza_no   表示する技のインデックス
   */
  //------------------------------------------------------------------
  virtual void WazaChange( u32 waza_no ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   表示グッズの変更通知
   * @param   goods_index   表示するグッズのインデックス
   */
  //------------------------------------------------------------------
  virtual void GoodsChange( u32 goods_index ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   グッズ選択の開始
   */
  //------------------------------------------------------------------
  virtual void StartGoodsSelect( void ) { ; }


protected:
  int   m_subseq;     //!< プロセス関数の動作シーケンス
  bool  m_bEnd;       //!< Endフラグ

private:
  app::util::Heap*                  m_pAppHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  App::Status::APP_PARAM*      m_pAppParam;

  TestStatusLowerDraw*    m_pStatusLowerDraw;

  void*   m_pArcReadBuff;
  int     m_ReadSeq;

  gfl2::str::MsgData*   m_pMsgData;
  print::WordSet*       m_pWordSet;
};


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )


#endif  // #ifndef __TEST_FUKUSHIMA_TEST_STATUS_FRAME_H__

#endif // PM_DEBUG