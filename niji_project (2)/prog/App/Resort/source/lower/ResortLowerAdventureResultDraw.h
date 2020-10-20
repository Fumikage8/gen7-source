#if !defined( __RESORT_LOWER_ADVENTURE_RESULT_DRAW_H__ )
#define __RESORT_LOWER_ADVENTURE_RESULT_DRAW_H__
#pragma once
//======================================================================
/**
 * @file    ResortLowerAdventureResultDraw.h
 * @date    2015/10/02 11:54:47
 * @author  fukushima_yuya
 * @brief   ポケリゾート：探検リゾート結果画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortLowerBaseDraw.h"
#include <App/Resort/source/parts/ResortPartsPaneList.h>

#include <Savedata/include/ResortSave.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)
class ResortInfoWindow;
GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   探検リゾートアイテム確認画面(下)
//------------------------------------------------------------------------------
class LowerAdventureResultDraw
  : public LowerBaseDraw
  , public app::tool::PaneList::CallBack
  , public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN(LowerAdventureResultDraw);

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   param   生成パラメータ
   */
  //------------------------------------------------------------------
  LowerAdventureResultDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~LowerAdventureResultDraw( void ) { ; }

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  virtual void Initialize2D( InitParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   クラスの初期化
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  virtual void InitializeClass( InitParam& param );
  
  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト
   */
  //------------------------------------------------------------------
  virtual void InitializeText( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  virtual bool Terminate( void );

public:
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

public:
  //------------------------------------------------------------------
  /**
   * @brief   開く
   */
  //------------------------------------------------------------------
  virtual void Open( void );

  //------------------------------------------------------------------
  /**
   * @brief   閉じる
   */
  //------------------------------------------------------------------
  virtual void Close( void );

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
  void CreatePaneList( void );

  //------------------------------------------------------------------
  /**
   * @brief   ペインリストの更新
   */
  //------------------------------------------------------------------
  void UpdatePaneList( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   リスト背景の更新
   */
  //------------------------------------------------------------------
  void UpdateListBG( void );

  //------------------------------------------------------------------
  /**
   * @brief   インフォメーションボタンの有効設定更新
   */
  //------------------------------------------------------------------
  void UpdateInfoBtn( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   取得したアイテムの初期化
   */
  //------------------------------------------------------------------
  void InitResultItem( void );

  //------------------------------------------------------------------
  /**
   * @brief   取得したアイテムをセーブデータに反映
   */
  //------------------------------------------------------------------
  void SetResultItemData( void );

  //------------------------------------------------------------------
  /**
   * @brief   取得したアイテムが欠片かどうか
   *
   * @param[in]   itemNo    アイテムNo
   *
   * @retval  "true"なら欠片系
   * @retval  "false"ならそれ以外
   */
  //------------------------------------------------------------------
  bool IsItemKindKakera( u16 itemNo );

private:
  //------------------------------------------------------------------
  /**
   * @brief   インフォメーションウィンドウの表示
   */
  //------------------------------------------------------------------
  void OpenInformationWindow( u32 pos );


private:   //!< UIInputListener
  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション通知用リスナ
   *
   * @param   paneID
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneID );

  //------------------------------------------------------------------
  /**
   * @brief   ペイン選択アニメ開始通知用リスナー
   *
   * @param   paneID
   */
  //------------------------------------------------------------------
  virtual void OnLayoutPaneStartingAnimeEvent( const u32 paneID );

private:   //!< PaneListView::Listener
  //------------------------------------------------------------------
  /**
   * @brief   カーソル移動時のコールバック
   */
  //------------------------------------------------------------------
  virtual void CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   カーソルを非表示にするコールバック関数
   */
  //------------------------------------------------------------------
  virtual void CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos ) { ; }

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
  virtual void CallBack_ItemChange( void * work, u32 pos1, u32 pos2 ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   ペインリスト決定時に呼ばれるリスナー
   */
  //------------------------------------------------------------------
  virtual void PaneListItemSelectListener( u32 pos ) { ; }


private:   //!< ボタンマネージャ
  //------------------------------------------------------------------
  /**
   * @brief   ボタンの個別セットアップ
   *
   * @param   btnMan    ボタンマネージャ
   */
  //------------------------------------------------------------------
  virtual void SetupButton( app::tool::ButtonManager* btnMan );
  
private:
  static const u32 LIST_BTN_MAX = 7;
  static const u32 ITEM_RESULT_MAX = Savedata::ResortSave::ADVENTURE_ITEM_MAX;

private:
  ResortInfoWindow*   m_pInfoWindow;

private:
  ResortPaneList*                   m_pPaneList;

  app::tool::PaneList::PANE_DATA    m_paneData[LIST_BTN_MAX];

  gfl2::lyt::LytPicture*            m_pItemIcon[LIST_BTN_MAX];

  gfl2::lyt::LytTextBox*            m_pItemCount[LIST_BTN_MAX];

  gfl2::lyt::LytPane*               m_pItemBG[LIST_BTN_MAX];

  u32                               m_ItemMax;

  u16                               m_ItemNo[ITEM_RESULT_MAX];

  u32                               m_ItemNum[ITEM_RESULT_MAX];

  u16                               m_InfoItemNo[LIST_BTN_MAX]; //!< インフォメーションウィンドウで表示する道具No
};

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

#endif // __RESORT_LOWER_ADVENTURE_RESULT_DRAW_H__