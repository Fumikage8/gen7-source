//=============================================================================
/**
 * @file    app_tool_MenuWindow.h
 * @brief   汎用メニューウィンドウ
 * @author  Hiroyuki Nakamura
 * @date    2015.03.10
 */
//=============================================================================
#if !defined( APP_TOOL_MENU_WINDOW_H_INCLUDED )
#define APP_TOOL_MENU_WINDOW_H_INCLUDED

#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Layout/include/gfl2_LytSys.h>

#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/ui/UIInputListener.h"
#include "Sound/include/Sound.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

// 前方宣言
class ButtonManager;
class MenuWindowListener;
class MenuCursor;


//=============================================================================
/**
 * @class MenuWindow  
 * @brief 汎用メニューウィンドウ
 * @date  2015.03.10
 */
//=============================================================================
class MenuWindow
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
public:
  //! 配置タイプ
  enum PutType
  {
    PUT_LT = 0, //!< 左上
    PUT_RT,     //!< 右上
    PUT_LD,     //!< 左下
    PUT_RD      //!< 右下
  };

  static const s32 DEFAULT_PUT_X;     //!< デフォルト表示X座標（左上(0,0)の画面座標）
  static const s32 DEFAULT_PUT_Y;     //!< デフォルト表示Y座標（左上(0,0)の画面座標）
  static const f32 DEFAULT_PUT_FX;    //!< デフォルト表示X座標
  static const f32 DEFAULT_PUT_FY;    //!< デフォルト表示Y座標

  static const s32 DEFAULT_PUT_X_UPPER;     //!< 上画面用デフォルト表示X座標（左上(0,0)の画面座標）
  static const s32 DEFAULT_PUT_Y_UPPER;     //!< 上画面用デフォルト表示Y座標（左上(0,0)の画面座標）
  static const f32 DEFAULT_PUT_FX_UPPER;    //!< 上画面用デフォルト表示X座標
  static const f32 DEFAULT_PUT_FY_UPPER;    //!< 上画面用デフォルト表示Y座標

private:
  static const u32 ITEM_MAX = 8;  //!< 項目最大数

  static const u32 SIZE_CALC_PANE_NUM = 4;                                //!< 可変サイズペイン数
  static const gfl2::lyt::LytPaneIndex SizeCalcPane[SIZE_CALC_PANE_NUM];  //!< 可変サイズペインテーブル

  static const u32 CursorAnimeTable[ITEM_MAX][2]; //!< カーソルアニメテーブル

  static const f32 YESNO_DEFAULT_PUT_FX;  //!< "はい/いいえ"用デフォルト表示X座標
  static const f32 YESNO_DEFAULT_PUT_FY;  //!< "はい/いいえ"用デフォルト表示Y座標

  static const u32 ITEM_STR_LENGTH = 18*3+1;  //!< 項目文字列長 ( 日本語18文字 * ローカライズ対応 + EOM )

  //! メインシーケンス
  enum MainSeq
  {
    SEQ_START = 0,  //!< 開始
    SEQ_CREATE,     //!< レイアウト生成
    SEQ_MAIN,       //!< メイン
  };

  //! 項目データ
  struct ItemData
  {
    gfl2::lyt::LytParts * parts;  //!< 部品ペイン
    gfl2::str::StrBuf * str;      //!< 文字列
    bool is_mark;                 //!< 戻るマークを表示するか
    bool is_b_button;             //!< Bボタンに関連付けるか
    u32  select_se_id;            //!< 選択時のSEID
  };


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap              ヒープ
   * @param   rendering_manager 描画マネージャ
   * @param   cur_buff          カーソルのリソースバッファ ※デフォルト = NULL
   * @param   disp              表示画面  ※デフォルト = 下画面
   */
  //-----------------------------------------------------------------------------
  MenuWindow(
    app::util::Heap * heap,
    app::util::AppRenderingManager * rendering_manager,
    void * cur_buff = NULL,
    app::util::G2DUtil::SETUP_DISPLAY_TYPE disp = app::util::G2DUtil::SETUP_LOWER );

  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap              ヒープ
   * @param   rendering_manager 描画マネージャ
   * @param   cur_buff          カーソルのリソースバッファ
   * @param   disp              表示画面
   * @param   menu_window_buff  メニューウィンドウのレイアウトリソースバッファ
   */
  //-----------------------------------------------------------------------------
  MenuWindow(
    app::util::Heap * heap,
    app::util::AppRenderingManager * rendering_manager,
    void * cur_buff,
    app::util::G2DUtil::SETUP_DISPLAY_TYPE disp,
    void * menu_window_buff );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~MenuWindow();

  //-----------------------------------------------------------------------------
  /**
   * @func    Create
   * @brief   レイアウトとボタン生成
   * @date    2015.03.12
   *
   * @retval  false = 生成中
   * @retval  true  = それ以外
   *
   * @note  UIViewのUpdateが回らない場合、削除前に回し続けてください
   */
  //-----------------------------------------------------------------------------
  bool Create(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsCreate
   * @brief   レイアウトとボタンの生成が完了したか
   * @date    2015.03.12
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsCreate(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsModuleFree
   * @brief   メニューウィンドウを破棄できるか
   * @date    2015.03.12
   *
   * @retval  true  = 可
   * @retval  false = 不可
   */
  //-----------------------------------------------------------------------------
  bool IsModuleFree(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetListener
   * @brief   リスナーをセット
   * @date    2015.03.12
   *
   * @param   セットするリスナー
   */
  //-----------------------------------------------------------------------------
  void SetListener( MenuWindowListener * listener );

  //-----------------------------------------------------------------------------
  /**
   * @func    AddItem
   * @brief   項目追加 ( MsgData版 )
   * @date    2015.03.12
   *
   * @param   msg           メッセージデータ
   * @param   id            文字列ID
   * @param   is_cancel     true = Bボタンを割り当てる　※デフォルト = false
   * @param   is_mark       true = 戻るマークを表示する ※デフォルト = false
   * @param   select_se_id  選択時のSEID                ※デフォルト = SMID_NULL デフォルトの場合は内部で設定値を優先
   */
  //-----------------------------------------------------------------------------
  void AddItem( gfl2::str::MsgData * msg, u32 id, bool is_cancel=false, bool is_mark=false, u32 select_se_id = SMID_NULL );

  //-----------------------------------------------------------------------------
  /**
   * @func    AddItem
   * @brief   項目追加 ( StrBuf版 )
   * @date    2015.03.12
   *
   * @param   str           文字列
   * @param   is_cancel     true = Bボタンを割り当てる　※デフォルト = false
   * @param   is_mark       true = 戻るマークを表示する ※デフォルト = false
   * @param   select_se_id  選択時のSEID                ※デフォルト = SMID_NULL デフォルトの場合は内部で設定値を優先
   */
  //-----------------------------------------------------------------------------
  void AddItem( const gfl2::str::StrBuf & str, bool is_cancel=false, bool is_mark=false, u32 select_se_id = SMID_NULL );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartMenuDisplay
   * @brief   メニュー処理開始 ( 画面座標(左上0,0)指定版 )
   * @date    2015.03.12
   *
   * @param   cursor_pos  初期カーソル位置
   * @param   type        配置タイプ  ※デフォルト = PUT_RD
   * @param   x           配置X座標   ※デフォルト = DEFAULT_PUT_X
   * @param   y           配置Y座標   ※デフォルト = DEFAULT_PUT_Y
   *
   * @retval  true  = 成功
   * @retval  false = 失敗
   */
  //-----------------------------------------------------------------------------
  bool StartMenuDisplay( u32 cursor_pos, PutType type=PUT_RD, s32 x=DEFAULT_PUT_X, s32 y=DEFAULT_PUT_Y );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartMenu
   * @brief   メニュー処理開始 ( 3D座標(中心0,0)指定版 )
   * @date    2015.03.12
   *
   * @param   cursor_pos  初期カーソル位置
   * @param   type        配置タイプ  ※デフォルト = PUT_RD
   * @param   x           配置X座標   ※デフォルト = DEFAULT_PUT_FX
   * @param   y           配置Y座標   ※デフォルト = DEFAULT_PUT_FY
   *
   * @retval  true  = 成功
   * @retval  false = 失敗
   */
  //-----------------------------------------------------------------------------
  bool StartMenu( u32 cursor_pos, PutType type=PUT_RD, f32 x=DEFAULT_PUT_FX, f32 y=DEFAULT_PUT_FY );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartYesNoMenu
   * @brief   "はい/いいえ"用メニュー処理開始
   * @date    2015.04.23
   *
   * @param   cursor_pos  初期カーソル位置
   *
   * @retval  true  = 成功
   * @retval  false = 失敗
   */
  //-----------------------------------------------------------------------------
  bool StartYesNoMenu( u32 cursor_pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    EndMenu
   * @brief   メニュー処理終了
   * @date    2015.03.12
   *
   * @param   clear   再度開始する場合にはfalseを指定しておく
   */
  //-----------------------------------------------------------------------------
  void EndMenu( bool clear = true );

  //-----------------------------------------------------------------------------
  /**
   * @func    RemoveSubView
   * @brief   親への登録を解除
   * @date    2015.03.12
   */
  //-----------------------------------------------------------------------------
  void RemoveSubView(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetCursorPos
   * @brief   カーソルの位置を取得
   * @date    2015.04.07
   *
   * @return  カーソル位置
   */
  //-----------------------------------------------------------------------------
  u32 GetCursorPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetInputMode
   * @brief   入力モード設定
   * @date    2016.01.26
   *
   * @param   mode  入力モード
   */
  //-----------------------------------------------------------------------------
  void SetInputMode( app::tool::ButtonManager::InputMode mode );

  //------------------------------------------------------------------
  /**
    *  @brief    AddされたItem数の取得
    */
  //------------------------------------------------------------------
  u32 GetMenuItemNum(void) const  {return m_max;}


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新処理
   * @date    2015.03.12
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画処理
   * @date    2015.03.12
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    Delete
   * @brief   レイアウトとボタンの削除
   * @date    2015.03.12
   */
  //-----------------------------------------------------------------------------
  void Delete(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateItemString
   * @brief   項目文字列生成
   * @date    2015.03.12
   */
  //-----------------------------------------------------------------------------
  void CreateItemString(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteItemString
   * @brief   項目文字列削除
   * @date    2015.03.12
   */
  //-----------------------------------------------------------------------------
  void DeleteItemString(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateLayout
   * @brief   レイアウト生成
   * @date    2015.03.12
   */
  //-----------------------------------------------------------------------------
  void CreateLaytout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateButtonManager
   * @brief   ボタンマネージャ生成
   * @date    2015.03.12
   */
  //-----------------------------------------------------------------------------
  void CreateButtonManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPane
   * @brief   部品からペインを取得
   * @date    2015.03.12
   *
   * @param   id    項目ID
   * @param   pane  取得するペイン
   *
   * @return  指定されたペイン
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytPane * GetPane( u32 id, gfl2::lyt::LytPaneIndex pane );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetTextBoxPane
   * @brief   部品から項目を書き込むテキストボックスペインを取得
   * @date    2015.03.12
   *
   * @param   id    項目ID
   *
   * @return  テクストボックスペイン
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytTextBox * GetTextBoxPane( u32 id  );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitData
   * @brief   データ初期化
   * @date    2015.03.12
   */
  //-----------------------------------------------------------------------------
  void InitData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CalcItemWidth
   * @brief   項目の幅を計算
   * @date    2015.03.12
   */
  //-----------------------------------------------------------------------------
  void CalcItemWidth(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetItemParam
   * @brief   項目のパラメータを設定
   * @date    2015.03.12
   */
  //-----------------------------------------------------------------------------
  void SetItemParam(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPutPos
   * @brief   メニューの配置座標を取得
   * @date    2015.03.12
   *
   * @param   type    配置タイプ
   * @param   px      配置基準X座標
   * @param   py      配置基準Y座標
   * @param   height  １項目の高さ
   * @param   pos     座標格納場所
   */
  //-----------------------------------------------------------------------------
  void GetPutPos( PutType type, f32 px, f32 py, f32 height, gfl2::math::VEC3 * pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutCursor
   * @brief   カーソル配置
   * @date    2015.03.12
   *
   * @param   pos   カーソル位置
   */
  //-----------------------------------------------------------------------------
  void PutCursor( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuCursor
   * @brief   カーソルレイアウト作成
   * @date    2015.03.25
   */
  //-----------------------------------------------------------------------------
  void CreateMenuCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMenuCursor
   * @brief   カーソルレイアウト削除
   * @date    2015.03.25
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetDispType
   * @brief   gflのディスプレイタイプを取得
   * @date    2015.03.25
   *
   * @return  ディスプレイタイプ
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::DisplayType GetDispType(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveCursor
   * @brief   カーソル移動
   * @date    2015.10.10
   *
   * @param   mv      移動値
   * @param   is_trg  true = トリガー入力
   */
  //-----------------------------------------------------------------------------
  void MoveCursor( s32 mv, bool is_trg );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneTouchTriggerEvent
   * @brief   ペインタッチ時を検知
   * @date    2015.03.10
   *
   * @param   button_id   選択されたボタンID
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual void OnLayoutPaneTouchTriggerEvent( const u32 button_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneEvent
   * @brief   ペインイベントの検知
   * @date    2015.03.10
   *
   * @param   button_id   選択されたボタンID
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
   * @date    2015.03.10
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
  app::util::G2DUtil::SETUP_DISPLAY_TYPE m_disp;        //!< 表示画面
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ
  void * m_pLytResBuff;                                 //!< レイアウトリソースバッファ
  f32 m_width;                                          //!< 項目の幅

  nw::lyt::Size m_paneSize[SIZE_CALC_PANE_NUM];   //!< 可変サイズペインのサイズ保存場所
  f32 m_defTextWidth;                             //!< テキストボックスペインの初期幅
  f32 m_markWidth;                                //!< 戻るボタンの幅

  bool m_isCreate;    //!< レイアウトの生成が終了したか
  bool m_isCreateSelf;//!< レイアウトの生成を自分で行ったか
  bool m_isEnable;    //!< メニューが有効か

  gfl2::lyt::LytWk * m_lytwk;       //!< レイアウトワーク
  gfl2::lyt::LytMultiResID m_resID; //!< リソースID

  app::tool::MenuCursor * m_pMenuCursor;  //!< メニューカーソル
  void * m_pCursorResBuff;                //!< カーソルリソースバッファ

  ItemData m_item[ITEM_MAX];  //!< 項目データ

  MenuWindowListener * m_listener;  //!< メニューリスナー

  u8 m_max;       //!< 項目登録数
  s8 m_cursorPos; //!< カーソル位置

  u8 m_mainSeq;   //!< メインシーケンス
  u8 m_subSeq;    //!< サブシーケンス



#if PM_DEBUG
private:
  bool m_debugYesNoInvalid;
public:
  //-----------------------------------------------------------------------------
  /**
   * @brief "はい/いいえ"の座標チェックのアサートを無効化する
   *
   * @note  無断使用禁止。使用する場合は中村まで。
   */
  //-----------------------------------------------------------------------------
  void Debug_YesNoPosAssertInvalid(void);
#endif

};


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif  // APP_TOOL_MENU_WINDOW_H_INCLUDED
