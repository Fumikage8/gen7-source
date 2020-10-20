//=============================================================================
/**
 * @file    ShopDraw.h
 * @brief   ショップ購入画面描画処理
 * @author  Hiroyuki Nakamura
 * @date    2015.10.02
 */
//=============================================================================
#if !defined( SHOP_DRAW_H_INCLUDED )
#define SHOP_DRAW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// pmlib
#include <pml/include/pmlib.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "Savedata/include/MyItemSave.h"
#include "Savedata/include/MiscSave.h"
#include "Savedata/include/JoinFestaDataSave.h"
// module
#include "App/Shop/include/ShopDef.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class MenuCursor;
class ItemIcon;
class ScrollBar;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(print)
class ItemMessage;
GFL_NAMESPACE_END(print)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Shop)

// 前方宣言
class ShopDrawListener;


//=============================================================================
/**
 * @class ShopDraw
 * @brief ショップ購入画面描画クラス
 * @date  2015.10.02
 */
//=============================================================================
class ShopDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
public:
  static const s32 ITEM_PUT_MAX = 7;    //!< 道具表示数


private:
  //! レイアウトID
  enum LyoutID
  {
    LYTID_BASE = 0,   //!< ベースレイアウト
    LYTID_CURSOR,     //!< カーソル
    LYTID_MAX         //!< 最大数
  };

//  static const u32 CURSOR_MOVE_MAX = 6;

  static const u32 PRICE_KETA_MAX = 5;    //!< 道具の値段の桁数

  static const f32 CURSOR_MOVE_Y;   //!< カーソルY座標表示間隔

  //! ショップタイプごとのデータ
  struct SHOP_TYPE_DETA
  {
    u32 gold_name;    //!< メッセージID: 消費するポイントのタイトル
    u32 gold_unit;    //!< メッセージID: 消費するポイントの単位
    u32 gold_keta;    //!< 消費するポイントの桁数
    u32 bg_color;     //!< 背景色アニメ
    u32 info_win;     //!< 説明表示切り替えアニメ
  };

  static const SHOP_TYPE_DETA ShopTypeData[TYPE_MAX];   //!< ショップタイプデータテーブル

  static const u32 PUT_INFO_CANCEL_MESSAGE = 0xffffffff;  //!< "やめる"の説明表示用ID


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   type        ショップタイプ
   * @param   heap        ヒープ
   * @param   render_man  描画マネージャ
   * @param   cursor      メニューカーソル
   */
  //-----------------------------------------------------------------------------
  ShopDraw( ShopType type, app::util::Heap * heap, app::util::AppRenderingManager * render_man, app::tool::MenuCursor * cursor );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~ShopDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetupItem
   * @brief   道具データ設定
   * @date    2015.10.05
   *
   * @param   item  データリスト
   * @param   max   データ数
   * @param   man   アイテムマネージャ
   * @param   icon  アイテムアイコン
   */
  //-----------------------------------------------------------------------------
  void SetupItem( const SHOP_ITEM * item, u32 max, itemman::ITEM_MANAGER * man, app::tool::ItemIcon * icon );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetDrawListener
   * @brief   リスナーをセット
   * @date    2015.10.02
   *
   * @param   listener リスナー
   */
  //-----------------------------------------------------------------------------
  void SetDrawListener( ShopDrawListener * listener );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateItemMessage
   * @brief   道具説明メッセージ作成
   * @date    2015.10.05
   *
   * @param   buf   メッセージデータ
   *
   * @note  MsgDataを持っているだけなので、FCショップでもMsgData目的で使う
   */
  //-----------------------------------------------------------------------------
  void CreateItemMessage( void * buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteItemMessage
   * @brief   道具説明メッセージ削除
   * @date    2015.10.05
   */
  //-----------------------------------------------------------------------------
  void DeleteItemMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   初期化処理
   * @date    2015.10.02
   *
   * @param   res_buf       レイアウトリソースバッファ
   * @param   msg_buf       メッセージリソースバッファ
   * @param   wset_loader   WordSetLoader
   * @param   scroll_max    スクロール最大値
   */
  //-----------------------------------------------------------------------------
  void Init( void * res_buf, void * msg_buf, WordSetLoader * wset_loader, u32 scroll_max );

  //-----------------------------------------------------------------------------
  /**
   * @func    End
   * @brief   終了処理
   * @date    2015.10.02
   *
   * @retval  true  = 処理完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool End(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMsgData
   * @brief   メッセージデータ取得
   * @date    2015.10.07
   *
   * @return  メッセージデータ
   */
  //-----------------------------------------------------------------------------
  gfl2::str::MsgData * GetMsgData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Start
   * @brief   開始処理
   * @date    2015.10.02
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Start(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutGold
   * @brief   所持金表示
   * @date    2015.10.05
   *
   * @param   gold  所持金
   */
  //-----------------------------------------------------------------------------
  void PutGold( u32 gold );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutItemList
   * @brief   道具リスト描画
   * @date    2015.10.05
   *
   * @param   scroll_now  現在のスクロール値
   * @param   scroll_max  スクロール最大値
   */
  //-----------------------------------------------------------------------------
  void PutItemList( u32 scroll_now, u32 scroll_max );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleInfoWindow
   * @brief   道具説明表示切り替え
   * @date    2015.10.07
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleInfoWindow( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutMyItemNum
   * @brief   持っている数表示
   * @date    2015.10.05
   *
   * @param   item  道具番号
   */
  //-----------------------------------------------------------------------------
  void PutMyItemNum( u32 item );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateNumCounter
   * @brief   個数選択ウィンドウ表示更新
   * @date    2015.10.05
   *
   * @param   gold  金額
   * @param   num   個数
   */
  //-----------------------------------------------------------------------------
  void UpdateNumCounter( u32 gold, u32 num );

  //-----------------------------------------------------------------------------
  /**
   * @func    InNumCounter
   * @brief   個数選択ウィンドウ表示
   * @date    2015.10.05
   *
   * @param   gold  金額
   * @param   num   個数
   */
  //-----------------------------------------------------------------------------
  void InNumCounter( u32 gold, u32 num );

  //-----------------------------------------------------------------------------
  /**
   * @func    OutNumCounter
   * @brief   個数選択ウィンドウ非表示
   * @date    2015.10.05
   */
  //-----------------------------------------------------------------------------
  void OutNumCounter(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsNumCounterAnimeEnd
   * @brief   個数選択ウィンドウIN/OUTアニメ動作チェック
   * @date    2015.10.05
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsNumCounterAnimeEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetStrYes
   * @brief   メニュー項目文字列"はい"取得
   * @date    2015.10.06
   *
   * @return  文字列バッファ
   */
  //-----------------------------------------------------------------------------
  gfl2::str::StrBuf * GetStrYes(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetStrNo
   * @brief   メニュー項目文字列"いいえ"取得
   * @date    2015.10.06
   *
   * @return  文字列バッファ
   */
  //-----------------------------------------------------------------------------
  gfl2::str::StrBuf * GetStrNo(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveCursor
   * @brief   カーソル移動
   * @date    2015.10.06
   *
   * @param   pos     カーソル位置
   * @param   scroll  スクロール値
   */
  //-----------------------------------------------------------------------------
  void MoveCursor( s32 pos, s32 scroll );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMessageItemSet
   * @brief   "ポケットへしまった"メッセージ取得
   * @date    2015.10.07
   *
   * @param   item  道具番号
   *
   * @return  文字列バッファ
   */
  //-----------------------------------------------------------------------------
  gfl2::str::StrBuf * GetMessageItemSet( u32 item );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMessageHowMany
   * @brief   "いくつ？"メッセージ取得
   * @date    2015.10.07
   *
   * @param   id    メッセージID
   * @param   item  道具番号
   *
   * @return  文字列バッファ
   */
  //-----------------------------------------------------------------------------
  gfl2::str::StrBuf * GetMessageHowMany( u32 id, u32 item );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMessageReally
   * @brief   "xx円になりますが？"メッセージ取得
   * @date    2015.10.07
   *
   * @param   id    メッセージID
   * @param   item  道具番号
   * @param   num   個数
   * @param   gold  金額
   *
   * @return  文字列バッファ
   */
  //-----------------------------------------------------------------------------
  gfl2::str::StrBuf * GetMessageReally( u32 id, u32 item, u32 num, u32 gold );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMessageCoupon
   * @brief   クーポン使用メッセージ取得
   * @date    2016.01.09
   *
   * @param   id    メッセージID
   * @param   gold  金額
   *
   * @return  文字列バッファ
   */
  //-----------------------------------------------------------------------------
  gfl2::str::StrBuf * GetMessageCoupon( u32 id, u32 gold );

  //-----------------------------------------------------------------------------
  /**
   * @func    VanishMyItemNum
   * @brief   "もっているかず"を非表示にする
   * @date    2015.11.30
   */
  //-----------------------------------------------------------------------------
  void VanishMyItemNum(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleSelectedCursor
   * @brief   選択した項目カーソルの表示切り替え
   * @date    2016.01.20
   *
   * @param   flg   true = 表示, false = 非表示
   * @param   pos   カーソル位置
   */
  //-----------------------------------------------------------------------------
  void SetVisibleSelectedCursor( bool flg, u32 pos = 0 );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    GetGameData
   * @brief   GameData関連取得
   * @date    2015.10.05
   */
  //-----------------------------------------------------------------------------
  void GetGameData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2015.10.02
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.10.02
   *
   * @param   displayNo   描画面
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize2D
   * @brief   2D関連初期化
   * @date    2015.10.02
   *
   * @param   res_buf       レイアウトリソースバッファ
   * @param   msg_buf       メッセージリソースバッファ
   * @param   wset_loader   WordSetLoader
   */
  //-----------------------------------------------------------------------------
  void Initialize2D( void * res_buf, void * msg_buf, WordSetLoader * wset_loader );

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate2D
   * @brief   2D関連削除
   * @date    2015.10.02
   */
  //-----------------------------------------------------------------------------
  void Terminate2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuCursorLayout
   * @brief   メニューカーソル用レイアウト作成
   * @date    2015.10.06
   */
  //-----------------------------------------------------------------------------
  void CreateMenuCursorLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMenuCursorLayout
   * @brief   メニューカーソル用レイアウト削除
   * @date    2015.10.06
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuCursorLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutCursor
   * @brief   カーソル配置
   * @date    2015.10.06
   *
   * @param   pos   カーソル位置
   */
  //-----------------------------------------------------------------------------
  void PutCursor( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateScrollBar
   * @brief   スクロールバー生成
   * @date    2015.10.06
   */
  //-----------------------------------------------------------------------------
  void CreateScrollBar(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteScrollBar
   * @brief   スクロールバー削除
   * @date    2015.10.06
   */
  //-----------------------------------------------------------------------------
  void DeleteScrollBar(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetDefaultAnime
   * @brief   デフォルトアニメ再生
   * @date    2015.10.05
   */
  //-----------------------------------------------------------------------------
  void SetDefaultAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetAnimeMaxFrame
   * @brief   アニメ最大フレーム取得
   * @date    2015.10.05
   *
   * @param   id  アニメID
   *
   * @return  最大フレーム
   */
  //-----------------------------------------------------------------------------
  f32 GetAnimeMaxFrame( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteDefaultSrring
   * @brief   デフォルト文字列描画
   * @date    2015.10.05
   */
  //-----------------------------------------------------------------------------
  void WriteDefaultString(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutItemInfo
   * @brief   道具説明描画
   * @date    2015.10.05
   *
   * @param   item  道具番号
   */
  //-----------------------------------------------------------------------------
  void PutItemInfo( u32 item );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetItemNumStrAnime
   * @brief   持っている数表示変更アニメ開始
   * @date    2015.10.09
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsItemNumStrAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetItemIconRequest
   * @brief   道具アイコン変更リクエスト
   * @date    2015.10.05
   *
   * @param   item  道具番号
   */
  //-----------------------------------------------------------------------------
  void SetItemIconRequest( u32 item );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateItemIcon
   * @brief   道具アイコン更新
   * @date    2015.10.05
   */
  //-----------------------------------------------------------------------------
  void UpdateItemIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetNumCounterParts
   * @brief   個数選択の部品ペインを取得
   * @date    2015.10.05
   *
   * @return 部品ペイン
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytParts * GetNumCounterParts(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetStrMenu
   * @brief   メニュー項目文字列取得
   * @date    2015.10.06
   *
   * @param   id  メッセージID
   *
   * @return  文字列バッファ
   */
  //-----------------------------------------------------------------------------
  gfl2::str::StrBuf * GetStrMenu( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateCursorPosInfo
   * @brief   カーソル位置の情報を更新
   * @date    2015.10.10
   *
   * @param   pos   リスト位置
   */
  //-----------------------------------------------------------------------------
  void UpdateCursorPosInfo( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnKeyAction
   * @brief   キー入力の検知
   * @date    2015.10.02
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
  ShopType m_type;                                      //!< ショップタイプ
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ
  app::tool::MenuCursor * m_menuCursor;                 //!< メニューカーソル

  const SHOP_TYPE_DETA * m_shopData;    //!< ショップタイプデータ

  Savedata::MyItem * m_myItem;                    //!< 手持ちアイテムセーブデータ
  Savedata::JoinFestaDataSave * m_saveJoinFesta;  //!< ジョインフェスタセーブデータ

  ShopDrawListener * m_listener;   //!< リスナー

  gfl2::str::MsgData * m_pMsgData;  //!< メッセージデータ
  print::WordSet * m_pWordSet;      //!< 単語セット

  app::tool::ScrollBar * m_pScrollBar;  //!< スクロールバー

  const SHOP_ITEM * m_shopItem;   //!< 商品データ
  u32 m_itemTableMax;             //!< 商品データ数
  itemman::ITEM_MANAGER * m_itemManager;  //!< アイテムマネージャ
  print::ItemMessage * m_pItemMessage;    //!< 道具説明メッセージデータ

  app::tool::ItemIcon * m_itemIcon;   //!< アイテムアイコン
  u32 m_iconLoadID;                   //!< 読み込んだアイコンID
  u32 m_iconReqID;                    //!< リクエストしたアイコンID
  u32 m_iconSeq;                      //!< アイコンロードシーケンス

  f32 m_scroll;   //!< スクロール値

  u32 m_subSeq;         //!< サブシーケンス
  bool m_isDrawEnable;  //!< 描画許可フラグ
};


GFL_NAMESPACE_END(Shop)
GFL_NAMESPACE_END(App)

#endif  // SHOP_DRAW_H_INCLUDED
