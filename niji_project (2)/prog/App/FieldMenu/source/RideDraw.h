//=============================================================================
/**
 * @file    RideDraw.h
 * @brief   ライドポケモン選択画面
 * @author  Hiroyuki Nakamura
 * @date    2015.07.15
 */
//=============================================================================
#if !defined( RIDE_DRAW_H_INCLUDED )
#define RIDE_DRAW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Fade/include/gfl2_FadeManager.h>
// niji
#include "Savedata/include/FieldMenuSave.h"
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "Field/FieldRo/include/Event/FieldEventPokemonRide.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class MenuCursor;
class PokeIcon;
class AppCommonGrpIconData;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(Field)
class EventWork;
GFL_NAMESPACE_END(Field)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

// 前方宣言
struct RIDE_PARAM;
class PlayData;
class RideDrawListener;


//=============================================================================
/**
 * @class RideDraw
 * @brief ライドポケモン選択画面
 * @date  2015.07.15
 */
//=============================================================================
class RideDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
private:
  //! リストデータ
  struct ListData
  {
    u32 anm_frame;  //!< アニメフレーム
    u32 text;       //!< テキストラベル
  };

  //! レイアウトID
  enum LytID
  {
    LYTID_LIST = 0,   //!< リスト
    LYTID_SHORTCUT,   //!< ショートカット登録
    LYTID_CURSOR,     //!< カーソル

    LYTID_MAX         //!< 最大数
  };

  static const ListData RideList[Savedata::FieldMenu::RIDE_ID_MAX];   //!< リストデータ

  static const u32 LIST_PANE_MAX = 10;  //!< 項目ペイン数

  static const f32 LIST_ITEM_PX;  //!< リスト開始X座標
  static const f32 LIST_ITEM_PY;  //!< リスト開始Y座標
  static const f32 LIST_ITEM_SX;  //!< リスト項目Xサイズ
  static const f32 LIST_ITEM_SY;  //!< リスト項目Yサイズ

  //! 項目ペインテーブル
  static const gfl2::lyt::LytPaneIndex ItemPartsTable[LIST_PANE_MAX];

  static const u32 SHORTCUT_ANIME_MAX = Savedata::FieldMenu::KEY_ID_MAX + 1;

  //! ショートカット登録ペインテーブル
  static const gfl2::lyt::LytPaneIndex ShortcutCrossKeyPaneTable[Savedata::FieldMenu::KEY_ID_MAX];
  //! ショートカット登録アニメテーブル
  static const u32 ShortcutCrossKeyPokeIconAnmTable[Savedata::FieldMenu::KEY_ID_MAX];

  //! ボタンID
  enum ButtonID
  {
    BUTTON_ITEM_00 = 0,     //!< リスト項目０
    BUTTON_ITEM_01,         //!< リスト項目１
    BUTTON_ITEM_02,         //!< リスト項目２
    BUTTON_ITEM_03,         //!< リスト項目３
    BUTTON_ITEM_04,         //!< リスト項目４
    BUTTON_ITEM_05,         //!< リスト項目５
    BUTTON_ITEM_06,         //!< リスト項目６
    BUTTON_ITEM_07,         //!< リスト項目７
    BUTTON_ITEM_08,         //!< リスト項目８
    BUTTON_ITEM_09,         //!< リスト項目９

    BUTTON_SHORTCUT_00,     //!< ショートカット登録０
    BUTTON_SHORTCUT_01,     //!< ショートカット登録１
    BUTTON_SHORTCUT_02,     //!< ショートカット登録２
    BUTTON_SHORTCUT_03,     //!< ショートカット登録３
    BUTTON_SHORTCUT_04,     //!< ショートカット登録４
    BUTTON_SHORTCUT_05,     //!< ショートカット登録５
    BUTTON_SHORTCUT_06,     //!< ショートカット登録６
    BUTTON_SHORTCUT_07,     //!< ショートカット登録７
    BUTTON_SHORTCUT_08,     //!< ショートカット登録８
    BUTTON_SHORTCUT_09,     //!< ショートカット登録９

    BUTTON_EXIT,            //!< キャンセル

    BUTTON_KEY_UP,          //!< ショートカット登録・上
    BUTTON_KEY_DOWN,        //!< ショートカット登録・下
    BUTTON_KEY_LEFT,        //!< ショートカット登録・左
    BUTTON_KEY_RIGHT,       //!< ショートカット登録・右

    BUTTON_KEY_CANCEL,      //!< ショートカット登録キャンセル

    BUTTON_MAX              //!< 最大数
  };


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap        ヒープ
   * @param   render_man  描画マネージャ
   * @param   param       外部設定パラメータ
   * @param   msg_lyt     メッセージレイアウト
   * @param   msg_res_id  メッセージレイアウトのマルチリソースID
   */
  //-----------------------------------------------------------------------------
  RideDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man, const RIDE_PARAM * param, gfl2::lyt::LytWk * msg_lyt, gfl2::lyt::LytMultiResID msg_res_id );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~RideDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize
   * @brief   初期化
   * @date    2015.07.15
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Initialize(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate
   * @brief   終了処理
   * @date    2015.07.15
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Terminate(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetSelectListener
   * @brief   リスナーをセット
   * @date    2015.07.15
   *
   * @param   listener  リスナー
   */
  //-----------------------------------------------------------------------------
  void SetSelectListener( RideDrawListener * listener );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    GetGameData
   * @brief   ゲームデータ取得
   * @date    2015.08.24
   */
  //-----------------------------------------------------------------------------
  void GetGameData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadLayout
   * @brief   レイアウトリソース読み込み
   * @date    2015.08.04
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateLayout
   * @brief   レイアウト生成
   * @date    2015.07.15
   */
  //-----------------------------------------------------------------------------
  void CreateLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteLayout
   * @brief   レイアウト削除
   * @date    2015.07.15
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadMessage
   * @brief   メッセージ読み込み
   * @date    2015.08.04
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UnloadMessage
   * @brief   メッセージ削除
   * @date    2015.08.04
   */
  //-----------------------------------------------------------------------------
  void UnloadMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuCursor
   * @brief   メニューカーソル生成
   * @date    2015.08.04
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
   * @date    2015.08.04
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuCursorLayout
   * @brief   メニューカーソル用レイアウト生成
   * @date    2015.08.04
   */
  //-----------------------------------------------------------------------------
  void CreateMenuCursorLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMenuCursorLayout
   * @brief   メニューカーソル用レイアウト削除
   * @date    2015.08.04
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuCursorLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitMenuCursor
   * @brief   メニューカーソル初期化
   * @date    2015.08.06
   */
  //-----------------------------------------------------------------------------
  void InitMenuCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SaveMenuCursorPos
   * @brief   メニューカーソル位置を記憶
   * @date    2015.08.06
   */
  //-----------------------------------------------------------------------------
  void SaveMenuCursorPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMenuCursor
   * @brief   メニューカーソルセット
   * @date    2015.08.06
   *
   * @param   pos   カーソル位置
   */
  //-----------------------------------------------------------------------------
  void SetMenuCursor( s32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveMenuCursor
   * @brief   メニューカーソル移動
   * @date    2015.08.06
   *
   * @param   mv      移動方向
   * @param   is_trg  true = トリガー入力
   *
   * @retval  true  = 移動した
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool MoveMenuCursor( s32 mv, bool is_trg );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateCommonIcon
   * @brief   汎用アイコン生成
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
   * @brief   汎用アイコン削除
   * @date    2016.01.16
   */
  //-----------------------------------------------------------------------------
  void DeleteCommonIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeNewIconTexture
   * @brief   Newアイコンのテクスチャを切り替え
   * @date    2016.01.16
   */
  //-----------------------------------------------------------------------------
  void ChangeNewIconTexture(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateButton
   * @brief   ボタン作成
   * @date    2015.08.06
   */
  //-----------------------------------------------------------------------------
  void CreateButton(void);

  // @note 本体なし
  void SetButtonInfoEx(
        gfl2::lyt::LytWk * lytwk,
        app::ui::ButtonInfoLytWk * info,
        gfl2::lyt::LytParts * parts_tbl,
        const app::ui::ButtonInfoLytWk * info_tbl,
        gfl2::lyt::LytPaneIndex bound,
        u32 max );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateListButton
   * @brief   通常ボタンの情報をセット
   * @date    2015.08.06
   *
   * @param   info  情報格納場所
   */
  //-----------------------------------------------------------------------------
  void CreateListButton( app::ui::ButtonInfoLytWk * info );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateKeyButton
   * @brief   ショートカットボタンの情報をセット
   * @date    2015.08.06
   *
   * @param   info  情報格納場所
   */
  //-----------------------------------------------------------------------------
  void CreateKeyButton( app::ui::ButtonInfoLytWk * info );

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新処理
   * @date    2015.07.15
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画処理
   * @date    2015.07.15
   *
   * @param   displayNo   描画ディスプレイ番号
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitListData
   * @brief   リストデータ初期化
   * @date    2015.08.06
   */
  //-----------------------------------------------------------------------------
  void InitListData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutList
   * @brief   リスト配置
   * @date    2015.08.06
   */
  //-----------------------------------------------------------------------------
  void PutList(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutItemInfo
   * @brief   項目情報表示
   * @date    2015.10.10
   *
   * @param   pos   カーソル位置
   */
  //-----------------------------------------------------------------------------
  void PutItemInfo( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetDefaultString
   * @brief   デフォルト文字列書き込み
   * @date    2015.08.04
   */
  //-----------------------------------------------------------------------------
  void SetDefaultString(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleShortcut
   * @brief   ショートカット登録レイアウト表示切り替え
   * @date    2015.08.06
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleShortcut( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetShortcutKey
   * @brief   ショートカット登録
   * @date    2015.08.06
   *
   * @param   key_id  登録先のID
   */
  //-----------------------------------------------------------------------------
  void SetShortcutKey( Savedata::FieldMenu::RideKeyID key_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetShortcutKeyAnime
   * @brief   項目上のショートカットボタンのアニメを変更
   * @date    2015.08.06
   *
   * @param   button_no   ボタン番号
   * @param   key_id      登録先のID
   */
  //-----------------------------------------------------------------------------
  void SetShortcutKeyAnime( u32 button_no, Savedata::FieldMenu::RideKeyID key_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetShortcutButtonIDIndex
   * @brief   項目のボタンインデックスを取得
   * @date    2015.08.06
   *
   * @param   id  ライドID
   *
   * @return  ライドが表示されているボタンのインデックス
   */
  //-----------------------------------------------------------------------------
  u32 GetShortcutButtonIDIndex( Savedata::FieldMenu::RideID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitShortcutCrossKeyIcon
   * @brief   ショートカット登録レイアウトのアイコン表示を初期化
   * @date    2015.08.06
   */
  //-----------------------------------------------------------------------------
  void InitShortcutCrossKeyIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetSelect
   * @brief   選択したライドIDをセット
   * @date    2015.08.24
   *
   * @param   id  ライドID
   */
  //-----------------------------------------------------------------------------
  void SetSelect( Savedata::FieldMenu::RideID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutUpperMessage
   * @brief   上画面に情報表示切り替え
   * @date    2016.03.28
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void PutUpperMessage( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneStartingAnimeEvent
   * @brief   ペイン選択アニメーションの開始を検知
   * @date    2015.12.16
   *
   * @param   button_id   通知されたボタンID
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual void OnLayoutPaneStartingAnimeEvent( const u32 button_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneEvent
   * @brief   ペインイベントの検知
   * @date    2015.08.06
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
   * @func    OnLayoutPaneTouchTriggerEvent
   * @brief   ボタンタッチを検知
   * @date    2015.08.06
   *
   * @param   button_id   通知されたボタンID
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual void OnLayoutPaneTouchTriggerEvent( const u32 button_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnKeyAction
   * @brief   キー入力の検知
   * @date    2015.07.15
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
  RIDE_PARAM m_rideParam;         //!< 外部設定パラメータ
  gfl2::lyt::LytWk * m_msgLytWk;  //!< メッセージ用レイアウトワーク
  gfl2::lyt::LytMultiResID m_msgLytMultiResID;  //!< メッセージレイアウト用マルチリソースID

  Savedata::FieldMenu * m_saveFieldMenu;  //!< フィールドメニューセーブデータ
  PlayData * m_playData;                  //!< フィールドメニューゲームデータ
  const Field::EventWork * m_eventWork;   //!< イベントワーク

  RideDrawListener * m_listener;  //!< リスナー

  void * m_pLytResBuff;   //!< レイアウトリソースバッファ
  void * m_pMsgBuff;      //!< メッセージリソースバッファ
  void * m_pNameMsgBuff;  //!< ライド項目名リソースバッファ

  gfl2::str::MsgData * m_pMsgData;      //!< メッセージデータ
  gfl2::str::MsgData * m_pNameMsgData;  //!< メッセージデータ
  print::WordSet * m_pWordSet;          //!< 単語セット

  app::tool::MenuCursor * m_pMenuCursor;  //!< メニューカーソル
  void * m_pMenuCursorBuffer;   //!< メニューカーソルリソースバッファ
  s32 m_cursorPos;              //!< カーソル位置

  app::tool::AppCommonGrpIconData * m_pCommonIcon;  //!< Newアイコン

  u32 m_rideMax;  //!< 解放されているライド数

  Savedata::FieldMenu::RideID m_selectRideID;   //!< 選択したライドID

  Savedata::FieldMenu::RideID m_listData[LIST_PANE_MAX];  //!< ライドリストデータ
  Savedata::FieldMenu::RideID m_setRideID;                //!< ショートカットに登録するライドID

  bool m_isSetShortcut;   //!< true = ショートカット登録モード

  u8 m_mainSeq;     //!< メインシーケンス
  u8 m_subSeq;      //!< サブシーケンス
  u16 m_setupSeq;   //!< セットアップシーケンス

  bool m_isDrawEnable;  //!< 描画許可フラグ
};


GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

#endif  // RIDE_DRAW_H_INCLUDED
