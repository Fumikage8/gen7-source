﻿#if !defined( __RESORT_LOWER_FARM_LIST_DRAW_H__ )
#define __RESORT_LOWER_FARM_LIST_DRAW_H__
#pragma once
//======================================================================
/**
 * @file    ResortLowerFarmListDraw.h
 * @date    2015/10/02 11:19:11
 * @author  fukushima_yuya
 * @brief   ポケリゾート：植えるきのみのリスト画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortLowerBaseDraw.h"

#include <App/Resort/source/parts/ResortPartsPaneList.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)
class ResortInfoWindow;     //!< インフォメーションウィンドウ
GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   ポケリゾート：植えるきのみのリスト画面(下)
//------------------------------------------------------------------------------
class LowerFarmListDraw
  : public LowerBaseDraw
  , public app::tool::PaneList::CallBack
  , public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN(LowerFarmListDraw);

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   param   生成パラメータ
   */
  //------------------------------------------------------------------
  LowerFarmListDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~LowerFarmListDraw( void ) { ; }


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
   * @brief   ペインの初期化
   */
  //------------------------------------------------------------------
  virtual void InitializePane( void );

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
   * @brief   はたけのインデックスを設定
   *
   * @param   fieldIndex    はたけのインデックス
   */
  //------------------------------------------------------------------
  void SetFieldIndex( u8 fieldIndex ) { m_FieldIndex = fieldIndex; }

  //------------------------------------------------------------------
  /**
   * @brief   はたけのインデックスを取得
   *
   * @return  はたけのインデックス
   */
  //------------------------------------------------------------------
  u8 GetFieldIndex( void ) const { return m_FieldIndex; }
  
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

  //------------------------------------------------------------------
  /**
   * @brief   きのみリスト画面を開けるか？
   *
   * @return  "true  == 開ける"
   * @return  "false == 開けない"
   */
  //------------------------------------------------------------------
  bool IsCanOpen( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   アイテムアイコンの生成
   */
  //------------------------------------------------------------------
  void CreateItemIcon( void );

  //------------------------------------------------------------------
  /**
   * @brief   アイテムアイコンのロードが完了したか
   *
   * @return  "true  == 完了"
   * @return  "false == それ以外"
   */
  //------------------------------------------------------------------
  bool IsLoadedItemIcon( void );

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
   * @brief   ペインリストの更新をリクエスト
   */
  //------------------------------------------------------------------
  void RequestUpdatePaneList( void );
  
  //------------------------------------------------------------------
  /**
   * @brief   インフォメーションボタンの有効設定更新
   */
  //------------------------------------------------------------------
  void UpdateInfoBtn( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   植えるきのみの追加
   * @param   pos   データリストのインデックス
   */
  //------------------------------------------------------------------
  void AddNuts( u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   植えるきのみの除外
   * @param   pos   除外するきのみのインデックス
   */
  //------------------------------------------------------------------
  void RemoveNuts( u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   除外ボタンの有効設定の更新
   */
  //------------------------------------------------------------------
  void UpdateRemoveBtn( void );

  //------------------------------------------------------------------
  /**
   * @brief   はたけの状態を更新
   */
  //------------------------------------------------------------------
  void UpdateField( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   インフォメーションウィンドウの表示
   */
  //------------------------------------------------------------------
  void OpenInformationWindow( u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   決定ボタンの更新
   */
  //------------------------------------------------------------------
  void UpdateDecideBtn( void );

  //------------------------------------------------------------------
  /**
   * @brief   決定処理
   */
  //------------------------------------------------------------------
  void DecideFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   キャンセル処理
   */
  //------------------------------------------------------------------
  void CancelFunc( void );
  
private:
  //------------------------------------------------------------------
  /**
   * @brief   アイテム情報の取得
   *
   * @param   index     [IN]データインデックス
   * @param   itemNo    [OUT]道具No
   * @param   itemNum   [OUT]道具の数
   */
  //------------------------------------------------------------------
  void SetItemInfo( u32 index, u16* itemNo, u32* itemNum );

private:
  //------------------------------------------------------------------
  /**
   * @brief   アイテム名の表示
   *
   * @param   textBox   テキストボックス
   * @param   itemNo    アイテムNo
   * @param   itemNum   アイテムの数
   */
  //------------------------------------------------------------------
  void SetItemName( gfl2::lyt::LytTextBox* textBox, u16 itemNo, u32 itemNum );
  
  //------------------------------------------------------------------
  /**
   * @brief   アイテム数の表示
   *
   * @param   pane_index    ペインのインデックス
   * @param   itemNum       アイテムの数
   */
  //------------------------------------------------------------------
  void SetItemNum( u32 pane_index, u32 itemNum );
  
  //------------------------------------------------------------------
  /**
   * @brief   アイテムアイコンの差し替え
   *
   * @param   pane_index    ペインのインデックス
   * @param   itemNo        アイテムNo
   * @param   itemNum       アイテム数
   */
  //------------------------------------------------------------------
  void SetItemIcon( u32 pane_index, u16 itemNo, u32 itemNum );

  //------------------------------------------------------------------
  /**
   * @brief   アイテムアイコンの差し替え
   *
   * @param   itemNo    アイテムNo
   * @param   picture   ピクチャペイン
   */
  //------------------------------------------------------------------
  void _SetItemIcon( u16 itemNo, gfl2::lyt::LytPicture* picture );
  
  //------------------------------------------------------------------
  /**
   * @brief   きのみ未所持のアラートメッセージの表示
   */
  //------------------------------------------------------------------
  void SetAlertMessage( void );



private:   // app::ui::UIInputListener
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

private:  // app::tool::PaneListView::Listener
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
  virtual void PaneListItemSelectListener( u32 pos );


private:   // app::tool::ButtonManager
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
  static const u32 FIELD_PUT_MAX = 6;

private:
  ResortInfoWindow*   m_pInfoWindow;

private:
  u8    m_FieldIndex;

  u32   m_ItemMax;

  bool  m_DiaryFlag;

private:
  gfl2::lyt::LytTextBox*    m_pHeaderText;

private:
  ResortPaneList*           m_pPaneList;

  app::tool::PaneList::PANE_DATA   m_paneData[LIST_BTN_MAX];

  gfl2::lyt::LytPicture*    m_pItemIcon[LIST_BTN_MAX];

  gfl2::lyt::LytTextBox*    m_pItemCount[LIST_BTN_MAX];

  u16   m_ItemNo[LIST_BTN_MAX];

  gfl2::lyt::LytTextBox*    m_pAlertText;

private:
  u16   m_FieldItemNo[FIELD_PUT_MAX];

  bool  m_FieldItemEnable[FIELD_PUT_MAX];

  gfl2::lyt::LytPicture*    m_pFieldItemIcon[FIELD_PUT_MAX];

  gfl2::lyt::LytPane*       m_pRemoveMarkPane[FIELD_PUT_MAX];

  u8    m_FieldAddCount;

private:
  gfl2::lyt::LytParts*      m_pListBtnParts[LIST_BTN_MAX];
};

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

#endif // __RESORT_LOWER_FARM_LIST_DRAW_H__