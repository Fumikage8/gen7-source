#if PM_DEBUG

#ifndef __TEST_FUKUSHIMA_TEST_DYNAMIC_HEADER_TEST_FRAME_H__
#define __TEST_FUKUSHIMA_TEST_DYNAMIC_HEADER_TEST_FRAME_H__
#pragma once
//======================================================================
/**
 * @file    DynamicHeaderTestFrame.h
 * @date    2015/10/06 13:08:43
 * @author  fukushima_yuya
 * @brief   DynamicHeaderテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/LangSelect/source/LangSelectDrawListener.h"
#include "App/LangSelect/source/LangSelectConst.h"

#include <AppLib/include/Frame/AppFrameCellSuper.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )
class DynamicHeaderTestDraw;
GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   DynamicHeaderテストフレーム
//------------------------------------------------------------------------------
class DynamicHeaderTestFrame
  : public applib::frame::CellSuper
  , public App::LangSelect::LangSelectDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN( DynamicHeaderTestFrame );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  DynamicHeaderTestFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~DynamicHeaderTestFrame( void );

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
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result EndFunc( void );

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

public:
  //------------------------------------------------------------------
  /**
   * @brief	  レイアウトデータの読み込み
   */
  //------------------------------------------------------------------
  bool LoadLayoutData( void );

  //------------------------------------------------------------------
  /**
   * @brief	  メッセージデータの読み込み
   */
  //------------------------------------------------------------------
  bool LoadMsgData( void );

  //------------------------------------------------------------------
  /**
   * @brief	  動作に必要な引数を登録
   */
  //------------------------------------------------------------------
  void Setup( app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager );

public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   言語選択の開始
   */
  //------------------------------------------------------------------
  virtual void StartLangSelect( void ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   画面モードの設定
   * @param   dispMode    画面モード
   */
  //------------------------------------------------------------------
  virtual void SetDispMode( App::LangSelect::DispMode dispMode ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   言語選択画面の終了通知
   */
  //------------------------------------------------------------------
  virtual void EndLangSelect( void );

  //------------------------------------------------------------------
  /**
   * @brief   選択した言語
   * @param   langID    言語ID
   * @param   kanji     漢字フラグ
   */
  //------------------------------------------------------------------
  virtual void SetLangID( u8 langID, bool kanji ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   セーブデータ削除コマンドの入力
   */
  //------------------------------------------------------------------
  virtual void InputSavedataDeleteCommand( void ) { ; }

protected:
  int   m_subseq;     //!< プロセス関数の動作シーケンス
  bool  m_bEnd;       //!< Endフラグ

private:
  app::util::Heap*                  m_pAppHeap;

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  DynamicHeaderTestDraw*            m_pDraw;

  void*   m_pArcReadBuff;
  void*   m_pMsgReadBuff;
  int     m_ReadSeq;
  u32     m_InitSeq;
};

GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // __TEST_FUKUSHIMA_TEST_DYNAMIC_HEADER_TEST_FRAME_H__

#endif // PM_DEBUG