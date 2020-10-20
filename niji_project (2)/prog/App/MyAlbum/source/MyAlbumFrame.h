//=============================================================================
/**
 * @file    MyAlbumFrame.h
 * @brief   マイアルバム画面フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2017.01.25
 */
//=============================================================================
#if !defined( MYALBUM_FRAME_H_INCLUDED )
#define MYALBUM_FRAME_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Str.h>
// momiji
#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"
#include "App/FinderStudioStatic/include/FinderStudioMessageMenuView.h"
// module
#include "MyAlbumDraw.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class MenuCursor;
class InfoWindowDataManager;
class InfoWindowBookType;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)
class FinderStudioViewer;
GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(Savedata)
class Misc;
class FinderStudioSave;
GFL_NAMESPACE_END(Savedata)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(sort)
class StringMonsName;
GFL_NAMESPACE_END(sort)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MyAlbum)

// 前方宣言
struct APP_PARAM;
class MyAlbumPhotoData;

//=============================================================================
/**
 * @class MyAlbumFrame
 * @brief マイアルバム画面フレームクラス
 * @date  2017.01.25
 */
//=============================================================================
class MyAlbumFrame
  : public applib::frame::CellSuper
  , public MyAlbumDraw::Listener
  , public App::FinderStudio::Static::MessageMenuViewListener
  , public app::tool::InfoWindowBookTypeListener
  , public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN(MyAlbumFrame);    // コピーコンストラクタ＋代入禁止

private:
  //! メニュー項目ID
  enum MenuItemID
  {
    // 写真メニュー
    MENU_PHOTO_EDIT = 0,  //!< 編集する
    MENU_PHOTO_DELETE,    //!< 捨てる
    MENU_PHOTO_FAVORITE,  //!< お気に入り
    MENU_PHOTO_SHARE,     //!< 共有
    MENU_PHOTO_CANCEL,    //!< やめる
    // 削除"はい・いいえ"
    MENU_DELETE_YES,      //!< はい
    MENU_DELETE_NO,       //!< いいえ
    // 共有"はい・いいえ"
    MENU_SHARE_YES,       //!< はい
    MENU_SHARE_NO,        //!< いいえ
    // 共有UGC"はい・いいえ"
    MENU_SHARE_UGC_YES,   //!< はい
    MENU_SHARE_UGC_NO,    //!< いいえ
  };

  static const u32 PHOTO_MENU_MAX = 5;  //!< 写真メニュー項目数

  //! ビューワ用ワーク
  struct VIEWER_WORK
  {
    u32 seq;    //!< シーケンス
    u16 req;    //!< リクエスト
    u16 data;   //!< 読み込んだデータ
  };

  static const u16 VIEWER_REQUEST_NONE = 0xffff;  //!< ビューワへのリクエストなし

  static const u32 INFOWIN_PAGE_MAX = 2;  //!< インフォウィンドウのページ数
  static const u32 INFOWIN_STR_LEN = 18*10*3;   //!< インフォウィンドウに表示する文字列長 ( 18文字x10行xローカライズ対応 )

  static const u32 SDCARD_INIT_MSG_WAIT = 45;   //!< SDカード確認メッセージ表示ウェイト ( 1.5秒 )


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  MyAlbumFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~MyAlbumFrame();

  //-----------------------------------------------------------------------------
  /**
   * @func    Setup
   * @brief   パラメータセット
   * @date    2017.01.25
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
   * @date    2017.01.25
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result InitFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFunc
   * @brief   フレーム終了
   * @date    2017.01.25
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result EndFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateFunc
   * @brief   フレーム更新
   * @date    2017.01.25
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   フレーム描画
   * @date    2017.01.25
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
   * @date    2017.02.09
   */
  //-----------------------------------------------------------------------------
  void GetGameData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetInputExculusiveKeyMode
   * @brief   UIViewのボタンアニメ中の処理モードを取得
   * @date    2017.02.01
   */
  //-----------------------------------------------------------------------------
  void GetInputExculusiveKeyMode(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetInputExculusiveKeyMode
   * @brief   UIViewのボタンアニメ中の処理モードを設定
   * @date    2017.02.01
   *
   * @param   flg   true = 排他制御ON
   */
  //-----------------------------------------------------------------------------
  void SetInputExculusiveKeyMode( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateLocalHeap
   * @brief   ローカルヒープ生成
   * @date    2017.02.07
   */
  //-----------------------------------------------------------------------------
  void CreateLocalHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteLocalHeap
   * @brief   ローカルヒープ削除
   * @date    2017.02.07
   */
  //-----------------------------------------------------------------------------
  void DeleteLocalHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePhotoData
   * @brief   写真データクラス生成
   * @date    2017.02.08
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreatePhotoData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeletePhotoData
   * @brief   写真データクラス生成
   * @date    2017.02.08
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeletePhotoData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadPhotoData
   * @brief   写真データ読み込み
   * @date    2017.03.13
   *
   * @return  処理結果
   */
  //-----------------------------------------------------------------------------
  MyAlbumPhotoData::SDCardAccessResult LoadPhotoData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateDraw
   * @brief   描画クラス生成
   * @date    2017.01.25
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
   * @date    2017.01.25
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteDraw(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteStudioViewer
   * @brief   上画面クラス削除
   * @date    2017.02.07
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteStudioViewer(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuCursor
   * @brief   メニューカーソル生成
   * @date    2017.01.27
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
   * @date    2017.01.27
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMsgMenuView
   * @brief   メッセージメニュー生成
   * @date    2017.01.27
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateMsgMenuView(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMsgMenuView
   * @brief   メッセージメニュー削除
   * @date    2017.01.27
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMsgMenuView(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateInfoWindow
   * @brief   インフォウィンドウ生成
   * @date    2017.02.17
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateInfoWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteInfoWindow
   * @brief   インフォウィンドウ削除
   * @date    2017.02.17
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteInfoWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadLayoutResource
   * @brief   レイアウトリソース読み込み
   * @date    2017.01.25
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
   * @date    2017.01.25
   */
  //-----------------------------------------------------------------------------
  void UnloadLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadMessage
   * @brief   メッセージ読み込み
   * @date    2017.01.25
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
   * @date    2017.01.25
   */
  //-----------------------------------------------------------------------------
  void UnloadMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFade
   * @brief   初期フェード設定
   * @date    2017.01.25
   */
  //-----------------------------------------------------------------------------
  void InitFade(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFadeIn
   * @brief   フェードイン開始
   * @date    2017.01.25
   */
  //-----------------------------------------------------------------------------
  void SetFadeIn(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFadeOut
   * @brief   フェードアウト開始
   * @date    2017.01.25
   */
  //-----------------------------------------------------------------------------
  void SetFadeOut(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsFadeEnd
   * @brief   フェード終了チェック
   * @date    2017.01.25
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsFadeEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeSequence
   * @brief   シーケンス切り替え
   * @date    2017.01.25
   *
   * @param   seq   シーケンス
   */
  //-----------------------------------------------------------------------------
  void ChangeSequence( u32 seq );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeEndSequence
   * @brief   終了シーケンスへ切り替え
   * @date    2017.02.17
   *
   * @param   mode  終了モード
   */
  //-----------------------------------------------------------------------------
  void ChangeEndSequence( EndMode mode );

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqStart
   * @brief   アップデート：開始処理
   * @date    2017.01.25
   */
  //-----------------------------------------------------------------------------
  void Update_SeqStart(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqWaitMessage
   * @brief   アップデート：メッセージ終了待ち
   * @date    2017.01.30
   */
  //-----------------------------------------------------------------------------
  void Update_SeqWaitMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqPhotoDelete
   * @brief   アップデート：写真削除
   * @date    2017.01.30
   */
  //-----------------------------------------------------------------------------
  void Update_SeqPhotoDelete(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqPhotoSave
   * @brief   アップデート：写真保存
   * @date    2017.02.14
   */
  //-----------------------------------------------------------------------------
  void Update_SeqPhotoSave(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqThumbnailError
   * @brief   アップデート：サムネイル読み込みエラー
   * @date    2017.02.17
   */
  //-----------------------------------------------------------------------------
  void Update_SeqThumbnailError(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqEnd
   * @brief   アップデート：終了処理
   * @date    2017.01.25
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Update_SeqEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutPhotoMenu
   * @brief   写真メニュー表示
   * @date    2017.01.27
   */
  //-----------------------------------------------------------------------------
  void PutPhotoMenu(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutYesNoMenu
   * @brief   "はい・いいえ"メニュー表示
   * @date    2017.02.06
   */
  //-----------------------------------------------------------------------------
  void PutYesNoMenu( u32 msg );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutDeleteMenu
   * @brief   写真削除メニュー表示
   * @date    2017.01.30
   */
  //-----------------------------------------------------------------------------
  void PutDeleteMenu(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutShareMenu
   * @brief   写真共有メニュー表示
   * @date    2017.02.06
   */
  //-----------------------------------------------------------------------------
  void PutShareMenu(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutShareUgcMenu
   * @brief   写真共有UGCメニュー表示
   * @date    2017.02.09
   *
   * @param   is_button   true = 共有ボタンから
   */
  //-----------------------------------------------------------------------------
  void PutShareUgcMenu( bool is_button );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetStreamingMessage
   * @brief   送りメッセージ開始
   * @date    2017.04.14
   *
   * @param   msg   メッセージID
   */
  //-----------------------------------------------------------------------------
  void SetStreamingMessage( u32 msg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetStreamingMessageExpand
   * @brief   送りメッセージ開始（展開版）
   * @date    2017.04.14
   *
   * @param   msg   メッセージID
   * @param   val   Register[0]に入れる値
   * @param   keta  valの桁
   */
  //-----------------------------------------------------------------------------
  void SetStreamingMessageExpand( u32 msg, u32 val, u32 keta );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartMessage
   * @brief   メッセージ開始
   * @date    2017.01.30
   *
   * @param   msg   メッセージID
   */
  //-----------------------------------------------------------------------------
  void StartMessage( u32 msg );

  //-----------------------------------------------------------------------------
  /**
   * @func    HideMessageWindow
   * @brief   メッセージウィンドウを非表示にする
   * @date    2017.04.14
   */
  //-----------------------------------------------------------------------------
  void HideMessageWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndMultiDelete
   * @brief   複数削除モード終了
   * @date    2017.01.31
   */
  //-----------------------------------------------------------------------------
  void EndMultiDelete(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DecideDelete
   * @brief   削除決定
   * @date    2017.02.06
   *
   * @retval  true  = 削除可能
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DecideDelete(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsSave
   * @brief   セーブするか（全体チェック）
   * @date    2017.02.14
   *
   * @retval  true  = する
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsSave(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsSave
   * @brief   セーブするか（個別チェック）
   * @date    2017.02.14
   *
   * @param   index   バッファのインデックス
   *
   * @retval  true  = する
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsSave( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateRecoverData
   * @brief   画面復帰用データ生成
   * @date    2017.02.14
   */
  //-----------------------------------------------------------------------------
  void CreateRecoverData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRecoverData
   * @brief   画面復帰データ設定
   * @date    2017.02.14
   */
  //-----------------------------------------------------------------------------
  void SetRecoverData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitViewerRequest
   * @brief   上画面ビューワリクエストデータ初期設定
   * @date    2017.02.15
   */
  //-----------------------------------------------------------------------------
  void InitViewerRequest(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetViewerRequest
   * @brief   上画面ビューワリクエスト
   * @date    2017.02.15
   *
   * @param   pos   カーソル位置
   */
  //-----------------------------------------------------------------------------
  void SetViewerRequest( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateViewer
   * @brief   上画面ビューワ更新
   * @date    2017.02.15
   */
  //-----------------------------------------------------------------------------
  void UpdateViewer(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsViewerTerminate
   * @brief   上画面ビューワを削除可能か
   * @date    2017.02.16
   *
   * @retval  true  = 可
   * @retval  false = 不可
   */
  //-----------------------------------------------------------------------------
  bool IsViewerTerminate(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleViewer
   * @brief   上画面ビューワ表示切り替え
   * @date    2017.03.17
   *
   * @param   flg   true = 非表示, false = 表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleViewer( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckLoadThumbnail
   * @brief   サムネイル読み込み中にエラーが発生したかをチェック
   * @date    2017.02.17
   */
  //-----------------------------------------------------------------------------
  void CheckLoadThumbnail(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartInfoWindow
   * @brief   インフォウィンドウ開始
   * @date    2017.02.20
   */
  //-----------------------------------------------------------------------------
  void StartInfoWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePaneListView
   * @brief   ペインリストビュー生成
   * @date    2017.02.20
   */
  //-----------------------------------------------------------------------------
  void CreatePaneListView(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeletePaneListView
   * @brief   ペインリストビュー削除
   * @date    2017.02.20
   */
  //-----------------------------------------------------------------------------
  void DeletePaneListView(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetListTopInputEnabled
   * @brief   ソートトップリストの入力切り替え
   * @date    2017.02.20
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetListTopInputEnabled( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetListCoreInputEnabled
   * @brief   ソート項目別リストの入力切り替え
   * @date    2017.02.20
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetListCoreInputEnabled( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    ReturnTopList
   * @brief   項目別リストからトップリストへ戻る
   * @date    2017.02.21
   *
   * @param   is_cancel   true = キャンセル
   */
  //-----------------------------------------------------------------------------
  void ReturnTopList( bool is_cancel );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateSortBinary
   * @brief   ソートデータ生成
   * @date    2017.02.22
   */
  //-----------------------------------------------------------------------------
  bool CreateSortBinary(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteSortBinary
   * @brief   ソートデータ削除
   * @date    2017.02.22
   */
  //-----------------------------------------------------------------------------
  void DeleteSortBinary(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CopySortData
   * @brief   ソートデータを退避
   * @date    2017.02.24
   */
  //-----------------------------------------------------------------------------
  void CopySortData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsSetSortData
   * @brief   ソートデータが設定されているか
   * @date    2017.02.24
   *
   * @retval  true  = 設定されている
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsSetSortData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsChangeSortData
   * @brief   ソートデータが変更されたか
   * @date    2017.02.24
   *
   * @retval  true  = 変更された
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
//  bool IsChangeSortData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsUpperViewerOn
   * @brief   上画面の写真ビューワを表示できるか
   * @date    2017.02.22
   *
   * @retval  true  = 可
   * @retval  false = 不可
   */
  //-----------------------------------------------------------------------------
  bool IsUpperViewerOn(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpperViewerOn
   * @brief   上画面の写真ビューワを表示
   * @date    2017.02.23
   */
  //-----------------------------------------------------------------------------
  void UpperViewerOn(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SelectEdit
   * @brief   "編集する"選択処理
   * @date    2017.03.06
   */
  //-----------------------------------------------------------------------------
  void SelectEdit(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OnAction
   * @brief   入力時のリスナー
   * @date    2017.01.25
   *
   * @param   id    動作ID
   *
   * @note  override MyAlbumDraw::Listener
   */
  //-----------------------------------------------------------------------------
  virtual void OnAction( MyAlbumDraw::Listener::ActionID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnEvent
   * @brief   メッセージ表示リスナー
   * @date    2017.01.27
   *
   * @param   event   イベントコード
   *
   * @note  override App::FinderStudio::Static::MessageMenuViewListener
   */
  //-----------------------------------------------------------------------------
  virtual void OnEvent( const App::Tool::MessageMenuView::IEventHandler::EventCode event );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnSelectMenu
   * @brief   メニュー選択時のリスナー
   * @date    2017.01.27
   *
   * @param   index   項目番号
   *
   * @note  override App::FinderStudio::Static::MessageMenuViewListener
   */
  //-----------------------------------------------------------------------------
  virtual void OnSelectMenu( const u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnClose
   * @brief   インフォウィンドウを閉じたときのリスナー
   * @date    2017.02.17
   *
   * @note  override app::tool::InfoWindowBookTypeListener
   */
  //-----------------------------------------------------------------------------
  virtual void OnClose(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OnChangePage
   * @brief   インフォウィンドウのページ切り替え時のリスナー
   * @date    2017.02.17
   *
   * @param   pageNo  これから表示するページ番号
   *
   * @note  override app::tool::InfoWindowBookTypeListener
   */
  //-----------------------------------------------------------------------------
  virtual void OnChangePage( u32 pageNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    PaneListItemSelectListener
   * @brief   ペインリスト決定時に呼ばれるリスナー
   * @date    2017.02.20
   *
   * @param   pos   カーソル位置
   *
   * @note  override app::tool::PaneListView::Listener
   */
  //-----------------------------------------------------------------------------
  virtual void PaneListItemSelectListener( u32 pos );


private:
  APP_PARAM * m_appParam;                               //!< 外部設定パラメータ
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ

  Savedata::Misc * m_misc;  //!< 未分類セーブデータ
  Savedata::FinderStudioSave * m_finderStudioSave;  //!< ファインダースタジオのセーブデータ

  gfl2::heap::HeapBase * m_pViewerHeap;   //!< 上画面ビューワ用ヒープ

  void * m_pLytResBuff;     //!< 下画面用レイアウトリソース
  void * m_pMsgBuff;        //!< メッセージバッファ
  void * m_pBgMsgBuff;      //!< 背景リスト項目メッセージバッファ
  void * m_pInitialMsgBuf;  //!< イニシャルメッセージバッファ

  bool m_isUIViewInputExclusiveKey;   //!< UIViewマネージャのボタン監視モード

  MyAlbumDraw * m_pDraw;  //!< 描画クラス

  app::tool::MenuCursor * m_pMenuCursor;  //!< メニューカーソル
  void * m_pMenuCursorBuffer;             //!< メニューカーソルリソースバッファ

  App::FinderStudio::Static::MessageMenuView * m_pMsgMenuView;  //!< メッセージメニュー
  bool m_isEndMessage;    //!< true = メッセージ終了
  bool m_isEnableWindow;  //!< true = ウィンドウ表示中
  MenuItemID m_menuItemID[PHOTO_MENU_MAX];  //!< メニュー項目ID

  bool m_isUgcButton;   //!< UGCメッセージ表示時に共有ボタンから表示されたか

  App::FinderStudio::Viewer::FinderStudioViewer * m_pViewer;  //!< 上画面ビューワ
  VIEWER_WORK m_viewerWork;

  MyAlbumPhotoData * m_pPhotoData;  //!< 写真データ

  app::tool::InfoWindowDataManager * m_pInfoWinManager;   //!< インフォウィンドウマネージャ
  app::tool::InfoWindowBookType * m_pInfoWinBook;         //!< インフォウィンドウ（ブックタイプ）
  gfl2::str::StrBuf * m_pInfoWinStrBuf;                   //!< インフォウィンドウで使用する文字列長

  app::tool::PaneListView * m_pListTop;   //!< ソートトップリスト
  app::tool::PaneListView * m_pListCore;  //!< ソート項目別リスト
  SORT_DATA m_copySortData;               //!< ソートデータ退避場所

  app::sort::StringMonsName * m_pSort;    //!< ポケモンソート

  u16 m_mainSeq;  //!< メインシーケンス
  u16 m_subSeq;   //!< サブシーケンス
};


GFL_NAMESPACE_END(MyAlbum)
GFL_NAMESPACE_END(App)


#endif  // MYALBUM_FRAME_H_INCLUDED
