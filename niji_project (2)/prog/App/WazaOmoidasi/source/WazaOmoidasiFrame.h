//=============================================================================
/**
 * @file    WazaOmoidasiFrame.h
 * @brief   技思い出し画面フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2015.12.08
 */
//=============================================================================
#if !defined( WAZA_OMOIDASI_FRAME_H_INCLUDED )
#define WAZA_OMOIDASI_FRAME_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Str.h>
// pmlib
#include <pml/include/pmlib.h>
// niji
#include "AppLib/include/Frame/AppFrameCellSuper.h"
#include "AppLib/include/Tool/app_tool_MenuWindowListener.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"
// module
#include "App/WazaOmoidasi/include/WazaOmoidasiAppParam.h"
#include "WazaOmoidasiDraw.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_BEGIN(tool)
class MenuWindow;
class MenuCursor;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(WazaOmoidasi)

// 前方宣言
struct APP_PARAM;

//=============================================================================
/**
 * @class WazaOmoidasiFrame
 * @brief 技思い出し画面フレームクラス
 * @date  2015.12.10
 */
//=============================================================================
class WazaOmoidasiFrame
  : public applib::frame::CellSuper
  , public app::tool::MenuWindowListener
  , public app::tool::PaneListView::Listener
  , public WazaOmoidasiDraw::InputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(WazaOmoidasiFrame);    // コピーコンストラクタ＋代入禁止


private:
  //! "はい・いいえ"の結果
  enum YesNoResult
  {
    YN_RESULT_NONE = 0,   //!< 処理中
    YN_RESULT_YES,        //!< はい
    YN_RESULT_NO,         //!< いいえ
  };


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  WazaOmoidasiFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~WazaOmoidasiFrame();

  //-----------------------------------------------------------------------------
  /**
   * @func    Setup
   * @brief   パラメータセット
   * @date    2015.12.10
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
   * @date    2015.12.10
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result InitFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFunc
   * @brief   フレーム終了
   * @date    2015.12.10
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result EndFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateFunc
   * @brief   フレーム更新
   * @date    2015.12.10
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   フレーム描画
   * @date    2015.12.10
   *
   * @param   no  描画先のディスプレイ番号
   */
  //-----------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    CreateDraw
   * @brief   描画クラス生成
   * @date    2015.12.10
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
   * @date    2015.12.10
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
   * @date    2015.12.10
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
   * @date    2015.12.10
   */
  //-----------------------------------------------------------------------------
  void UnloadLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadMessage
   * @brief   メッセージ読み込み
   * @date    2015.12.10
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
   * @date    2015.12.10
   */
  //-----------------------------------------------------------------------------
  void UnloadMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateWordSetLoader
   * @brief   WordSetLoader生成
   * @date    2015.12.10
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
   * @date    2015.12.10
   */
  //-----------------------------------------------------------------------------
  void DeleteWordSetLoader(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateNumFont
   * @brief   Numフォント生成
   * @date    2015.12.11
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateNumFont(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteNumFont
   * @brief   Numフォント削除
   * @date    2015.12.11
   */
  //-----------------------------------------------------------------------------
  void DeleteNumFont(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuCurosr
   * @brief   メニューカーソル生成
   * @date    2015.12.12
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CreateMenuCurosr(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMenuCursor
   * @brief   メニューカーソル削除
   * @date    2015.12.12
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuCursor(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMenuWindow
   * @brief   メニューウィンドウ生成
   * @date    2015.12.12
   */
  //-----------------------------------------------------------------------------
  void CreateMenuWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMenuWindow
   * @brief   メニューウィンドウ削除
   * @date    2015.12.12
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMenuWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreatePaneListView
   * @brief   ペインリストビュー生成
   * @date    2015.12.12
   */
  //-----------------------------------------------------------------------------
  void CreatePaneListView(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeletePaneListView
   * @brief   ペインリストビュー削除
   * @date    2015.12.12
   */
  //-----------------------------------------------------------------------------
  void DeletePaneListView(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFade
   * @brief   初期フェード設定
   * @date    2015.12.10
   */
  //-----------------------------------------------------------------------------
  void InitFade(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFadeIn
   * @brief   フェードイン開始
   * @date    2015.12.10
   */
  //-----------------------------------------------------------------------------
  void SetFadeIn(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFadeOut
   * @brief   フェードアウト開始
   * @date    2015.12.10
   */
  //-----------------------------------------------------------------------------
  void SetFadeOut(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsFadeEnd
   * @brief   フェード終了チェック
   * @date    2015.12.10
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsFadeEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartYesNo
   * @brief   "はい・いいえ"処理開始
   * @date    2015.12.26
   */
  //-----------------------------------------------------------------------------
  void StartYesNo(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPaneListPos
   * @brief   ペインリストの位置を取得
   * @date    2015.12.26
   */
  //-----------------------------------------------------------------------------
  void GetPaneListPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetPaneListInputEnabled
   * @brief   ペインリストの入力切り替え
   * @date    2016.01.26
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetPaneListInputEnabled( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeSequence
   * @brief   シーケンス切り替え
   * @date    2015.12.10
   *
   * @param   seq   シーケンス
   */
  //-----------------------------------------------------------------------------
  void ChangeSequence( u32 seq );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetEnd
   * @brief   終了設定
   * @date    2015.12.14
   *
   * @param   waza  技番号
   * @param   mode  終了モード
   */
  //-----------------------------------------------------------------------------
  void SetEnd( WazaNo waza, EndMode mode );

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqStart
   * @brief   アップデート：開始処理
   * @date    2015.12.10
   */
  //-----------------------------------------------------------------------------
  void Update_SeqStart(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqDecide
   * @brief   アップデート：技決定処理
   * @date    2015.12.14
   */
  //-----------------------------------------------------------------------------
  void Update_SeqDecide(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqReturnMain
   * @brief   アップデート：メッセージ表示を終了してメインへ戻る
   * @date    2015.12.14
   */
  //-----------------------------------------------------------------------------
  void Update_SeqReturnMain(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqCallWazaSelect
   * @brief   アップデート：技忘れ画面へ
   * @date    2015.12.26
   */
  //-----------------------------------------------------------------------------
  void Update_SeqCallWazaSelect(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqWazaChange
   * @brief   アップデート：技を忘れて覚える
   * @date    2015.12.26
   */
  //-----------------------------------------------------------------------------
  void Update_SeqWazaChange(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqAppCancel
   * @brief   アップデート：思い出し画面キャンセル
   * @date    2015.12.26
   */
  //-----------------------------------------------------------------------------
  void Update_SeqAppCancel(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update_SeqEnd
   * @brief   アップデート：終了処理
   * @date    2015.12.10
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Update_SeqEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    OnAction
   * @brief   入力時のリスナー
   * @date    2015.12.10
   *
   * @param   id    動作ID
   *
   * @note  override WazaOmoidasiDraw::InputListener
   */
  //-----------------------------------------------------------------------------
  virtual void OnAction( WazaOmoidasiDraw::InputListener::ActionID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetWazaNo
   * @brief   技をセット
   * @date    2015.12.26
   *
   * @note  override WazaOmoidasiDraw::InputListener
   */
  //-----------------------------------------------------------------------------
  virtual void SetWazaNo(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PaneListItemSelectListener
   * @brief   ペインリスト決定時に呼ばれるリスナー
   * @date    2015.12.12
   *
   * @param   pos   カーソル位置
   *
   * @note  override app::tool::PaneListView::Listener
   */
  //-----------------------------------------------------------------------------
  virtual void PaneListItemSelectListener( u32 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    SelectMenu
   * @brief   項目選択時のリスナー
   * @date    2015.12.12
   *
   * @param   id  メニュー項目のID
   *
   * @note  override app::tool::MenuWindowListener
   */
  //-----------------------------------------------------------------------------
  virtual void SelectMenu( u32 id );


private:
  APP_PARAM * m_appParam;                               //!< 外部設定パラメータ
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ

  void * m_pLytResBuff;     //!< レイアウトリソース
  void * m_pMsgBuff;        //!< メッセージバッファ
  void * m_pTokuseiMsgBuff; //!< 特性説明メッセージバッファ
  void * m_pWazaMsgBuff;    //!< 技説明メッセージバッファ

  WordSetLoader * m_pWordSetLoader; //!< ワードセットローダー

  WazaOmoidasiDraw * m_pDraw;            //!< 描画クラス（上画面）

  app::tool::MenuWindow * m_pMenuWindow;  //!< メニューウィンドウ
  YesNoResult m_ynResult;                 //!< "はい・いいえ"の結果

  app::tool::MenuCursor * m_pMenuCursor;  //!< メニューカーソル
  void * m_pMenuCursorBuffer;             //!< メニューカーソルバッファ

  app::tool::PaneListView * m_pPaneListView;  //!< ペインリストビュー

  bool m_createNumFont;   //!< Numフォント生成フラグ

  WazaNo m_selectWazaNo;  //!< 選択した技番号

  u16 m_mainSeq;  //!< メインシーケンス
  u16 m_subSeq;   //!< サブシーケンス

  bool m_isUIViewInputExclusiveKey;   //!< UIViewマネージャのボタン監視モード
};


GFL_NAMESPACE_END(WazaOmoidasi)
GFL_NAMESPACE_END(App)


#endif  // WAZA_OMOIDASI_FRAME_H_INCLUDED
