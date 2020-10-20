#if PM_DEBUG
//=============================================================================
/**
 * @file    ItemMakeProc.h
 * @brief   デバッグ用アイテム作成処理
 * @author  Hiroyuki Nakamura
 * @date    2015.05.26
 */
//=============================================================================
#if !defined( DEBUG_ITEM_MAKE_PROC_H_INCLUDED )
#define DEBUG_ITEM_MAKE_PROC_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Str.h>
// niji
#include "GameSys/include/GameProc.h"
#include "AppLib/include/Util/app_util_Heap.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(debug)
class KeyboardProc;
GFL_NAMESPACE_END(debug)


GFL_NAMESPACE_BEGIN(Debug)

// 前方宣言
class ItemMakeFrame;

//=============================================================================
/**
 * @class ItemMakeProc
 * @brief デバッグ用アイテム作成Procクラス
 * @date  2015.05.26
 */
//=============================================================================
class ItemMakeProc
  : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(ItemMakeProc);  // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  ItemMakeProc(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~ItemMakeProc();

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFunc
   * @brief   Proc初期化処理
   * @date    2015.05.26
   *
   * @param   pManager    プロセス管理マネージャー
   *
   * @return  gfl2::proc::Result
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFunc
   * @brief   Proc終了処理
   * @date    2015.05.26
   *
   * @param   pManager    プロセス管理マネージャー
   *
   * @return  gfl2::proc::Result
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateFunc
   * @brief   Procアップデート処理
   * @date    2015.05.26
   *
   * @param   pManager    プロセス管理マネージャー
   *
   * @return  gfl2::proc::Result
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    PreDrawFunc
   * @brief   Proc描画下準備
   * @date    2015.07.09
   *
   * @param   pManager    プロセス管理マネージャー
   */
  //-----------------------------------------------------------------------------
  virtual void PreDrawFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   Proc描画処理
   * @date    2015.05.26
   *
   * @param   pManager    プロセス管理マネージャー
   * @param   displayNo   描画対象画面
   */
  //-----------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::proc::Manager * pManager, gfl2::gfx::CtrDisplayNo displayNo );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    CreateHeap
   * @brief   ヒープ生成
   * @date    2015.07.09
   */
  //-----------------------------------------------------------------------------
  void CreateHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteHeap
   * @brief   ヒープ削除
   * @date    2015.07.09
   */
  //-----------------------------------------------------------------------------
  void DeleteHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateRenderManager
   * @brief   描画マネージャ生成
   * @date    2015.07.09
   */
  //-----------------------------------------------------------------------------
  void CreateRenderManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteRenderManager
   * @brief   描画マネージャ削除
   * @date    2015.07.09
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteRenderManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateMainFrame
   * @brief   フレーム生成
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  void CreateMainFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteMainFrame
   * @brief   フレーム削除
   * @date    2015.05.26
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteMainFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateKeybord
   * @brief   キーボード生成
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  void CreateKeybord(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteKeybord
   * @brief   キーボード削除
   * @date    2015.05.26
   */
  //-----------------------------------------------------------------------------
  void DeleteKeybord(void);


private:
  gfl2::heap::HeapBase * m_pManagerHeap;    //!< マネージャ用ヒープ

  app::util::Heap m_heap;   //!< ローカルヒープ

  app::util::AppRenderingManager * m_pRenderingManager;   //!< 描画マネージャ
  applib::frame::Manager * m_pFrameManager;               //!< フレームマネージャ

  ItemMakeFrame * m_pItemMakeFrame;   //!< 道具生成フレーム
  u32 m_exitID;                       //!< 道具生成フレームの終了ID

  gfl2::str::MsgData * m_pItemName;   //!< 道具名メッセージ
  gfl2::str::StrBuf * m_pStrBuf;      //!< 文字列バッファ
  int m_keyboardIndex;                //!< デバッグキーボードで選択されたインデックス

  u16 m_itemID;   //!< 道具ID
  u16 m_itemNum;  //!< 個数
};


GFL_NAMESPACE_END(Debug)

#endif  // DEBUG_ITEM_MAKE_PROC_H_INCLUDED

#endif  // PM_DEBUG
