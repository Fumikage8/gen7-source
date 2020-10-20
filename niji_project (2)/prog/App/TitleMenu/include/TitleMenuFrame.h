#ifndef NIJI_PROJECT_APP_TITLE_MENU_FRAME_H_INCLUDED
#define NIJI_PROJECT_APP_TITLE_MENU_FRAME_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file TitleMenuFrame.h
 * @date 2015/07/21 14:49:44
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "TitleMenuAppParam.h"

#include <App/TitleMenu/source/TitleMenuConst.h>
#include <App/TitleMenu/source/TitleMenuDrawListener.h>

#include <AppLib/include/Frame/AppFrameCellSuper.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( TitleMenu )
class TitleMenuUpperDraw;
class TitleMenuLowerDraw;
class MenuCursor;
GFL_NAMESPACE_END( TitleMenu )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( str )
class MsgData;
GFL_NAMESPACE_END( str )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( print )
class WordSet;
GFL_NAMESPACE_END( print )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( TitleMenu )

//------------------------------------------------------------------------------
// @brief   タイトルメニューフレーム
//------------------------------------------------------------------------------
class TitleMenuFrame
  : public applib::frame::CellSuper
  , public TitleMenuDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN( TitleMenuFrame );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  TitleMenuFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~TitleMenuFrame( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   PROCの初期化を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン更新を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result EndFunc( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ダイナミックヘッダーの読み込み
   */
  //------------------------------------------------------------------
  bool LoadDynamicHeader( void );

  //------------------------------------------------------------------
  /**
   * @brief   レイアウトデータの読み込み
   */
  //------------------------------------------------------------------
  bool LoadLayoutData( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージデータの読み込み
   */
  //------------------------------------------------------------------
  bool LoadMsgData( void );
  
  //------------------------------------------------------------------
  /**
   * @brief   ワードセットローダーの生成
   */
  //------------------------------------------------------------------
  bool CreateWordSetLoader( void );

  //------------------------------------------------------------------
  /**
   * @brief   メニューカーソルの生成
   */
  //------------------------------------------------------------------
  bool CreateMenuCursor( void );

  //------------------------------------------------------------------
  /**
   * @brief   動作に必要な引数を登録
   */
  //------------------------------------------------------------------
  void Setup( APP_PARAM* pParam, app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager );

public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   タイトルメニュー画面の終了通知
   */
  //------------------------------------------------------------------
  virtual void EndTitleMenu( void );

  //------------------------------------------------------------------
  /**
   * @brief   終了モードのセット
   * @param   endMode   終了モード
   */
  //------------------------------------------------------------------
  virtual void SetEndMode( EndMode endMode );

  //------------------------------------------------------------------
  /**
   * @brief   セーブデータフォーマット開始を通知
   */
  //------------------------------------------------------------------
  virtual void OnSavedataFormat( void );

protected:
  int   m_subseq;     //!< プロセス関数の動作シーケンス
  bool  m_bEnd;       //!< Endフラグ

private:
  app::util::Heap*                  m_pAppHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  void*   m_pArcReadBuff[ARC_MAX];
  void*   m_pMsgReadBuff;

  u8      m_DynaLoadSeq;
  u8      m_LytCount;
  u8      m_LytLoadSeq;
  u8      m_MsgLoadSeq;
  u8      m_WordSetSeq;
  u8      m_CursorSeq;

  gfl2::str::MsgData*   m_pMsgData;
  
  print::WordSet*       m_pWordSet;
  
  WordSetLoader*        m_pWordSetLoader;

  APP_PARAM*            m_pAppParam;

  TitleMenuUpperDraw*   m_pUpperDraw;
  TitleMenuLowerDraw*   m_pLowerDraw;
  MenuCursor*           m_pCursor;
};

GFL_NAMESPACE_END( TitleMenu )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_TITLE_MENU_FRAME_H_INCLUDED