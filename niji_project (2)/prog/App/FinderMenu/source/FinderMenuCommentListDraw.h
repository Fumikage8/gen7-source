#if !defined( __FINDER_MENU_COMMENT_LIST_DRAW_H__ )
#define __FINDER_MENU_COMMENT_LIST_DRAW_H__
#pragma once
//======================================================================
/**
 * @file    FinderMenuCommentListDraw.h
 * @date    2015/11/02 19:26:08
 * @author  fukushima_yuya
 * @brief   評価リスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/FinderMenu/include/FinderMenuAppParam.h>

#include <AppLib/include/Tool/app_tool_PaneListView.h>
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


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//------------------------------------------------------------------------------
// @brief   写真選択画面の描画
//------------------------------------------------------------------------------
class FinderMenuCommentListDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
  , public app::tool::PaneList::CallBack
  , public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderMenuCommentListDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FinderMenuCommentListDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~FinderMenuCommentListDraw( void ) { ; }

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
   * @param   msgData       メッセージデータ
   * @param   wordSet       ワードセット
   */
  //------------------------------------------------------------------
  void Init( APP_PARAM* appParam, void* pAppLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );

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
  void Open( void );

  //------------------------------------------------------------------
  /**
   * @brief   クローズ
   */
  //------------------------------------------------------------------
  void Close( void );

  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   * @param   photoIdx    写真インデックス
   */
  //------------------------------------------------------------------
  void Setup( u32 photoIdx );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pHeap         ヒープ
   * @param   pAppLytBuff   レイアウトバイナリデータ
   */
  //------------------------------------------------------------------
  void Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ペインリストの初期化
   */
  //------------------------------------------------------------------
  void InitPaneList( void );

  //------------------------------------------------------------------
  /**
   * @brief   ペインリストの生成
   */
  //------------------------------------------------------------------
  void CreatePaneList( u32 listMax );

  //------------------------------------------------------------------
  /**
   * @brief   ペインリストの更新
   */
  //------------------------------------------------------------------
  void UpdatePaneList( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   リストパラメータのセット
   */
  //------------------------------------------------------------------
  void SetParamList( app::tool::PaneList::SETUP_PARAM* param, u32 listMax );

public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション通知用リスナ
   * @param   paneId
   */
  //------------------------------------------------------------------
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );

  //------------------------------------------------------------------
  /**
   * @brief   カーソル移動時のコールバック
   */
  //------------------------------------------------------------------
  virtual void CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   カーソルを非表示にするコールバック関数
   */
  //------------------------------------------------------------------
  virtual void CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   リストの項目を描画するコールバック関数
   */
  //------------------------------------------------------------------
  virtual void CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   入れ替え用コールバック関数
   */
  //------------------------------------------------------------------
  virtual void CallBack_ItemChange( void * work, u32 pos1, u32 pos2 );

  //------------------------------------------------------------------
  /**
   * @brief   ペインリスト決定時に呼ばれるリスナー
   */
  //------------------------------------------------------------------
  virtual void PaneListItemSelectListener( u32 pos );

public:   //!< ボタンマネージャ関連
  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   */
  //------------------------------------------------------------------
  void CreateButtonManager( void );

private:
  static const u32 LIST_MAX = 8;

private:
  app::util::Heap*                  m_pHeap;        //!< ヒープ

  FinderMenuDrawListener*           m_uiListener;   //!< UIイベントリスナー

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  gfl2::str::MsgData*               m_pMsgData;

  print::WordSet*                   m_pWordSet;

  APP_PARAM*                        m_pAppParam;

  u8    m_Seq;

private:
  DrawerBase*             m_pDrawerBase;

  Field::TrialModel::FinderPhotoData*   m_pPhotoData;

  gfl2::lyt::LytPane*     m_pNextBtn;

private:  //!< ペインリスト
  app::tool::PaneListView*          m_pPaneListView;

  app::tool::PaneList::PANE_DATA    m_paneData[LIST_MAX];
};

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)

#endif // __FINDER_MENU_COMMENT_LIST_DRAW_H__