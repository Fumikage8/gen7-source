//=============================================================================
/**
 * @file    MyAlbumDraw.h
 * @brief   マイアルバム画面描画処理
 * @author  Hiroyuki Nakamura
 * @date    2017.01.25
 */
//=============================================================================
#if !defined( MYALBUM_DRAW_H_INCLUDED )
#define MYALBUM_DRAW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// module
#include "MyAlbumPhotoData.h"
// momiji
#include "AppLib/include/Tool/app_tool_PaneList.h"
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "App/FinderStudioStatic/include/FinderStudioDefine.h"
// resource
#include "niji_conv_header/app/data/finder_studio/finder_studio_bg_def.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class MenuCursor;
class PaneListView;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(sort)
class StringMonsName;
GFL_NAMESPACE_END(sort)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MyAlbum)

// 前方宣言
struct APP_PARAM;

//=============================================================================
/**
 * @class MyAlbumDraw
 * @brief マイアルバム画面描画クラス
 * @date  2017.01.25
 */
//=============================================================================
class MyAlbumDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
  , public app::tool::PaneList::CallBack
{
public:
  //=============================================================================
  /**
   * @class Listener
   * @brief リスナークラス
   * @date  2017.01.25
   */
  //=============================================================================
  class Listener
  {
  public:
    //! 動作ID
    enum ActionID
    {
      ACTION_ID_RETURN = 0,       //!< 戻る
      ACTION_ID_SEARCH,           //!< 検索
      ACTION_ID_DELETE,           //!< 削除
      ACTION_ID_PHOTO,            //!< 写真選択
      ACTION_ID_FAVORITE,         //!< お気に入り
      ACTION_ID_SHARE,            //!< 共有
      ACTION_ID_INFO,             //!< インフォボタン

      ACTION_ID_END_DELETE,       //!< 削除モード終了
      ACTION_ID_SELECT_DELETE,    //!< 削除する写真を選択
      ACTION_ID_DECIDE_DELETE,    //!< 削除決定

      ACTION_ID_CURSOR_MOVE,      //!< カーソル移動

      ACTION_ID_SEARCH_TYPE_RESET,    //!< 検索：リセット
      ACTION_ID_SEARCH_TYPE_CANCEL,   //!< 検索：キャンセル
      ACTION_ID_SEARCH_TYPE_DECIDE,   //!< 検索：実行

      ACTION_ID_SEARCH_CORE_CANCEL,   //!< 検索条件：キャンセル
    };

  public:
    //-----------------------------------------------------------------------------
    /**
     * @func    OnAction
     * @brief   入力時のリスナー
     * @date    2017.01.25
     *
     * @param   id    動作ID
     */
    //-----------------------------------------------------------------------------
    virtual void OnAction( ActionID id ) = 0;
  };

  //! 画面モード
  enum DispMode
  {
    DISP_MODE_SELECT = 0,   //!< 写真選択
    DISP_MODE_DELETE,       //!< 写真削除
    DISP_MODE_SEARCH_TYPE,  //!< 検索
    DISP_MODE_SEARCH_CORE,  //!< 検索条件
    DISP_MODE_ERROR,        //!< エラー表示時
  };

  static const u32 PHOTO_PUT_WIDTH = 3;     //!< 写真横配置数
  static const u32 PHOTO_PUT_HEIGHT = 2;    //!< 写真縦配置数
  static const u32 PHOTO_PUT_MAX = PHOTO_PUT_WIDTH * PHOTO_PUT_HEIGHT;  //!< 写真配置数

  //! 削除チェック結果
  enum DeleteResult
  {
    DEL_RESULT_OK = 0,      //!< 削除可能
    DEL_RESULT_NG_SELECT,   //!< 選択されていない
    DEL_RESULT_NG_FAVORITE, //!< お気に入りに登録されている
    DEL_RESULT_NG_SHARE,    //!< 共有されている
  };

  //! リスト種類
  enum ListType
  {
    LIST_TYPE_TOP = 0,      //!< トップ
    LIST_TYPE_LOW,          //!< 並び
    LIST_TYPE_CONSONANT,    //!< ポケモン：子音
    LIST_TYPE_VOWEL,        //!< ポケモン：母音
    LIST_TYPE_MONS_NAME,    //!< ポケモン名
    LIST_TYPE_FAVORITE,     //!< お気に入り
    LIST_TYPE_DECO,         //!< 飾りつけ
    LIST_TYPE_PHOTO,        //!< 撮り方
    LIST_TYPE_BG,           //!< 背景
    LIST_TYPE_SHARE,        //!< 共有
    LIST_TYPE_MAX           //!< 最大数
  };

  static const u32 CORE_LIST_LOW_MAX = 2;           //!< 並びリスト項目数
  static const u32 CORE_LIST_FAVORITE_MAX = 3;      //!< お気に入りリスト項目数
  static const u32 CORE_LIST_DECO_MAX = 3;          //!< 飾りつけリスト項目数
  static const u32 CORE_LIST_PHOTO_MAX = 3;         //!< 撮り方リスト項目数
  static const u32 CORE_LIST_BG_MAX = BG_ID_NUM+1;  //!< 背景リスト項目数 @fix MMCat[568]: "-"があるため+1
  static const u32 CORE_LIST_SHARE_MAX = 3;         //!< 共有リスト項目数
  static const u32 CORE_LIST_CONSONANT_MAX = 11;    //!< 子音リスト項目数


private:
  //! レイアウトID
  enum LyoutID
  {
    LYTID_UPPER_BG = 0,       //!< 上画面背景
    LYTID_LOWER_BG,           //!< 下画面背景
    LYTID_LOWER_PHOTO,        //!< 写真選択
    LYTID_LOWER_SEARCH_TYPE,  //!< 検索
    LYTID_LOWER_SEARCH_CORE,  //!< 検索条件
    LYTID_MAX                 //!< 最大数
  };

  //! ボタンID
  enum ButtonID
  {
    // 写真選択画面
    BTNID_SEL_PHOTO_00 = 0, //!< 写真（決定用）
    BTNID_SEL_PHOTO_01,
    BTNID_SEL_PHOTO_02,
    BTNID_SEL_PHOTO_03,
    BTNID_SEL_PHOTO_04,
    BTNID_SEL_PHOTO_05,

    BTNID_MV_PHOTO_00,     //!< 写真（カーソル移動用）
    BTNID_MV_PHOTO_01,
    BTNID_MV_PHOTO_02,
    BTNID_MV_PHOTO_03,
    BTNID_MV_PHOTO_04,
    BTNID_MV_PHOTO_05,

    BTNID_PAGE_LEFT,    //!< ページ切り替え・左
    BTNID_PAGE_RIGHT,   //!< ページ切り替え・右

    BTNID_FAVORITE,     //!< お気に入り
    BTNID_SHARE,        //!< 共有

    BTNID_INFO,         //!< InfoWindow起動

    BTNID_RETURN,       //!< 戻る
    BTNID_SEARCH,       //!< 検索
    BTNID_DELETE,       //!< 削除

    BTNID_DEL_MULTI,    //!< 消す（複数モード）

    // 検索画面
    BTNID_SEARCH_TYPE_RETURN, //!< 戻る
    BTNID_SEARCH_TYPE_RESET,  //!< リセット
    BTNID_SEARCH_TYPE_SEARCH, //!< 探す

    // 検索条件画面
    BTNID_SEARCH_CORE_RETURN, //!< 戻る

    BTNID_MAX,          //!< 最大値
  };

  //! 写真ボタン部品ペインテーブル
//  static const gfl2::lyt::LytPaneIndex PhotoButtonPaneTable[PHOTO_PUT_MAX];

  static const u32 PHOTO_DELTE_BUF_SIZE = 53;   //!< 写真削除フラグのバッファサイズ 420/8 = 52.5

  static const u32 THUMBNAIL_TEX_WIDTH = 112;   //!< サムネイルテクスチャの横幅
  static const u32 THUMBNAIL_TEX_HEIGHT = 64;   //!< サムネイルテクスチャの縦幅

  static const u32 THUMBNAIL_BUF_WIDTH = App::FinderStudio::Static::THUMB_BUFFER_WIDTH;   //!< サムネイルバッファの横幅
  static const u32 THUMBNAIL_BUF_HEIGHT = App::FinderStudio::Static::THUMB_BUFFER_HEIGHT; //!< サムネイルバッファの縦幅

  static const u32 TOP_LIST_PANE_MAX = 9;   //!< ソートトップリストのペイン数
  static const u32 TOP_LIST_ITEM_MAX = 7;   //!< ソートトップリストの項目数
  static const u32 TOP_LIST_CURSOR_MOVE_MAX = 6;  //!< ソートトップリストのカーソル移動値
  static const f32 TOP_LIST_PX;   //!< ソートトップリストの表示X座標
  static const f32 TOP_LIST_PY;   //!< ソートトップリストの表示Y座標
  static const f32 TOP_LIST_SX;   //!< ソートトップリストのXサイズ
  static const f32 TOP_LIST_SY;   //!< ソートトップリストのYサイズ
  //! ソートトップリストの部品テーブル
  static const gfl2::lyt::LytPaneIndex ListTopPartsTable[TOP_LIST_PANE_MAX];

  static const u32 CORE_LIST_PANE_MAX = 9;   //!< ソート項目リストのペイン数
  static const u32 CORE_LIST_ITEM_MAX = 7;   //!< ソート項目リストの項目数
  static const u32 CORE_LIST_CURSOR_MOVE_MAX = 6;  //!< ソート項目リストのカーソル移動値
  static const f32 CORE_LIST_PX;   //!< ソート項目リストの表示X座標
  static const f32 CORE_LIST_PY;   //!< ソート項目リストの表示Y座標
  static const f32 CORE_LIST_SX;   //!< ソート項目リストのXサイズ
  static const f32 CORE_LIST_SY;   //!< ソート項目リストのYサイズ
  //! ソート項目リストの部品テーブル
  static const gfl2::lyt::LytPaneIndex ListCorePartsTable[CORE_LIST_PANE_MAX];

  //! 母音データ
  struct VOWEL_DATA
  {
    u16 msg;    //!< メッセージ
    u16 sort;   //!< ソートデータ
  };

  static const u32 VOWEL_DATA_MAX = 5;      //!< 母音データ数（通常）
  static const u32 VOWEL_DATA_YA_MAX = 3;   //!< 母音データ数（ヤ行）
  static const u32 VOWEL_DATA_WA_MAX = 1;   //!< 母音データ数（ワ行）

  //! 子音テーブル
  static const u32 Initial_Consonant[CORE_LIST_CONSONANT_MAX];
  //! 母音データ
  static const VOWEL_DATA VowelData_A[VOWEL_DATA_MAX];
  static const VOWEL_DATA VowelData_KA[VOWEL_DATA_MAX];
  static const VOWEL_DATA VowelData_SA[VOWEL_DATA_MAX];
  static const VOWEL_DATA VowelData_TA[VOWEL_DATA_MAX];
  static const VOWEL_DATA VowelData_NA[VOWEL_DATA_MAX];
  static const VOWEL_DATA VowelData_HA[VOWEL_DATA_MAX];
  static const VOWEL_DATA VowelData_MA[VOWEL_DATA_MAX];
  static const VOWEL_DATA VowelData_YA[VOWEL_DATA_YA_MAX];
  static const VOWEL_DATA VowelData_RA[VOWEL_DATA_MAX];
  static const VOWEL_DATA VowelData_WA[VOWEL_DATA_WA_MAX];
  //! 母音データテーブル
  static const VOWEL_DATA * VowelDataTable[CORE_LIST_CONSONANT_MAX];
  //! 母音データテーブルサイズ
  static const u32 VowelDataTableLen[CORE_LIST_CONSONANT_MAX];

  static const u32 INITIAL_LIST_ALPHABET_MAX = 27;  //!< アルファベット頭文字数
  static const u32 INITIAL_LIST_KORIA_MAX = 15;     //!< ハングル頭文字数
  static const u32 INITIAL_LIST_CHINA_MAX = 24;     //!< 簡体字頭文字数
  static const u32 INITIAL_LIST_TAIWAN_MAX = 36;    //!< 繁体字頭文字数

  //! アルファベット頭文字テーブル
  static const u32 Initial_Alphabet[INITIAL_LIST_ALPHABET_MAX];
  //! ハングル頭文字テーブル
  static const u32 Initial_Koria[INITIAL_LIST_KORIA_MAX];
  //! 簡体字頭文字テーブル
  static const u32 Initial_China[INITIAL_LIST_CHINA_MAX];
  //! 繁体字頭文字テーブル
  static const u32 Initial_Taiwan[INITIAL_LIST_TAIWAN_MAX];

  static const u32 POKE_SORT_DATA_MAX = MONSNO_END;   // ソートデータ数 @note MONSNO_ENDは最終ポケモンの番号だが、MONSNO_NULLはいらないのでバッファ数=MONSNO_ENDでよい

  //! ボタン表示状態
  struct BUTTON_VIEW
  {
    ButtonID id;          //!< ボタンID
    bool is_view_active;  //!< 見た目がアクティブか
  };

  static const u32 BUTTON_VIEW_MAX = 4;   //!< ボタン表示管理数


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   param       外部設定パラメータ
   * @param   heap        ヒープ
   * @param   render_man  描画マネージャ
   * @param   listener    リスナー
   */
  //-----------------------------------------------------------------------------
  MyAlbumDraw( APP_PARAM * param, app::util::Heap * heap, app::util::AppRenderingManager * render_man, Listener * listener );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~MyAlbumDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   初期化処理
   * @date    2017.01.25
   *
   * @param   res_buf         レイアウトリソースバッファ
   * @param   msg_buf         メッセージリソースバッファ
   * @param   msg_bg_buf      背景リスト項目用メッセージリソースバッファ
   * @param   msg_initial_buf イニシャル用メッセージリソースバッファ
   *
   * @retval  true  = 処理完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Init( void * res_buf, void * msg_buf, void * msg_bg_buf, void * msg_initial_buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    Setup
   * @brief   初期データセットアップ
   * @date    2017.02.16
   *
   * @param   photo_data  写真データ
   * @param   cursor      メニューカーソル
   * @param   pos         カーソル位置
   */
  //-----------------------------------------------------------------------------
  void Setup( MyAlbumPhotoData * photo_data, app::tool::MenuCursor * cursor, u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    End
   * @brief   終了処理
   * @date    2017.01.25
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
   * @date    2017.01.27
   *
   * @return  メッセージデータ
   */
  //-----------------------------------------------------------------------------
  gfl2::str::MsgData * GetMsgData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Start
   * @brief   開始処理
   * @date    2017.01.25
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
   * @date    2017.01.25
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsStart(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetDispMode
   * @brief   画面モード取得
   * @date    2017.01.31
   *
   * @return  画面モード
   */
  //-----------------------------------------------------------------------------
  DispMode GetDispMode(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeDispMode
   * @brief   画面モード切り替え
   * @date    2017.01.26
   *
   * @param   mode  画面モード
   */
  //-----------------------------------------------------------------------------
  void ChangeDispMode( DispMode mode );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetCursorPos
   * @brief   カーソル位置を取得
   * @date    2017.01.31
   *
   * @return  カーソル位置
   */
  //-----------------------------------------------------------------------------
  u32 GetCursorPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetCursorPosIndex
   * @brief   カーソル位置の写真のインデックスを取得
   * @date    2017.01.30
   *
   * @return  カーソル位置のインデックス
   */
  //-----------------------------------------------------------------------------
  u32 GetCursorPosIndex(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsPhotoFavorite
   * @brief   お気に入りに登録されているか
   * @date    2017.01.30
   *
   * @retval  true  = されている
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsPhotoFavorite(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPhotoFavoriteFlag
   * @brief   お気に入り登録を変更
   * @date    2017.01.30
   *
   * @return  表示するメッセージ
   */
  //-----------------------------------------------------------------------------
  u32 SetPhotoFavoriteFlag(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsPhotoShareOther
   * @brief   共有されている他の写真があるか
   * @date    2017.02.06
   *
   * @retval  true  = ある
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsPhotoShareOther(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsPhotoShare
   * @brief   共有されているか
   * @date    2017.02.06
   *
   * @retval  true  = されている
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsPhotoShare(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPhotoShareFlag
   * @brief   共有状態を変更
   * @date    2017.02.06
   *
   * @return  表示するメッセージ
   */
  //-----------------------------------------------------------------------------
  u32 SetPhotoShareFlag(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeletePhotoData
   * @brief   写真データ削除
   * @date    2017.01.30
   *
   * @return  処理結果
   */
  //-----------------------------------------------------------------------------
  MyAlbumPhotoData::SDCardAccessResult DeletePhotoData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitDeletePhotoBuffer
   * @brief   写真データ削除ビット初期化
   * @date    2017.01.30
   */
  //-----------------------------------------------------------------------------
  void InitDeletePhotoBuffer(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetDeletePhotoBit
   * @brief   写真データ削除ビット設定
   * @date    2017.01.30
   *
   * @param   index   写真のインデックス
   */
  //-----------------------------------------------------------------------------
  void SetDeletePhotoBit( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsDeletePhotoBit
   * @brief   写真データ削除ビットチェック
   * @date    2017.01.30
   *
   * @param   index   写真のインデックス
   *
   * @retval  true  = ON
   * @retval  false = OFF
   */
  //-----------------------------------------------------------------------------
  bool IsDeletePhotoBit( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckSelectDeletePhoto
   * @brief   写真が削除できるか
   * @date    2017.01.31
   *
   * @return  結果
   */
  //-----------------------------------------------------------------------------
  DeleteResult CheckSelectDeletePhoto(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisiblePhotoSelectMark
   * @brief   選択マークの表示を変更
   * @date    2017.01.31
   *
   * @param   pos   写真の表示位置
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisiblePhotoSelectMark( u32 pos, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleUpperBg
   * @brief   上画面の背景レイアウト表示切り替え
   * @date    2017.02.07
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleUpperBg( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetInfoPos
   * @brief   インフォボタンの座標を取得
   * @date    2017.02.17
   *
   * @param   vec   座標格納場所
   */
  //-----------------------------------------------------------------------------
  void GetInfoPos( gfl2::math::Vector3 * vec );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetListType
   * @brief   ペインリストタイプを設定
   * @date    2017.02.21
   *
   * @param   type  リストタイプ
   */
  //-----------------------------------------------------------------------------
  void SetListType( ListType type );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListType
   * @brief   ペインリストタイプを取得
   * @date    2017.02.21
   *
   * @return  リストタイプ
   */
  //-----------------------------------------------------------------------------
  ListType GetListType(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitListTop
   * @brief   ソートトップリスト初期化
   * @date    2017.02.20
   *
   * @param   view    リストビュークラス
   */
  //-----------------------------------------------------------------------------
  void InitListTop( app::tool::PaneListView * view );

  //-----------------------------------------------------------------------------
  /**
   * @func    EndListTop
   * @brief   ソートトップリスト終了
   * @date    2017.02.20
   */
  //-----------------------------------------------------------------------------
  void EndListTop(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ResetTopList
   * @brief   ソートトップリスト再生成
   * @date    2017.02.21
   */
  //-----------------------------------------------------------------------------
  void ResetTopList(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitListCore
   * @brief   ソート項目リスト初期化
   * @date    2017.02.21
   *
   * @param   view    リストビュークラス
   */
  //-----------------------------------------------------------------------------
  void InitListCore( app::tool::PaneListView * view );

  //-----------------------------------------------------------------------------
  /**
   * @func    EndListCore
   * @brief   ソート項目リスト終了
   * @date    2017.02.21
   */
  //-----------------------------------------------------------------------------
  void EndListCore(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ResetCoreList
   * @brief   ソート項目リスト再生成
   * @date    2017.02.21
   *
   * @param   type      リストタイプ
   * @param   max       項目数
   * @param   pos_init  true = カーソル位置を初期化
   */
  //-----------------------------------------------------------------------------
  void ResetCoreList( ListType type, u32 max, bool pos_init );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetSelectVowelData
   * @brief   選択された母音データをセット
   * @date    2017.02.22
   *
   * @param   pos   選択位置 ( 0オリジン )
   */
  //-----------------------------------------------------------------------------
  void SetSelectVowelData( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetSelectVowelData
   * @brief   母音データ数を取得
   * @date    2017.02.22
   *
   * @return  データ数
   */
  //-----------------------------------------------------------------------------
  u32 GetVowelListMax(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPokemonSortData
   * @brief   ポケモンソートデータをセット
   * @date    2017.02.22
   *
   * @param   sort  ソートデータクラス
   */
  //-----------------------------------------------------------------------------
  void SetPokemonSortData( const app::sort::StringMonsName * sort );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePokemonInitialData
   * @brief   ポケモン頭文字ソートデータをセット
   * @date    2017.02.22
   *
   * @param   pos   リスト位置
   *
   * @return  データ数
   */
  //-----------------------------------------------------------------------------
  u32 CreatePokemonInitialData( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPokemonInitialData
   * @brief   ポケモン頭文字ソートデータを取得
   * @date    2017.02.22
   *
   * @param   pos   リスト位置
   *
   * @return  ポケモン番号
   */
  //-----------------------------------------------------------------------------
  u32 GetPokemonInitialData( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartSort
   * @brief   絞込み開始
   * @date    2017.02.27
   */
  //-----------------------------------------------------------------------------
  void StartSort(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetLangListMax
   * @brief   言語別頭文字数取得
   * @date    2017.02.28
   *
   * @return  頭文字数
   */
  //-----------------------------------------------------------------------------
  u32 GetLangListMax(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetExpandMessage
   * @brief   メッセージを展開して取得
   * @date    2017.04.14
   *
   * @param   msg   メッセージID
   * @param   val   Register[0]に入れる値
   * @param   keta  valの桁
   * 
   * @return  文字列バッファ
   */
  //-----------------------------------------------------------------------------
  gfl2::str::StrBuf * GetExpandMessage( u32 msg, u32 val, u32 keta );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    GetGameData
   * @brief   GameData関連取得
   * @date    2017.01.25
   */
  //-----------------------------------------------------------------------------
  void GetGameData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2017.01.25
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2017.01.25
   *
   * @param   displayNo   描画面
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateNumFont
   * @brief   NUMフォント生成
   * @date    2017.02.06
   */
  //-----------------------------------------------------------------------------
  void CreateNumFont(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetNumFont
   * @brief   NUMフォントをセット
   * @date    2017.02.06
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
   * @date    2017.02.06
   */
  //-----------------------------------------------------------------------------
  void DeleteNumFont(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize2D
   * @brief   2D関連初期化
   * @date    2017.01.25
   *
   * @param   res_buf         レイアウトリソースバッファ
   * @param   msg_buf         メッセージリソースバッファ
   * @param   msg_bg_buf      背景リスト項目用メッセージリソースバッファ
   * @param   msg_initial_buf イニシャル用メッセージリソースバッファ
   */
  //-----------------------------------------------------------------------------
  void Initialize2D( void * res_buf, void * msg_buf, void * msg_bg_buf, void * msg_initial_buf );

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate2D
   * @brief   2D関連削除
   * @date    2017.01.25
   */
  //-----------------------------------------------------------------------------
  void Terminate2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteDefaultSrring
   * @brief   デフォルト文字列描画
   * @date    2017.01.25
   */
  //-----------------------------------------------------------------------------
  void WriteDefaultString(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutPhotoMax
   * @brief   写真枚数表示
   * @date    2017.02.06
   */
  //-----------------------------------------------------------------------------
  void PutPhotoMax(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutPhotoNumber
   * @brief   カーソル位置の写真番号を表示
   * @date    2017.02.06
   */
  //-----------------------------------------------------------------------------
  void PutPhotoNumber(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisiblePhotoNumber
   * @brief   写真番号表示切り替え
   * @date    2017.02.06
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisiblePhotoNumber( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisiblePhotoTitle
   * @brief   タイトル表示切り替え
   * @date    2017.02.06
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisiblePhotoTitle( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuCursorLayout
   * @brief   メニューカーソル用レイアウト作成
   * @date    2017.01.27
   *
   * @param   menu_cursor   メニューカーソルクラス
   */
  //-----------------------------------------------------------------------------
  void CreateMenuCursorLayout( app::tool::MenuCursor * menu_cursor );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMenuCursorLayout
   * @brief   メニューカーソル用レイアウト削除
   * @date    2017.01.27
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuCursorLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateMenuCursor
   * @brief   メニューカーソル更新
   * @date    2017.01.27
   */
  //-----------------------------------------------------------------------------
  void UpdateMenuCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawMenuCursor
   * @brief   メニューカーソル描画
   * @date    2017.01.27
   */
  //-----------------------------------------------------------------------------
  void DrawMenuCursor( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitButtonView
   * @brief   ボタン表示管理初期化
   * @date    2017.02.28
   */
  //-----------------------------------------------------------------------------
  void InitButtonView(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsButtonViewActive
   * @brief   ボタン表示がアクティブか
   * @date    2017.02.28
   *
   * @param   id  ボタンID
   *
   * @retval  true  = アクティブ
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsButtonViewActive( ButtonID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetButtonViewActive
   * @brief   ボタン表示状態を設定
   * @date    2017.02.28
   *
   * @param   id    ボタンID
   * @param   flg   true = アクティブ
   */
  //-----------------------------------------------------------------------------
  void SetButtonViewActive( ButtonID id, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateButton
   * @brief   ボタン生成
   * @date    2017.01.26
   */
  //-----------------------------------------------------------------------------
  void CreateButton(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetButtonInfoEx
   * @brief   ボタン情報生成
   * @date    2017.01.26
   *
   * @param   info        生成場所
   * @param   info_tbl    デフォルト情報テーブル
   * @param   parts_tbl   部品ペインテーブル
   * @param   bound_tbl   境界ペインテーブル
   * @param   max         生成数
   */
  //-----------------------------------------------------------------------------
  void SetButtonInfoEx(
        u32 lyt_id,
        app::ui::ButtonInfoLytWk * info,
        const app::ui::ButtonInfoLytWk * info_tbl, 
        const gfl2::lyt::LytPaneIndex * parts_tbl,
        const gfl2::lyt::LytPaneIndex * bound_tbl,
        u32 max );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateBaseButton
   * @brief   写真選択画面のボタン生成
   * @date    2017.01.26
   *
   * @param   info  生成場所
   */
  //-----------------------------------------------------------------------------
  void CreateBaseButton( app::ui::ButtonInfoLytWk * info );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateSearchTypeButton
   * @brief   検索画面のボタン生成
   * @date    2017.01.26
   *
   * @param   info  生成場所
   */
  //-----------------------------------------------------------------------------
  void CreateSearchTypeButton( app::ui::ButtonInfoLytWk * info );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateSearchCoreButton
   * @brief   検索条件画面のボタン生成
   * @date    2017.01.26
   *
   * @param   info  生成場所
   */
  //-----------------------------------------------------------------------------
  void CreateSearchCoreButton( app::ui::ButtonInfoLytWk * info );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePageData
   * @brief   ページ関連のデータを生成
   * @date    2017.01.26
   */
  //-----------------------------------------------------------------------------
  void CreatePageData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitCursorPos
   * @brief   初期カーソル位置を設定
   * @date    2017.02.15
   *
   * @param   pos   カーソル位置
   */
  //-----------------------------------------------------------------------------
  void InitCursorPos( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutCursor
   * @brief   カーソル配置
   * @date    2017.01.26
   *
   * @param   pos   配置位置
   *
   * @retval  true  = 指定位置にデータあり
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool PutCursor( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveCursor
   * @brief   カーソル移動
   * @date    2017.01.26
   *
   * @param   mx  X移動方向
   * @param   my  Y移動方向
   */
  //-----------------------------------------------------------------------------
  void MoveCursor( s32 mx, s32 my );

  //-----------------------------------------------------------------------------
  /**
   * @func    指定位置より前の存在するデータ位置を取得
   * @brief   カーソル移動
   * @date    2017.01.27
   *
   * @param   pos   検索開始位置
   *
   * @retval  -1  = データなし
   * @retval  -1 != データ位置
   */
  //-----------------------------------------------------------------------------
  s32 GetEnableDataPos( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangePage
   * @brief   ページ切り替え
   * @date    2017.01.26
   *
   * @param   mv        切り替え方向
   * @param   is_touch  true = タッチ操作
   *
   * @retval  true  = 切り替えた
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool ChangePage( s32 mv, bool is_touch );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisiblePageArrow
   * @brief   ページ矢印表示切り替え
   * @date    2017.01.26
   */
  //-----------------------------------------------------------------------------
  void SetVisiblePageArrow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisiblePhoto
   * @brief   写真ペイン表示切り替え
   * @date    2017.01.27
   */
  //-----------------------------------------------------------------------------
  void SetVisiblePhoto(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetThumnailPane
   * @brief   サムネイルのペインを取得
   * @date    2017.02.10
   *
   * @param   index   インデックス
   *
   * @return  ペイン
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytPicture * GetThumnailPane( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetThumbnail
   * @brief   写真ペインにサムネイルをセット
   * @date    2017.02.10
   *
   * @param   index   インデックス
   */
  //-----------------------------------------------------------------------------
  void SetThumbnail( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadThumbnail
   * @brief   サムネイル読み込み
   * @date    2017.02.10
   *
   * @param   photo_index   写真データインデックス
   * @param   put_index     配置位置インデックス
   */
  //-----------------------------------------------------------------------------
  void LoadThumbnail( u32 photo_index, u32 put_index );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateThumbnail
   * @brief   サムネイル更新（個別）
   * @date    2017.02.10
   *
   * @param   index   インデックス
   */
  //-----------------------------------------------------------------------------
  void UpdateThumbnail( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateThumbnail
   * @brief   サムネイル更新（全体）
   * @date    2017.02.10
   */
  //-----------------------------------------------------------------------------
  void UpdateThumbnail(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPhotoLoadFlag
   * @brief   サムネイル読み込みフラグ初期化
   * @date    2017.02.10
   */
  //-----------------------------------------------------------------------------
  void InitPhotoLoadFlag(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisiblePhotoMark
   * @brief   写真マーク表示設定
   * @date    2017.01.30
   */
  //-----------------------------------------------------------------------------
  void SetVisiblePhotoMark(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisiblePhotoFavoriteMark
   * @brief   お気に入り登録マークの表示を変更
   * @date    2017.01.30
   *
   * @param   pos   写真の表示位置
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisiblePhotoFavoriteMark( u32 pos, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisiblePhotoShareMark
   * @brief   共有マークの表示を変更
   * @date    2017.01.30
   *
   * @param   pos   写真の表示位置
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisiblePhotoShareMark( u32 pos, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisiblePhotoEditMark
   * @brief   編集済みマークの表示を変更
   * @date    2017.03.10
   *
   * @param   pos   写真の表示位置
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisiblePhotoEditMark( u32 pos, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPhotoNone
   * @brief   写真なしの場合の表示
   * @date    2017.02.06
   */
  //-----------------------------------------------------------------------------
  void SetPhotoNone(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartTimerIconAnime
   * @brief   タイマーアイコンアニメ開始
   * @date    2017.02.10
   */
  //-----------------------------------------------------------------------------
  void StartTimerIconAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetTimerIcon
   * @brief   タイマーアイコンアニメ表示切り替え
   * @date    2017.02.10
   *
   * @param   index   インデックス
   * @param   flg     true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetTimerIcon( u32 index, bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneEvent
   * @brief   ペインイベントの検知
   * @date    2017.01.25
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
   * @date    2017.01.25
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

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPaneListPos
   * @brief   ペインリスト位置データ初期化
   * @date    2017.02.21
   */
  //-----------------------------------------------------------------------------
  void InitPaneListPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetListTopParam
   * @brief   ソートトップのペインリスト初期化パラメータ設定
   * @date    2017.02.21
   *
   * @param   param   パラメータ生成場所
   * @param   pos     カーソル位置
   * @param   scroll  スクロール値
   */
  //-----------------------------------------------------------------------------
  void SetListTopParam( app::tool::PaneList::SETUP_PARAM * param, u32 pos, f32 scroll );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetListCoreParam
   * @brief   ソート項目のペインリスト初期化パラメータ設定
   * @date    2017.02.21
   *
   * @param   param   パラメータ生成場所
   * @param   pos     カーソル位置
   * @param   scroll  スクロール値
   * @param   max     項目数
   */
  //-----------------------------------------------------------------------------
  void SetListCoreParam( app::tool::PaneList::SETUP_PARAM * param, u32 pos, f32 scroll, u32 max );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPaneListConfig
   * @brief   ペインリストの設定変更
   * @date    2017.02.20
   *
   * @param   list    ペインリスト
   * @param   setup   ペインリストのセットアップパラメータ
   * @param   px      項目配置X座標
   * @param   py      項目配置Y座標
   * @param   sx      項目Xサイズ
   * @param   sy      項目Yサイズ
   */
  //-----------------------------------------------------------------------------
  void SetPaneListConfig(
        app::tool::PaneList * list,
        app::tool::PaneList::PANE_DATA * data,
        app::tool::PaneList::SETUP_PARAM * setup,
        f32 px, f32 py, f32 sx, f32 sy );

  //-----------------------------------------------------------------------------
  /**
   * @func    CallBack_CursorMove
   * @brief   カーソル移動時のコールバック関数
   * @date    2017.02.20
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
   * @date    2017.02.20
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
   * @date    2017.02.20
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
   * @date    2017.02.20
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
   * @func    WriteListTopItem
   * @brief		ソートトップリスト項目描画
   * @date    2017.02.20
   *
   * @param		pane_index	項目位置のペインデータインデックス
   * @param		pos					項目位置
   */
  //-----------------------------------------------------------------------------
  void WriteListTopItem( u32 pane_index, u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteListLowItem
   * @brief		ソート並びリスト項目描画
   * @date    2017.02.21
   *
   * @param		pane        ペインデータ
   * @param		pane_index	項目位置のペインデータインデックス
   * @param		pos					項目位置
   */
  //-----------------------------------------------------------------------------
  void WriteListLowItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteListConsonantItem
   * @brief		ソート子音リスト項目描画
   * @date    2017.02.21
   *
   * @param		pane				ペインデータ
   * @param		pane_index	項目位置のペインデータインデックス
   * @param		pos					項目位置
   */
  //-----------------------------------------------------------------------------
  void WriteListConsonantItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteListVowelItem
   * @brief		ソート母音リスト項目描画
   * @date    2017.02.21
   *
   * @param		pane				ペインデータ
   * @param		pane_index	項目位置のペインデータインデックス
   * @param		pos					項目位置
   */
  //-----------------------------------------------------------------------------
  void WriteListVowelItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteListMonsNameItem
   * @brief		ソートポケモン名リスト項目描画
   * @date    2017.02.21
   *
   * @param		pane				ペインデータ
   * @param		pane_index	項目位置のペインデータインデックス
   * @param		pos					項目位置
   */
  //-----------------------------------------------------------------------------
  void WriteListMonsNameItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteListFavoriteItem
   * @brief		ソートお気に入りリスト項目描画
   * @date    2017.02.21
   *
   * @param		pane				ペインデータ
   * @param		pane_index	項目位置のペインデータインデックス
   * @param		pos					項目位置
   */
  //-----------------------------------------------------------------------------
  void WriteListFavoriteItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteListDecoItem
   * @brief		ソート飾りつけリスト項目描画
   * @date    2017.02.21
   *
   * @param		pane				ペインデータ
   * @param		pane_index	項目位置のペインデータインデックス
   * @param		pos					項目位置
   */
  //-----------------------------------------------------------------------------
  void WriteListDecoItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteListPhotoItem
   * @brief		ソート撮り方リスト項目描画
   * @date    2017.02.21
   *
   * @param		pane				ペインデータ
   * @param		pane_index	項目位置のペインデータインデックス
   * @param		pos					項目位置
   */
  //-----------------------------------------------------------------------------
  void WriteListPhotoItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteListBgItem
   * @brief		ソート背景リスト項目描画
   * @date    2017.02.21
   *
   * @param		pane				ペインデータ
   * @param		pane_index	項目位置のペインデータインデックス
   * @param		pos					項目位置
   */
  //-----------------------------------------------------------------------------
  void WriteListBgItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteListShareItem
   * @brief		ソート共有リスト項目描画
   * @date    2017.02.21
   *
   * @param		pane				ペインデータ
   * @param		pane_index	項目位置のペインデータインデックス
   * @param		pos					項目位置
   */
  //-----------------------------------------------------------------------------
  void WriteListShareItem( const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetBgListItemString
   * @brief   背景名取得
   * @date    2017.02.21
   *
   * @param   buf   文字列格納場所
   * @param   val   背景値
   */
  //-----------------------------------------------------------------------------
  void GetBgListItemString( gfl2::str::StrBuf * buf, u32 val );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPokemonSortData
   * @brief   ポケモンソートデータ初期化
   * @date    2017.02.22
   */
  //-----------------------------------------------------------------------------
  void InitPokemonSortData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPokemonInitialDataCount
   * @brief   ポケモン頭文字ソートデータ数を取得
   * @date    2017.02.22
   *
   * @param   pos   リスト位置
   *
   * @return  データ数
   */
  //-----------------------------------------------------------------------------
  u32 GetPokemonInitialDataCount( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleListInfoWindow
   * @brief   リスト説明ウィンドウ表示切り替え
   * @date    2017.02.23
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleListInfoWindow( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetListInfoMessage
   * @brief   リスト説明メッセージセット
   * @date    2017.02.23
   *
   * @param   type  リストタイプ
   */
  //-----------------------------------------------------------------------------
  void SetListInfoMessage( ListType type );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetLangInitial
   * @brief   言語別頭文字設定
   * @date    2017.02.28
   */
  //-----------------------------------------------------------------------------
  void SetLangInitial(void);


private:
  APP_PARAM * m_appParam;                               //!< 外部設定パラメータ
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ

  gfl2::str::MsgData * m_pMsgData;        //!< メッセージデータ
  gfl2::str::MsgData * m_pBgMsgData;      //!< 背景リスト項目用メッセージデータ
  gfl2::str::MsgData * m_pInitialMsgData; //!< イニシャル用メッセージデータ
  print::WordSet * m_pWordSet;            //!< 単語セット

  app::tool::MenuCursor * m_menuCursor;   //!< メニューカーソル

  Listener * m_listener;  //!< リスナー

  DispMode m_dispMode;    //!< 画面モード
  BUTTON_VIEW m_buttonView[BUTTON_VIEW_MAX];  //!< ボタン表示状態

  MyAlbumPhotoData * m_photoData;   //!< 写真データ
  u16 m_cursorPos;
  u16 m_page;
  u16 m_pageMax;

  u16 m_photoDeleteCount;
  u8 m_photoDeleteBit[PHOTO_DELTE_BUF_SIZE];
  u8 m_photoLoadFlag[PHOTO_PUT_MAX];

  app::tool::PaneListView * m_listTop;    //!< ソートトップリスト
  app::tool::PaneListView * m_listCore;   //!< ソート項目別リスト
  app::tool::PaneList::PANE_DATA * m_pListTopData;    //!< ソートトップペインリストデータ
  app::tool::PaneList::PANE_DATA * m_pListCoreData;   //!< ソート項目ペインリストデータ

  ListType m_listType;  //!< ペインリストタイプ

  u32 m_listPos[LIST_TYPE_MAX];     //!< ペインリスト位置
  f32 m_listScroll[LIST_TYPE_MAX];  //!< ペインリストスクロール値

  const VOWEL_DATA * m_vowelData;   //!< 母音データテーブル
  u32 m_vowelDataLen;               //!< 母音データテーブルサイズ

  const app::sort::StringMonsName * m_sort;   //!< ソートクラス
  MonsNo m_sortAll[POKE_SORT_DATA_MAX];       //!< ポケモンソートデータ全体
  MonsNo m_sortInitial[POKE_SORT_DATA_MAX];   //!< ポケモンソート頭文字データ
  u32 m_sortInitialMax;                       //!< ポケモンソート頭文字データ数

  const u32 * m_langInitialTable;   //!< 言語別頭文字テーブル
  u32 m_langInitialMax;             //!< 言語別頭文字数

  u32 m_subSeq;         //!< サブシーケンス
  bool m_isDrawEnable;  //!< 描画フラグ
};


GFL_NAMESPACE_END(MyAlbum)
GFL_NAMESPACE_END(App)

#endif  // MYALBUM_DRAW_H_INCLUDED
