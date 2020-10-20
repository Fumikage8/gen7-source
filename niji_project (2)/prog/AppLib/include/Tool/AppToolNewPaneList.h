//=============================================================================
/**
 * @file    AppToolNewPaneList.h
 * @brief   ペインリスト処理
 * @author  Hiroyuki Nakamura
 * @date    2015.07.22
 */
//=============================================================================
#if !defined( APP_TOOL_NEW_PANE_LIST_H_INCUDED )
#define APP_TOOL_NEW_PANE_LIST_H_INCUDED

#pragma once

// gflib2
#include <Layout/include/gfl2_Layout.h>
// niji
#include "AppLib/include/Util/app_util_ScrollBar.h"
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class MenuCursor;
class ScrollValue;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

// 前方宣言
class NewPaneListPartsBase;
class SlideScroll;


//=============================================================================
/**
 * @class NewPaneList
 * @brief ペインリストクラス
 * @date  2015.07.22
 */
//=============================================================================
class NewPaneList
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( NewPaneList );

public:
  //! リストタイプ
  enum ListType
  {
    TYPE_H = 0,   //!< 水平方向
    TYPE_V,       //!< 垂直方向   @note 未実装
  };

  //! スキップモード
  enum SkipMode
  {
    SKIP_MODE_NONE = 0,   //!< スキップなし
    SKIP_MODE_KEY,        //!< 十字キーでスキップ
    SKIP_MODE_BUTTON,     //!< LRボタンでスキップ
  };

  //! エフェクトモード
  enum EffectMode
  {
    EFFECT_MODE_NONE = 0, //!< エフェクトなし
    EFFECT_MODE_ALPHA,    //!< 半透明エフェクト
    EFFECT_MODE_COLOR,    //!< 黒フェードエフェクト
  };

	//! ペインデータ
	struct PANE_DATA
  {
    gfl2::lyt::LytPane * base;        //!< 親ペイン（移動用）
    gfl2::lyt::LytPane * bound;       //!< 当たり判定用ペイン
    gfl2::lyt::LytPane * cursor_pos;  //!< カーソル位置の基準となるペイン
    u32 anm_touch;                    //!< タッチ時のアニメ
    u32 anm_release;                  //!< リリース時のアニメ
    u32 anm_cancel;                   //!< キャンセル時のアニメ
    u32 anm_select;                   //!< キー選択アニメ
  };

  //! セットアップパラメータ
  struct SETUP_PARAM
  {
    const ListType list_type;     //!< リストタイプ

    gfl2::lyt::LytWk * lytwk;     //!< レイアウトワーク
    const PANE_DATA * pane_data;  //!< ペインデータ   ※画面に同時に表示される数+1個必要
    u32 pane_data_max;            //!< ペインデータ数 ※画面に同時に表示される数+1個必要

    u16 list_max;       //!< リスト項目数
    u16 list_pos_max;   //!< 画面上のカーソル移動範囲の最大位置
    u32 list_pos;       //!< 画面上のカーソル位置
    f32 list_scroll;    //!< スクロール値

    f32 start_x;        //!< 項目配置開始X座標（項目0番の表示X座標）
    f32 start_y;        //!< 項目配置開始Y座標（項目0番の表示Y座標）

    f32 space;          //!< 項目配置間隔　※小数点以下に値があるとバグる可能性があります

    s16 limit_top;      //!< －方向の表示限界
    s16 limit_bottom;   //!< ＋方向の表示限界

    app::tool::MenuCursor * menu_cursor;   //!< カーソル（NULLでもよい）
  };

  //=============================================================================
  /**
   * @class ListListener
   * @brief リスナークラス
   * @date  2015.07.22
   */
  //=============================================================================
  class ListListener
  {
  public:
    //-----------------------------------------------------------------------------
    /**
     * @func  PaneListListener_Write
     * @brief リスト項目書き込み用リスナー
     * @date  2015.07.22
     *
     * @param pane        ペインデータ
     * @param pane_index  書き込み先のペインデータのインデックス
     * @param pos         リスト項目位置
     */
    //-----------------------------------------------------------------------------
    virtual void PaneListListener_Write( const PANE_DATA * pane, u32 pane_index, u32 pos ){};

    //-----------------------------------------------------------------------------
    /**
     * @func  PaneListListener_MoveCursor
     * @brief カーソル移動時のリスナー
     * @date  2015.07.22
     *
     * @param pane        ペインデータ
     * @param pane_index  移動先のペインデータのインデックス
     * @param pos         リスト項目位置
     */
    //-----------------------------------------------------------------------------
    virtual void PaneListListener_MoveCursor( const PANE_DATA * pane, u32 pane_index, u32 pos ){};

    //-----------------------------------------------------------------------------
    /**
     * @func  PaneListListener_Decide
     * @brief 決定時のリスナー
     * @date  2015.07.30
     *
     * @param pos   リスト項目位置
     */
    //-----------------------------------------------------------------------------
    virtual void PaneListListener_Decide( u32 pos ){};

    //-----------------------------------------------------------------------------
    /**
     * @func  PaneListListener_StartItemChange
     * @brief 入れ替え開始時のリスナー
     * @date  2015.08.01
     *
     * @note  未実装
     */
    //-----------------------------------------------------------------------------
    virtual void PaneListListener_StartItemChange( const PANE_DATA * pane, u32 pane_index, u32 pos ){};

    //-----------------------------------------------------------------------------
    /**
     * @func  PaneListListener_EndItemChange
     * @brief 入れ替え終了時のリスナー
     * @date  2015.08.01
     *
     * @note  未実装
     */
    //-----------------------------------------------------------------------------
    virtual void PaneListListener_EndItemChange( const PANE_DATA * pane, u32 pane_index, u32 pos1, u32 pos2 ){};

    //-----------------------------------------------------------------------------
    /**
     * @func  PaneListListener_VisibleCursor
     * @brief カーソル表示切り替え
     * @date  2015.07.29
     *
     * @param flg   true = 表示, false = 非表示
     *
     * @note  カーソルを独自実装する場合に使用してください
     */
    //-----------------------------------------------------------------------------
    virtual void PaneListListener_VisibleCursor( bool flg ){};

    //-----------------------------------------------------------------------------
    /**
     * @func  PaneListListener_PutCursor
     * @brief カーソル配置
     * @date  2015.07.29
     *
     * @param x     配置X座標
     * @param y     配置Y座標
     * @param flg   true = 表示, false = 非表示
     *
     * @note  カーソルを独自実装する場合に使用してください
     */
    //-----------------------------------------------------------------------------
    virtual void PaneListListener_PutCursor( f32 x, f32 y, bool flg ){};

    //-----------------------------------------------------------------------------
    /**
     * @func  PaneListListener_PaneLongPress
     * @brief ペイン長押し時のリスナー
     * @date  2015.10.21
     *
     * @param pane        ペインデータ
     * @param pane_index  移動先のペインデータのインデックス
     * @param pos         リスト項目位置
     */
    //-----------------------------------------------------------------------------
    virtual void PaneListListener_PaneLongPress( const PANE_DATA * pane, u32 pane_index, u32 pos ){};
  };

  //! スクロールパラメータ
  struct ScrollParam
  {
    f32 speed;  //!< 初速度
    f32 decel;  //!< 減速度
  };

  static const u32 SCROLL_PARAM_MAX = 4;  //!< スクロールパラメータの要素数

  //! 外部スクロール指定
  enum ScrollMode
  {
    SCROLL_MODE_PLUS = 0,   //!< +方向にスクロール
    SCROLL_MODE_MINUS,      //!< -方向にスクロール
  };

  //! いれかえ設定
  enum ItemChangeParam
  {
    ITEM_CHANGE_START = 0,  //!< いれかえ開始
    ITEM_CHANGE_DECIDE,     //!< いれかえ決定
    ITEM_CHANGE_CANCEL      //!< いれかえキャンセル
  };


private:
  static const ScrollParam DefaultScrollParam[SCROLL_PARAM_MAX];  //!< デフォルトスクロールパラメータ

  static const u32 KEY_SCROLL_COUNT_MAX = 4;  //!< キースクロールカウンタ最大値

  static const u8 KeyScrollFrame[KEY_SCROLL_COUNT_MAX];   //!< キースクロールフレームテーブル

  //! SEテーブルID
  enum SE_ID
  {
    SE_ID_DECIDE = 0,   //!< 決定
    SE_ID_CURSOR_MOVE,  //!< カーソル移動
    SE_ID_MAX           //!< 最大値
  };

  static const u32 ITEM_CHANGE_NONE = 0;  //!< キー操作で入れ替えをしない場合の設定


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param heap      ヒープ
   * @param setup     セットアップパラメータ
   * @param listener  リスナー
   */
  //-----------------------------------------------------------------------------
  NewPaneList( app::util::Heap * heap, const SETUP_PARAM * setup, ListListener * listener );

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~NewPaneList();

  //-----------------------------------------------------------------------------
  /**
   * @func  Initialize
   * @brief 初期化
   * @date  2015.07.30
   */
  //-----------------------------------------------------------------------------
  void Initialize(void);

  //-----------------------------------------------------------------------------
  /**
   * @func  Terminate
   * @brief 破棄
   * @date  2015.07.30
   */
  //-----------------------------------------------------------------------------
  void Terminate(void);

  //-----------------------------------------------------------------------------
  /**
   * @func  SetupInsertPane
   * @brief ペインの表示優先変更の基準となるペインを設定
   * @date  2015.07.22
   *
   * @param pane  基準ペイン
   *
   * @note
   *  デフォルト：NULL
   *
   *  ペインが被らない場合は設定しなくてよい
   *  入れ替えを有効にする場合は設定すること
   */
  //-----------------------------------------------------------------------------
  void SetupInsertPane( gfl2::lyt::LytPane * pane );

  //-----------------------------------------------------------------------------
  /**
   * @func  SetupTouchArea
   * @brief タッチ範囲を設定
   * @date  2015.07.22
   *
   * @param up    上Y座標
   * @param down  下Y座標
   * @param left  左X座要
   * @param right 右X座標
   *
   * @note
   *  デフォルト：画面全体
   */
  //-----------------------------------------------------------------------------
  void SetupTouchArea( s16 up, s16 down, s16 left, s16 right );

  //-----------------------------------------------------------------------------
  /**
   * @func  SetupScrollParam
   * @brief スライド用のスクロールパラメータを設定
   * @date  2015.07.27
   *
   * @param param   スクロールパラメータ
   */
  //-----------------------------------------------------------------------------
  void SetupScrollParam( const NewPaneList::ScrollParam * param );

  //-----------------------------------------------------------------------------
  /**
   * @func  SetupSE
   * @brief SEを設定
   * @date  2015.07.30
   *
   * @param decide  決定時のSE
   * @param move    カーソル移動時のSE
   *
   * @note
   *  デフォルト：
   *    decide = SEQ_SE_DECIDE1
   *      move = SEQ_SE_SELECT1
   */
  //-----------------------------------------------------------------------------
  void SetupSE( u32 decide, u32 move );

  //-----------------------------------------------------------------------------
  /**
   * @func  SetupNoLoop
   * @brief リストのループ処理を無効化
   * @date  2015.07.30
   */
  //-----------------------------------------------------------------------------
  void SetupNoLoop(void);

  //-----------------------------------------------------------------------------
  /**
   * @func  SetupSkipMode
   * @brief リストのスキップ処理を設定
   * @date  2015.07.30
   *
   * @param mode  スキップモード
   *
   * @note
   *  デフォルト：十字キー
   */
  //-----------------------------------------------------------------------------
  void SetupSkipMode( SkipMode mode );

  //-----------------------------------------------------------------------------
  /**
   * @func  SetupEffectMode
   * @brief エフェクトモードを設定
   * @date  2015.07.31
   *
   * @param mode  エフェクトモード
   *
   * @note
   *  デフォルト：エフェクトなし
   */
  //-----------------------------------------------------------------------------
  void SetupEffectMode( EffectMode mode );

  //-----------------------------------------------------------------------------
  /**
   * @func  SetupInputMode
   * @brief 入力処理の有効/無効切り替え
   * @date  2015.07.30
   *
   * @param is_key      キー
   * @param is_touch    タッチ
   * @param is_decide   決定
   *
   * @note
   *  デフォルト：全て有効
   */
  //-----------------------------------------------------------------------------
  void SetupInputMode( bool is_key, bool is_touch, bool is_decide );

  //-----------------------------------------------------------------------------
  /**
   * @func  IsModuleAction
   * @brief ペインリストが動作中か
   * @date  2015.07.29
   *
   * @retval  true  = 動作中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsModuleAction(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetCursorPos
   * @brief   画面上のカーソル位置を取得
   * @date    2015.07.30
   *
   * @return  カーソル位置
   */
  //-----------------------------------------------------------------------------
  u32 GetCursorPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetScroll
   * @brief   スクロール値を取得
   * @date    2015.07.30
   *
   * @return  スクロール値
   */
  //-----------------------------------------------------------------------------
  f32 GetScroll(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListPos
   * @brief   リスト上のカーソル位置を取得
   * @date    2015.07.30
   *
   * @return  カーソル位置 ( GetCursorPos()+GetScroll() )
   */
  //-----------------------------------------------------------------------------
  u32 GetListPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func  SetExScroll
   * @brief 外部スクロール開始リクエスト
   * @date  2015.07.31
   *
   * @param mode  スクロール方向
   */
  //-----------------------------------------------------------------------------
  void SetExScroll( ScrollMode mode );

  //-----------------------------------------------------------------------------
  /**
   * @func  StopExScroll
   * @brief 外部スクロール停止リクエスト
   * @date  2015.07.31
   *
   * @note  SetExScroll()のリクエストを無効化
   */
  //-----------------------------------------------------------------------------
  void StopExScroll(void);

  //-----------------------------------------------------------------------------
  /**
   * @func  SetItemChange
   * @brief いれかえ設定
   * @date  2015.08.04
   *
   * @param param   設定パラメータ
   */
  //-----------------------------------------------------------------------------
  void SetItemChange( ItemChangeParam param );

  //-----------------------------------------------------------------------------
  /**
   * @func  IsItemChange
   * @brief いれかえモードか
   * @date  2015.08.04
   *
   * @retval  true  = いれかえモード
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsItemChange(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateScrollBar
   * @brief   スクロールバーを生成
   * @date    2016.02.02
   *
   * @param   setup   セットアップパラメータ
   */
  //-----------------------------------------------------------------------------
  void CreateScrollBar( const app::util::ScrollBar::SETUP_PARAM * setup );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteScrollBar
   * @brief   スクロールバーを削除
   * @date    2016.02.02
   */
  //-----------------------------------------------------------------------------
  void DeleteScrollBar(void);


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePaneLowTable
   * @brief   ペインの並び順テーブル生成
   * @date    2015.07.30
   */
  //-----------------------------------------------------------------------------
  void CreatePaneLowTable(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeletePaneLowTable
   * @brief   ペインの並び順テーブル削除
   * @date    2015.07.30
   */
  //-----------------------------------------------------------------------------
  void DeletePaneLowTable(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsTouchArea
   * @brief   タッチ有効範囲チェック
   * @date    2015.07.28
   *
   * @param   x   X座標
   * @param   y   Y座標
   *
   * @retval  true  = 範囲内
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsTouchArea( u16 x, u16 y ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    SetScrollMax
   * @brief   スクロール最大値を設定
   * @date    2015.07.27
   */
  //-----------------------------------------------------------------------------
  void SetScrollMax(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetCursorMoveParam
   * @brief   リストがスクロールするカーソル位置を設定
   * @date    2015.07.29
   */
  //-----------------------------------------------------------------------------
  void SetCursorMoveParam(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateButton
   * @brief   ボタン生成
   * @date    2015.07.24
   */
  //-----------------------------------------------------------------------------
  void CreateButton(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateScrollValue
   * @brief   スクロール値管理クラス生成
   * @date    2015.07.27
   *
   * @param   level   速度レベル
   * @param   mv      スクロール方向
   */
  //-----------------------------------------------------------------------------
  void CreateScrollValue( u32 level, s32 mv );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteScrollValue
   * @brief   スクロール値管理クラス削除
   * @date    2015.07.27
   */
  //-----------------------------------------------------------------------------
  void DeleteScrollValue(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartSlide
   * @brief   スライド動作開始
   * @date    2015.07.28
   *
   * @param   mv  スクロール方向
   */
  //-----------------------------------------------------------------------------
  void StartSlide( s32 mv );

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2015.07.24
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.07.24
   *
   * @param   displayNo   描画ディスプレイ
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_Drag
   * @brief   アップデート：ドラッグ処理
   * @date    2015.07.31
   */
  //-----------------------------------------------------------------------------
  void Update_Drag(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_Release
   * @brief   アップデート：リリース処理（タッチリリース）
   * @date    2015.07.31
   */
  //-----------------------------------------------------------------------------
  void Update_Release(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_Slide
   * @brief   アップデート：スライド処理
   * @date    2015.07.29
   *
   * @param   mv  スクロール方向
   */
  //-----------------------------------------------------------------------------
  void Update_Slide( s32 mv );

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_KeyScrollPlus
   * @brief   アップデート：キー入力でのスクロール処理（＋方向）
   * @date    2015.07.29
   */
  //-----------------------------------------------------------------------------
  void Update_KeyScrollPlus(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_KeyScrollMinus
   * @brief   アップデート：キー入力でのスクロール処理（－方向）
   * @date    2015.07.29
   */
  //-----------------------------------------------------------------------------
  void Update_KeyScrollMinus(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_KeySkip
   * @brief   アップデート：キー入力でのページスキップ処理
   * @date    2015.07.30
   *
   * @param   mv  スクロール方向
   */
  //-----------------------------------------------------------------------------
  void Update_KeySkip( s32 mv );

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_ExScrollPlus
   * @brief   アップデート：外部スクロール処理（＋方向）
   * @date    2015.07.31
   */
  //-----------------------------------------------------------------------------
  void Update_ExScrollPlus(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_ExScrollMinus
   * @brief   アップデート：外部スクロール処理（－方向）
   * @date    2015.07.31
   */
  //-----------------------------------------------------------------------------
  void Update_ExScrollMinus(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_KeyItemChangePlus
   * @brief   アップデート：キー入力での入れ替え処理（＋方向）
   * @date    2015.08.01
   */
  //-----------------------------------------------------------------------------
  void Update_KeyItemChangePlus(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_KeyItemChangeMinus
   * @brief   アップデート：キー入力での入れ替え処理（－方向）
   * @date    2015.08.01
   */
  //-----------------------------------------------------------------------------
  void Update_KeyItemChangeMinus(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_KeyItemChangeSkip
   * @brief   アップデート：キー入力での入れ替えページスキップ処理
   * @date    2015.08.03
   *
   * @param   mv  スクロール方向
   */
  //-----------------------------------------------------------------------------
  void Update_KeyItemChangeSkip( s32 mv );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeUpdate
   * @brief   アップデートシーケンス切り替え
   * @date    2015.07.29
   *
   * @param   main  メインシーケンス
   * @param   sub   サブシーケンス
   */
  //-----------------------------------------------------------------------------
  void ChangeUpdate( u32 main, u32 sub=0 );

  //-----------------------------------------------------------------------------
  /**
   * @func    CountupKeyScroll
   * @brief   キー入力スクロールのカウントアップ処理
   * @date    2015.07.29
   *
   * @retval  true  = カウントアップ
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CountupKeyScroll(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndKeyScroll
   * @brief   キー入力スクロール終了
   * @date    2015.07.30
   */
  //-----------------------------------------------------------------------------
  void EndKeyScroll(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    KeyScrollLoop
   * @brief   キー入力のループ処理
   * @date    2015.07.30
   *
   * @param   pos     カーソル位置
   * @param   scroll  スクロール値
   */
  //-----------------------------------------------------------------------------
  void KeyScrollLoop( u32 pos, f32 scroll );

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveCursor
   * @brief   カーソル移動
   * @date    2015.07.29
   *
   * @param   pos           カーソル位置
   * @param   pane_index    配置先のペインインデックス
   * @param   is_cursor_on  true = カーソル表示
   */
  //-----------------------------------------------------------------------------
  void MoveCursor( u32 pos, u32 pane_index, bool is_cursor_on );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetDragPos
   * @brief   ドラッグ位置を設定
   * @date    2015.07.29
   *
   * @retval  true  = 成功
   * @retval  false = 失敗
   */
  //-----------------------------------------------------------------------------
  bool SetDragPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    KeyScrollLoopChange
   * @brief   入れ替え時のキー入力のループ処理
   * @date    2015.08.01
   *
   * @param   pos     カーソル位置
   * @param   scroll  スクロール値
   * @param   mv      スクロール方向
   */
  //-----------------------------------------------------------------------------
  void KeyScrollLoopChange( u32 pos, f32 scroll, s32 mv );

  //-----------------------------------------------------------------------------
  /**
   * @func    ResetScrollBar
   * @brief   スクロールバーを再設定
   * @date    2016.02.02
   *
   * @param   max   最大スクロール値
   */
  //-----------------------------------------------------------------------------
  void ResetScrollBar( u32 max );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateScrollBar
   * @brief   スクロールバーの更新処理
   * @date    2016.02.02
   */
  //-----------------------------------------------------------------------------
  void UpdateScrollBar(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateKeyScrollValue
   * @brief   キースクロール用のワーク更新
   * @date    2016.02.06
   */
  //-----------------------------------------------------------------------------
  void UpdateKeyScrollValue(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OnTouchEvent
   * @brief   タッチパネルイベントの検知
   * @date    2015.07.22
   *
   * @param   touch_panel タッチパネル
   * @param   is_touch    true = タッチされている
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel * touch_panel, bool is_touch );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneEvent
   * @brief   ペインイベントの検知
   * @date    2015.07.22
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
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 button_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneLongPressEvent
   * @brief   ペイン長押しイベントの検知
   * @date    2015.07.22
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
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneLongPressEvent( const u32 button_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneTouchTriggerEvent
   * @brief   ペインタッチ時を検知
   * @date    2015.07.22
   *
   * @param   button_id   選択されたボタンID
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual void OnLayoutPaneTouchTriggerEvent( const u32 button_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnKeyAction
   * @brief   キー入力の検知
   * @date    2015.07.22
   *
   * @param   button  ボタン
   * @param   key     十字キー
   * @param   stick   アナログスティック
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick );


private:
  app::util::Heap * m_heap;       //!< ヒープ
  SETUP_PARAM m_param;            //!< セットアップパラメータ
  ListListener * m_listListener;  //!< リスナー

  gfl2::lyt::LytPane * m_insertPane;  //!< 並び替え基準ペイン

  f32 m_scrollMax;        //!< スクロール最大値
  u16 m_plusCursorPos;    //!< ＋方向のスクロールを開始するカーソル位置
  u16 m_minusCursorPos;   //!< －方向のスクロールを開始するカーソル位置

  s16 m_touchAreaUp;      //!< タッチ範囲座標・上
  s16 m_touchAreaDown;    //!< タッチ範囲座標・下
  s16 m_touchAreaLeft;    //!< タッチ範囲座標・左
  s16 m_touchAreaRight;   //!< タッチ範囲座標・右

  u32 m_se[SE_ID_MAX];    //!< SEテーブル

  bool m_isLoop;  //!< リストがループするか

  bool m_isInputKey;    //!< true = キー入力可能
  bool m_isInputTouch;  //!< true = タッチ入力可能
  bool m_isDecide;      //!< true = 決定可能

  SkipMode m_skipMode;      //!< スキップモード
  EffectMode m_effectMode;  //!< エフェクトモード

  u8 * m_pPaneLowTable;   //!< ペインの並び順テーブル

  NewPaneListPartsBase * m_pPartsBase;          //!< 部品クラス
  SlideScroll * m_pSlideScroll;                 //!< スライドスクロール
  app::tool::ScrollValue * m_pScrollValue;      //!< スクロール値
  ScrollParam m_scrollParam[SCROLL_PARAM_MAX];  //!< スクロールパラメータ

  s32 m_dragOffset;       //!< ドラッグ操作のオフセット値
  f32 m_dragStartScroll;  //!< ドラッグ操作開始時にスクロール値

  f32 m_slideValue;   //!< スライド値
  f32 m_slideRest;    //!< スライド値の端数（小数点以下）
  bool m_slideStop;   //!< スライドスクロールの強制停止フラグ
  u16 m_slideStopX;   //!< スライドスクロール強制停止時のX座標
  u16 m_slideStopY;   //!< スライドスクロール強制停止時のY座標

  f32 m_keyScrollValue;     //!< キーでのスクロール値
  u8 m_keyScrollCount;      //!< キーでのスクロールカウンタ
  u8 m_keyScrollFrame;      //!< キースクロールカウントアップフレーム
  u8 m_keyScrollMoveCount;  //!< キースクロール動作カウンタ

  bool m_buttonTouchFlag;   //!< true = ボタンをタッチ

  bool m_isExScrollRepeat;  //!< 外部スクロール繰り返しフラグ

  app::util::ScrollBar * m_pScrollBar;  //!< スクロールバー

  u16 m_mainSeq;    //!< メインシーケンス
  u16 m_subSeq;     //!< サブシーケンス
};


GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

#endif  // APP_TOOL_NEW_PANE_LIST_H_INCUDED
