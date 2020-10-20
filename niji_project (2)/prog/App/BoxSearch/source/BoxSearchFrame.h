#if !defined( __APP_BOX_SEARCH_FRAME_H_INCLUDED__ )
#define __APP_BOX_SEARCH_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    BoxSearchFrame.h
 * @date    2015/07/27 12:57:31
 * @author  fukushima_yuya
 * @brief   ボックス検索：フレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/BoxSearch/source/BoxSearchDrawListener.h"

#include <AppLib/include/Frame/AppFrameCellSuper.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <str/include/gfl2_Str.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BoxSearch)
struct APP_PARAM;
class MenuCursor;
class BoxSearchUpperDraw;
class BoxSearchLowerDraw;
class BoxSearchBGDraw;
GFL_NAMESPACE_END(BoxSearch)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(ui)
class UIView;
GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(print)
class WordSet;
GFL_NAMESPACE_END(print)

class WordSetLoader;


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )

//------------------------------------------------------------------------------
// @brief   ボックス検索：フレーム
//------------------------------------------------------------------------------
class BoxSearchFrame
  : public applib::frame::CellSuper
  , public BoxSearchDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN( BoxSearchFrame );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BoxSearchFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxSearchFrame( void ) { ; }

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

private:
  //------------------------------------------------------------------
  /**
   * @brief	  上画面の生成
   */
  //------------------------------------------------------------------
  void CreateUpperDraw( void );

  //------------------------------------------------------------------
  /**
   * @brief	  下画面の生成
   */
  //------------------------------------------------------------------
  void CreateLowerDraw( void );

  //------------------------------------------------------------------
  /**
   * @brief	  上画面背景の生成
   */
  //------------------------------------------------------------------
  void CreateUpperBGDraw( void );

  //------------------------------------------------------------------
  /**
   * @brief	  下画面背景の生成
   */
  //------------------------------------------------------------------
  void CreateLowerBGDraw( void );

private:
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
   * @brief	  ワードセットローダーの生成
   */
  //------------------------------------------------------------------
  bool CreateWordSetLoader( void );
  
  //------------------------------------------------------------------
  /**
   * @brief	  メニューカーソルの生成
   */
  //------------------------------------------------------------------
  bool CreateMenuCursor( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  動作に必要な引数を登録
   */
  //------------------------------------------------------------------
  void Setup( APP_PARAM* pParam, app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager );

public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   言語選択画面の終了通知
   */
  //------------------------------------------------------------------
  virtual void EndBoxSearch( void );

  //------------------------------------------------------------------
  /**
   * @brief   終了モードのセット
   * @param   endMode   終了モード
   */
  //------------------------------------------------------------------
  virtual void SetEndMode( EndMode endMode );

public:
  //------------------------------------------------------------------
  /**
   * @brief   上画面メッセージの設定(検索項目リスト用)
   */
  //------------------------------------------------------------------
  virtual void SetUpperMessage_ItemList( void );

  //------------------------------------------------------------------
  /**
   * @brief   上画面メッセージの設定(頭文字選択用)
   *
   * @param   listID    リストの種類
   */
  //------------------------------------------------------------------
  virtual void SetUpperMessage_Initial( u32 listID );

  //------------------------------------------------------------------
  /**
   * @brief   上画面メッセージの設定(ガイド用)
   *
   * @param   listID    リストの種類
   */
  //------------------------------------------------------------------
  virtual void SetUpperMessage_Guide( u32 listID );

  //------------------------------------------------------------------
  /**
   * @brief   上画面メッセージの設定(タイプ用)
   *
   * @param   listID  リストの種類
   * @param   type1   タイプ１
   * @param   type2   タイプ２
   */
  //------------------------------------------------------------------
  virtual void SetUpperMessage_Type( u32 listID, u8 type1, u8 type2 );

  //------------------------------------------------------------------
  /**
   * @brief   上画面メッセージの設定(特性用)
   *
   * @param   tokusei   特性No
   */
  //------------------------------------------------------------------
  virtual void SetUpperMessage_Tokusei( TokuseiNo tokusei );

  //------------------------------------------------------------------
  /**
   * @brief   上画面メッセージの設定(性格用)
   *
   * @param   seikaku   性格ID
   */
  //------------------------------------------------------------------
  virtual void SetUpperMessage_Seikaku( pml::pokepara::Seikaku seikaku );

protected:
  int   m_subseq;     //!< プロセス関数の動作シーケンス
  bool  m_bEnd;       //!< Endフラグ

private:
  enum ArcIndex {
    ARC_LOWER,
    ARC_LOWER_BG,
    ARC_UPPER,
    ARC_UPPER_BG,
    ARC_NUM,
  };
  enum MsgIndex {
    MSG_BOX_SEARCH,
    MSG_TOKUSEI_INFO,
    MSG_INITIAL,
    MSG_NUM,
  };
  enum CursorIndex {
    CUR_ITEM_LIST,
    CUR_CONT_LIST,
    CUR_NUM,
  };
private:
  app::util::Heap*                  m_pAppHeap;

  app::util::AppRenderingManager*   m_pAppRenderingManager;

private:
  void*   m_pArcReadBuff[ARC_NUM];

  u32     m_ArcCount;

  u32     m_LoadArcSeq;

private:
  void*                 m_pMsgReadBuff[MSG_NUM];

  u32                   m_MsgCount;

  u32                   m_LoadMsgSeq;

  gfl2::str::MsgData*   m_pMsgData[MSG_NUM];

private:
  print::WordSet*       m_pWordSet;

  WordSetLoader*        m_pWordSetLoader;

  u32                   m_WordSetSeq;

private:
  MenuCursor*           m_pMenuCursor[CUR_NUM];

  u32                   m_CursorCount;

  u32                   m_CursorSeq;

private:
  APP_PARAM*            m_pAppParam;

  app::ui::UIView*      m_pParentView;

  BoxSearchUpperDraw*   m_pUpperDraw;

  BoxSearchLowerDraw*   m_pLowerDraw;

  BoxSearchBGDraw*      m_pUpperBGDraw;

  BoxSearchBGDraw*      m_pLowerBGDraw;
};

GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )

#endif // __APP_BOX_SEARCH_FRAME_H_INCLUDED__