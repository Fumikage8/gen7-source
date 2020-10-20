#ifndef NIJI_PROJECT_APP_TITLE_MENU_UPPER_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_TITLE_MENU_UPPER_DRAW_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file    TitleMenuUpperDraw.h
 * @date    2015/07/21 15:13:34
 * @author  fukushima_yuya
 * @brief   タイトルメニュー上画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/TitleMenu/include/TitleMenuAppParam.h"
#include "App/TitleMenu/source/TitleMenuConst.h"

#include <AppLib/include/Tool/app_tool_MenuWindowListener.h>
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( TitleMenu )
class TitleMenuCursor;
class YesNoWindow;
class MenuCursor;
GFL_NAMESPACE_END( TitleMenu )
GFL_NAMESPACE_BEGIN( Tool )
class TalkWindow;
GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class AppToolTrainerIconRendering;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_BEGIN( util )
class KeyRepeatController;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( GameSys )
class GameData;
GFL_NAMESPACE_END( GameSys )

GFL_NAMESPACE_BEGIN( Savedata )
class MyItem;
GFL_NAMESPACE_END( Savedata )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( TitleMenu )

//------------------------------------------------------------------------------
// @brief   タイトルメニュー下画面
//------------------------------------------------------------------------------
class TitleMenuUpperDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
  , public app::tool::MenuWindowListener
{
  GFL_FORBID_COPY_AND_ASSIGN( TitleMenuUpperDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief    コンストラクタ
   */
  //------------------------------------------------------------------
  TitleMenuUpperDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager, MenuCursor* pCursor );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~TitleMenuUpperDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   pAppParam       外部設定パラメータ
   * @param   pAppLytBuff     レイアウトバイナリ
   * @param   pAppWndLytBuff  レイアウトバイナリ
   * @param   msgData         メッセージデータ
   * @param   wordSet         ワードセット
   * @param   wordSetLoader   ワードセットローダー
   */
  //------------------------------------------------------------------
  void Init( APP_PARAM* pAppParam, void* pAppLytBuff, void* pAppWndLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet, WordSetLoader* wordSetLoader );

  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  bool End( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  virtual void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   破棄チェック
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool IsExit( void );

  //------------------------------------------------------------------
  /**
   * @brief   オープン
   */
  //------------------------------------------------------------------
  void TitleMenuOpen( void );

  //------------------------------------------------------------------
  /**
   * @brief   クローズ
   */
  //------------------------------------------------------------------
  void TitleMenuClose( void );

  //------------------------------------------------------------------
  /**
   * @brief   イン・アウトアニメが終了したかどうか
   * @param   in    インアニメかどうか
   * @return  true = 終了、false = 継続
   */
  //------------------------------------------------------------------
  bool IsEndInOutAnim( bool in );

  //------------------------------------------------------------------
  /**
   * @brief    このビュー専用のUIリスナー設定
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( TitleMenuDrawListener* pListener );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pAppLytBuff     レイアウトバイナリ
   * @param   pAppWndLytBuff  レイアウトバイナリ
   */
  //------------------------------------------------------------------
  void Initialize2D( void* pAppLytBuff, void* pAppWndLytBuff );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

  //------------------------------------------------------------------
  /**
   * @brief   ビーズアイコンの初期化
   */
  //------------------------------------------------------------------
  void InitBeadsIcon( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   メインシーケンス：準備
   */
  //------------------------------------------------------------------
  bool MainSequenceReady( void );

  //------------------------------------------------------------------
  /**
   * @brief   つづきからはじめるを選択
   */
  //------------------------------------------------------------------
  void SelectContinue( void );

  //------------------------------------------------------------------
  /**
   * @brief   ふしぎなおくりものを選択
   */
  //------------------------------------------------------------------
  void SelectPresent( void );

  //------------------------------------------------------------------
  /**
   * @brief   ライブ大会を選択
   */
  //------------------------------------------------------------------
  void SelectLiveCup( void );

  //------------------------------------------------------------------
  /**
   * @brief   セーブデータの初期化シーケンス：初期化
   */
  //------------------------------------------------------------------
  void SubSequenceInit( void );

  //------------------------------------------------------------------
  /**
   * @brief   セーブデータの初期化シーケンス：処理
   */
  //------------------------------------------------------------------
  void SubSequenceMain( void );

  //------------------------------------------------------------------
  /**
   * @brief   セーブデータの初期化シーケンス：解放
   */
  //------------------------------------------------------------------
  void SubSequenceTerm( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   顔アイコンの生成
   */
  //------------------------------------------------------------------
  bool CreateFaceIcon( void );

  //------------------------------------------------------------------
  /**
   * @brief   顔アイコンの破棄
   */
  //------------------------------------------------------------------
  bool DeleteFaceIcon( void );

public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション通知用リスナ
   * @param   paneId
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );

  //------------------------------------------------------------------
  /**
   * @brief   キー入力通知用リスナ
   * @param   pKey
   * @param   pStick
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  //------------------------------------------------------------------
  /**
   * @brief   メニューウィンドウの選択
   * @param   選択された項目ID
   */
  //------------------------------------------------------------------
  virtual void SelectMenu( u32 id );

public:   //!< ボタンマネージャ
  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   */
  //------------------------------------------------------------------
  void CreateButtonManager( void );

private:
  app::util::Heap*        m_pHeap;        //!< ヒープ
  TitleMenuDrawListener*  m_uiListener;   //!< UIイベントリスナー

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  GameSys::GameManager*     m_pGameManager;

  gfl2::Fade::FadeManager*  m_pFadeManager;

  gfl2::str::MsgData*       m_pMsgData;

  print::WordSet*           m_pWordSet;

  WordSetLoader*            m_pWordSetLoader;

  u8    m_Seq;
  u8    m_SubSeq;

  APP_PARAM*                m_pAppParam;

  DrawerBase*               m_pDrawerBase;

  MenuCursor*               m_pCursor;

  DispMode                  m_DispMode;

  u8                        m_InitSaveSeq;

  u8                        m_yesNoRes;

  App::Tool::TalkWindow*    m_pTalkWindow;

  YesNoWindow*              m_pWindow;

  GameSys::GameData*        m_pGameData;

  bool                      m_isInitSaveData;

  app::util::KeyRepeatController*   m_pKeyController;

  u32                               m_CursorPos;

private:
  u8        m_CreateSeq;

  u8        m_DeleteSeq;

  app::tool::AppToolTrainerIconRendering*     m_pTrainerIconRendering;

  Savedata::MyStatus*     m_pMyStatus;

  Savedata::MyItem*       m_pMyItem;
};

GFL_NAMESPACE_END( TitleMenu )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_TITLE_MENU_UPPER_DRAW_H_INCLUDED