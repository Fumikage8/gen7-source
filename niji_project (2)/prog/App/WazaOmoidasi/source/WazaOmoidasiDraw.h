//=============================================================================
/**
 * @file    WazaOmoidasiDraw.h
 * @brief   技思い出し画面描画処理
 * @author  Hiroyuki Nakamura
 * @date    2015.12.08
 */
//=============================================================================
#if !defined( WAZA_OMOIDASI_DRAW_H_INCLUDED )
#define WAZA_OMOIDASI_DRAW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "AppLib/include/Tool/app_tool_PaneList.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class PokeIcon;
class AppCommonGrpIconData;
class MenuCursor;
class PaneListView;
class StatusRaderChart;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(PokeTool)
class RemaindSkill;
GFL_NAMESPACE_END(PokeTool)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(WazaOmoidasi)

//=============================================================================
/**
 * @class WazaOmoidasiDraw
 * @brief 技思い出し画面描画クラス
 * @date  2015.12.10
 */
//=============================================================================
class WazaOmoidasiDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
  , public app::tool::PaneList::CallBack
  , public print::MsgCallback
{
public:
  //=============================================================================
  /**
   * @class InputListener
   * @brief 技思い出し画面リスナークラス
   * @date  2015.12.10
   */
  //=============================================================================
  class InputListener
  {
  public:
    //! 動作ID
    enum ActionID
    {
      ACTION_ID_RETURN = 0,       //!< 戻る
    };

  public:
    //-----------------------------------------------------------------------------
    /**
     * @func    OnAction
     * @brief   入力時のリスナー
     * @date    2015.12.10
     *
     * @param   id    動作ID
     */
    //-----------------------------------------------------------------------------
    virtual void OnAction( ActionID id ) = 0;

    //-----------------------------------------------------------------------------
    /**
     * @func    SetWazaNo
     * @brief   技をセット
     * @date    2015.12.26
     */
    //-----------------------------------------------------------------------------
    virtual void SetWazaNo(void) = 0;
  };

  //! メッセージID
  enum MsgID
  {
    MSGID_CANCEL = 0,       //!< あきらめますか？
    MSGID_SELECT,           //!< おもいだしますか？
    MSGID_DECIDE,           //!< おもいだした
    MSGID_CHANGE_START,     //!< わすれるわざを　えらんでください
    MSDID_CHANGE_DECIDE,    //!< わすれて　おもいだした
  };


private:
  //! レイアウトID
  enum LyoutID
  {
    LYTID_UPPER = 0,  //!< 上画面
    LYTID_LOWER,      //!< 下画面
    LYTID_CURSOR,     //!< カーソル
    LYTID_MAX         //!< 最大数
  };

  //! ボタンID
  enum ButtonID
  {
    BTNID_RETURN = 0,   //!< 戻るボタン
    BTNID_MAX           //!< 最大数
  };

  static const u32 LIST_PANE_MAX = 7;         //!< ペインリスト項目数
  static const u32 LIST_CURSOR_MOVE_MAX = 4;  //!< ペインリストカーソル動作範囲

  static const f32 LIST_ITEM_PX;  //!< リスト項目X座標
  static const f32 LIST_ITEM_PY;  //!< リスト項目Y座標
  static const f32 LIST_ITEM_SX;  //!< リスト項目Xサイズ
  static const f32 LIST_ITEM_SY;  //!< リスト項目Xサイズ

  static const gfl2::lyt::LytPaneIndex ListPaneTable[LIST_PANE_MAX];  //!< 技リストペインテーブル


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap        ヒープ
   * @param   render_man  描画マネージャ
   * @param   pp          対象ポケモン
   */
  //-----------------------------------------------------------------------------
  WazaOmoidasiDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man, pml::pokepara::PokemonParam * pp );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~WazaOmoidasiDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetListener
   * @brief   リスナーをセット
   * @date    2015.12.10
   *
   * @param   listener  リスナー
   */
  //-----------------------------------------------------------------------------
  void SetListener( InputListener * listener );

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   初期化処理
   * @date    2015.12.10
   *
   * @param   res_buf           レイアウトリソースバッファ
   * @param   msg_buf           メッセージリソースバッファ
   * @param   msg_tokusei_buf   特性説明メッセージリソースバッファ
   * @param   msg_waza_buf      技説明メッセージリソースバッファ
   * @param   wset_loader       WordSetLoader
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Init( void * res_buf, void * msg_buf, void * msg_tokusei_buf, void * msg_waza_buf, WordSetLoader * wset_loader );

  //-----------------------------------------------------------------------------
  /**
   * @func    End
   * @brief   終了処理
   * @date    2015.12.10
   *
   * @retval  true  = 処理完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool End(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuCursorLayout
   * @brief   メニューカーソル用レイアウト作成
   * @date    2015.12.12
   *
   * @param   cursor  メニューカーソル
   */
  //-----------------------------------------------------------------------------
  void CreateMenuCursorLayout( app::tool::MenuCursor * cursor );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMenuCursorLayout
   * @brief   メニューカーソル用レイアウト削除
   * @date    2015.12.12
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuCursorLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Start
   * @brief   開始処理
   * @date    2015.12.10
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Start(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsStart
   * @brief   開始処理が終了したかをチェック
   * @date    2015.12.10
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsStart(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutPokeWaza
   * @brief   技表示
   * @date    2015.12.26
   *
   * @param   pos   位置
   */
  //-----------------------------------------------------------------------------
  void PutPokeWaza( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetWazaNo
   * @brief   リストから技を取得
   * @date    2015.12.14
   *
   * @return  技番号
   */
  //-----------------------------------------------------------------------------
  WazaNo GetWazaNo( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleMessage
   * @brief   メッセージ表示部分の表示切り替え
   * @date    2015.12.14
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleMessage( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartMessage
   * @brief   メッセージ処理開始
   * @date    2015.12.14
   */
  //-----------------------------------------------------------------------------
  void StartMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndMessage
   * @brief   メッセージ処理終了
   * @date    2015.12.14
   */
  //-----------------------------------------------------------------------------
  void EndMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsMessageWindowAnimeEnd
   * @brief   メッセージウィンドウのアニメ終了チェック
   * @date    2015.12.14
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsMessageWindowAnimeEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMessage
   * @brief   メッセージセット
   * @date    2015.12.14
   *
   * @param   id        メッセージID
   * @param   set_waza  思い出す技
   * @param   del_waza  忘れる技
   */
  //-----------------------------------------------------------------------------
  void SetMessage( MsgID id, WazaNo set_waza, WazaNo del_waza );

  //-----------------------------------------------------------------------------
  /**
   * @func    PrintMessage
   * @brief   メッセージプリント
   * @date    2015.12.14
   *
   * @retval  true  = 処理中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool PrintMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetString
   * @brief   文字列取得
   * @date    2015.12.14
   *
   * @param   id  メッセージID
   *
   * @return  文字列
   */
  //-----------------------------------------------------------------------------
  gfl2::str::StrBuf * GetString( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutListWazaInfo
   * @brief   カーソル位置の技情報を表示
   * @date    2016.01.28
   *
   * @param   pos   カーソル位置
   */
  //-----------------------------------------------------------------------------
  void PutListWazaInfo( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPaneList
   * @brief   ペインリスト初期化
   * @date    2015.12.12
   *
   * @param   view        リストビュークラス
   * @param   list_pos    カーソル位置
   * @parma   list_scroll スクロール値
   */
  //-----------------------------------------------------------------------------
  void InitPaneList( app::tool::PaneListView * view, u16 list_pos, u16 list_scroll );

  //-----------------------------------------------------------------------------
  /**
   * @func    EndPaneList
   * @brief   ペインリスト終了
   * @date    2015.12.12
   *
   * @param   view    リストビュークラス
   */
  //-----------------------------------------------------------------------------
  void EndPaneList( app::tool::PaneListView * view );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2015.12.10
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.12.10
   *
   * @param   displayNo   描画面
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize2D
   * @brief   2D関連初期化
   * @date    2015.12.10
   *
   * @param   res_buf           レイアウトリソースバッファ
   * @param   msg_buf           メッセージリソースバッファ
   * @param   msg_tokusei_buf   特性説明メッセージリソースバッファ
   * @param   msg_waza_buf      技説明メッセージリソースバッファ
   * @param   wset_loader       WordSetLoader
   */
  //-----------------------------------------------------------------------------
  void Initialize2D( void * res_buf, void * msg_buf, void * msg_tokusei_buf, void * msg_waza_buf, WordSetLoader * wset_loader );

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate2D
   * @brief   2D関連削除
   * @date    2015.12.10
   */
  //-----------------------------------------------------------------------------
  void Terminate2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteDefaultSrring
   * @brief   デフォルト文字列描画
   * @date    2015.12.10
   */
  //-----------------------------------------------------------------------------
  void WriteDefaultString(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateButton
   * @brief   ボタン生成
   * @date    2015.12.10
   */
  //-----------------------------------------------------------------------------
  void CreateButton(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutPokeInfo
   * @brief   ポケモン情報表示
   * @date    2015.12.11
   */
  //-----------------------------------------------------------------------------
  void PutPokeInfo(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutRaderChart
   * @brief   レーダーチャート表示
   * @date    2016.03.17
   */
  //-----------------------------------------------------------------------------
  void PutRaderChart(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetUpperWazaColor
   * @brief   上画面の技の背景色を変更
   * @date    2015.12.11
   *
   * @param   id    項目ID
   * @param   waza  技番号
   */
  //-----------------------------------------------------------------------------
  void SetUpperWazaColor( u32 id, WazaNo waza );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetLowerWazaColor
   * @brief   下画面の技の背景色を変更
   * @date    2015.12.12
   *
   * @param   id    項目ID
   * @param   waza  技番号
   */
  //-----------------------------------------------------------------------------
  void SetLowerWazaColor( u32 id, WazaNo waza );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateCommonIcon
   * @brief   汎用アイコン生成
   * @date    2015.12.11
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
   * @date    2015.12.11
   */
  //-----------------------------------------------------------------------------
  void DeleteCommonIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePokeIcon
   * @brief   ポケモンアイコン生成
   * @date    2015.12.11
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreatePokeIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeletePokeIcon
   * @brief   ポケモンアイコン削除
   * @date    2015.12.11
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeletePokeIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdatePokeIcon
   * @brief   ポケモンアイコン更新
   * @date    2015.12.11
   */
  //-----------------------------------------------------------------------------
  void UpdatePokeIcon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateWazaList
   * @brief   技リスト生成
   * @date    2015.12.12
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateWazaList(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteWazaList
   * @brief   技リスト削除
   * @date    2015.12.12
   */
  //-----------------------------------------------------------------------------
  void DeleteWazaList(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMessageWindow
   * @brief   メッセージウィンドウ生成
   * @date    2015.12.14
   */
  //-----------------------------------------------------------------------------
  void CreateMessageWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMessageWindow
   * @brief   メッセージウィンドウ削除
   * @date    2015.12.14
   */
  //-----------------------------------------------------------------------------
  void DeleteMessageWindow(void);

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
   * @date    2015.12.26
   *
   * @param   se  SE番号
   */
  //-----------------------------------------------------------------------------
  void SetMsgCallBackSE( u32 se );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMsgCallBackME
   * @brief   メッセージ表示コールバック中のME再生
   * @date    2015.12.26
   *
   * @param   me  ME番号
   */
  //-----------------------------------------------------------------------------
  void SetMsgCallBackME( u32 me );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateRaderChart
   * @brief   レーダーチャート生成
   * @date    2015.12.15
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateRaderChart(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteRaderChart
   * @brief   レーダーチャート削除
   * @date    2015.12.15
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteRaderChart(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPaneListParam
   * @brief   ペインリスト初期化パラメータ設定
   * @date    2015.12.12
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
   * @date    2015.12.12
   *
   * @param   list    ペインリスト
   * @param   setup   ペインリストのセットアップパラメータ
   */
  //-----------------------------------------------------------------------------
  void SetPaneListConfig( app::tool::PaneList * list, app::tool::PaneList::SETUP_PARAM * setup );

  //-----------------------------------------------------------------------------
  /**
   * @func    CallBack_CursorMove
   * @brief   カーソル移動時のコールバック関数
   * @date    2015.12.12
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
   * @func    CallBack_ListWrite
   * @brief		リストの項目を描画するコールバック関数
   * @date    2015.12.12
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
   * @func    CallBack_CursorOff
   * @brief		カーソルを非表示にするコールバック関数
   * @date    2015.12.12
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
   * @func    CallBack_ItemChange
   * @brief		入れ替え用コールバック関数
   * @date    2015.12.12
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
   * @func    OnLayoutPaneEvent
   * @brief   ペインイベントの検知
   * @date    2015.12.10
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
   * @date    2015.12.10
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
  pml::pokepara::PokemonParam * m_pp;                   //!< PokemonParam

  gfl2::str::MsgData * m_pMsgData;          //!< メッセージデータ
  gfl2::str::MsgData * m_pTokuseiMsgData;   //!< 特性説明メッセージデータ
  gfl2::str::MsgData * m_pWazaMsgData;      //!< 技説明メッセージデータ
  print::WordSet * m_pWordSet;              //!< 単語セット

  gfl2::str::StrWin::FinishType m_msgFinishType;  //!< メッセージ終了タイプ
  u32 m_msgCallBackWrok;                          //!< メッセージコールバック用ワーク

  app::tool::PokeIcon * m_pPokeIcon;  //!< ポケモンアイコン
  bool m_isLoadPokeIcon;              //!< ポケモンアイコンを読み込んだか

  app::tool::AppCommonGrpIconData * m_pCommonIcon;  //!< 汎用アイコン

  app::tool::MenuCursor * m_menuCursor;             //!< メニューカーソル

  app::tool::PaneList::PANE_DATA * m_pPaneListData; //!< ペインリストデータ

  PokeTool::RemaindSkill * m_pRemaindSkill;   //!< 技リストデータ

  app::tool::StatusRaderChart * m_pRaderChart;  //!< レーダーチャート
  bool m_isRaderChartUnset;   //!< レーダーチャートを消したか

  InputListener * m_listener;   //!< リスナー

  u16 m_mainSeq;        //!< メインシーケンス
  u16 m_subSeq;         //!< サブシーケンス
  bool m_isDrawEnable;  //!< 描画フラグ

  u32 m_statusMode;   //!< レーダーチャートの表示モード
};


GFL_NAMESPACE_END(WazaOmoidasi)
GFL_NAMESPACE_END(App)

#endif  // WAZA_OMOIDASI_DRAW_H_INCLUDED
