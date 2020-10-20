#if PM_DEBUG
//=============================================================================
/**
 * @file    PokeIconViewProc.h
 * @brief   ポケモンアイコン確認画面Proc処理
 * @author  Hiroyuki Nakamura
 * @date    2016.04.15
 */
//=============================================================================
#if !defined( POKEICON_VIEW_PROC_H_INCLUDED )
#define POKEICON_VIEW_PROC_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
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


GFL_NAMESPACE_BEGIN(Debug)

// 前方宣言
class PokeIconViewFrame;

//=============================================================================
/**
 * @class PokeIconViewProc
 * @brief ポケモンアイコン確認画面Procクラス
 * @date  2016.04.16
 */
//=============================================================================
class PokeIconViewProc
  : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeIconViewProc);  // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  PokeIconViewProc(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~PokeIconViewProc();

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFunc
   * @brief   Proc初期化処理
   * @date    2016.04.15
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
   * @date    2016.04.15
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
   * @date    2016.04.15
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
   * @date    2016.04.15
   *
   * @param   pManager    プロセス管理マネージャー
   */
  //-----------------------------------------------------------------------------
  virtual void PreDrawFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   Proc描画処理
   * @date    2016.04.15
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
   * @date    2016.04.15
   */
  //-----------------------------------------------------------------------------
  void CreateHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteHeap
   * @brief   ヒープ削除
   * @date    2016.04.15
   */
  //-----------------------------------------------------------------------------
  void DeleteHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateRenderManager
   * @brief   描画マネージャ生成
   * @date    2016.04.15
   */
  //-----------------------------------------------------------------------------
  void CreateRenderManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteRenderManager
   * @brief   描画マネージャ削除
   * @date    2016.04.15
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteRenderManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateFrame
   * @brief   フレーム生成
   * @date    2016.04.15
   */
  //-----------------------------------------------------------------------------
  void CreateFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteFrame
   * @brief   フレーム削除
   * @date    2016.04.15
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteFrame(void);


private:
  gfl2::heap::HeapBase * m_pManagerHeap;    //!< マネージャ用ヒープ

  app::util::Heap m_heap;   //!< ローカルヒープ

  app::util::AppRenderingManager * m_pRenderingManager;   //!< 描画マネージャ
  applib::frame::Manager * m_pFrameManager;               //!< フレームマネージャ

  PokeIconViewFrame * m_pFrame;  //!< フレーム
};


GFL_NAMESPACE_END(Debug)

#endif  // POKEICON_VIEW_PROC_H_INCLUDED

#endif  // PM_DEBUG
