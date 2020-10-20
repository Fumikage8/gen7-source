#if PM_DEBUG
//=============================================================================
/**
 * @file    ItemMakeFrame.h
 * @brief   デバッグ用アイテム作成フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2015.05.26
 */
//=============================================================================
#if !defined( DEBUG_ITEM_MAKE_FRAME_H_INCLUDED )
#define DEBUG_ITEM_MAKE_FRAME_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Str.h>
// niji
#include "AppLib/include/Frame/AppFrameCellSuper.h"
// module
#include "ItemMakeListener.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(Savedata)
class MyItem;
GFL_NAMESPACE_END(Savedata)

GFL_NAMESPACE_BEGIN(debug)
class NumInput;
GFL_NAMESPACE_END(debug)


GFL_NAMESPACE_BEGIN(Debug)

// 前方宣言
class ItemMakeDraw;

//=============================================================================
/**
 * @class ItemMakeFrame
 * @brief デバッグ用アイテム作成フレームクラス
 * @date  2015.05.26
 */
//=============================================================================
class ItemMakeFrame
  : public applib::frame::CellSuper
  , public ItemMakeListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ItemMakeFrame);   // コピーコンストラクタ＋代入禁止

public:
  enum ExitID
  {
    EXIT_NONE = 0,
    EXIT_END,
    EXIT_CALL_KEYBORD,
  };


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  ItemMakeFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~ItemMakeFrame();

  //-----------------------------------------------------------------------------
  /**
   * @func    Setup
   * @brief   パラメータセット
   * @date    2015.05.26
   *
   * @parma   heap        ヒープ
   * @param   render_man  描画マネージャー
   * @param   exit_id     終了ID格納場所
   * @param   item_id     初期道具ID
   * @param   item_num    個数格納場所
   * @param   item_name   道具名メッセージデータ
   */
  //-----------------------------------------------------------------------------
  void Setup( app::util::Heap * heap, app::util::AppRenderingManager * render_man, u32 * exit_id, u16 * item_id, u16 * item_num, gfl2::str::MsgData * item_name );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFunc
   * @brief   フレーム初期化
   * @date    2015.05.26
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result InitFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFunc
   * @brief   フレーム終了
   * @date    2015.05.26
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result EndFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateFunc
   * @brief   フレーム更新
   * @date    2015.05.26
   *
   * @return  applib::frame::Result
   */
  //-----------------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   フレーム描画
   * @date    2015.05.26
   *
   * @param   no  描画先のディスプレイ番号
   */
  //-----------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    GetGameData
   * @brief   ゲームデータ関連取得
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  void GetGameData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFade
   * @brief   初期フェード設定
   * @date    2015.11.17
   */
  //-----------------------------------------------------------------------------
  void InitFade(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFadeIn
   * @brief   フェードイン開始
   * @date    2015.11.17
   */
  //-----------------------------------------------------------------------------
  void SetFadeIn(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFadeOut
   * @brief   フェードアウト開始
   * @date    2015.11.17
   */
  //-----------------------------------------------------------------------------
  void SetFadeOut(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsFadeEnd
   * @brief   フェード終了チェック
   * @date    2015.11.17
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsFadeEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    AddItem
   * @brief   道具追加（個別）
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  void AddItem(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    AddItem
   * @brief   全道具追加
   * @date    2015.05.26
   *
   * @param   num   追加する個数
   */
  //-----------------------------------------------------------------------------
  void AddItem( u32 num );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateDraw
   * @brief   描画クラス生成
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  void CreateDraw(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteDraw
   * @brief   描画クラス削除
   * @date    2015.05.26
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
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  void LoadLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UnloadLayourResource
   * @brief   レイアウトリソース削除
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  void UnloadLayoutResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateNumInput
   * @brief   数値入力生成
   * @date    2015.05.27
   */
  //-----------------------------------------------------------------------------
  void CreateNumInput(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateItemIDInput
   * @brief   数値入力生成（道具ID入力）
   * @date    2016.04.11
   */
  //-----------------------------------------------------------------------------
  void CreateItemIDInput(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteNumInput
   * @brief   数値入力削除
   * @date    2015.05.27
   */
  //-----------------------------------------------------------------------------
  void DeleteNumInput(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateNumInput
   * @brief   数値入力更新
   * @date    2015.05.27
   */
  //-----------------------------------------------------------------------------
  void UpdateNumInput(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateItemIDInput
   * @brief   道具ID入力更新
   * @date    2016.04.11
   */
  //-----------------------------------------------------------------------------
  void UpdateItemIDInput(void);

  //--------------------------------------------------------------------------
  /**
   * @func    OnAction
   * @brief   入力リスナー
   * @date    2015.05.26
   *
   * @param   id  入力ID
   */
  //--------------------------------------------------------------------------
  virtual void OnAction( u32 id );


private:
  app::util::Heap * m_heap;                               //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;    //!< 描画マネージャ
  u32 * m_exitID;     //!< 終了ID格納場所
  u16 * m_itemNum;    //!< 個数格納場所
  u16 * m_itemID;     //!< 道具ID

  gfl2::str::MsgData * m_itemName;  //!< 道具名メッセージデータ

  Savedata::MyItem * m_myItem;  //!< 手持ちアイテムセーブデータ

  void * m_pLytResBuff;   //!< レイアウトリソースバッファ

  ItemMakeDraw * m_pDraw;   //!< 描画クラス

  debug::NumInput * m_pNumInput;  //!< 数値入力
  bool m_isNumInputDraw;          //!< 数値入力描画制御フラグ true = 許可
};


GFL_NAMESPACE_END(Debug)

#endif  // DEBUG_ITEM_MAKE_FRAME_H_INCLUDED

#endif  // PM_DEBUG
