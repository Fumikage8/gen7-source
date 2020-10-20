#ifndef NIJI_PROJECT_APP_STRINPUT_GRAPHIC_H_INCLUDED
#define NIJI_PROJECT_APP_STRINPUT_GRAPHIC_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    StrInputGraphic.h
 * @brief   グラフィック関連
 * @author  fukushima_yuya
 * @date    2015.05.22
 * @note    sango_project app strinput_graphic.hから移植
 */
//==============================================================================

// niji
#include <AppLib/include/Tool/PokeIcon.h>
//#include <AppLib/include/Tool/app_tool_TimeIcon.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <System/include/GflUse.h>


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace  App::StrInput
 * @brief      文字入力画面
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( StrInput )


//------------------------------------------------------------------------------
/**
 * @brief   グラフィックワーク
 */
//------------------------------------------------------------------------------
class GraphicWork
{
  GFL_FORBID_COPY_AND_ASSIGN( GraphicWork );

public:
  //! @brief  レイアウトID
  enum {
    LAYOUT_ID_UPPER = 0,        //!< 上画面
    LAYOUT_ID_LOWER,            //!< 下画面
    LAYOUT_ID_LOWER_KEYBOARD,   //!< 下画面：キーボード
    LAYOUT_ID_LOWER_INPUT,      //!< 下画面：入力エリア
    LAYOUT_ID_LOWER_CHANGE,     //!< 下画面：ページ切り替え
    LAYOUT_ID_MAX               //!< レイアウトID最大数定義
  };

  //! @brief  ボタンアニメの並び順
  enum {
    BTNANM_TOUCH = 0,     //!< タッチ
    BTNANM_RELEASE,       //!< 選択
    BTNANM_CANCEL,        //!< キャンセル
    BTNANM_PASSIVE,       //!< パッシブ
    BTNANM_ACTIVE,        //!< アクティブ
    BTNANM_TOUCH_RELEASE, //!< タッチリリース
    BTNANM_MAX            //!< 並び順最大数定義
  };

  //! @brief  アニメID
  enum {
    // 入力フィールド用
    ANMID_INPUT_TYPE_12 = 0,
    ANMID_INPUT_TYPE_16,

    ANMID_INPUT_BAR,
    ANMID_INPUT_BAR_UP = 0,
    ANMID_INPUT_BAR_DOWN = 1,
    ANMID_INPUT_BAR_PANE_MAX = 18,
    ANMID_INPUT_BAR_PAT_MAX  = 2,
    ANMID_INPUT_BAR_MAX = ANMID_INPUT_BAR_PANE_MAX * ANMID_INPUT_BAR_PAT_MAX,

    ANMID_INPUT_ICON = ANMID_INPUT_BAR_MAX + ANMID_INPUT_BAR,
    ANMID_INPUT_ICON_TOUCH = ANMID_INPUT_ICON,
    ANMID_INPUT_ICON_RELEASE,
    ANMID_INPUT_ICON_CANSEL,
    ANMID_INPUT_ICON_PASSIVE,
    ANMID_INPUT_ICON_ACTIVE,
    ANMID_INPUT_ICON_TOUCH_RELEASE,

    ANMID_INPUT_ICON_GENDER_MALE,
    ANMID_INPUT_ICON_GENDER_FEMALE,

    ANMID_INPUT_MAX,

    //ANMID_INPUT_ICON_TOUCH = 0,
    //ANMID_INPUT_ICON_RELEASE,
    //ANMID_INPUT_ICON_CANCEL,
    //ANMID_INPUT_ICON_PASSIVE,
    //ANMID_INPUT_ICON_ACTIVE,
    //ANMID_INPUT_TYPE_12,
    //ANMID_INPUT_TYPE_16,
    //ANMID_INPUT_BAR_UP,
    //ANMID_INPUT_BAR_DOWN,

    //ANMID_INPUT_BAR_SHARE,  //!< ここからバーの共有アニメ
    //ANMID_INPUT_BAR_SHARE_PANE_MAX = 15,
    //ANMID_INPUT_BAR_SHARE_PAT_MAX = 2,
    //ANMID_INPUT_BAR_SHARE_MAX = ANMID_INPUT_BAR_SHARE_PANE_MAX * ANMID_INPUT_BAR_SHARE_PAT_MAX,

    //ANMID_INPUT_ICON_SHARE = ANMID_INPUT_BAR_SHARE + ANMID_INPUT_BAR_SHARE_MAX,   //!< ここからアイコンの共有アニメ
    //ANMID_INPUT_ICON_SHARE_MAX = BTNANM_MAX,

    //ANMID_INPUT_SHARE_MAX = ANMID_INPUT_BAR_SHARE_MAX + ANMID_INPUT_ICON_SHARE_MAX,

    //ANMID_INPUT_MAX = ANMID_INPUT_BAR_SHARE + ANMID_INPUT_SHARE_MAX,  //!< 入力エリアアニメ最大数

    // キーマップ切り替え用
    ANMID_CHANGE_ARROW_TOUCH = 0,
    ANMID_CHANGE_ARROW_RELEASE,
    ANMID_CHANGE_ARROW_CANSEL,
    ANMID_CHANGE_ARROW_PASSIVE,
    ANMID_CHANGE_ARROW_ACTIVE,
    ANMID_CHANGE_ARROW_TOUCH_RELEASE,
    ANMID_CHANGE_MAX,

    //ANMID_CHANGE_ARROW_LOOP = 0,
    //ANMID_CHANGE_ARROW_TOUCH,
    //ANMID_CHANGE_ARROW_RELEASE,
    //ANMID_CHANGE_ARROW_CANCEL,
    //ANMID_CHANGE_ARROW_PASSIVE,
    //ANMID_CHANGE_ARROW_ACTIVE,
    //ANMID_CHANGE_ARROW_SELECT,

    //ANMID_CHANGE_SHARE,   //!< ここから共有アニメ
    //ANMID_CHANGE_SHARE_PANE_MAX = 2,    //!< 共有アニメを適応するペインの数
    //ANMID_CHANGE_SHARE_MAX = ANMID_CHANGE_SHARE_PANE_MAX * (BTNANM_MAX + 1),  //!< 共有アニメ数
    //ANMID_CHANGE_ARROW_MAX = ANMID_CHANGE_SHARE_MAX + ANMID_CHANGE_SHARE,     //!< +ダミー

    // キーマップ(50音)用
    ANMID_KEYMAP_N = 0,
    ANMID_KEYMAP_N_WORD_MAX = 13 * 5,
    ANMID_KEYMAP_N_PANE_MAX = ANMID_KEYMAP_N_WORD_MAX + 6, //!< 50音 + 6ボタン(かな・カナなど)

    ANMID_KEYMAP_N_TOUCH = 0,
    ANMID_KEYMAP_N_RELEASE,
    ANMID_KEYMAP_N_CANCEL,
    ANMID_KEYMAP_N_PASSIVE,
    ANMID_KEYMAP_N_ACTIVE,
    ANMID_KEYMAP_N_TOUCH_RELEASE,
    ANMID_KEYMAP_N_PAT_MAX,

    ANMID_KEYMAP_N_CURSOR = ANMID_KEYMAP_N_WORD_MAX * ANMID_KEYMAP_N_PAT_MAX + ANMID_KEYMAP_N,

    //ANMID_KEYMAP_N_TOUCH = 0,
    //ANMID_KEYMAP_N_RELEASE,
    //ANMID_KEYMAP_N_CANCEL,
    //ANMID_KEYMAP_N_PASSIVE,
    //ANMID_KEYMAP_N_ACTIVE,
    //ANMID_KEYMAP_N_KEY_SELECT,
    //ANMID_KEYMAP_N_CURSOR,

    //ANMID_KEYMAP_N_SHARE,   //!< ここから共有アニメ
    //ANMID_KEYMAP_N_SHARE_PANE_MAX = 13*5+6,   //!< 共有アニメを適応するペインの数
    //ANMID_KEYMAP_N_SHARE_MAX = ANMID_KEYMAP_N_SHARE_PANE_MAX * (BTNANM_MAX+1),  //!< 共有アニメ数
    //ANMID_KEYMAP_N_MAX = ANMID_KEYMAP_N_SHARE_MAX + ANMID_KEYMAP_N_SHARE,       //!< +ダミー

    // キーマップ(キーボード)用
    ANMID_KEYMAP_B = 0,
    ANMID_KEYMAP_B_WORD_MAX = 11+10+12+12,
    ANMID_KEYMAP_B_PANE_MAX = ANMID_KEYMAP_B_WORD_MAX+1+1+5,

    ANMID_KEYMAP_B_TOUCH = 0,
    ANMID_KEYMAP_B_RELEASE,
    ANMID_KEYMAP_B_CANCEL,
    ANMID_KEYMAP_B_PASSIVE,
    ANMID_KEYMAP_B_ACTIVE,
    ANMID_KEYMAP_B_TOUCH_RELEASE,
    ANMID_KEYMAP_B_PAT_MAX,

    ANMID_KEYMAP_B_CURSOR = ANMID_KEYMAP_B_WORD_MAX * ANMID_KEYMAP_B_PAT_MAX + ANMID_KEYMAP_B,
    //ANMID_KEYMAP_B_TOUCH = 0,
    //ANMID_KEYMAP_B_RELEASE,
    //ANMID_KEYMAP_B_CANCEL,
    //ANMID_KEYMAP_B_PASSIVE,
    //ANMID_KEYMAP_B_ACTIVE,
    //ANMID_KEYMAP_B_KEY_SELECT,
    //ANMID_KEYMAP_B_CURSOR,

    //ANMID_KEYMAP_B_SHARE,   //!< ここから共有アニメ
    //ANMID_KEYMAP_B_SHARE_PANE_MAX = 12+11+12+12+5,    //!< 共有アニメを適応するペインの数
    //ANMID_KEYMAP_B_SHARE_MAX = ANMID_KEYMAP_B_SHARE_PANE_MAX * (BTNANM_MAX+1),  //!< 共有アニメ数
    //ANMID_KEYMAP_B_MAX = ANMID_KEYMAP_B_SHARE_MAX + ANMID_KEYMAP_B_SHARE        //!< +ダミー
  };

  // 入力文字列カラーモード
  enum {
    INPUT_COLOR_MODE_NORMAL = 0,
    INPUT_COLOR_MODE_ROMAJI
  };

  //! エラーID
  enum {
    ERR_ID_NG_WORD = 0,     //!< NGワード
    ERR_ID_NUMBER,          //!< 数字入力数オーバー
    ERR_ID_FAST,            //!< 連打入力
    ERR_ID_CHARNAME_NULL,   //!< 空欄（名前入力時）
    ERR_ID_NICKNAME_NULL,   //!< 空欄（ニックネーム入力時）
    ERR_ID_INTERVIEW_NULL   //!< 空欄（インタビュー入力時）
  };

public:
  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   heap  ヒープ
   * @param   pFileManager  ファイルマネージャ
   * @param   pRenderingManager   レンダリングマネージャ
   * @param   pAppLytBuff         レイアウトバッファ
   * @param   pTexLytBuff         テクスチャバッファ
   * @param   isInitApplicationMemory アプリケーションメモリを割り当て直すならtrueを指定
   * @param   isFieldSubProc
   */
  //----------------------------------------------------------------------------
  GraphicWork( app::util::Heap * heap, gfl2::fs::AsyncFileManager * pFileManager,
    app::util::AppRenderingManager * pRenderingManager, void * pAppLytBuff, void * pTexLytBuff, bool isInitApplicationMemory, bool isFieldSubProc );

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  ~GraphicWork( void );

public:
  //----------------------------------------------------------------------------
  /**
   * @brief   アップデート
   *
   * @param   none
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  virtual void Update( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   描画
   *
   * @param   none
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  //----------------------------------------------------------------------------
  /**
   * @brief   レイアウトワークの取得
   *
   * @param   id    レイアウトID
   *
   * @return  レイアウトワーク
   */
  //----------------------------------------------------------------------------
  gfl2::lyt::LytWk* GetLayoutWork( u32 id );

  //----------------------------------------------------------------------------
  /**
   * @brief   レイアウトリソースIDの取得
   *
   * @param   id    レイアウトID
   *
   * @return  レイアウトリソースID
   */
  //----------------------------------------------------------------------------
  gfl2::lyt::LytMultiResID GetLayoutResourceID( u32 id );

  //----------------------------------------------------------------------------
  /**
   * @brief   デフォルト文字列描画
   *
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void PutDefaultString( app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   文字コード描画
   *
   * @param   id    レイアウトID
   * @param   code  文字コード
   * @param   pane  ペイン
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void PutStrCode( u32 lytID, const gfl2::str::STRCODE * code, gfl2::lyt::LytPaneIndex pane, app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   文字コード描画
   *
   * @param   id    レイアウトID
   * @param   code  文字コード
   * @param   parts パーツペイン
   * @param   pane  ペイン
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void PutStrCode( u32 lytID, const gfl2::str::STRCODE * code, gfl2::lyt::LytPaneIndex parts, gfl2::lyt::LytPaneIndex pane, app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   入力フィールド表示初期化
   *
   * @param   none
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void InitInputField( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   入力文字描画
   *
   * @param   lytID レイアウトID
   * @param   code  文字コード
   * @param   index 入力位置
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void PutInputStr( u32 lytID, const gfl2::str::STRCODE * code, u32 index, app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   入力文字を消す
   *
   * @param   lytID レイアウトID
   * @param   index 入力位置
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void ClearInputStr( u32 lytID, u32 index );

  //----------------------------------------------------------------------------
  /**
   * @brief   入力文字色を変更
   *
   * @param   lytID レイアウトID
   * @param   index 入力位置
   * @param   mode  色モード
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void SetInputStrColor( u32 lytID, u32 index, u32 mode );

  //----------------------------------------------------------------------------
  /**
   * @brief   入力位置カーソル表示
   *
   * @param   lytID レイアウトID
   * @param   index 入力位置
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void PutInputCursor( u32 lytID, u32 index );

  //----------------------------------------------------------------------------
  /**
   * @brief   上書き用カーソル表示切替
   *
   * @param   flg   true = 表示、false = 非表示
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void SetOverwriteCursorVisible( bool flg );

  //----------------------------------------------------------------------------
  /**
   * @brief   カーソル移動
   *
   * @param   mode  入力モード
   * @param   base  動先のペイン
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  //void MoveCursor( u32 mode, gfl2::lyt::LytPaneIndex base );
  void MoveCursor( u32 mode, gfl2::lyt::LytPaneIndex parts, gfl2::lyt::LytPaneIndex pane );

  //----------------------------------------------------------------------------
  /**
   * @brief   上書き用カーソル配置
   *
   * @param   lytID レイアウトID
   * @param   pane  カーソルペイン
   * @param   pos   入力開始位置
   * @param   size  表示サイズ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void PutInputSize( u32 lytID, gfl2::lyt::LytPaneIndex pane, u32 pos, u32 size );

  //----------------------------------------------------------------------------
  /**
   * @brief   上画面メッセージ表示
   *
   * @param   mode  画面モード
   * @param   prm   パラメータ(ポケモン番号など)
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void PutUpperMessage( u32 mode, u32 prm, app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   エラーメッセージ表示
   *
   * @param   id    エラーID
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void PutErrorMessage( u32 id, app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   キーボード表示切替
   *
   * @param   mode  入力モード
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void PutKeyBoard( u32 mode );

  //----------------------------------------------------------------------------
  /**
   * @brief   キーボードマップのペインを取得
   *
   * @param   mode  入力モード
   * @param   id    取得位置
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  gfl2::lyt::LytPaneIndex GetKeyMapTextPane( u32 mode, u32 id );

  //----------------------------------------------------------------------------
  /**
   * @brief   モード切替矢印のループアニメ設定
   *
   * @param   flg   true = 再生、false = 停止
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void SetArrowAutoAnime( bool flg ); 

  //----------------------------------------------------------------------------
  /**
   * @brief   モード切替矢印の選択アニメ再生
   *
   * @param   mv    切り替え方向
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void SetArrowSelectAnime( u32 mv );

  //----------------------------------------------------------------------------
  /**
   * @brief   入力タイプのマーク表示
   *
   * @param   mode  入力モード
   * @param   type  入力タイプ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void PutTypeMark( u32 mode, u32 type );

  //----------------------------------------------------------------------------
  /**
   * @brief   シフトマーク表示切り替え
   *
   * @param   flg   true = 表示、false = 非表示
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void PutShiftMark( bool flg );

  //----------------------------------------------------------------------------
  /**
   * @brief   キー選択アニメ再生
   *
   * @param   mode  入力モード
   * @param   pos   入力位置
   *
   * @return  再生するアニメ
   */
  //----------------------------------------------------------------------------
  u32 StartKeySelectAnime( u32 mode, u32 pos );

  //----------------------------------------------------------------------------
  /**
   * @brief   入力バー表示
   *
   * @param   size  入力可能文字数
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void PutInputBar( u32 size );

  //----------------------------------------------------------------------------
  /**
   * @brief   入力バーアニメ設定
   *
   * @param   pos   入力開始位置
   * @param   size  入力サイズ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void SetInputBarAnime( u32 pos, u32 size );

  //----------------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコン作成
   *
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void CreatePokeIcon( app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコン削除
   *
   * @param   none
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void DeletePokeIcon( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコン配置
   *
   * @param   pp    ポケモンデータ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void PutPokeIcon( const pml::pokepara::CoreParam * pp );

  //----------------------------------------------------------------------------
  /**
   * @brief   ボックスアイコン配置
   *
   * @param   none
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void PutBoxIcon( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   プレイヤーアイコン表示
   *
   * @param   man   ファイルリードマネージャ
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void CreatePlayerIcon( gfl2::fs::AsyncFileManager * man, app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   プレイヤーアイコン削除
   *
   * @param   none
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void DeletePlayerIcon( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   プレイヤーアイコン生成リクエスト
   *
   * @param   myst  プレイヤーデータ
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void CreatePlayerIconTexRequest( Savedata::MyStatus * myst, app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   プレイヤーアイコン生成チェック
   *
   * @param   none
   *
   * @return  "true = 生成完了"
   * @return  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool CheckCreatePlayerIconTex( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   画面開始アニメ再生
   *
   * @param   anm   開始アニメ番号
   * @param   flg   アイコン表示フラグ true = 表示
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void SetStartAnime( u32 anm, bool flg );

  //----------------------------------------------------------------------------
  /**
   * @brief   画面開始アニメ再生チェック
   *
   * @param   anm   開始アニメ番号
   *
   * @return  "false = 再生中"
   * @return  "true = それ以外"
   */
  //----------------------------------------------------------------------------
  bool CheckStartAnime( u32 anm );

  //----------------------------------------------------------------------------
  /**
   * @brief   タイマーアイコン表示切り替え
   *
   * @param   flg   true = 表示
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void SetVisibleTimeIcon( bool flg );

  //----------------------------------------------------------------------------
  /**
   * @brief   文字列がポケモン名かどうかをチェック（ゲーム中の言語のみ）
   *
   * @param   str   文字列
   *
   * @return  "true = ポケモン名"
   * @return  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool CheckPokemonName( gfl2::str::STRCODE * str );

  //----------------------------------------------------------------------------
  /**
   * @brief   多言語ポケモン名のオープン
   *
   * @param   lang    言語ID
   * @param   heap    ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void StartOpenPokeNameMsgData( u32 lang, gfl2::heap::HeapBase * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   多言語ポケモン名のオープン待ち
   *
   * @param   none
   *
   * @return  "true = 終了"
   * @return  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool WaitOpenPokeNameMsgData( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   多言語ポケモン名の読み込み
   *
   * @param   heap    ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void StartLoadPokeNameMsgData( gfl2::heap::HeapBase * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   多言語ポケモン名の読み込み待ち
   *
   * @param   heap    ヒープ
   *
   * @return  "true = 終了"
   * @return  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool WaitLoadPokeNameMsgData( gfl2::heap::HeapBase * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   多言語ポケモン名のクローズ
   *
   * @param   heap    ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void StartClosePokeNameMsgData( gfl2::heap::HeapBase * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   多言語ポケモン名のクローズ待ち
   *
   * @param   none
   *
   * @return  "true = 終了"
   * @return  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool WaitClosePokeNameMsgData( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   他言語ポケモン名を読み込み
   *
   * @param   lang  読み込む言語
   * @param   frman ファイルリードマネージャ
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void StartLoadPokeNameMsgData( u32 lang, gfl2::fs::AsyncFileManager * frman, gfl2::heap::HeapBase * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   他言語ポケモン名削除
   *
   * @param   none
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void EndLoadPokeNameMsgData( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   他言語ポケモン名読み込み待ち
   *
   * @param   frman ファイルマネージャ
   * @param   heap  ヒープ
   *
   * @return  "true = 終了"
   * @return  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool WaitLoadPokeNameMsgData( gfl2::fs::AsyncFileManager * frman, gfl2::heap::HeapBase * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   文字列がポケモン名かどうかをチェック（ゲーム中の言語以外）
   *
   * @param   str   文字列
   *
   * @return  "true = ポケモン名"
   * @return  "false = それ以外"
   */
  //----------------------------------------------------------------------------
  bool CheckPokemonNameLocalize( gfl2::str::STRCODE * str );

  //----------------------------------------------------------------------------
  /**
   * @brief   UGCメッセージ表示
   *
   * @param   flg   true = 表示、false = 非表示
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void PutUgcMessage( bool flg );

private:
  //----------------------------------------------------------------------------
  /**
   * @brief   グラフィックシステム作成
   *
   * @param   heap  ヒープ
   * @param   isInitApplicationMemory アプリケーションメモリを割り当て直すならtrueを指定
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void CreateGraphicsSystem( app::util::Heap * heap, bool isInitApplicationMemory );

  //----------------------------------------------------------------------------
  /**
   * @brief   グラフィックシステム削除
   *
   * @param   none
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void DeleteGraphicsSystem( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   heap  ヒープ
   * @param   pAppLytBuff   レイアウトバッファ
   * @param   pTexLytBuff   テクスチャバッファ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void Create2D( app::util::Heap * heap, void* pAppLytBuff, void* pTexLytBuff, bool isFieldSubProc );

  //----------------------------------------------------------------------------
  /**
   * @brief   2D関連削除
   *
   * @param   none
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void Delete2D( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   カーソル配置
   *
   * @param   lytwk   レイアウトワーク
   * @param   cursor  カーソルペイン
   * @param   base    配置位置のペイン
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void PutCursor( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytPane * cursor, gfl2::lyt::LytPane * base );

  //----------------------------------------------------------------------------
  /**
   * @brief   共有アニメ登録（入力フィールド）
   *
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void SetShareAnime_Input( app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   共有アニメ登録（モード切替矢印）
   *
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void SetShareAnime_Change( app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   共有アニメ登録（50音入力のキーマップ）
   *
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void SetShareAnime_KeyMapNormal( app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   共有アニメ登録（キーボードのキーマップ）
   *
   * @param   heap  ヒープ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void SetShareAnime_KeyMapKeyBoard( app::util::Heap * heap );

  //----------------------------------------------------------------------------
  /**
   * @brief   タイマーアイコン作成
   *
   * @param   heap        ヒープ
   * @param   frameMode   フレームモード
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void CreateTimeIcon( app::util::Heap * heap, bool isFieldSubProc );

  //----------------------------------------------------------------------------
  /**
   * @brief   タイマーアイコン削除
   *
   * @param   none
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void DeleteTimeIcon( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   ペインのセット
   *
   * @param   none
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void SetLayoutPane( void );

public:
  //----------------------------------------------------------------------------
  /**
   * @brief   グラフィックフォントのセット
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void SetTextureGraphicFont( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   グラフィックフォントのセット：韓国語版
   *
   * @return  none
   *
   * @fix     NMCat[914]
   */
  //----------------------------------------------------------------------------
  void SetTextureGraphicFont_KOR( void );

private:
  //----------------------------------------------------------------------------
  /**
   * @brief   テクスチャの差し替え：グラフィックフォント
   *
   * @param   lytID     レイアウトID
   * @param   partsID   パーツID
   * @param   texID     テクスチャID
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void ReplaceTextureGraphicFont( const u32 lytID, const u32 partsID, const u32 texID );

  //-----------------------------------------------------------------------------
  /**
   * @brief   半角大文字を小文字に変換
   *
   * @param   code  文字コード
   *
   * @return  変換後の文字コード
   */
  //-----------------------------------------------------------------------------
  gfl2::str::STRCODE ToLower( gfl2::str::STRCODE code );

  //-----------------------------------------------------------------------------
  /**
   * @brief   文字列を小文字に変換して比較
   *
   * @param   str1  文字列１
   * @param   str2  文字列２
   *
   * @retval  true  = 一致
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool ToLowerComp( const gfl2::str::STRCODE * str1, const gfl2::str::STRCODE * str2 );


private:
  //gfl2::grp::GraphicsSystem * pGraSystem;    //!< グラフィックシステム //使用しない
  gfl2::fs::AsyncFileManager * m_pAsyncFileManager;           //!< ファイルマネージャ
  app::util::AppRenderingManager * m_pAppRenderingManager;    //!< レンダリングマネージャ
  app::util::G2DUtil * m_pG2D;        //!< 2D関連
  bool  m_isInitApplicationMemory;    //!< アプリケーションメモリの割り当て済みフラグ

  app::tool::PokeIcon * m_pPokeIcon;    //!< ポケモンアイコン
  //app::tool::TimeIcon * m_pTimeIcon;    //!< 時間アイコン

  //common::resource::PlayerIconTexManager * pPlayerIconMan;  //!< プレイヤーアイコンマネージャ //使用しない
  //common::resource::PlayerIconTex * pPlayerIconTex;         //!< プレイヤーアイコンテクスチャ //使用しない

  gfl2::str::MsgData * m_pPokeName;   //!< 他言語ポケモン名チェック：メッセージデータ
  void * m_pPokeNameBuff;             //!< 他言語ポケモン名チェック：ポケモン名展開バッファ
  size_t m_pPokeNameBuffSize;         //!< 他言語ポケモン名チェック：ポケモン名展開バッファサイズ
  u32  m_pokeNameArcID;               //!< 他言語ポケモン名チェック：アークID

  u32  m_KeyMapMode;

private:
  // 上画面ペイン
  gfl2::lyt::LytTextBox*    m_pUppMsgTextBox00;

  // 下画面(BGR_LOW)
  gfl2::lyt::LytParts*      m_pLowBgrChg00;
  gfl2::lyt::LytPane*       m_pLowBgrChg00Bound;
  gfl2::lyt::LytParts*      m_pLowBgrChg01;
  gfl2::lyt::LytPane*       m_pLowBgrChg01Bound;

  // 下画面(50音)
  gfl2::lyt::LytParts*      m_pPrevWord;
  gfl2::lyt::LytPane*       m_pCursorSizePane;
  gfl2::lyt::LytWindow*     m_pCursorWindow;
};


GFL_NAMESPACE_END( StrInput )
GFL_NAMESPACE_END( App )

#endif // #ifndef NIJI_PROJECT_APP_STRINPUT_GRAPHIC_H_INCLUDED
