#if PM_DEBUG
//=============================================================================
/**
 * @file    LauncherSub.h
 * @brief   デバッグ用ランチャー処理サブルーチン
 * @author  Hiroyuki Nakamura
 * @date    2015.02.05
 */
//=============================================================================
#if !defined( LAUNCHERSUB_H_INCLUDED )
#define LAUNCHERSUB_H_INCLUDED

#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <heap/include/gfl2_NwAllocator.h>
#include <layout/include/gfl2_Layout.h>
#include <layout/include/gfl2_LayoutRenderPath.h>
#include <str/include/gfl2_Str.h>
#include <ui/include/gfl2_UI.h>

#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_AppLytAccessor.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(launcher)


//=============================================================================
/**
 * @class   LauncherSub
 * @brief   ランチャーサブルーチンクラス
 * @date    2015.02.05
 */
//=============================================================================
class LauncherSub
{
private:
  static const u32 ITEM_H_MAX = 2;                        //!< 表示中の横項目数最大値
  static const u32 ITEM_V_MAX = 12;                       //!< 表示中の縦項目数最大値
  static const u32 ITEM_MAX = ITEM_H_MAX * ITEM_V_MAX;    //!< 表示の項目数最大値

  static const u32 SCROLL_MAX = 4;    //!< スクロールカウンタ最大値

  static const u32 LIST_ITEM_ANM_MAX = 2;   //!< 項目部品レイアウトアニメ数

  //! レイアウトID
  enum LayoutID
  {
    LYTID_UPPER = 0,  //!< 上画面
    LYTID_LOWER,      //!< 下画面
    LYTID_MAX
  };

  //! リソースID
  enum ResourceID
  {
    RESID_BASE = 0,   //!< 画面用リソース
    RESID_PARTS,      //!< 部品リソース
    RESID_MAX
  };

public:
  class EventListener {
  public:
    virtual void OnPrintString( u32 pos, gfl2::str::StrBuf * strBuf ) {}
  };

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap            ヒープ
   * @param   dev_heap        デバイスヒープ
   * @param   allocator       アロケータ
   * @param   dev_allocator   デバイスアロケータ
   */
  //-----------------------------------------------------------------------------
  LauncherSub( gfl2::heap::HeapBase * heap, gfl2::heap::HeapBase * dev_heap, gfl2::heap::NwAllocator * allocator, gfl2::heap::NwAllocator * dev_allocator );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~LauncherSub();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetupUI
   * @brief   UI関連作成
   * @date    2015.02.05
   */
  //-----------------------------------------------------------------------------
  void SetupUI(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsButtonTrigger
   * @brief   ボタンが押されたか
   * @date    2015.02.05
   *
   * @param   button  チェックするボタン
   *
   * @retval  true  = 押された
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsButtonTrigger( u32 button );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateGraphicSustem
   * @brief   グラフィックシステム作成
   * @date    2015.02.05
   */
  //-----------------------------------------------------------------------------
  void CreateGraphicsSystem(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteGraphicsSustem
   * @brief   グラフィックシステム削除
   * @date    2015.02.05
   */
  //-----------------------------------------------------------------------------
  void DeleteGraphicsSystem(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateLayout
   * @brief   レイアウト作成
   * @date    2015.02.05
   */
  //-----------------------------------------------------------------------------
  void CreateLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PreDraw
   * @brief   描画下準備
   * @date    2015.07.09
   */
  //-----------------------------------------------------------------------------
  void PreDraw(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteLayout
   * @brief   レイアウト削除
   * @date    2015.02.05
   */
  //-----------------------------------------------------------------------------
  void DeleteLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateLayout
   * @brief   レイアウト更新
   * @date    2015.02.05
   */
  //-----------------------------------------------------------------------------
  void UpdateLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawLayout
   * @brief   レイアウト描画
   * @date    2015.02.05
   *
   * @param   displayNo   描画対象画面
   */
  //-----------------------------------------------------------------------------
  void DrawLayout( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsLayoutDrawing
   * @brief   レイアウト描画中か
   * @date    2015.08.21
   *
   * @retval  true  = 描画中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsLayoutDrawing(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartAnime
   * @brief   アニメ開始
   * @date    2015.02.05
   *
   * @param   lyt   レイアウトID
   * @param   anm   アニメID
   */
  //-----------------------------------------------------------------------------
  void StartAnime( LayoutID lyt, u32 anm );

  //-----------------------------------------------------------------------------
  /**
   * @func    CalcCursorPos
   * @brief   全体のカーソル位置を計算して取得
   * @date    2015.02.05
   *
   * @param   page  ページ番号
   * @param   pos   カーソル表示位置
   *
   * @return  全体のカーソル位置
   */
  //-----------------------------------------------------------------------------
  u32 CalcCursorPos( s16 page, s16 pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    MoveCursor
   * @brief   カーソル移動
   * @date    2015.02.05
   *
   * @param   page      ページ番号
   * @param   pos       カーソル表示位置
   * @param   item_max  項目最大数
   *
   * @retval  true  = ページスクロール
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool MoveCursor( s16 * page, s16 * pos, u32 item_max );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetCursorAnime
   * @brief   カーソルアニメ設定
   * @date    2015.02.05
   *
   * @param   pos   カーソル表示位置
   * @param   mode  ON = true, OFF = false
   *
   * @retval  true  = ページスクロール
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  void SetCursorAnime( u32 pos, bool on );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsEndScrollAnime
   * @brief   スクロールアニメが終了したか
   * @date    2015.02.05
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsEndScrollAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ResetListPos
   * @brief   リスト表示位置を再設定
   * @date    2015.02.05
   *
   * @param   cursor_pos  カーソル表示位置
   */
  //-----------------------------------------------------------------------------
  void ResetListPos( u32 cursor_pos );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMessage
   * @brief   メッセージデータ作成
   * @date    2015.02.18
   *
   * @param   id        アークデータID
   * @param   list      項目データ
   * @param   list_max  項目データサイズ
   */
  //-----------------------------------------------------------------------------
  void CreateMessage( gfl2::fs::ArcFile::ARCDATID id, const u32 * list, u32 list_max );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMessage
   * @brief   メッセージデータ削除
   * @date    2015.02.18
   */
  //-----------------------------------------------------------------------------
  void DeleteMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateStrBuf
   * @brief   文字列バッファ作成
   * @date    2015.02.18
   */
  //-----------------------------------------------------------------------------
  void CreateStrBuf(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteStrBuf
   * @brief   文字列バッファ削除
   * @date    2015.02.18
   */
  //-----------------------------------------------------------------------------
  void DeleteStrBuf(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitPrintItem
   * @brief   初期項目書き込み
   * @date    2015.02.18
   */
  //-----------------------------------------------------------------------------
  void InitPrintItem(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetEventListner
   * @brief   このクラスのイベントリスナを登録・解除（解除の場合はNULLを渡すこと）
   *
   * @param   EventListener    イベントリスナ
   */
  //-----------------------------------------------------------------------------
  void SetEventListner( EventListener * listenr ) { m_eventListener = listenr; }

private:
  void CreatePartsAnime(
        app::util::AppLytAccessor * accessor,
        gfl2::lyt::LytWk * lytwk,
        const gfl2::lyt::LytMultiResID * res_id,
        u32 lytres_id,
        u32 * start_anm_no );

  //-----------------------------------------------------------------------------
  /**
   * @func    StopAnime
   * @brief   アニメ停止
   * @date    2015.02.05
   *
   * @param   lyt   レイアウトID
   * @param   anm   アニメID
   */
  //-----------------------------------------------------------------------------
  void StopAnime( LayoutID lyt, u32 anm );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsAnimeEnd
   * @brief   アニメ終了チェック
   * @date    2015.02.05
   *
   * @param   lyt   レイアウトID
   * @param   anm   アニメID
   */
  //-----------------------------------------------------------------------------
  bool IsAnimeEnd( LayoutID lyt, u32 anm );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitScroll
   * @brief   ページスクロール初期設定
   * @date    2015.02.05
   *
   * @param   page        ページ
   * @param   cursor_pos  カーソル表示位置
   * @param   mv          スクロール値
   */
  //-----------------------------------------------------------------------------
  void InitScroll( s16 page, s16 cursor_pos, s16 mv );

  //-----------------------------------------------------------------------------
  /**
   * @func    PrintItem
   * @brief   項目書き込み
   * @date    2015.02.18
   *
   * @param   page    ページ番号
   * @param   group   書き込むペインのグループ
   */
  //-----------------------------------------------------------------------------
  void PrintItem( u32 page, u32 group );

private:
  app::util::Heap m_heap;   //!< ヒープ

  gfl2::ui::DeviceManager * m_pUiDeviceManager;   //!< UIデバイスマネージャ
  gfl2::ui::VectorDevice * m_pKey;                //!< 十字キー
  gfl2::ui::Button * m_pButton;                   //!< ボタン

  app::util::AppRenderingManager * m_pRenderingManager; //!< 描画マネージャ
  gfl2::lyt::LytSys * m_pLayoutSystem;            //!< レイアウトシステム
  gfl2::lyt::LytWk * m_pLytWk[LYTID_MAX];         //!< レイアウトワーク
  gfl2::lyt::Env * m_pUpperLayoutEnv;             //!< 上画面のレイアウト環境
  gfl2::lyt::Env * m_pLowerLayoutEnv;             //!< 下画面のレイアウト環境
  gfl2::lyt::LytMultiResID * m_pLytMultiResID;    //!< リソースID
  gfl2::str::MsgData * m_pMsgData;                //!< メッセージデータ
  gfl2::str::StrBuf * m_pStrBuf;                  //!< 文字列バッファ

  void * m_pBaseLayoutBuff;

  const u32 * m_itemStr;  //!< 項目テーブル
  u32 m_itemStrMax;       //!< 項目テーブルサイズ

  s32 m_scrollCount;  //!< ページ用ペインスクロールカウンタ
  EventListener * m_eventListener; //!< このクラスのイベント
};


GFL_NAMESPACE_END(launcher)
GFL_NAMESPACE_END(debug)

#endif  // LAUNCHERSUB_H_INCLUDED

#endif  // PM_DEBUG
