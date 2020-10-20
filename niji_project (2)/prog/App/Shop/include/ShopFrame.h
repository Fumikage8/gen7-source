//=============================================================================
/**
 * @file    ShopFrame.h
 * @brief   ショップ購入画面フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2015.10.02
 */
//=============================================================================
#if !defined( SHOP_FRAME_H_INCLUDED )
#define SHOP_FRAME_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Str.h>
// pmlib
#include <pml/include/pmlib.h>
// niji
#include "Savedata/include/MiscSave.h"
#include "Savedata/include/MyItemSave.h"
#include "Savedata/include/JoinFestaDataSave.h"
#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Tool/app_tool_MenuWindowListener.h"
// module
#include "ShopDrawListener.h"


// 前方宣言
class WordSetLoader;

GFL_NAMESPACE_BEGIN(app)

GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)

GFL_NAMESPACE_BEGIN(tool)
class MenuCursor;
class MenuWindow;
class ItemIcon;
GFL_NAMESPACE_END(tool)

GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)
class TalkWindow;
GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(Field)
class EventWork;
GFL_NAMESPACE_END(Field)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Shop)

// 前方宣言
struct APP_PARAM;
struct SHOP_ITEM;
class ShopDraw;


//=============================================================================
/**
 * @class ShopFrame
 * @brief ショップ購入画面フレームクラス
 * @date  2015.10.02
 */
//=============================================================================
class ShopFrame
  : public applib::frame::CellSuper
  , public ShopDrawListener
  , public app::tool::MenuWindowListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ShopFrame);    // コピーコンストラクタ＋代入禁止

private:
  static const u32 ITEM_MANAGER_MAX = 32;   //!< アイテムマネージャに登録できる数 ( BPショップ最大が32 )

  static const u32 ITEM_BUY_MAX = 99;       //!< 一度に購入できる数

  //! 購入エラーID
  enum BuyErrorID
  {
    BUY_ERROR_NONE = 0,   //!< エラーなし
    BUY_ERROR_MAX,        //!< 持てない
    BUY_ERROR_NO_MONEY,   //!< 所持金不足
    BUY_ERRPR_SELECTED,   //!< 選択済み（フェスコイン用）
  };


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  ShopFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~ShopFrame();

  //-----------------------------------------------------------------------------
  /**
   * @func    Setup
   * @brief   パラメータセット
   * @date    2015.10.02
   *
   * @param   param       外部設定パラメータ
   * @parma   heap        ヒープ
   * @param   render_man  描画マネージャー
   */
  //-----------------------------------------------------------------------------
  void Setup( APP_PARAM * param, app::util::Heap * heap, app::util::AppRenderingManager * render_man );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFunc
   * @brief   フレーム初期化
   * @date    2015.10.02
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result InitFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFunc
   * @brief   フレーム終了
   * @date    2015.10.02
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result EndFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateFunc
   * @brief   フレーム更新
   * @date    2015.10.02
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   フレーム描画
   * @date    2015.10.02
   *
   * @param   no  描画先のディスプレイ番号
   */
  //-----------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );


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
   * @func    CreateShopTypeData
   * @brief   ショップタイプごとのデータを生成
   * @date    2015.10.08
   */
  //-----------------------------------------------------------------------------
  void CreateShopTypeData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteShopTypeData
   * @brief   ショップタイプごとのデータを削除
   * @date    2015.10.08
   */
  //-----------------------------------------------------------------------------
  void DeleteShopTypeData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateDraw
   * @brief   描画クラス生成
   * @date    2015.10.02
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateDraw(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteDraw
   * @brief   描画クラス削除
   * @date    2015.10.02
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteDraw(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadLayoutResource
   * @brief   レイアウトリソース読み込み
   * @date    2015.10.02
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UnloadLayourResource
   * @brief   レイアウトリソース削除
   * @date    2015.10.02
   */
  //-----------------------------------------------------------------------------
  void UnloadLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadMessage
   * @brief   メッセージ読み込み
   * @date    2015.10.05
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
   * @date    2015.10.05
   */
  //-----------------------------------------------------------------------------
  void UnloadMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateWordSetLoader
   * @brief   WordSetLoader生成
   * @date    2015.10.05
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateWordSetLoader(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteWordSetLoader
   * @brief   WordSetLoader削除
   * @date    2015.10.05
   */
  //-----------------------------------------------------------------------------
  void DeleteWordSetLoader(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateItemManager
   * @brief   アイテムマネージャ生成
   * @date    2015.10.05
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateItemManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteItemManager
   * @brief   アイテムマネージャ削除
   * @date    2015.10.05
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteItemManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateItemIcon
   * @brief   アイテムアイコン生成
   * @date    2015.10.05
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateItemIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteItemIcon
   * @brief   アイテムアイコン削除
   * @date    2015.10.05
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteItemIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuCursor
   * @brief   メニューカーソル生成
   * @date    2015.10.06
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
   * @date    2015.10.06
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuWindow
   * @brief   メニューウィンドウ作成
   * @date    2015.10.06
   */
  //-----------------------------------------------------------------------------
  void CreateMenuWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMenuWindow
   * @brief   メニューウィンドウ削除
   * @date    2015.10.06
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateTalkWindow
   * @brief   会話ウィンドウ生成
   * @date    2015.10.06
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateTalkWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteTalkWindow
   * @brief   会話ウィンドウ削除
   * @date    2015.10.06
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteTalkWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetEndParam
   * @brief   終了パラメータ設定
   * @date    2015.10.08
   */
  //-----------------------------------------------------------------------------
  void SetEndParam(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitListData
   * @brief   リストデータ初期化
   * @date    2015.10.06
   */
  //-----------------------------------------------------------------------------
  void InitListData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveCursor
   * @brief   カーソル移動
   * @date    2015.10.06
   *
   * @param   mv      移動値
   * @param   is_trg  true = トリガー入力
   */
  //-----------------------------------------------------------------------------
  void MoveCursor( s32 mv, bool is_trg );

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckBuyError
   * @brief   購入できるかをチェック
   * @date    2015.10.07
   *
   * @retval  BUY_ERROR_NONE     = エラーなし
   * @retval  BUY_ERROR_MAX      = 持てない
   * @retval  BUY_ERROR_NO_MONEY = 所持金不足
   * @retval  BUY_ERRPR_SELECTED = 選択済み（フェスコイン用）
   */
  //-----------------------------------------------------------------------------
  BuyErrorID CheckBuyError(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckGold
   * @brief   お金が足りるか
   * @date    2015.10.07
   *
   * @retval  true  = 足りる
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CheckGold(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    AddPremiereBall
   * @brief   プレミアボール追加処理
   * @date    2015.10.07
   *
   * @retval  true  = 追加した
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool AddPremiereBall(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateNumSelect
   * @brief   個数選択ウィンドウ更新
   * @date    2015.10.06
   *
   * @param   mv      個数増減値
   * @param   is_trg  true = トリガー入力
   */
  //-----------------------------------------------------------------------------
  void UpdateNumSelect( s32 mv, bool is_trg );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsCoupon
   * @brief   クーポンを持っているか
   * @date    2016.01.09
   *
   * @retval  true  = 持っている
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsCoupon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeSequence
   * @brief   シーケンス切り替え
   * @date    2015.10.07
   *
   * @param   seq   シーケンス
   */
  //-----------------------------------------------------------------------------
  void ChangeSequence( u32 seq );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeMessageSequence
   * @brief   メッセージ表示シーケンスへ切り替え
   * @date    2015.10.07
   *
   * @param   id        メッセージID
   * @param   str       文字列バッファ
   * @param   is_input  true = 入力で終了
   */
  //-----------------------------------------------------------------------------
  void ChangeMessageSequence( u32 id, gfl2::str::StrBuf * str, bool is_input );

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqNumSelect
   * @brief   メイン処理：個数選択
   * @date    2015.10.07
   */
  //-----------------------------------------------------------------------------
  void Update_SeqNumSelect(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqMessage
   * @brief   メイン処理：メッセージ表示
   * @date    2015.10.07
   */
  //-----------------------------------------------------------------------------
  void Update_SeqMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OnAction
   * @brief   入力時のリスナー
   * @date    2015.10.02
   *
   * @param   id    動作ID
   *
   * @note  override ShopDrawListener
   */
  //-----------------------------------------------------------------------------
  virtual void OnAction( ShopDrawListener::ActionID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    SelectMenu
   * @brief   項目選択時のリスナー
   * @date    2015.10.06
   *
   * @param   id  メニュー項目のID
   *
   * @note  override app::tool::MenuWindowListener
   */
  //-----------------------------------------------------------------------------
  virtual void SelectMenu( u32 id );


private:
  APP_PARAM * m_appParam;                                 //!< 外部設定パラメータ
  app::util::Heap * m_heap;                               //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;    //!< 描画マネージャ

  Savedata::Misc * m_saveMisc;                    //!< 未分類セーブデータ
  Savedata::MyItem * m_myItem;                    //!< 手持ちアイテムセーブデータ
  Savedata::JoinFestaDataSave * m_saveJoinFesta;  //!< ジョインフェスタセーブデータ
  const Field::EventWork * m_eventWork;           //!< イベントワーク

  void * m_pLytResBuff;               //!< レイアウトリソースバッファ
  void * m_pMsgBuff;                  //!< メッセージリソースバッファ
  WordSetLoader * m_pWordSetLoader;   //!< ワードセットローダー

  itemman::ITEM_MANAGER * m_pItemManager;   //!< アイテムマネージャ
  void * m_pItemMsgBuff;                    //!< 道具説明リソースバッファ

  app::tool::ItemIcon * m_pItemIcon;      //!< アイテムアイコン

  app::tool::MenuCursor * m_pMenuCursor;  //!< メニューカーソル
  void * m_pMenuCursorBuffer;             //!< メニューカーソルリソースバッファ

  ShopDraw * m_pDraw;   //!< 描画クラス

	app::tool::MenuWindow * m_pMenuWindow;  //!< メニューウィンドウ

  App::Tool::TalkWindow * m_pTalkWindow;  //!< 会話ウィンドウ
  void * m_pTalkWindowBuff;               //!< 会話ウィンドウリソースバッファ

  SHOP_ITEM * m_pShopItem;  //!< 商品データ
  u32 m_itemMax;            //!< 商品データ数
  u32 m_myGold;     //!< 所持金
  u32 m_cashBack;   //!< キャッシュバック額

  s16 m_cursorPos;        //!< カーソル位置
  s16 m_cursorScroll;     //!< スクロール値
  s16 m_cursorScrollMax;  //!< スクロール最大値

  s16 m_selectPos;  //!< 選択した項目位置
  s16 m_selectNum;  //!< 選択数
  s16 m_selectMax;  //!< 最大選択数

  u32 m_msgID;    //!< メッセージID

  u16 m_mainSeq;  //!< メインシーケンス
  u16 m_subSeq;   //!< サブシーケンス
};


GFL_NAMESPACE_END(Shop)
GFL_NAMESPACE_END(App)


#endif  // SHOP_FRAME_H_INCLUDED
