//=============================================================================
/**
 * @file    WazaOmoidasiProc.h
 * @brief   技思い出し画面Proc処理
 * @author  Hiroyuki Nakamura
 * @date    2015.12.08
 */
//=============================================================================
#if !defined( WAZA_OMOIDASI_PROC_H_INCLUDED )
#define WAZA_OMOIDASI_PROC_H_INCLUDED

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


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(WazaOmoidasi)

// 前方宣言
class WazaOmoidasiFrame;
struct APP_PARAM;

//=============================================================================
/**
 * @class WazaOmoidasiProc
 * @brief 技思い出し画面Procクラス
 * @date  2015.12.10
 */
//=============================================================================
class WazaOmoidasiProc
  : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(WazaOmoidasiProc);  // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  WazaOmoidasiProc(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~WazaOmoidasiProc();

  //-----------------------------------------------------------------------------
  /**
   * @func    Setup
   * @brief   セットアップ
   * @date    2015.12.10
   *
   * @param   param   外部設定パラメータ
   */
  //-----------------------------------------------------------------------------
  void Setup( APP_PARAM * param );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFunc
   * @brief   Proc初期化処理
   * @date    2015.12.10
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
   * @date    2015.12.10
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
   * @date    2015.12.10
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
   * @date    2015.12.10
   *
   * @param   pManager    プロセス管理マネージャー
   */
  //-----------------------------------------------------------------------------
  virtual void PreDrawFunc( gfl2::proc::Manager * pManager );

  //-----------------------------------------------------------------------------
  /**
   * @func    DrawFunc
   * @brief   Proc描画処理
   * @date    2015.12.10
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
   * @date    2015.12.10
   */
  //-----------------------------------------------------------------------------
  void CreateHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteHeap
   * @brief   ヒープ削除
   * @date    2015.12.10
   */
  //-----------------------------------------------------------------------------
  void DeleteHeap(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateRenderManager
   * @brief   描画マネージャ生成
   * @date    2015.12.10
   */
  //-----------------------------------------------------------------------------
  void CreateRenderManager(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteRenderManager
   * @brief   描画マネージャ削除
   * @date    2015.12.10
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
   * @date    2015.12.10
   */
  //-----------------------------------------------------------------------------
  void CreateFrame(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteFrame
   * @brief   フレーム削除
   * @date    2015.12.10
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteFrame(void);


private:
  APP_PARAM * m_param;  //!< 外部設定パラメータ

  gfl2::heap::HeapBase * m_pManagerHeap;    //!< マネージャ用ヒープ

  app::util::Heap m_heap;   //!< ローカルヒープ

  app::util::AppRenderingManager * m_pRenderingManager;   //!< 描画マネージャ
  applib::frame::Manager * m_pFrameManager;               //!< フレームマネージャ

  WazaOmoidasiFrame * m_pFrame;  //!< 技思い出し画面フレーム
};


GFL_NAMESPACE_END(WazaOmoidasi)
GFL_NAMESPACE_END(App)


#endif  // WAZA_OMOIDASI_PROC_H_INCLUDED
