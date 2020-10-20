#if !defined( __APP_FINDER_MENU_SCORE_DRAW_H_INCLUDED__ )
#define __APP_FINDER_MENU_SCORE_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderMenuScoreDraw.h
 * @date    2016/01/12 20:11:12
 * @author  fukushima_yuya
 * @brief   ポケファインダーメニュー：得点表示画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/FinderMenu/include/FinderMenuAppParam.h>
#include <App/FinderMenu/source/parts/FinderMenuWindow.h>

#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)
class FinderMenuDrawListener;
class DrawerBase;
GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(GameSys)
class GameManager;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(Savedata)
class PokeFinderSave;
GFL_NAMESPACE_END(Savedata)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//------------------------------------------------------------------------------
// @brief   ポケファインダーメニュー：得点表示画面ウィンドウ
//------------------------------------------------------------------------------
class FinderMenuScoreMessageWindow
{
GFL_FORBID_COPY_AND_ASSIGN( FinderMenuScoreMessageWindow );

public:
  //------------------------------------------------------------------
  /**
   * @brief    コンストラクタ
   */
  //------------------------------------------------------------------
  FinderMenuScoreMessageWindow( DrawerBase* base, app::util::AppRenderingManager* renderMan, MessageWindowListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief    デストラクタ
   */
  //------------------------------------------------------------------
  ~FinderMenuScoreMessageWindow( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief    初期化
   */
  //------------------------------------------------------------------
  bool InitFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief    破棄
   */
  //------------------------------------------------------------------
  bool EndFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief    更新
   */
  //------------------------------------------------------------------
  void UpdateFunc( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief    開く
   */
  //------------------------------------------------------------------
  void Open( WindowKind kind );

  //------------------------------------------------------------------
  /**
   * @brief    閉じる
   */
  //------------------------------------------------------------------
  void Close( void );

  //------------------------------------------------------------------
  /**
   * @brief    開いているか
   */
  //------------------------------------------------------------------
  bool IsOpened( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief    ウィンドウの種類を取得
   */
  //------------------------------------------------------------------
  WindowKind GetWindowKind( void ) const;

private:
  //------------------------------------------------------------------
  /**
   * @brief   カーソルの表示切り替え
   */
  //------------------------------------------------------------------
  void SetVisibleCursor( void );
  void SetInvisibleCursor( void );

private:
  DrawerBase*               m_pBase;

  MenuWindow*               m_pMenuWindow;

  MessageWindowListener*    m_pListener;

private:
  u32           m_InitSeq;

  u32           m_EndSeq;

  u32           m_MainSeq;

  WindowKind    m_WndKind;

  u32           m_ActiveAnim;

  WindowKind    m_OpenReqKind;

  bool          m_isOpened;

  bool          m_CursorVisible;

  bool          m_PlaySE;

private:
  gfl2::str::StrWin::Result   m_Result;

private:
  gfl2::lyt::LytTextBox*    m_pMsgLine_1;

  gfl2::lyt::LytTextBox*    m_pMsgLine_2;

  gfl2::lyt::LytPane*       m_pMsgCursor;
};



//------------------------------------------------------------------------------
// @brief   ポケファインダーメニュー：得点表示画面
//------------------------------------------------------------------------------
class FinderMenuScoreDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
  , public MessageWindowListener
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderMenuScoreDraw );

public:
  struct CreateParam
  {
    app::util::Heap*                    heap;
    app::util::AppRenderingManager*     renderMan;
    MenuCursor*                         cursor;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FinderMenuScoreDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~FinderMenuScoreDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   このビュー専用のUIリスナー設定
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( FinderMenuDrawListener* pListener );

  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   appParam      外部設定パラメータ
   * @param   pAppLytBuff   レイアウトバイナリ
   * @param   grpFontBuff   グラフィックフォント
   * @param   msgData       メッセージデータ
   * @param   wordSet       ワードセット
   */
  //------------------------------------------------------------------
  void Init( APP_PARAM* appParam, void* pAppLytBuff, void* grpFontBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );

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
  bool IsExit( void ) { return true; }

  //------------------------------------------------------------------
  /**
   * @brief   オープン
   */
  //------------------------------------------------------------------
  void Open( void );

  //------------------------------------------------------------------
  /**
   * @brief   クローズ
   */
  //------------------------------------------------------------------
  void Close( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pHeap         ヒープ
   * @param   pAppLytBuff   レイアウトバイナリデータ
   * @param   grpFontBuff   グラフィックフォント
   */
  //------------------------------------------------------------------
  void Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff, void* grpFontBuff );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   *
   * @param   photoIdx    写真のインデックス
   */
  //------------------------------------------------------------------
  void Setup( u32 photoIdx );

  //------------------------------------------------------------------
  /**
   * @brief   最高点のセットアップ
   *
   * @param   score   点数
   */
  //------------------------------------------------------------------
  void Setup_HighScore( u32 score );

  //------------------------------------------------------------------
  /**
   * @brief   総合点のセットアップ
   */
  //------------------------------------------------------------------
  void Setup_TotalScore( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   メインシーケンス
   */
  //------------------------------------------------------------------
  void Sequence_Main( void );

  //------------------------------------------------------------------
  /**
   * @brief   噴出しの尻尾の更新
   */
  //------------------------------------------------------------------
  void UpdateMsgTail( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   カメラアプリのバージョンを取得
   *
   * @return  カメラアプリのバージョン
   */
  //------------------------------------------------------------------
  u16 GetVersion( void );

  //------------------------------------------------------------------
  /**
   * @brief   カメラアプリのバージョンアップ処理
   */
  //------------------------------------------------------------------
  void VersionUp( void );

  //------------------------------------------------------------------
  /**
   * @brief   バージョンアップか？
   *
   * @return  "true  = バージョンアップ"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool CheckVersionUp( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   最高得点演出の開始(アウト)
   */
  //------------------------------------------------------------------
  void StartHighScoreOutDirect( void );

  //------------------------------------------------------------------
  /**
   * @brief   最高得点演出の開始(イン)
   */
  //------------------------------------------------------------------
  void StartHighScoreInDirect( void );

  //------------------------------------------------------------------
  /**
   * @brief   総合得点演出の開始
   */
  //------------------------------------------------------------------
  void StartTotalScoreDirect( void );

  //------------------------------------------------------------------
  /**
   * @brief   バージョンアップ演出の開始
   */
  //------------------------------------------------------------------
  void StartVersionUpDirect( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   バージョンアップ達成ウィンドウタイプの取得
   *
   * @return  WindowKind    バージョンアップ達成用
   */
  //------------------------------------------------------------------
  WindowKind GetWindowKind_VersionUp( void );

  //------------------------------------------------------------------
  /**
   * @brief   新バージョン機能説明ウィンドウタイプの取得
   *
   * @return  WindowKind    新バージョン機能説明用
   */
  //------------------------------------------------------------------
  WindowKind GetWindowKind_VersionDesc( void );

  //------------------------------------------------------------------
  /**
   * @brief   最終コメントウィンドウタイプの取得
   *
   * @return  WindowKind    最終コメント用
   */
  //------------------------------------------------------------------
  WindowKind GetWindowKind_FinalComment( void );

  //------------------------------------------------------------------
  /**
   * @brief   次の目標ウィンドウタイプの取得
   *
   * @return  WindowKind    次の目標用
   */
  //------------------------------------------------------------------
  WindowKind GetWindowKind_Next( void );

  //------------------------------------------------------------------
  /**
   * @brief   応援ウィンドウタイプの取得
   *
   * @return  WindowKind    応援用
   */
  //------------------------------------------------------------------
  WindowKind GetWindowKind_Support( void );

private:  //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   閉じたことを通知
   */
  //------------------------------------------------------------------
  virtual void OnClosed( void );

private:
  app::util::Heap*                    m_pHeap;        //!< ヒープ

  FinderMenuDrawListener*             m_uiListener;   //!< UIイベントリスナー

  app::util::AppRenderingManager*     m_pAppRenderingManager;

  gfl2::str::MsgData*                 m_pMsgData;

  print::WordSet*                     m_pWordSet;

  APP_PARAM*                          m_pAppParam;

  Savedata::PokeFinderSave*           m_pFinderSave;

  GameSys::GameManager*               m_pGameMan;

private:
  DrawerBase*                       m_pDrawerBase;

  FinderMenuScoreMessageWindow*     m_pMsgWindow;

private:
  u8    m_Seq;

  u8    m_MainSeq;

  u32   m_PhotoScore;

  u32   m_PhotoHighScore;

  u32   m_VersionUpAnim;

  u32   m_StartME;

  u32   m_SeWaitFrame;

private:
  gfl2::lyt::LytTextBox*      m_pTextNowScore;

  gfl2::lyt::LytTextBox*      m_pTextHighScore;

  gfl2::lyt::LytTextBox*      m_pTextTotalScore;

  gfl2::lyt::LytPane*         m_pTotalScorePlate;

  gfl2::lyt::LytPane*         m_pMessageTail;

  gfl2::lyt::LytPane*         m_pMessageWindow;
};

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)

#endif // __APP_FINDER_MENU_SCORE_DRAW_H_INCLUDED__