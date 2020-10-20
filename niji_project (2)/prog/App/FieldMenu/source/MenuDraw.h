//=============================================================================
/**
 * @file    MenuDraw.h
 * @brief   フィールド下画面メニュー表示
 * @author  Hiroyuki Nakamura
 * @date    2015.05.20
 */
//=============================================================================
#if !defined( MENU_DRAW_H_INCLUDED )
#define MENU_DRAW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "Savedata/include/FieldMenuSave.h"
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class MenuCursor;
class AppCommonGrpIconData;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)
class SlideScroll;
GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(Field)
class EventWork;
GFL_NAMESPACE_END(Field)

GFL_NAMESPACE_BEGIN(Savedata)
class ResortSave;
class BoxPokemon;
GFL_NAMESPACE_END(Savedata)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

// 前方宣言
class FrameListener;

//=============================================================================
/**
 * @class MenuDraw
 * @brief 下画面メニュー表示クラス
 * @date  2015.05.22
 */
//=============================================================================
class MenuDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
private:
  //! ボタンID
  enum ButtonID
  {
    BUTTON_ID_PAGE_R = 0,   //!< ページ切り替えボタン・右
    BUTTON_ID_PAGE_L,       //!< ページ切り替えボタン・左

    BUTTON_ID_ICON_00,      //!< 項目アイコン０
    BUTTON_ID_ICON_01,      //!< 項目アイコン１
    BUTTON_ID_ICON_02,      //!< 項目アイコン２
    BUTTON_ID_ICON_03,      //!< 項目アイコン３
    BUTTON_ID_ICON_04,      //!< 項目アイコン４
    BUTTON_ID_ICON_05,      //!< 項目アイコン５
    BUTTON_ID_ICON_06,      //!< 項目アイコン６
    BUTTON_ID_ICON_07,      //!< 項目アイコン７
    BUTTON_ID_ICON_08,      //!< 項目アイコン８
    BUTTON_ID_ICON_09,      //!< 項目アイコン９
    BUTTON_ID_ICON_10,      //!< 項目アイコン１０
    BUTTON_ID_ICON_11,      //!< 項目アイコン１１

    BUTTON_ID_PAGE_00,      //!< ページボタン０
    BUTTON_ID_PAGE_01,      //!< ページボタン１

    BUTTON_ID_CLOSE,        //!< 閉じる

    BUTTON_ID_REPORT,       //!< レポートボタン ( momiji追加 )

    BUTTON_ID_MAX           //!< 最大値
  };

  //! メニューアイコンの部品ペインテーブル
  static const gfl2::lyt::LytPaneIndex IconPartsPaneTable[Savedata::FieldMenu::MENU_ICON_MAX];
  //! メニューアイコンの親ペインテーブル
  static const gfl2::lyt::LytPaneIndex MenuIconParentTable[Savedata::FieldMenu::PAGE_MAX][Savedata::FieldMenu::PAGE_ICON_MAX];
  //! ページマークの部品ペインテーブル
  static const gfl2::lyt::LytPaneIndex PagePartsPaneTable[Savedata::FieldMenu::PAGE_MAX];
  //! ページNewマークアニメ
  static const u32 PageNewAnimeTable[Savedata::FieldMenu::PAGE_MAX];
  //! 各ページの親ペインテーブル
  static const gfl2::lyt::LytPaneIndex PageParentPaneTable[Savedata::FieldMenu::PAGE_MAX];
  //! ページ切り替え矢印の部品ペインテーブル
  static const gfl2::lyt::LytPaneIndex ArrowPartsPaneTable[2];
  //! メニューアイコンのアニメテーブル
  static const u32 MenuIconAnimeTable[Savedata::FieldMenu::MENU_ICON_MAX];

  static const u32 CATCH_ICON_MOVE_COUNT = 4;   //!< キャッチムーブ移動カウント

  static const s32 PAGE_RELEASE_SCROLL_ABS = ( DISPLAY_LOWER_WIDTH / 2 );   //!< ページスライドでページを切り替える座標オフセット値
  static const f32 PAGE_RELEASE_SCROLL_SPEED;   //!< ページスライドクロール速度

  static const f32 SRIDE_TOUCH_AREA_UY;   //!< スライド操作のタッチ範囲座標・上
  static const f32 SRIDE_TOUCH_AREA_DY;   //!< スライド操作のタッチ範囲座標・下
  static const f32 SRIDE_TOUCH_AREA_LX;   //!< スライド操作のタッチ範囲座標・左
  static const f32 SRIDE_TOUCH_AREA_RX;   //!< スライド操作のタッチ範囲座標・右

  //! メニューアイコンの場所ペイン（キャッチ時などに表示される影っぽいペイン）
  static const gfl2::lyt::LytPaneIndex IconCathcBgPaneTable[Savedata::FieldMenu::PAGE_ICON_MAX];

  //! メニューアイコンのお知らせ動作アニメ
  static const u32 IconInfoAnime[Savedata::FieldMenu::MENU_ICON_MAX];

  static const u32 RESORT_INFO_ICON_POKE_COUNT = 6;   //!< リゾートアイコンが[!]を表示するためのポケモン数


public:
  //! 項目配置ID
  enum IconPutID
  {
    PUT_ID_CANCEL = 0,  //!< キャンセル
    PUT_ID_ARROW,       //!< ページ切り替え矢印
    PUT_ID_ICON,        //!< 項目上
  };

  //! レイアウトID
  enum LayoutID
  {
    LYTID_MENU = 0,     //!< メニュー
    LYTID_MENU_UPPER,   //!< 上画面
    LYTID_MENU_CURSOR,  //!< カーソル
    LYTID_MAX           //!< 最大数
  };


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap        ヒープ
   * @param   render_man  描画マネージャ
   * @param   is_open     メニューを開いているか
   * @param   disp_mode   表示モード
   */
  //-----------------------------------------------------------------------------
  MenuDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man, const bool * is_open, const s32 * disp_mode );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~MenuDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFrameListener
   * @brief   リスナーをセット
   * @date    2015.05.22
   *
   * @param   リスナー
   */
  //-----------------------------------------------------------------------------
  void SetFrameListener( FrameListener * listener );

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadLayoutResource
   * @brief   レイアウトリソース読み込み
   * @date    2016.01.08
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UnloadLayoutResource
   * @brief   レイアウトリソース削除
   * @date    2016.01.08
   */
  //-----------------------------------------------------------------------------
  void UnloadLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadMessage
   * @brief   メッセージリソース読み込み
   * @date    2016.01.08
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UnloadMessage
   * @brief   メッセージリソース削除
   * @date    2016.01.08
   */
  //-----------------------------------------------------------------------------
  void UnloadMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuCursor
   * @brief   メニューカーソル生成
   * @date    2016.01.08
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateMenuCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMenuCursor
   * @brief   メニューカーソル削除
   * @date    2016.01.08
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMenuCursor
   * @brief   メニューカーソル取得
   * @date    2016.01.08
   *
   * @return  メニューカーソル
   */
  //-----------------------------------------------------------------------------
  app::tool::MenuCursor * GetMenuCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   初期化
   * @date    2015.05.22
   */
  //-----------------------------------------------------------------------------
  void Init(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    End
   * @brief   終了処理
   * @date    2015.05.22
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool End(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize2D
   * @brief   2D関連初期化
   * @date    2016.01.08
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Initialize2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate2D
   * @brief   2D関連削除
   * @date    2016.01.08
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Terminate2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsMenuIconAnimeEnd
   * @brief   メニュー項目アニメ終了チェック
   * @date    2015.08.24
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsMenuIconAnimeEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Start
   * @brief   開始
   * @date    2015.05.22
   */
  //-----------------------------------------------------------------------------
  void Start(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Stop
   * @brief   停止
   * @date    2015.05.22
   */
  //-----------------------------------------------------------------------------
  void Stop(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateLayout
   * @brief   レイアウト生成
   * @date    2016.01.08
   */
  //-----------------------------------------------------------------------------
  void CreateLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRotomMsgData
   * @brief   ロトムメッセージ取得
   * @date    2016.01.08
   *
   * @param   ロトムメッセージ
   */
  //-----------------------------------------------------------------------------
  gfl2::str::MsgData * GetRotomMsgData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuCursorLayout
   * @brief   メニューカーソル用レイアウト作成
   * @date    2016.01.08
   */
  //-----------------------------------------------------------------------------
  void CreateMenuCursorLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMenuCursorLayout
   * @brief   メニューカーソル用レイアウト削除
   * @date    2016.01.08
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuCursorLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeButtonEnable
   * @brief   ボタン入力制御切り替え
   * @date    2015.06.01
   *
   * @param   is_open   true = メニューが開いている
   */
  //-----------------------------------------------------------------------------
  void ChangeButtonEnable( bool is_open );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleCloseButton
   * @brief   閉じるボタン表示切り替え
   * @date    2016.01.07
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleCloseButton( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePageArrowVisible
   * @brief   ページ切り替え矢印表示切り替え
   * @date    2015.06.30
   *
   * @param   page  現在のページ
   */
  //-----------------------------------------------------------------------------
  void ChangePageArrowVisible( u32 page );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePageButton
   * @brief   ページボタン表示切り替え
   * @date    2015.06.01
   */
  //-----------------------------------------------------------------------------
  void ChangePageButton(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OpenMenu
   * @brief   メニューを開いたときの処理
   * @date    2015.05.29
   */
  //-----------------------------------------------------------------------------
  void OpenMenu(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CloseMenu
   * @brief   メニューを閉じたときの処理
   * @date    2015.05.29
   */
  //-----------------------------------------------------------------------------
  void CloseMenu(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartPageChangeArrowAnime
   * @brief   ページ切り替え矢印アニメ開始
   * @date    2015.06.11
   *
   * @param   mv  切り替え方向
   */
  //-----------------------------------------------------------------------------
  void StartPageChangeArrowAnime( int mv );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartPageChangeAnime
   * @brief   ページ切り替えアニメ開始
   * @date    2015.06.01
   *
   * @param   next_page   次のページ
   * @param   mv          切り替え方向
   */
  //-----------------------------------------------------------------------------
  void StartPageChangeAnime( int next_page, int mv );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsPageChangeAnimeEnd
   * @brief   ページ切り替えアニメ終了チェック
   * @date    2015.06.01
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsPageChangeAnimeEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndPageChangeAnime
   * @brief   ページ切り替えアニメ終了
   * @date    2015.06.01
   *
   * @param   page  ページ番号
   */
  //-----------------------------------------------------------------------------
  void EndPageChangeAnime( int page );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetForcePagePointerAnime
   * @brief   ページ位置アニメをセット
   * @date    2016.04.28
   */
  //-----------------------------------------------------------------------------
  void SetForcePagePointerAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutPagePane
   * @brief   ページペインを配置
   * @date    2015.06.02
   *
   * @param   page  ページ番号
   * @param   pos   配置位置
   */
  //-----------------------------------------------------------------------------
  void PutPagePane( u32 page, int pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutCursor
   * @brief   カーソル配置
   * @date    2015.06.02
   *
   * @param   id  メニュー項目ID
   */
  //-----------------------------------------------------------------------------
  void PutCursor( Savedata::FieldMenu::IconID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleCursor
   * @brief   カーソル表示切り替え
   * @date    2016.01.08
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleCursor( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitCatchMenuIcon
   * @brief   メニュー項目キャッチムーブ初期化
   * @date    2015.06.04
   *
   * @param   メニュー項目ID
   */
  //-----------------------------------------------------------------------------
  void InitCatchMenuIcon( Savedata::FieldMenu::IconID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetCatchMenuIconID
   * @brief   キャッチムーブ中のメニュー項目IDを取得
   * @date    2015.06.04
   *
   * @return  メニュー項目ID
   */
  //-----------------------------------------------------------------------------
  Savedata::FieldMenu::IconID GetCatchMenuIconID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveCatchMenuIcon
   * @brief   キャッチムーブ中のメニュー項目移動
   * @date    2015.06.04
   */
  //-----------------------------------------------------------------------------
  void MoveCatchMenuIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsIconCatch
   * @brief   メニュー項目をキャッチしているか
   * @date    2015.06.04
   *
   * @retval  true  = キャッチ中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsIconCatch(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetIconCatchPage
   * @brief   キャッチ中のメニュー項目のページを取得
   * @date    2015.06.04
   *
   * @return  ページ番号
   */
  //-----------------------------------------------------------------------------
  u32 GetIconCatchPage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetIconCatchPutID
   * @brief   キャッチしたメニュー項目の配置先ID
   * @date    2015.06.04
   *
   * @return  配置先ID
   */
  //-----------------------------------------------------------------------------
  IconPutID GetIconCatchPutID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetIconCatchPutIndex
   * @brief   キャッチしたメニュー項目の配置先インデックスを取得
   * @date    2015.06.04
   *
   * @return  配置先インデックス
   */
  //-----------------------------------------------------------------------------
  u32 GetIconCatchPutIndex(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndCatchMenuIcon
   * @brief   メニュー項目キッチムーブ終了
   * @date    2015.06.03
   */
  //-----------------------------------------------------------------------------
  void EndCatchMenuIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ReleaseMenuIcon
   * @brief   メニュー項目リリース動作
   * @date    2015.06.03
   *
   * @retval  true  = 動作中
   * @retval  false = 終了
   */
  //-----------------------------------------------------------------------------
  bool ReleaseMenuIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsReleaseMenuIconEnd
   * @brief   メニュー項目のリリース動作が終了したか
   * @date    2015.06.04
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsReleaseMenuIconEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ResetReleaseMenuIconPos
   * @brief   メニュー項目リリース動作後の位置設定
   * @date    2015.06.04
   */
  //-----------------------------------------------------------------------------
  void ResetReleaseMenuIconPos();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleMenuIconInfo
   * @brief   メニュー項目の情報表示切り替え
   * @date    2015.06.04
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleMenuIconInfo( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    ForceEndCatchMenuIcon
   * @brief   キャッチムーブを強制終了
   * @date    2015.06.23
   */
  //-----------------------------------------------------------------------------
  void ForceEndCatchMenuIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DragMenuPage
   * @brief   メニューページドラッブ処理
   * @date    2015.06.04
   */
  //-----------------------------------------------------------------------------
  void DragMenuPage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ReleaseMenuPage
   * @brief   メニューページリリース処理
   * @date    2015.06.04
   *
   * @return  ページ切り替え方向
   */
  //-----------------------------------------------------------------------------
  int ReleaseMenuPage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SlideMenuPage
   * @brief   メニューページスライド処理
   * @date    2015.06.04
   *
   * @return  ページ切り替え方向
   */
  //-----------------------------------------------------------------------------
  int SlideMenuPage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateSlideMenuPageScroll
   * @brief   メニューページスライドスクロール更新処理
   * @date    2015.06.04
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool UpdateSlideMenuPageScroll(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndSlideMenuPageScroll
   * @brief   メニューページスライドスクロール終了
   * @date    2015.06.04
   */
  //-----------------------------------------------------------------------------
  void EndSlideMenuPageScroll(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ForceEndSlideMenuPageScroll
   * @brief   メニューページスライドスクロール強制終了
   * @date    2015.06.23
   */
  //-----------------------------------------------------------------------------
  void ForceEndSlideMenuPageScroll(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleIconArea
   * @brief   項目配置エリア表示切り替え
   * @date    2015.09.17
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleIconArea( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsInputTrigger
   * @brief   トリガー入力か
   * @date    2015.10.10
   *
   * @retval  true  = トリガー入力
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsInputTrigger(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateCommonIcon
   * @brief   Newアイコン生成
   * @date    2016.01.16
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateCommonIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteCommonIcon
   * @brief   Newアイコン削除
   * @date    2016.01.16
   */
  //-----------------------------------------------------------------------------
  void DeleteCommonIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetIconInfoMoveAnime
   * @brief   お知らせアイコンアニメ操作
   * @date    2016.04.02
   *
   * @param   flg   true = 再生, false = 停止
   */
  //-----------------------------------------------------------------------------
  void SetIconInfoMoveAnime( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMenuInfoAnimeEnable
   * @brief   お知らせアイコンアニメ操作制御設定
   * @date    2016.05.12
   *
   * @param   flg   true = 操作可, false = 操作不可
   */
  //-----------------------------------------------------------------------------
  void SetMenuInfoAnimeEnable( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetInputLock
   * @brief   入力制御フラグ設定
   * @date    2016.06.10
   *
   * @param   flg   true = 入力可, false = 入力不可
   */
  //-----------------------------------------------------------------------------
  void SetInputLock( bool flg );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    GetGameData
   * @brief   ゲームデータ関連取得
   * @date    2015.09.09
   */
  //-----------------------------------------------------------------------------
  void GetGameData(void);

  //-----------------------------------------------------------------------------S
  /**
   * @func    Update
   * @brief   更新
   * @date    2015.05.22
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.05.22
   *
   * @param   displayNo   描画ディスプレイ番号
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateButton
   * @brief   ボタン生成
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  void CreateButton(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetButtonInfoEx
   * @brief   ボタン情報生成
   * @date    2015.05.26
   *
   * @param   info        情報格納場所
   * @param   parts_tbl   部品テーブル
   * @param   info_tbl    デフォルト情報
   * @param   bound       バウンディングペイン
   * @param   max         生成数
   */
  //-----------------------------------------------------------------------------
  void SetButtonInfoEx( app::ui::ButtonInfoEx * info, const gfl2::lyt::LytPaneIndex * parts_tbl, const app::ui::ButtonInfoEx * info_tbl, gfl2::lyt::LytPaneIndex bound, u32 max );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePageArrowButton
   * @brief   ページ切り替え矢印ボタン情報生成
   * @date    2015.05.26
   *
   * @param   info  格納場所
   */
  //-----------------------------------------------------------------------------
  void CreatePageArrowButton( app::ui::ButtonInfoEx * info );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateIconButton
   * @brief   メニュー項目ボタン情報生成
   * @date    2015.05.26
   *
   * @param   info  格納場所
   */
  //-----------------------------------------------------------------------------
  void CreateIconButton( app::ui::ButtonInfoEx * info );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePageButton
   * @brief   ページボタン情報生成
   * @date    2015.05.26
   *
   * @param   info  格納場所
   */
  //-----------------------------------------------------------------------------
  void CreatePageButton( app::ui::ButtonInfoEx * info );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateCloseButton
   * @brief   戻るボタン情報生成
   * @date    2016.01.06
   *
   * @param   info  格納場所
   */
  //-----------------------------------------------------------------------------
  void CreateCloseButton( app::ui::ButtonInfoEx * info );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateReportButton
   * @brief   レポートボタン情報生成
   * @date    2017.01.06
   *
   * @param   info  格納場所
   */
  //-----------------------------------------------------------------------------
  void CreateReportButton( app::ui::ButtonInfoEx * info );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeIconButtonEnable
   * @brief   メニュー項目ボタン入力切り替え
   * @date    2015.08.21
   *
   * @param   is_open   true = メニューが開いている
   */
  //-----------------------------------------------------------------------------
  void ChangeIconButtonEnable( bool is_open );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsPageChange
   * @brief   ページを切り替えられるか
   * @date    2015.06.09
   *
   * @retval  true  = 可
   * @retval  false = 不可
   */
  //-----------------------------------------------------------------------------
  bool IsPageChange(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitMenuIcon
   * @brief   メニュー項目表示初期化
   * @date    2015.05.29
   */
  //-----------------------------------------------------------------------------
  void InitMenuIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleIconName
   * @brief   メニュー項目名表示切り替え
   * @date    2015.05.29
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleIconName( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsInfoIcon
   * @brief   メニュー項目情報表示取得
   * @date    2015.12.03
   *
   * @param   id      項目ID
   * @param   is_new  Newアイコン表示格納場所
   * @param   is_info 情報アイコン表示格納場所
   * @param   num     数値格納場所
   */
  //-----------------------------------------------------------------------------
  void IsInfoIcon( Savedata::FieldMenu::IconID id, bool * is_new, bool * is_info, u32 * num );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsIconInfoMove
   * @brief   メニュー項目情報を動作させるか
   * @date    2016.04.02
   *
   * @param   id  項目ID
   *
   * @retval  true  = 動作
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsIconInfoMove( Savedata::FieldMenu::IconID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitMenuIconPos
   * @brief   メニュー項目表示位置初期化
   * @date    2015.05.29
   */
  //-----------------------------------------------------------------------------
  void InitMenuIconPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMenuIconPos
   * @brief   メニュー項目表示位置セット
   * @date    2015.05.29
   *
   * @param   id    項目ID
   * @param   page  ページ
   * @param   pos   位置
   */
  //-----------------------------------------------------------------------------
  void SetMenuIconPos( Savedata::FieldMenu::IconID id, u32 page, u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleMenuIcon
   * @brief   メニュー項目表示切り替え
   * @date    2015.08.21
   *
   * @param   id    項目ID
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleMenuIcon( Savedata::FieldMenu::IconID id, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetIconNewAnime
   * @brief   Newアイコンアニメ切り替え（メニュー項目用）
   * @date    2015.12.03
   *
   * @param   id    項目ID
   * @param   flg   true = 再生, false = 停止
   */
  //-----------------------------------------------------------------------------
  void SetIconNewAnime( u32 id, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartIconNewAnime
   * @brief   Newアイコンアニメ開始（メニュー項目用）
   * @date    2015.12.03
   */
  //-----------------------------------------------------------------------------
  void StartIconNewAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StopIconNewAnime
   * @brief   Newアイコンアニメ停止（メニュー項目用）
   * @date    2015.12.03
   */
  //-----------------------------------------------------------------------------
  void StopIconNewAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutPageMark
   * @brief   ページマーク表示（全体）
   * @date    2015.06.01
   */
  //-----------------------------------------------------------------------------
  void PutPageMark(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutPageMark
   * @brief   ページマーク表示（個別）
   * @date    2015.06.01
   *
   * @param   page  ページ
   */
  //-----------------------------------------------------------------------------
  void PutPageMark( u32 page );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsPageInfo
   * @brief   ページ内にお知らせを表示している項目があるか
   * @date    2015.11.14
   *
   * @param   page  ページ
   *
   * @retval  true  = ある
   * @retval  false = ない
   */
  //-----------------------------------------------------------------------------
  bool IsPageInfo( u32 page );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsPageNew
   * @brief   ページ内にNewを表示している項目があるか
   * @date    2015.11.14
   *
   * @param   page  ページ
   *
   * @retval  true  = ある
   * @retval  false = ない
   */
  //-----------------------------------------------------------------------------
  bool IsPageNew( u32 page );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetCatchMenuIconStatus
   * @brief   キャッチムーブ中の状態設定
   * @date    2015.06.04
   *
   * @param   x   X座標
   * @param   y   Y座標
   */
  //-----------------------------------------------------------------------------
  void SetCatchMenuIconStatus( u16 x, u16 y );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleIconBg
   * @brief   項目配置エリア表示切り替え
   * @date    2015.09.17
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleIconBg( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsMenuOpen
   * @brief   メニューが開いているか
   * @date    2015.05.29
   *
   * @retval  true  = 開いている
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsMenuOpen(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeNewIconTexture
   * @brief   Newアイコンのテクスチャ差し替え
   * @date    2016.01.16
   */
  //-----------------------------------------------------------------------------
  void ChangeNewIconTexture(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateTempCoreParam
   * @brief   汎用CoreParam生成
   * @date    2016.03.04
   */
  //-----------------------------------------------------------------------------
  void CreateTempCoreParam(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteTempCoreParam
   * @brief   汎用CoreParam削除
   * @date    2016.03.04
   */
  //-----------------------------------------------------------------------------
  void DeleteTempCoreParam(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartPagePointerAnime
   * @brief   表示ページ位置アニメ開始
   * @date    2016.04.02
   */
  //-----------------------------------------------------------------------------
  void StartPagePointerAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateMenuInfo
   * @brief   メニューアイコンの情報更新
   * @date    2016.05.12
   */
  //-----------------------------------------------------------------------------
  void UpdateMenuInfo(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutMenuInfoValue
   * @brief   メニューアイコン上の数値表示
   * @date    2016.05.12
   *
   * @param   lytwk   レイアウトワーク
   * @param   res_id  マルチリソースID
   * @param   parts   メニューアイコンペイン
   * @param   num     数値
   */
  //-----------------------------------------------------------------------------
  void PutMenuInfoValue( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytMultiResID * res_id, gfl2::lyt::LytParts * parts, u32 num );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnTouchEvent
   * @brief   タッチパネルイベントの検知
   * @date    2015.06.02
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
   * @date    2015.05.29
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
   * @date    2015.06.02
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
   * @func    OnKeyAction
   * @brief   キー入力の検知
   * @date    2015.05.29
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
  app::util::Heap * m_heap;   //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ
  const bool * m_isMenuOpen;  //!< メニューを開いているか
  const s32 * m_dispMode;     //!< 下画面表示モード

  FrameListener * m_frameListener;  //!< リスナー

  Savedata::FieldMenu * m_saveFieldMenu;    //!< フィールドメニューセーブデータ
  const Field::EventWork * m_eventWork;     //!< イベントワーク
  Savedata::ResortSave * m_saveResort;      //!< リゾートセーブデータ
  Savedata::BoxPokemon * m_saveBoxPokemon;  //!< ボックスポケモン
  pml::PokeParty * m_party;                 //!< 手持ちポケモン

  Savedata::FieldMenu::IconID m_cursorPosIconID;  //!< カーソル位置の項目ID

  Savedata::FieldMenu::IconID m_catchIconID;  //!< キャッチしている項目ID
  u16 m_catchIconPage;          //!< キャッチしている項目のページ
  u16 m_releaseIconMoveCount;   //!< キャッチムーブ移動カウント

  u16 m_lastTouchX;     //!< キャッチムーブ：最後にタッチしていたX座標
  u16 m_lastTouchY;     //!< キャッチムーブ：最後にタッチしていたY座標
  u16 m_iconPutPosID;   //!< キャッチムーブ：配置先のID
  s16 m_iconPutIndex;   //!< キャッチムーブ：配置先のインデックス
  f32 m_getIconCalcX;   //!< キャッチムーブ：取得アイコンのリリース時の移動計算用X値
  f32 m_getIconCalcY;   //!< キャッチムーブ：取得アイコンのリリース時の移動計算用Y値
  f32 m_putIconCalcX;   //!< キャッチムーブ：配置アイコンのリリース時の移動計算用X値
  f32 m_putIconCalcY;   //!< キャッチムーブ：配置アイコンのリリース時の移動計算用Y値
  bool m_isIconCatch;   //!< キャッチムーブフラグ

  App::Tool::SlideScroll * m_pSlideScroll;  //!< スライドスクロール
  f32 m_pageReleaseScrollSpeed;   //!< スライドページスクロール速度
  f32 m_pageReleaseScrollEnd;     //!< スライドページスクロール終了座標

  bool m_isInputTrigger;  //!< true = トリガー入力

  gfl2::str::MsgData * m_pMsgData;  //!< メッセージデータ
  print::WordSet * m_pWordSet;      //!< 単語セット

  void * m_pLytResBuff;   //!< レイアウトリソースバッファ
  void * m_pMsgBuff;      //!< メッセージバッファ

  app::tool::MenuCursor * m_pMenuCursor;  //!< メニューカーソル
  void * m_pMenuCursorBuffer;             //!< メニューカーソルリソースバッファ

  gfl2::str::MsgData * m_pRotomMsgData;   //!< ロトムメッセージデータ
  void * m_pRotomMsgBuff;                 //!< ロトムメッセージリソースバッファ

  app::tool::AppCommonGrpIconData * m_pCommonIcon;  //!< Newアイコン

  pml::pokepara::CoreParam * m_pTmpCoreParam;   //!< ボックス内のポケモン数取得用

  bool m_isInputLock;   //!< true = 操作をロックする（項目キャッチ操作中に使用）

  bool m_isDrawEnable;  //!< 描画許可フラグ

  bool m_isMenuInfoAnimeForceStop;   //!< true = お知らせアニメの動作をさせない

  bool m_isInitEnd;   //!< true = 初期化終了

  u32 m_festaInfoValue;   //!< フェスタ情報の数値

  u32 m_subSeq;
};



GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

#endif  // MENU_DRAW_H_INCLUDED
