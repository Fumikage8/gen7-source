#ifndef NIJI_PROJECT_FIELD_FINDER_FRAME_H_INCLUDED
#define NIJI_PROJECT_FIELD_FINDER_FRAME_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    FinderFrame.h
 * @author  fukushima_yuya
 * @date    2015.05.26
 * @brief   ポケファインダーUIフレーム
 */
//==============================================================================

// gflib2
#include "FinderAppParam.h"

#include <App/Finder/source/FinderDrawListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )
class FinderUpperDraw;
class FinderLowerDraw;
class FinderRotomFade;
class MenuCursor;
GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_BEGIN( Tool )
class TalkWindow;
GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_BEGIN( ui )
class UIView;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN(Savedata)
class PokeFinderSave;
GFL_NAMESPACE_END(Savedata)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
class MsgData;
GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(print)
class WordSet;
GFL_NAMESPACE_END(print)


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )

//------------------------------------------------------------------------------
// @brief   ポケファインダーUIフレーム
//------------------------------------------------------------------------------
class FinderFrame
  : public applib::frame::CellSuper
  , public FinderDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderFrame );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FinderFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~FinderFrame( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   動作に必要な引数を登録
   */
  //------------------------------------------------------------------
  void Setup( APP_PARAM* pParam, app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderingManager, bool debugFlg = false );

  //------------------------------------------------------------------
  /**
   * @brief   終了コール
   */
  //------------------------------------------------------------------
  void EndFinder( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   PROCの初期化を行う関数
   * @return Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン更新を行う関数
   * @return Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
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
   * @return Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result EndFunc( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   レイアウトデータの読み込み
   */
  //------------------------------------------------------------------
  bool LoadLayoutData( void );

  //------------------------------------------------------------------
  /**
   * @brief   グラフィックフォントデータの読み込み
   */
  //------------------------------------------------------------------
  bool LoadGrpFont( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージデータの読み込み
   */
  //------------------------------------------------------------------
  bool LoadMsgData( void );

  //------------------------------------------------------------------
  /**
   * @brief   メニューカーソルの生成
   */
  //------------------------------------------------------------------
  bool CreateMenuCursor( void );

  //------------------------------------------------------------------
  /**
   * @brief   会話ウィンドウの生成
   */
  //------------------------------------------------------------------
  bool CreateTalkWindow( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   チュートリアルメッセージのセット
   */
  //------------------------------------------------------------------
  void SetTutorialMsg( void );

public:   //! リスナー
  //------------------------------------------------------------------
  /**
   * @brief   終了モードの設定
   * @param   endMode   終了モード
   */
  //------------------------------------------------------------------
  virtual void SetEndMode( EndMode endMode );

  //------------------------------------------------------------------
  /**
   * @brief   ポケファインダーUI画面の操作開始
   *
   * @param   tutorial    チュートリアルフラグ(trueならチュートリアル)
   */
  //------------------------------------------------------------------
  virtual void FinderStart( bool tutorial = false );

  //------------------------------------------------------------------
  /**
   * @brief   ポケファインダーUI画面を閉じる
   */
  //------------------------------------------------------------------
  virtual void FinderClose( void );

  //------------------------------------------------------------------
  /**
   * @brief   シャッターアニメの開始
   */
  //------------------------------------------------------------------
  virtual void StartShutterAnim( void );

  //------------------------------------------------------------------
  /**
   * @brief   シャッターアニメの終了
   */
  //------------------------------------------------------------------
  virtual void EndShutterAnim( void );
 
  //------------------------------------------------------------------
  /**
   * @brief   照準の変更
   */
  //------------------------------------------------------------------
  virtual void ChangeSight( void );

  //------------------------------------------------------------------
  /**
   * @brief   ピントの変更
   * @param   mode    ピントのモード
   */
  //------------------------------------------------------------------
  virtual void SetPintMode( u8 mode );

  //------------------------------------------------------------------
  /**
   * @brief   システムメッセージの表示
   * @param   msgID     メッセージID
   * @param   endMode   終了モード
   */
  //------------------------------------------------------------------
  virtual void OpenSystemMessage( u32 msgID, EndMode endMode );

  //------------------------------------------------------------------
  /**
   * @brief   ファインダーの終了
   * @param   endMode   終了モード
   */
  //------------------------------------------------------------------
  virtual void FinishFinder( EndMode endMode );

  //------------------------------------------------------------------
  /**
   * @brief   情報表示の切り替え
   */
  //------------------------------------------------------------------
  virtual void ChangeVisibleInfo( void );


  //
  // ファインダーイベント向け公開関数
  //

  //------------------------------------------------------------------
  /**
   * @brief   タッチパッド、ボタン、アナログキーの入力検出有効設定
   *
   * @param   isEnable 入力検出を有効化するならtrue、無効化するならfalseを指定
   */
  //------------------------------------------------------------------
  void SetInputEnabled( bool isEnable );

  //------------------------------------------------------------------
  /**
  * @brief  開始準備が完了しているか取得する
  *
  * @return true 完了 false 完了していない
  */
  //------------------------------------------------------------------
  b32 IsStartSetupEnd( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief   キャンセルボタンが押されたか取得
  *
  * @return true 押された false 押されていない
  */
  //------------------------------------------------------------------
  b32 IsCanceled( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief   シャッター動作中か取得
  *
  * @return true 動作中 false 動作していない
  */
  //------------------------------------------------------------------
  b32 IsShutteUpdate( void ) const;

  //------------------------------------------------------------------
  /**
  * @brief   ロトムフェードの開始
  */
  //------------------------------------------------------------------
  void StartRotomFade( void );

protected:
  int   m_subseq;     //!< プロセス関数の動作シーケンス
  bool  m_bEnd;       //!< Endフラグ

private:
  enum {
    ARC_FINDER,
    ARC_FLD_WND,
    ARC_ROTOM,
    ARC_NUM,
  };

private:
  app::util::Heap*                  m_pAppHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  Savedata::PokeFinderSave*         m_pFinderSave;

  void*   m_pArcReadBuff[ARC_NUM];
  void*   m_pGrpFontReadBuff;
  void*   m_pMsgReadBuff;

  u8      m_LytCount;
  u8      m_LytLoadSeq;
  u8      m_GrpFontLoadSeq;
  u8      m_MsgLoadSeq;
  u8      m_CursorSeq;
  u8      m_TalkWndSeq;

  gfl2::str::MsgData*     m_pMsgData;
  
  print::WordSet*         m_pWordSet;

private:
  APP_PARAM*              m_pAppParam;

  FinderUpperDraw*        m_pUpperDraw;
  FinderLowerDraw*        m_pLowerDraw;
  FinderRotomFade*        m_pRotomFade;

  MenuCursor*             m_pCursor;      //!< カーソル

  App::Tool::TalkWindow*  m_pTalkWindow;

  applib::frame::Result   m_Result;

  app::ui::UIView*        m_pParentView;
};

GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_FIELD_FINDER_FRAME_H_INCLUDED
