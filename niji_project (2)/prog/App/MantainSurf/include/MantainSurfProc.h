//======================================================================
/**
 * @file MantainSurfProc.h
 * @date 2016/09/12
 * @author fang_yicheng
 * @brief マンタインサーフプロセス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __MANTAIN_SURF_PROC_H_INCLUDED__
#define __MANTAIN_SURF_PROC_H_INCLUDED__
//#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include <System/include/nijiAllocator.h>
#include <GameSys/include/GameProc.h>

#include <App/AppEvent/include/MantainSurf/MantainSurfParam.h>

#include "../source/MantainSurfAppManager.h"
#include "../source/MantainSurfDrawManager.h"

#ifdef  GF_PLATFORM_CTR
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#endif


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)
class DrawUtilText;
class UtilTextRenderingPipeLine;
GFL_NAMESPACE_END(util)

GFL_NAMESPACE_BEGIN(lyt)
class LytSys;
GFL_NAMESPACE_END(lyt)

GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(print)
class WordSet;
GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)

/**
 * @class MantainSurfProc
 * @brief マンタインサーフプロセスクラス
 */
class MantainSurfProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(MantainSurfProc);
  
public:
  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //---------------------------------------------------------------------------
  MantainSurfProc();

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  virtual ~MantainSurfProc();

  //---------------------------------------------------------------------------
  /**
   * @brief   PROCの初期化を行う関数
   * @param   プロセス管理マネージャー
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //---------------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

  //---------------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @param   プロセス管理マネージャー
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //---------------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

  //---------------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @param   プロセス管理マネージャー
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //---------------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
   *  @func     PreDrawFunc
   *  @brief    PROCの描画処理(下準備)
   *            登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
   *            例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
   *  @param pManager   プロセス管理マネージャー
   */
  //------------------------------------------------------------------
  virtual void PreDrawFunc(gfl2::proc::Manager* pManager);

  //---------------------------------------------------------------------------
  /**
   * @brief	PROCの描画処理
   * @param pManager   プロセス管理マネージャー
   * @param displayNo  描画対象画面
   * 登録したPROCによる、フレームバッファへの描画を実行します。
   * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
   */
  //---------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

public:

  //---------------------------------------------------------------------------
  /**
   * @brief 起動パラメータを設定する
   * @param pParam                パラメータ
   */
  //---------------------------------------------------------------------------
  void Setup(App::Event::MANTAIN_SURF_PARAM* pParam);

private:

  bool UpdateInitialize();
  bool UpdateNormal();
  bool UpdateDeinitialize();

private:

  s32 m_Seq;
  s32 m_SeqSub;
  u32 m_seWaitTimer;

  // ヒープ関係
  System::nijiAllocator* m_pNijiAllocator;
  gfl2::heap::HeapBase*  m_AppDevHeap;
  app::util::Heap*       m_AppLocalHeap;
  s32                    m_StartFreeSize;

  // 入力パラメータ
  App::Event::MANTAIN_SURF_PARAM* m_pAppParam;
  // 描画管理
  MantainSurfDrawManager*         m_pDrawManager;
  // UI
  UI::SurfUI*                     m_pSurfUI;
  // アプリケーション管理
  MantainSurfAppManager*          m_pAppManager;

  bool  m_isDrawEnable;
  s32   m_courseIdNow;
  s32   m_courseIdNext;

#if PM_DEBUG
  bool  m_debugIsRestartTest;
  s32   m_debugRestartTestInterval;
#endif

};

GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)

#endif // __MANTAIN_SURF_PROC_H_INCLUDED__
