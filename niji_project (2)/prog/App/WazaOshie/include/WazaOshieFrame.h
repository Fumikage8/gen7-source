#ifndef NIJI_PROJECT_APP_WAZAOSHIE_FRAME_H_INCLUDED
#define NIJI_PROJECT_APP_WAZAOSHIE_FRAME_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file WazaOshieFrame.h
 * @date 2015/07/01 14:47:31
 * @author fukushima_yuya
 * @brief 技教え・技忘れフレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "WazaOshieAppParam.h"

#include <App/WazaOshie/source/WazaOshieDrawListener.h>

#include <AppLib/include/Frame/AppFrameCellSuper.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )
class WazaOshieUpperDraw;    //!< 技教え上画面描画
class WazaOshieLowerDraw;    //!< 技教え下画面描画
class MenuCursor;
GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
class MsgData;
GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(print)
class WordSet;
GFL_NAMESPACE_END(print)

class WordSetLoader;


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )

//------------------------------------------------------------------------------
// @brief   技教え・技忘れフレーム
//------------------------------------------------------------------------------
class WazaOshieFrame
  : public applib::frame::CellSuper
  , public WazaOshieDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN( WazaOshieFrame );

public:
  struct SetupParam
  {
    app::util::Heap*                  heap;
    app::util::AppRenderingManager*   renderMan;
    gfl2::Fade::FadeManager*          fadeMan;
    App::PokeList::IntermediateData*  partyData;
    app::tool::AppCommonGrpIconData*  cmnIcon;
    bool                              createNumFont;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  WazaOshieFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~WazaOshieFrame( void );

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
  void Setup( APP_PARAM* appParam, SetupParam& param );


public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   技教え・技忘れ画面の終了通知
   */
  //------------------------------------------------------------------
  virtual void EndWazaOshie( void );

protected:
  int   m_subseq;     //!< プロセス関数の動作シーケンス
  bool  m_bEnd;       //!< Endフラグ

private:
  app::util::Heap*                  m_pAppHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  APP_PARAM*      m_pAppParam;

  SetupParam      m_SetupParam;

  void*   m_pArcReadBuff;
  void*   m_pMsgReadBuff;

  u8      m_LytLoadSeq;
  u8      m_MsgLoadSeq;
  u8      m_WordSetSeq;
  u8      m_CursorSeq;

  gfl2::str::MsgData*   m_pMsgData;

  print::WordSet*       m_pWordSet;

  WordSetLoader*        m_pWordSetLoader;

  WazaOshieUpperDraw*   m_pWazaOshieUpperDraw;

  WazaOshieLowerDraw*   m_pWazaOshieLowerDraw;

  MenuCursor*           m_pCursor;
};

GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_WAZAOSHIE_FRAME_H_INCLUDED
