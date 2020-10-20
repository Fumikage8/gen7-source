//=============================================================================
/**
 * @file    BagDraw.h
 * @brief   バッグ画面表示
 * @author  Hiroyuki Nakamura
 * @date    2015.03.04
 */
//=============================================================================
#if !defined( BAG_DRAW_H_INCLUDED )
#define BAG_DRAW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// pmlib
#include <pml/include/pmlib.h>
// niji
#include "Savedata/include/MyItemSave.h"
#include "Savedata/include/MyItemSaveDef.h"
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "AppLib/include/Tool/app_tool_MenuWindowListener.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
// module
#include "BagPokemonListener.h"
#include "App/AppEvent/include/Bag/BagGamedata.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)

GFL_NAMESPACE_BEGIN(tool)
class MenuWindow;
class ItemIcon;
class AppCommonGrpIconData;
GFL_NAMESPACE_END(tool)

GFL_NAMESPACE_BEGIN(util)
class AppCamera;
GFL_NAMESPACE_END(util)

GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)
class ItemEffect;
GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(bag)
// 前方宣言
struct APP_PARAM;
class BagDrawListener;
class BagPokemon;

//=============================================================================
/**
 * @class BagDraw
 * @brief バッグ画面表示クラス
 * @date  2015.03.04
 */
//=============================================================================
class BagDraw
  : public print::MsgCallback
  , public app::ui::UIView
  , public app::ui::UIInputListener
  , public app::tool::MenuWindowListener
  , public app::tool::PaneList::CallBack
  , public app::tool::PaneListView::Listener
  , public BagPokemonListener
{
private:
  //! レイアウトID
  enum LayoutID
  {
    LYTID_BAG = 0,  //!< バッグ本体
    LYTID_CURSOR,   //!< カーソル
    LYTID_MAX
  };

  static const u32 LIST_ITEM_PANE_MAX = 9;    //!< リスト項目ペイン数
  static const u32 LIST_CURSOR_MOVE_MAX = 7;  //!< リストカーソル動作範囲

  static const f32 LIST_ITEM_PX;  //!< リスト開始X座標
  static const f32 LIST_ITEM_PY;  //!< リスト開始Y座標
  static const f32 LIST_ITEM_SX;  //!< リスト項目Xサイズ
  static const f32 LIST_ITEM_SY;  //!< リスト項目Yサイズ

  static const u32 FIELD_POCKET_MAX = PlayData::POCKET_DATA_MAX;  //!< フィールドポケット最大数
  static const u32 BATTLE_POCKET_MAX = BATTLE_BAG_POCKET_MAX;     //!< バトルポケット最大数

  //! ポケットペインテーブル
  static const gfl2::lyt::LytPaneIndex PocketBasePaneTable[FIELD_POCKET_MAX];

  //! リスト項目ペインテーブル
  static const gfl2::lyt::LytPaneIndex ListBasePaneTable[LIST_ITEM_PANE_MAX];

  //! ボタンID
  enum ButtonID
  {
    BUTTON_ID_RETURN = 0,   //!< 戻るボタン
    BUTTON_ID_POCKET,       //!< ポケット（開始）

    FIELD_BUTTON_ID_MAX = BUTTON_ID_POCKET + FIELD_POCKET_MAX,    //!< フィールドポケットボタンID最大

//    BATTLE_BUTTON_ID_MAX = BUTTON_ID_POCKET + BATTLE_POCKET_MAX,  //!< バトルポケットボタンID最大
  };

  //! ページカラーアニメ
  struct PageColorAnime
  {
    u32 normal;   //!< 通常（非選択時）
    u32 select;   //!< 選択時
  };

  //! フィールド用ポケットカラーアニメ
  static const PageColorAnime FieldPocketAnime[FIELD_POCKET_MAX];
  //! バトル用ポケットカラーアニメ
  static const PageColorAnime BattlePocketAnime[BATTLE_POCKET_MAX];
  //! フィールド用リストカラーアニメ
  static const PageColorAnime FieldPaneListItemColorTable[LIST_ITEM_PANE_MAX][FIELD_POCKET_MAX];
  //! バトル用リストカラーアニメ
  static const PageColorAnime BattlePaneListItemColorTable[LIST_ITEM_PANE_MAX][BATTLE_POCKET_MAX];

  //! 動作モード
  enum InputMode
  {
    MODE_NORMAL = 0,        //!< 通常
    MODE_POKE_SELECT,       //!< ポケモン選択
//    MODE_POKE_ITEM_CATCH,   //!< ポケモンの道具取得
//    MODE_LIST_ITEM_CATCH,   //!< リスト項目の道具取得
  };


public:
  //! 動作ID
  enum ActionID
  {
    ACTION_B_BUTTON = 0,      //!< Bボタン

    ACTION_POCKET_00,         //!< ポケット０
    ACTION_POCKET_01,         //!< ポケット１
    ACTION_POCKET_02,         //!< ポケット２
    ACTION_POCKET_03,         //!< ポケット３
    ACTION_POCKET_04,         //!< ポケット４
    ACTION_POCKET_05,         //!< ポケット５
    ACTION_POCKET_06,         //!< ポケット６
    ACTION_POCKET_07,         //!< ポケット７

    ACTION_POCKET_LEFT,       //!< ポケット切り替え：左方向
    ACTION_POCKET_RIGHT,      //!< ポケット切り替え：右方向

    ACTION_TOUCH_ON_PLATE,    //!< ポケモンプレートタッチ
    ACTION_TOUCH_OFF_PLATE,   //!< ポケモンプレートリリース
    ACTION_SELECT_PLATE,      //!< ポケモンプレート選択
    ACTION_GET_POKE_ITEM,     //!< ポケモンの道具を取得
    ACTION_PUT_POKE_ITEM,     //!< ポケモンの道具を配置

    ACTION_PUT_LIST_ITEM,     //!< 道具リストから道具を取得

    ACTION_MENU_00,           //!< メニュー０
    ACTION_MENU_01,           //!< メニュー１
    ACTION_MENU_02,           //!< メニュー２
    ACTION_MENU_03,           //!< メニュー３
    ACTION_MENU_04,           //!< メニュー４
    ACTION_MENU_05,           //!< メニュー５
    ACTION_MENU_06,           //!< メニュー６
    ACTION_MENU_07,           //!< メニュー７

    ACTION_SORT,              //!< ソート
    ACTION_IREKAE,            //!< いれかえ
  };

  //! メッセージID
  enum MsgID
  {
    MSG_ID_ITEM_MENU = 0,           // @0を　どうしますか？
    MSG_ID_ITEM_CATCH_MENU,         // @0を　@1に　どうしますか？
    MSG_ID_SORT,                    // どの　ならびに　かえますか？
    MSG_ID_USE_SELECT,              // どの　ポケモンに　つかいますか？
    MSG_ID_NO_POKEMON,              // ポケモンが　いませんよ！
    MSG_ID_SET_SELECT,              // どの　ポケモンに　もたせますか？
    MSG_ID_SET_ITEM,                // @0を　もたせた
    MSG_ID_SET_ERROR_ITEM,          // @0を　もたせることは　できません！
    MSG_ID_SET_ERROR_EGG,           // タマゴには　もたせられません
    MSG_ID_SET_ERROR_MAX,           // バッグが　いっぱいで　どうぐを　うけとれません！
    MSG_ID_CHANGE_CHECK,            // もっている　どうぐを　とりかえますか？
    MSG_ID_CHANGE_ITEM,             // @0を　あずかって　@1を　もたせた！
    MSG_ID_COLLECT_ITEM,            // つかったり　もたせたり　できません
    MSG_ID_USE_ERROR,               // パパの　ことば……
    MSG_ID_USE_ITEM,                // @0は　@1を　つかった！
    MSG_ID_USE_GAKUSYUUSOUTI_ON,    // がくしゅうそうちを　オンに　しました
    MSG_ID_USE_GAKUSYUUSOUTI_OFF,   // がくしゅうそうちを　オフに　しました
    MSG_ID_ERR_SPRAY,               // まえに　つかった　スプレーの　こうかが　のこってます！
    MSG_ID_USE_ERROR_RCV,           // つかっても　こうかがないよ
    MSG_ID_USE_WAZAMACHINE_SELECT,  // どの　ポケモンに　おぼえさせますか？

    MSG_ID_TOKUSEI_CHANGE_CHECK,  // @0の　とくせいを　@1に　かえますか？
    MSG_ID_TOKUSEI_CHANGE,        // @0の　とくせいが　@1に　へんかした！

    MSG_ID_FORM_CHANGE,       // @0は　べつの　すがたに　へんかした！
    MSG_ID_UNION_ERR,         // その　ポケモンとは　がったい　できません
    MSG_ID_UNION_ERR_EGG,     // タマゴとは　がったい　できません
    MSG_ID_UNION_ERR_OFF,     // てもちが　いっぱい　なので　ぶんり　できません
    MSG_ID_UNION_ERR_DEATH1,  // ひんしの　ポケモンには　つかえません
    MSG_ID_UNION_ERR_DEATH2,  // ひんしの　ポケモンとは　がったい　できません
    MSG_ID_UNION_SELECT,      // どのポケモンと　がったい？

    MSG_ID_WM_START,        // わざマシンを　きどうした
    MSG_ID_WM_ERR_BAD,      // あいしょうが　わるかった
    MSG_ID_WM_ERR_SAME,     // すでに  おぼえています
    MSG_ID_WM_SELECT,       // わすれる　わざを　えらんでください
    MSG_ID_WM_COMP,         // おぼえた
    MSG_ID_WM_COMP_ERASE,   // わずれて　おぼえた
    MSG_ID_WM_COMP_CANCEL,  // おぼえずに　おわった

    MSG_ID_ADD_FREESPACE,   // フリースペースへ　いどうしました
    MSG_ID_SUB_FREESPACE,   // (ポケット名)へ　もどしました

    MSG_ID_SORT_TYPE,       // しゅるいじゅんに　せいり　しました
    MSG_ID_SORT_NAME,       // なまえじゅんに　せいり　しました
    MSG_ID_SORT_NUMBER,     // ばんごうじゅんに　せいり　しました
    MSG_ID_SORT_NEW,        // あたらしいじゅんに　せいり　しました

    MSG_ID_SALE_SELECT,     // @0を　いくつ　うりますか？
    MSG_ID_SALE_NOT,        // @0を　かいとることは　できません
    MSG_ID_SALE_YESNO,      // @0円で　おひきとり　いたしましょう
    MSG_ID_SALE_COMP,       // @0を　わたして　@1円を　うけとった
    MSG_ID_SALE_COMP_2,     // @0を　わたして　@1円を　うけとった（複数用）

    MSG_ID_BEADS_WAZA_NONE,   // 全力技を覚えていない
    MSG_ID_BEADS_START,       // もたせます

    MSG_ID_ZCUBE_MENU,      // @0で　なにを　しますか？
    MSG_ID_ZCUBE_FORM_ERR,  // この　とくせいでは　すがたを　かえられません
    MSG_ID_ZCUBE_CHECK,     // @0/100こ　かいしゅう　ぜんぶで　@1こ

    MSG_ID_USE_ERROR_ROTOM_POWER,   // まだ　まえの　ロトムパワーの　こうかが　のこってます！
  };

  //! 道具キャッチムーブ時の配置先ID
  enum ItemPutPosID
  {
    PUT_ID_CANCEL = 0,  //!< キャンセル
    PUT_ID_PLATE,       //!< ポケモンプレート
    PUT_ID_LIST,        //!< 道具リスト
    PUT_ID_POCKET,      //!< ポケット
    PUT_ID_SCROLL,      //!< 道具リストスクロール位置
    PUT_ID_DELETE,      //!< 道具削除（ピース用）
  };

  static const u32 RESET_LIST_POS_NORMAL = 0xffffffff;  //!< リスト再生成時のカーソル位置を指定しない

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap              ヒープ
   * @param   rendering_manager 描画マネージャー
   * @param   is_demo_mode      デモモードフラグ
   */
  //-----------------------------------------------------------------------------
  BagDraw( app::util::Heap * heap, app::util::AppRenderingManager * rendering_manager, app::tool::AppCommonGrpIconData * common_icon, const bool * is_demo_mode );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~BagDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetInputEnabled
   * @brief   入力判定の切り替え
   * @date    2015.06.25
   *
   * @param   flg   true = 有効, false = 無効
   */
  //-----------------------------------------------------------------------------
  void SetInputEnabled( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateNumFont
   * @brief   NUMフォント生成
   * @date    2015.07.18
   */
  //-----------------------------------------------------------------------------
  void CreateNumFont(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetNumFont
   * @brief   NUMフォントをセット
   * @date    2015.07.18
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool SetNumFont(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteNumFont
   * @brief   NUMフォント削除
   * @date    2015.07.18
   */
  //-----------------------------------------------------------------------------
  void DeleteNumFont(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   初期化処理
   * @date    2015.03.05
   *
   * @param   param         外部設定パラメータ
   * @param   res_buf       レイアウトリソースバッファ
   * @param   menu_cursor   メニューカーソル
   * @param   pocket_bit    ポケット表示フラグ
   */
  //-----------------------------------------------------------------------------
  void Init( APP_PARAM * param, void * res_buf, void * msg_buf, WordSetLoader * wset_loader, app::tool::MenuCursor * menu_cursor, const u8 * pocket_bit );

  //-----------------------------------------------------------------------------
  /**
   * @func    End
   * @brief   終了処理
   * @date    2015.03.05
   *
   * @retval  true  = 処理完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool End(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Start
   * @brief   開始処理
   * @date    2015.03.23
   *
   * @retval  false = 処理中
   * @retval  true  = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Start(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartBattleMode
   * @brief   バトル用追加開始処理
   * @date    2015.06.09
   */
  //-----------------------------------------------------------------------------
  void StartBattleMode(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartShopMode
   * @brief   ショップ用追加開始処理
   * @date    2015.07.16
   */
  //-----------------------------------------------------------------------------
  void StartShopMode(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetDrawListener
   * @brief   表示リスナーをセット
   * @date    2015.03.05
   *
   * @param   listener  表示リスナー
   */
  //-----------------------------------------------------------------------------
  void SetDrawListener( BagDrawListener * listener );
  
  //-----------------------------------------------------------------------------
  /**
   * @func    StartAnime
   * @brief   アニメ開始
   * @date    2015.04.10
   *
   * @param   id  アニメID
   */
  //-----------------------------------------------------------------------------
//  void StartAnime( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsAnimeEnd
   * @brief   アニメ終了チェック
   * @date    2015.04.10
   *
   * @param   id  アニメID
   *
   * @retval  false = 再生中
   * @retval  true  = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsAnimeEnd( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitDisplay
   * @brief   初期画面表示設定
   * @date    2015.03.23
   *
   * @param   pocket      ポケット番号
   * @param   bg_no       背景番号
   * @param   is_select   true = ポケモン選択状態
   * @param   is_message  true = メッセージ表示状態
   */
  //-----------------------------------------------------------------------------
  void InitDisplay( u32 pocket, u32 bg_no, bool is_select, bool is_message );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartInAnime
   * @brief   バッグ画面開始アニメ開始
   * @date    2015.05.15
   */
  //-----------------------------------------------------------------------------
  void StartInAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartOutAnime
   * @brief   終了アニメ開始
   * @date    2015.06.02
   */
  //-----------------------------------------------------------------------------
  void StartOutAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsOutAnime
   * @brief   終了アニメ終了チェック
   * @date    2015.06.02
   *
   * @retval  false = 処理中
   * @retval  true  = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsOutAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPocketButtonActive
   * @brief   ポケットボタンのアクティブ/パッシブ切り替え
   * @date    2015.04.30
   *
   * @param   flg   true = アクティブ, false = パッシブ
   */
  //-----------------------------------------------------------------------------
  void SetPocketButtonActive( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPocketButtonActive
   * @brief   ポケットボタンのアクティブ/パッシブ切り替え
   * @date    2015.05.08
   *
   * @param   bit   切り替えるポケット（ビット指定）
   * @param   flg   true = アクティブ, false = パッシブ
   */
  //-----------------------------------------------------------------------------
  void SetPocketButtonActive( u32 bit, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsPocketButtonActive
   * @brief   ポケットのボタンがアクティブか
   * @date    2015.05.08
   *
   * @param   pocket  ポケット番号
   *
   * @retval  true  = アクティブ
   * @retval  false = パッシブ
   */
  //-----------------------------------------------------------------------------
  bool IsPocketButtonActive( u32 pocket );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePocket
   * @brief   ポケット切り替え（カーソル位置補正版）
   * @date    2015.03.24
   *
   * @param   old_pocket  切り替え前のポケット
   * @param   new_pocket  切り替え後のポケット
   * @param   table       道具データテーブル
   * @param   table_max   道具データテーブルの要素数
   * @param   item        選択位置の道具
   * @param   list_pos    リスト位置
   */
  //-----------------------------------------------------------------------------
  void ChangePocket( u32 old_pocket, u32 new_pocket, Savedata::MyItem::ITEM_ST ** table, u32 table_max, u32 itemm=ITEM_DUMMY_DATA, u32 list_pos=RESET_LIST_POS_NORMAL );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePocket
   * @brief   ポケット切り替え（カーソル位置指定版）
   * @date    2015.05.12
   *
   * @param   old_pocket  切り替え前のポケット
   * @param   new_pocket  切り替え後のポケット
   * @param   table       道具データテーブル
   * @param   table_max   道具データテーブルの要素数
   * @param   list_pos    リスト位置
   * @param   list_scroll リストスクロール値
   */
  //-----------------------------------------------------------------------------
  void ChangePocket( u32 old_pocket, u32 new_pocket, Savedata::MyItem::ITEM_ST ** table, u32 table_max, u32 list_pos, f32 list_scroll );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsInitPocketAnimeEnd
   * @brief   ポケット切り替えアニメが終了しているか
   * @date    2015.12.09
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsInitPocketAnimeEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPaneList
   * @brief   ペインリスト初期化
   * @date    2015.04.10
   *
   * @param   view        リストビュークラス
   * @param   pocket      ポケット番号
   * @param   table       道具データテーブル
   * @param   table_max   道具データテーブルの要素数
   */
  //-----------------------------------------------------------------------------
  void InitPaneList( app::tool::PaneListView * view, u32 pocket, Savedata::MyItem::ITEM_ST ** table, u32 table_max );

  //-----------------------------------------------------------------------------
  /**
   * @func    EndPaneList
   * @brief   ペインリスト終了
   * @date    2015.04.10
   */
  //-----------------------------------------------------------------------------
  void EndPaneList(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPaneListEnable
   * @brief   ペインリストの入力許可設定（カーソル表示切り替え込み）
   * @date    2015.04.15
   *
   * @param   flg   true = 有効, false = 無効
   */
  //-----------------------------------------------------------------------------
  void SetPaneListEnable( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeListPos
   * @brief   道具リストのカーソル位置を変更
   * @date    2015.04.24
   *
   * @param   sel_item  移動先の道具ID
   * @param   pos_item  現在の位置の道具ID
   */
  //-----------------------------------------------------------------------------
  void ChangeListPos( u32 sel_item, u32 pos_item );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePaneListItemColor
   * @brief   カーソル位置のペインリストの項目カラー変更
   * @date    2015.04.30
   *
   * @param   flg   true = 選択状態, false = 非選択状態
   */
  //-----------------------------------------------------------------------------
  void ChangePaneListItemColor( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutMessage
   * @brief   メッセージ表示（一括）
   * @date    2015.04.10
   *
   * @param   id    メッセージID
   * @param   item  道具データ
   * @param   pp    ポケモンデータ
   */
  //-----------------------------------------------------------------------------
  void PutMessage( MsgID id, const Savedata::MyItem::ITEM_ST * item=NULL, const pml::pokepara::PokemonParam * pp=NULL );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutMessageWindowSmall
   * @brief   小メッセージウィンドウ表示切り替え
   * @date    2015.04.21
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void PutMessageWindowSmall( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdatePrintMessage
   * @brief   メッセージ表示更新
   * @date    2015.04.15
   *
   * @retval  true  = 処理中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool UpdatePrintMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetDispActive
   * @brief   画面のアクティブ/パッシブ切り替え
   * @date    2015.04.10
   *
   * @param   flg   true = アクティブ, false = パッシブ
   *
   * @note  メッセージウィンドウの表示切り替え
   */
  //-----------------------------------------------------------------------------
  void SetDispActive( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    PrintMessage
   * @brief   メッセージ表示開始
   * @date    2015.04.15
   *
   * @param   id      メッセージID
   * @param   item1   道具１
   * @param   item2   道具２
   * @param   pp      ポケモンパラメータ
   */
  //-----------------------------------------------------------------------------
  void PrintMessage( MsgID id, u32 item1=ITEM_DUMMY_DATA, u32 item2=ITEM_DUMMY_DATA, const pml::pokepara::PokemonParam * pp=NULL );

  //-----------------------------------------------------------------------------
  /**
   * @func    PrintTokuseikapuseruMessage
   * @brief   特性カプセル用メッセージ表示開始
   * @date    2015.04.20
   *
   * @param   id        メッセージID
   * @param   pp        ポケモンパラメータ
   * @param   tokusei   特性番号
   */
  //-----------------------------------------------------------------------------
  void PrintTokuseikapuseruMessage( MsgID id, const pml::pokepara::PokemonParam * pp=NULL, TokuseiNo tokusei=TOKUSEI_NULL );

  //-----------------------------------------------------------------------------
  /**
   * @func    PrintFormChangeMessage
   * @brief   フォルムチェンジ用メッセージ表示開始
   * @date    2015.04.20
   *
   * @param   id  メッセージID
   * @param   pp  ポケモンパラメータ
   */
  //-----------------------------------------------------------------------------
  void PrintFormChangeMessage( MsgID id, const pml::pokepara::PokemonParam * pp=NULL );

  //-----------------------------------------------------------------------------
  /**
   * @func    PrintUnionWazaSubMessage
   * @brief   分離技忘れメッセージ表示開始
   * @date    2017.01.27
   *
   * @param   pp          ポケモンパラメータ
   * @param   waza        技
   * @param   is_nenriki  true = ねんりきをセット @fix MMCat[441]
   */
  //-----------------------------------------------------------------------------
  void PrintUnionWazaSubMessage( const pml::pokepara::PokemonParam * pp, WazaNo waza, bool is_nenriki );

  //-----------------------------------------------------------------------------
  /**
   * @func    PrintWazaMachineMessage
   * @brief   技マシン用メッセージ表示開始
   * @date    2015.04.20
   *
   * @param   id      メッセージID
   * @param   pp      ポケモンパラメータ
   * @param   waza1   技１
   * @param   waza2   技２
   */
  //-----------------------------------------------------------------------------
  void PrintWazaMachineMessage( MsgID id, const pml::pokepara::PokemonParam * pp=NULL, WazaNo waza1=WAZANO_NULL, WazaNo waza2=WAZANO_NULL );

  //-----------------------------------------------------------------------------
  /**
   * @func    PrintBeadsMessage
   * @brief   ビーズ用メッセージ表示開始
   * @date    2015.11.05
   *
   * @param   id      メッセージID
   * @param   pp      ポケモンパラメータ
   * @param   item1   道具１
   * @param   item2   道具２
   */
  //-----------------------------------------------------------------------------
  void PrintBeadsMessage( MsgID id, const pml::pokepara::PokemonParam * pp=NULL, u32 item1=ITEM_DUMMY_DATA, u32 item2=ITEM_DUMMY_DATA );

  //-----------------------------------------------------------------------------
  /**
   * @func    PrintZCubeMessage
   * @brief   ジガルデキューブ用メッセージ表示開始
   * @date    2015.11.27
   *
   * @param   id        メッセージID
   * @param   collect   回収した数
   * @param   cube      キューブに入っている数
   */
  //-----------------------------------------------------------------------------
  void PrintZCubeMessage( MsgID id, u32 collect=0, u32 cube=0 );

  //-----------------------------------------------------------------------------
  /**
   * @func    PrintSortMessage
   * @brief   ソート用メッセージ表示開始
   * @date    2015.04.27
   *
   * @param   id  メッセージID
   */
  //-----------------------------------------------------------------------------
  void PrintSortMessage( MsgID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    PrintItemUseMessage
   * @brief   道具使用メッセージ表示開始
   * @date    2015.04.21
   *
   * @param   id        回復タイプID
   * @param   pp        ポケモンパラメータ
   * @param   prm_tbl   パラメータテーブル
   */
  //-----------------------------------------------------------------------------
  void PrintItemUseMessage( u32 id, const pml::pokepara::PokemonParam * pp, s32 * prm_tbl=NULL );

  //-----------------------------------------------------------------------------
  /**
   * @func    PrintLvupWazaOboeMessage
   * @brief   レベルアップ技覚え用メッセージ表示開始
   * @date    2015.04.21
   *
   * @param   pp    ポケモンパラメータ
   * @param   waza  技番号
   */
  //-----------------------------------------------------------------------------
  void PrintLvupWazaOboeMessage( const pml::pokepara::PokemonParam * pp, WazaNo waza );

  //-----------------------------------------------------------------------------
  /**
   * @func    PrintFreeSpaceMessage
   * @brief   フリースペース用メッセージ表示開始
   * @date    2015.04.24
   *
   * @param   id      メッセージID
   * @param   item    道具
   * @param   pocket  ポケット
   */
  //-----------------------------------------------------------------------------
  void PrintFreeSpaceMessage( MsgID id, u32 item, u32 pocket=0 );

  //-----------------------------------------------------------------------------
  /**
   * @func    PrintSaleMessage
   * @brief   道具売却用メッセージ表示開始
   * @date    2015.07.16
   *
   * @param   id    メッセージID
   * @param   item  道具
   * @param   num   個数
   * @param   gold  値段
   */
  //-----------------------------------------------------------------------------
  void PrintSaleMessage( MsgID id, u32 item, u32 gold=0 );

  //-----------------------------------------------------------------------------
  /**
   * @func    PrintSasiosaeMessage
   * @brief   さしおさえエラーメッセージ表示開始
   * @date    2015.12.24
   *
   * @param   pp  ポケモンパラメータ
   */
  //-----------------------------------------------------------------------------
  void PrintSasiosaeMessage( const pml::pokepara::PokemonParam * pp );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetString
   * @brief   文字列取得
   * @date    2015.04.10
   *
   * @param   id  文字列ID
   *
   * @return  文字列
   */
  //-----------------------------------------------------------------------------
  gfl2::str::StrBuf * GetString( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeNormalMode
   * @brief   通常モードの切り替え
   * @date    2015.04.15
   *
   * @param   flg   true = モード有効, false = モード無効
   */
  //-----------------------------------------------------------------------------
  void ChangeNormalMode( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePokeSelectMode
   * @brief   ポケモン選択モードの切り替え
   * @date    2015.04.13
   *
   * @param   flg   true = モード有効, false = モード無効
   */
  //-----------------------------------------------------------------------------
  void ChangePokeSelectMode( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutPocketNewIcon
   * @brief   ポケットのNewマーク表示
   * @date    2015.04.28
   *
   * @param   pos   ポケット位置
   * @param   max   Newアイテムの個数
   */
  //-----------------------------------------------------------------------------
  void PutPocketNewIcon( u32 pos, u32 max );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutItemNewIcon
   * @brief   道具のNewマーク表示
   * @date    2015.04.28
   *
   * @param   pos   位置
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void PutItemNewIcon( u32 pos, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetItemChangeMode
   * @brief   道具入れ替えモード切り替え
   * @date    2015.04.30
   *
   * @param   flg   true = モード開始, false = モード終了
   */
  //-----------------------------------------------------------------------------
  void SetItemChangeMode( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    ResetCatchItemChangeModeFlag
   * @brief   リストの道具のキャッチムーブフラグを下げる
   * @date    2015.05.12
   */
  //-----------------------------------------------------------------------------
  void ResetCatchItemChangeModeFlag(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetCatchItemChangeMode
   * @brief   リストの道具キャッチムーブモード切り替え
   * @date    2015.05.12
   *
   * @param   flg   true = モード開始, false = モード終了
   */
  //-----------------------------------------------------------------------------
  void SetCatchItemChangeMode( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsItemChangeMode
   * @brief   道具入れ替えモードかをチェック
   * @date    2015.04.30
   */
  //-----------------------------------------------------------------------------
  bool IsItemChangeMode(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    VisibleItemIcon
   * @brief   アイテムアイコン表示切り替え
   * @date    2015.05.01
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void VisibleItemIcon( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetItemIconPicture
   * @brief   アイテムアイコンのピクチャペインを取得
   * @date    2015.05.01
   *
   * @return  ピクチャペイン
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytPicture * GetItemIconPicture(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartItemCatch
   * @brief   道具キッチムーブ開始
   * @date    2015.05.01
   */
  //-----------------------------------------------------------------------------
  void StartItemCatch(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndItemCatch
   * @brief   道具キッチムーブ終了
   * @date    2015.05.01
   *
   * @param   mv_pos  アイコン移動座標
   */
  //-----------------------------------------------------------------------------
  void EndItemCatch( gfl2::math::VEC3 * mv_pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    ReleaseItemIcon
   * @brief   道具アイコンリリース動作
   * @date    2015.05.01
   *
   * @retval  true  = 動作中
   * @retval  false = 終了
   */
  //-----------------------------------------------------------------------------
  bool ReleaseItemIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveItemIcon
   * @brief   アイテムアイコンキャッチムーブ
   * @date    2015.05.01
   */
  //-----------------------------------------------------------------------------
  void MoveItemIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveItemIcon
   * @brief   アイテムアイコンキャッチムーブ
   * @date    2016.02.06
   *
   * @param   tpx   タッチX座標
   * @param   tpy   タッチY座標
   */
  //-----------------------------------------------------------------------------
  void MoveItemIcon( u16 tpx, u16 tpy );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetItemPutPosID
   * @brief   道具をリリースした場所のIDを取得
   * @date    2015.05.01
   *
   * @return  リリース場所のID
   */
  //-----------------------------------------------------------------------------
  ItemPutPosID GetItemPutPosID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetItemPutIndex
   * @brief   道具リリース時の場所内のインテックスを取得
   * @date    2015.05.01
   */
  //-----------------------------------------------------------------------------
  s32 GetItemPutIndex(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPocketIconPos
   * @brief   ポケットアイコンの座標を取得
   * @date    2015.05.08
   *
   * @param   id  ポケット
   *
   * @return  アイコンの座標
   */
  //-----------------------------------------------------------------------------
  gfl2::math::VEC3 GetPocketIconPos( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeScrollPutData
   * @brief   キャッチムーブの外部スクロールIDを変更
   * @date    2015.05.08
   *
   * @li  道具リスト上ならリストIDへ, それ以外はキャンセルIDへ変更
   */
  //-----------------------------------------------------------------------------
  void ChangeScrollPutData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePutDataCancel
   * @brief   キャッチムーブの配置IDをキャンセルIDにする
   * @date    2015.05.13
   */
  //-----------------------------------------------------------------------------
  void ChangePutDataCancel(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePutDataDelete
   * @brief   キャッチムーブの配置IDを削除IDにする
   * @date    2015.11.05
   */
  //-----------------------------------------------------------------------------
  void ChangePutDataDelete(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListItemPos
   * @brief   道具リストの項目座標を取得
   * @date    2015.05.12
   *
   * @param   list_pos  項目位置
   *
   * @return  座標
   */
  //-----------------------------------------------------------------------------
  gfl2::math::VEC3 GetListItemPos( u32 list_pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    売値表示
   * @brief   PutPrice
   * @date    2015.07.17
   *
   * @param   val   売値
   */
  //-----------------------------------------------------------------------------
  void PutPrice( u32 val );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartSelectPocketAnime
   * @brief   捕獲デモ用：ポケット選択アニメを再生
   * @date    2015.06.25
   *
   * @param   pocket  ポケット番号
   */
  //-----------------------------------------------------------------------------
//  void StartSelectPocketAnime( u32 pocket );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartSelectItemAnime
   * @brief   捕獲デモ用：項目選択アニメを再生
   * @date    2015.06.25
   */
  //-----------------------------------------------------------------------------
  void StartSelectItemAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsDemoAnimeEnd
   * @brief   捕獲デモ用：アニメ終了チェック
   * @date    2015.06.25
   *
   * @retval  false = 再生中
   * @retval  true  = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsDemoAnimeEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetWazaName
   * @brief   技名取得
   * @date    2015.11.30
   *
   * @param   waza  技番号
   *
   * @return  技名
   */
  //-----------------------------------------------------------------------------
  gfl2::str::StrBuf * GetWazaName( WazaNo waza );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleMessageWindow
   * @brief   メッセージウィンドウ表示切り替え
   * @date    2015.11.30
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleMessageWindow( bool flg );

  void SetItemEffect( App::Tool::ItemEffect * effect );
  void UnsetItemEffect(void);


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    GetGameData
   * @brief   ゲームデータ関連取得
   * @date    2015.11.26
   */
  //-----------------------------------------------------------------------------
  void GetGameData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2015.03.05
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.03.05
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize2D
   * @brief   2D関連初期化
   * @date    2015.03.05
   *
   * @param   res_buf   レイアウトリソースバッファ
   */
  //-----------------------------------------------------------------------------
  void Initialize2D( void * res_buf, void * msg_buf, WordSetLoader * wset_loader );

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate2D
   * @brief   2D関連削除
   * @date    2015.03.23
   */
  //-----------------------------------------------------------------------------
  void Terminate2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuCursorLayout
   * @brief   メニューカーソル用レイアウト作成
   * @date    2015.04.10
   *
   * @param   menu_cursor   メニューカーソルクラス
   */
  //-----------------------------------------------------------------------------
  void CreateMenuCursorLayout( app::tool::MenuCursor * menu_cursor );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMenuCursorLayout
   * @brief   メニューカーソル用レイアウト削除
   * @date    2015.04.10
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuCursorLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateMenuCursor
   * @brief   メニューカーソル更新
   * @date    2015.03.25
   */
  //-----------------------------------------------------------------------------
  void UpdateMenuCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawMenuCursor
   * @brief   メニューカーソル描画
   * @date    2015.03.25
   */
  //-----------------------------------------------------------------------------
  void DrawMenuCursor( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteDefaultString
   * @brief   デフォルト文字列書き込み
   * @date    2015.03.23
   */
  //-----------------------------------------------------------------------------
  void WriteDefaultString(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMainControl
   * @brief   入力設定
   * @date    2015.03.24
   */
  //-----------------------------------------------------------------------------
  void SetMainControl(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetExitButtonEnable
   * @brief   戻るボタンの入力許可設定
   * @date    2015.04.15
   *
   * @param   flg   true = 有効, false = 無効
   */
  //-----------------------------------------------------------------------------
  void SetExitButtonEnable( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPocketButtonEnable
   * @brief   ポケットボタンの入力許可設定
   * @date    2015.04.15
   *
   * @param   flg   true = 有効, false = 無効
   */
  //-----------------------------------------------------------------------------
  void SetPocketButtonEnable( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPocketAnime
   * @brief   ポケットアニメ取得
   * @date    2015.03.24
   *
   * @param   max   アニメ数格納場所
   *
   * @return  アニメテーブル
   */
  //-----------------------------------------------------------------------------
  const PageColorAnime * GetPocketAnime( u32 * max );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPocketAnime
   * @brief   ポケットアニメ初期化
   * @date    2015.03.24
   */
  //-----------------------------------------------------------------------------
  void InitPocketAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetNowPocketAnime
   * @brief   選択状態のポケットアニメへ変更
   * @date    2015.03.24
   *
   * @param   old     前回のポケット
   * @param   now     現在のポケット
   * @param   is_end  true = 最終フレーム
   */
  //-----------------------------------------------------------------------------
  void SetNowPocketAnime( u32 old, u32 now, bool is_end = false );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetNowPocketTable
   * @brief   現在のポケットデータを設定
   * @date    2015.03.23
   *
   * @param   pocket      ポケット番号
   * @param   table       道具データテーブル
   * @param   table_max   道具データテーブルの要素数
   */
  //-----------------------------------------------------------------------------
  void SetNowPocketTable( u32 pocket, Savedata::MyItem::ITEM_ST ** table, u32 table_max );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPaneListParam
   * @brief   ペインリスト初期化パラメータ設定
   * @date    2015.03.23
   *
   * @param   param   パラメータ生成場所
   * @param   pos     カーソル位置
   * @param   scroll  スクロール値
   */
  //-----------------------------------------------------------------------------
  void SetPaneListParam( app::tool::PaneList::SETUP_PARAM * param, u32 pos, u32 scroll );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPaneListConfig
   * @brief   ペインリストの設定変更
   * @date    2015.03.30
   *
   * @param   setup   ペインリストのセットアップパラメータ
   */
  //-----------------------------------------------------------------------------
  void SetPaneListConfig( app::tool::PaneList::SETUP_PARAM * setup );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPaneListInputEnable
   * @brief   ペインリストの入力許可設定
   * @date    2015.04.15
   *
   * @param   flg   true = 有効, false = 無効
   */
  //-----------------------------------------------------------------------------
  void SetPaneListInputEnable( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetListItemBaseY
   * @brief   ペインリストの基準Y座標を設定
   * @date    2015.05.08
   */
  //-----------------------------------------------------------------------------
  void SetListItemBaseY(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    AdjustPaneListPos
   * @brief   リストのカーソル位置を補正
   * @date    2015.04.13
   *
   * @param   pos         カーソル位置
   * @param   scroll      スクロール値
   * @param   table       道具データテーブル
   * @param   table_max   道具データテーブルの要素数
   * @param   item        カーソル位置の道具
   */
  //-----------------------------------------------------------------------------
  void AdjustPaneListPos( u32 * pos, u32 * scroll, Savedata::MyItem::ITEM_ST ** table, u32 table_max, u32 item );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListCursorPos
   * @brief   道具リストのカーソル位置を取得
   * @date    2015.04.13
   *
   * @param   pos     カーソル位置格納場所
   * @param   scroll  スクロール値格納場所
   * @param   pocket  ポケット番号
   */
  //-----------------------------------------------------------------------------
  void GetListCursorPos( u32 * pos, u32 * scroll, u32 pocket );

  //-----------------------------------------------------------------------------
  /**
   * @func    PaneListItemSelectListener
   * @brief   ペインリスト決定時に呼ばれるリスナー
   * @date    2015.04.02
   *
   * @param   pos   カーソル位置
   */
  //-----------------------------------------------------------------------------
  virtual void PaneListItemSelectListener( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    PaneListItemCatchListener
   * @brief   ペインリスト項目キャッチ時に呼ばれるリスナー
   * @date    2015.05.12
   *
   * @param   pos   カーソル位置
   */
  //-----------------------------------------------------------------------------
  virtual void PaneListItemCatchListener( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePaneListItemColor
   * @brief   ペインリストの項目カラーを非選択状態に変更
   * @date    2015.03.27
   */
  //-----------------------------------------------------------------------------
  void ChangePaneListItemColor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePaneListItemColor
   * @brief   ペインリストの項目カラー変更
   * @date    2015.03.27
   *
   * @param   pos   項目位置
   * @param   flg   true = 選択状態, false = 非選択状態
   */
  //-----------------------------------------------------------------------------
  void ChangePaneListItemColor( u32 pos, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    CallBack_CursorMove
   * @brief   カーソル移動時のコールバック関数
   * @date    2015.03.16
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
   * @date    2015.03.16
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
   * @date    2015.03.16
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
   * @date    2015.03.16
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
   * @func    ResetItemList
   * @brief   道具リスト再設定
   * @date    2015.03.23
   *
   * @param   item    カーソル位置に指定する道具ID
   * @param   set_pos カーソル位置
   */
  //-----------------------------------------------------------------------------
  void ResetItemList( u32 item, u32 set_pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutListNoItem
   * @brief   道具がない場合の表示
   * @date    2015.03.23
   */
  //-----------------------------------------------------------------------------
  void PutListNoItem(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SelectPlate
   * @brief   ポケモンのプレート選択時に呼ばれるリスナー
   * @date    2015.04.13
   *
   * @param   id  動作ID
   *
   * @note  override BagPokemonListener
   */
  //-----------------------------------------------------------------------------
  virtual void SelectPlate( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    PokeItemCatch
   * @brief   ポケモンの持ち物取得動作時に呼ばれるリスナー
   * @date    2015.05.01
   *
   * @param   x         タッチX座標
   * @param   y         タッチY座標
   * @param   poke_pos  タッチ中のポケモン位置
   *
   * @note  override BagPokemonListener
   */
  //-----------------------------------------------------------------------------
  virtual void PokeItemCatch( u16 x, u16 y, u32 poke_pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMessageWindow
   * @brief   メッセージウィンドウに文字列を書き込む
   * @date    2015.04.03
   *
   * @param   str   文字列
   */
  //-----------------------------------------------------------------------------
  void SetMessageWindow( gfl2::str::StrBuf * str );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMessageWindowSmall
   * @brief   小メッセージウィンドウに文字列を書き込む
   * @date    2015.04.13
   *
   * @param   str   文字列
   */
  //-----------------------------------------------------------------------------
  void SetMessageWindowSmall( gfl2::str::StrBuf * str );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMessageWindow
   * @brief   メッセージウィンドウ生成
   * @date    2015.04.15
   */
  //-----------------------------------------------------------------------------
  void CreateMessageWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMessageWindow
   * @brief   メッセージウィンドウ削除
   * @date    2015.04.15
   */
  //-----------------------------------------------------------------------------
  void DeleteMessageWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPrintMessage
   * @brief   メッセージ表示開始
   * @date    2015.04.15
   *
   * @param   str   文字列
   * @param   type  終了タイプ
   */
  //-----------------------------------------------------------------------------
  void SetPrintMessage( const gfl2::str::StrBuf * str, gfl2::str::StrWin::FinishType type );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateMsgCallback
   * @brief   メッセージ表示コールバック関数
   * @date    2015.04.20
   *
   * @param   arg   コールバックID
   *
   * @retval  true  = 継続
   * @retval  false = 終了
   */
  //-----------------------------------------------------------------------------
  virtual bool UpdateMsgCallback( u16 arg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMsgCallBackSE
   * @brief   メッセージ表示コールバック中のSE再生
   * @date    2015.04.20
   *
   * @param   se  SE番号
   */
  //-----------------------------------------------------------------------------
  void SetMsgCallBackSE( u32 se );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMsgCallBackME
   * @brief   メッセージ表示コールバック中のME再生
   * @date    2015.04.20
   *
   * @param   me  ME番号
   */
  //-----------------------------------------------------------------------------
  void SetMsgCallBackME( u32 me );

  //-----------------------------------------------------------------------------
  /**
   * @func    SelectMenu
   * @brief   項目選択時のリスナー
   * @date    2015.03.10
   *
   * @param   id  メニュー項目のID
   *
   * @note  override MenuWindowListener
   */
  //-----------------------------------------------------------------------------
  virtual void SelectMenu( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartItemNewIconAnime
   * @brief   道具のNewマークのアニメ開始
   * @date    2015.11.13
   */
  //-----------------------------------------------------------------------------
  void StartItemNewIconAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ItemChange
   * @brief   道具入れ替え
   * @date    2015.04.30
   *
   * @param   pos1  位置１
   * @param   pos2  位置２
   */
  //-----------------------------------------------------------------------------
  void ItemChange( int pos1, int pos2 );

  //-----------------------------------------------------------------------------
  /**
   * @func    ItemChangeFreeSpace
   * @brief   道具入れ替え（フリースペース用）
   * @date    2015.04.30
   *
   * @param   pos1  位置１
   * @param   pos2  位置２
   */
  //-----------------------------------------------------------------------------
  void ItemChangeFreeSpace( int pos1, int pos2 );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetItemIcon
   * @brief   アイテムアイコンの部品ペインを取得
   * @date    2015.04.10
   *
   * @return  部品ペイン
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytParts * GetItemIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckItemListScrollPane
   * @brief   指定座標がペインリストの外部スクロール実行箇所かをチェック
   * @date    2015.05.07
   *
   * @param   x   X座標
   * @param   y   Y座標
   *
   * @retval   1 = 下方向へスクロール
   * @retval  -1 = 上方向へスクロール
   * @retval   0 = スクロールしない
   */
  //-----------------------------------------------------------------------------
  int CheckItemListScrollPane( u16 x, u16 y );

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckPocketIconHit
   * @brief   指定座標にポケットアイコンがあるか
   * @date    2015.05.08
   *
   * @param   x   X座標
   * @param   y   Y座標
   *
   * @retval  FIELD_POCKET_MAX = ポケットなし
   * @retval  FIELD_POCKET_MAX != ポケット番号
   */
  //-----------------------------------------------------------------------------
  u32 CheckPocketIconHit( u16 x, u16 y );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetTouchItemPos
   * @brief   タッチ座標の項目位置を取得
   * @date    2015.05.07
   *
   * @param   tpy       タッチX座標
   * @param   tpy       タッチY座標
   * @param   pos       項目位置格納場所
   *
   * @retval  true  = 成功
   * @retval  false = 失敗
   */
  //-----------------------------------------------------------------------------
  bool GetTouchItemPos( s32 tpx, s32 tpy, int * pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPocketIDMax
   * @brief   ポケット数を取得
   * @date    2015.05.13
   *
   * @return  ポケット数
   */
  //-----------------------------------------------------------------------------
//  u32 GetPocketIDMax(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneEvent
   * @brief   ペインイベントの検知
   * @date    2015.03.05
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
   * @func    OnKeyAction
   * @brief   キー入力の検知
   * @date    2015.03.05
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
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ
  app::tool::AppCommonGrpIconData * m_commonIcon;

  APP_PARAM * m_appParam;   //!< バッグ画面外部設定パラメータ

  Savedata::MyStatus * m_myStatus;      //!< プレイヤーデータ
  PlayData * m_playData;                //!< バッグ画面ゲームデータ
  const Savedata::MyItem * m_myItem;    //!< 道具セーブデータ

  Savedata::MyItem::ITEM_ST ** m_nowPocketItem;   //!< 現在のポケットの道具テーブル
  u32 m_nowPocketItemMax;                         //!< 現在のポケットの道具数

  BagDrawListener * m_uiListener;   //!< 表示リスナー

  gfl2::str::MsgData * m_pMsgData;  //!< メッセージデータ
  print::WordSet * m_pWordSet;      //!< 単語セット

  app::tool::MenuCursor * m_pMenuCursor;  //!< メニューカーソル

  app::tool::PaneListView * m_pPaneListView;        //!< ペインリストビュー
  app::tool::PaneList::PANE_DATA * m_pPaneListData; //!< ペインリストのペインデータ
  b8 m_isPaneListChangeMode;  //!< 入れ替えモードフラグ
  b8 m_isPaneListCatchMode;   //!< キャッチムーブフラグ
  u16 m_changeItem;           //!< 入れ替えアイテム

  s32 m_pocket;   //!< ポケット番号

  bool m_createNumFont;   //!< ture = Numフォントを生成した

  InputMode m_inputMode;  //!< 入力モード

  gfl2::str::StrWin::FinishType m_msgFinishType;  //!< メッセージ終了モード
  bool m_isMsgEnd;                                //!< メッセージ終了フラグ
  u32 m_msgCallBackWrok;                          //!< メッセージコールバックワーク

  bool m_isMsgWinPut;     //!< メッセージウィンドウが表示されているか
  bool m_isSelectWinPut;  //!< ポケモン選択などの小メッセージウィンドウが表示されているか

  u16 m_lastTouchX;     //!< キャッチムーブ：最後にタッチしていたX座標
  u16 m_lastTouchY;     //!< キャッチムーブ：最後にタッチしていたY座標
  u16 m_itemPutPosID;   //!< キャッチムーブ：配置先のID
  s16 m_itemPutIndex;   //!< キャッチムーブ：配置先のインデックス
  f32 m_itemCalcX;      //!< キャッチムーブ：リリース動作の加算X値
  f32 m_itemCalcY;      //!< キャッチムーブ：リリース動作の加算Y値

  f32 m_listItemBaseY;  //!< リストの項目ペインの部品内のY座標の差分

  const bool * m_isDemoMode;  //!< デモモードフラグ
  u32 m_demoAnime;            //!< デモ用再生アニメ番号

  App::Tool::ItemEffect * m_itemEffect;   //!< エフェクト
  app::util::AppCamera * m_pAppCamera;    //!< カメラ

  bool m_isDrawEnable;  //!< 描画許可フラグ

  const u8 * m_pocketOnBit;   //!< ポケット存在フラグ（ビット単位）
};

GFL_NAMESPACE_END(bag)
GFL_NAMESPACE_END(app)

#endif  // BAG_DRAW_H_INCLUDED
