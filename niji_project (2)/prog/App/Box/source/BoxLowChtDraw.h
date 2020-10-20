//=============================================================================
/**
 * @file    BoxLowChtDraw.h
 * @brief   チャット下画面描画
 * @author  k.ohno
 * @date    January 18, 2015
 *
 * @author	Hiroyuki Nakamura
 * @data	  2015.10.28
 */
//=============================================================================
#if !defined( __BOX_LOW_CHT_DRAW_H__ )
#define	__BOX_LOW_CHT_DRAW_H__

#pragma once

// niji
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/app_util_2d.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"
#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/ui/UIInputListener.h"
#include "Print/include/WordSet.h"
// module
#include "BoxLowChtDrawListener.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

//=============================================================================
/**
 * @class BoxLowChtDraw
 * @brief チャット下画面描画クラス
 */
//=============================================================================
class BoxLowChtDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
  , public app::tool::PaneList::CallBack
  , public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxLowChtDraw);
  
private:
  //! レイアウトID    
  enum LytWorkID
  {
    LYT_WORK_NORMAL=0,
    LYT_WORK_CURSOR,
    LYT_WORK_MAX,
  };

  //! ボタンID
  enum ButtonID
  {
    BTN_ID_CANCEL = 0,
    BTN_ID_MAX
  };

  static const int _MAX = 7;  //!< チャットの数

  static const u32 LIST_CURSOR_MOVE_MAX = 4;  //!< ペインリストのカーソル移動限界

  static const f32 LIST_ITEM_PY;  //!< ペインリストの表示開始Y座標
  static const f32 LIST_ITEM_SX;  //!< ペインリストの項目Xサイズ
  static const f32 LIST_ITEM_SY;  //!< ペインリストの項目Yサイズ

  static const u32 MSG_TBL_MAX = 8;   //!< メッセージ数（項目数）


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap                  ヒープ
   * @param   pAppRenderingManager  描画マネージャ
   */
  //-----------------------------------------------------------------------------
  BoxLowChtDraw( app::util::Heap * heap, app::util::AppRenderingManager * pAppRanderingManager );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~BoxLowChtDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   初期化
   *
   * @param   pAppLytBuff     レイアウトリソース
   * @param   pMsgData        メッセージデータ
   * @param   pMenuCursorBuf  メッセージカーソル
   */
  //-----------------------------------------------------------------------------
  void Init( void * pAppLytBuff, gfl2::str::MsgData * pMsgData, void * pMenuCursorBuf );

  //-----------------------------------------------------------------------------
  /**
   * @func    End
   * @brief   終了処理
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool End(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetUIListener
   * @brief   UIイベント用リスナーの登録
   *
   * @param   listener  リスナー
   */
  //-----------------------------------------------------------------------------
  void SetUIListener( BoxLowChtDrawListener * listener );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsExit
   * @brief   破棄チェック
   *
   * @retval  true  = 破棄完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
//  bool IsExit( void );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetChatMessage
   * @brief   チャットメッセージ取得
   * @date    2015.10.28
   *
   * @param   idx   取得データインデックス
   * @param   str   文字列取得場所
   */
  //-----------------------------------------------------------------------------
  void GetChatMessage( u32 idx, gfl2::str::StrBuf * str );


  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   */
  //-----------------------------------------------------------------------------
  virtual void Update( void );

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   *
   * @param   no  描画面
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

private:
  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize2D
   * @brief   2D関連作成
   *
   * @param   heap          ヒープ
   * @param   pAppLytBuff   レイアウトリソース
   * @param   pMsgData      メッセージデータ
   */
  //-----------------------------------------------------------------------------
  void Initialize2D( app::util::Heap * heap, void * pAppLytBuff, gfl2::str::MsgData * pMsgData );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitializeText
   * @brief   テキスト関連生成
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  void InitializeText( app::util::Heap * heap );

  //-----------------------------------------------------------------------------
  /**
   * @func    FinalizeText
   * @brief   テキスト関連削除
   */
  //-----------------------------------------------------------------------------
  void FinalizeText( void );

  //-----------------------------------------------------------------------------
  /**
   * @func    MyCreateButtonManager
   * @brief   ボタンマネージャの生成
   */
  //-----------------------------------------------------------------------------
  void MyCreateButtonManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuCursor
   * @brief   カーソル生成
   * @date    2015.10.28
   *
   * @param   buf   カーソルリソースバッファ
   */
  //-----------------------------------------------------------------------------
  void CreateMenuCursor( void * buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMenuCursor
   * @brief   カーソル削除
   * @date    2015.10.28
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePaneList
   * @brief   ペインリスト生成
   * @date    2015.10.28
   */
  //-----------------------------------------------------------------------------
  void CreatePaneList(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeletePaneList
   * @brief   ペインリスト削除
   * @date    2015.10.28
   */
  //-----------------------------------------------------------------------------
  void DeletePaneList(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePaneListData
   * @brief   ペインリストデータ生成
   * @date    2015.10.28
   */
  //-----------------------------------------------------------------------------
  void CreatePaneListData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPaneListParam
   * @brief   ペインリストのパラメータ設定
   * @date    2015.10.28
   *
   * @param   param   設定場所
   * @param   pos     初期カーソル位置
   * @param   scroll  初期スクロール位置
   */
  //-----------------------------------------------------------------------------
  void SetPaneListParam( app::tool::PaneList::SETUP_PARAM * param, u32 pos, u32 scroll );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPaneListConfig
   * @brief   ペインリストサブ設定
   * @date    2015.10.28
   */
  //-----------------------------------------------------------------------------
  void SetPaneListConfig(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetScrollBarParam
   * @brief   スクロールバーパラメータ設定
   * @date    2015.10.28
   *
   * @param   setup   設定場所
   * @param   max     リスト項目数
   */
  //-----------------------------------------------------------------------------
  void SetScrollBarParam( app::util::ScrollBar::SETUP_PARAM * setup, u32 max );

  //-----------------------------------------------------------------------------
  /**
   * @func    CallBack_CursorMove
   * @brief   カーソル移動時のコールバック関数
   * @date    2015.10.28
   *
   * @param   work        外部から指定されたワーク
   * @param   cur_pane    ペインデータ
   * @param   pane_index  カーソル位置のペインデータインデックス
   * @param   pos         カーソル位置
   *
   * @return	none
   *
   * @note  override PaneList::CallBack
   */
  //-----------------------------------------------------------------------------
  virtual void CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    CallBack_CursorOff
   * @brief		カーソルを非表示にするコールバック関数
   * @date    2015.10.28
   *
   * @param		work				外部から指定されたワーク
   * @param		pane				ペインデータ
   * @param		pane_index	カーソル位置のペインデータインデックス
   * @param		pos					カーソル位置
   *
   * @return	none
   *
   * @note  override PaneList::CallBack
   */
  //-----------------------------------------------------------------------------
  virtual void CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    CallBack_ListWrite
   * @brief		リストの項目を描画するコールバック関数
   * @date    2015.10.28
   *
   * @param		work				外部から指定されたワーク
   * @param		pane				ペインデータ
   * @param		pane_index	項目位置のペインデータインデックス
   * @param		pos					項目位置
   *
   * @return	none
   *
   * @note  override PaneList::CallBack
   */
  //-----------------------------------------------------------------------------
  virtual void CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    CallBack_ItemChange
   * @brief		入れ替え用コールバック関数
   * @date    2015.10.28
   *
   * @param		work	外部から指定されたワーク
   * @param		pos1	入れ替え位置１
   * @param		pos2	入れ替え位置２
   *
   * @return	none
   *
   * @note  override PaneList::CallBack
   */
  //-----------------------------------------------------------------------------
  virtual void CallBack_ItemChange( void * work, u32 pos1, u32 pos2 );

  //-----------------------------------------------------------------------------
  /**
   * @func    PaneListItemSelectListener
   * @brief   ペインリスト決定時に呼ばれるリスナー
   * @date    2015.10.28
   *
   * @param   pos   カーソル位置
   *
   * @note  override PaneListView::Listener
  */
  //-----------------------------------------------------------------------------
  virtual void PaneListItemSelectListener( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneEvent
   * @brief   ペインイベントの検知
   * @date    2015.10.28
   *
   * @param   button_id   通知されたボタンID
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 button_id );
  
private:
  app::util::Heap * mpAppHeap;                              //!< ヒープ
  app::util::AppRenderingManager * mpAppRenderingManager;   //!< 描画マネージャ
  BoxLowChtDrawListener * m_uiListener;                     //!< UIイベントリスナー
	print::WordSet* mpWordSet;                                //!< 単語セット
  app::tool::MenuCursor * mpMenuCursor;                     //!< メニューカーソル
  app::tool::PaneListView* mpPaneListView;                  //!< ペインリストビュー
  app::tool::PaneList::PANE_DATA* mpPaneListData;           //!< ペインリストデータ
  bool m_isDrawEnable;                                      //!< 描画許可フラグ
};


GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

#endif	/* __BOX_LOW_BG_DRAW_H__ */
