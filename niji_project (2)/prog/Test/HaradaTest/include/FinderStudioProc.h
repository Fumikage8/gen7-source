#if PM_DEBUG
//======================================================================
/**
 * @file FinderStudioProc.h
 * @date 2016/08/25
 * @author harada_takumi
 * @brief ファインダースタジオプロック
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FINDER_STUDIO_TESTPROC_H_INCLUDED__
#define __FINDER_STUDIO_TESTPROC_H_INCLUDED__
#pragma once

// 継承元
#include "GameSys/include/GameProc.h"

// System
#include "System/include/GflUse.h"

#include "PokeTool/include/PokeModel.h"

#include "App/FinderStudioStatic/include/FinderStudioViewerDefine.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)
  class FinderStudioViewer;
GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(HaradaTest)

class FinderStudioTestProc : public GameSys::GameProc 
{
  GFL_FORBID_COPY_AND_ASSIGN(FinderStudioTestProc); //コピーコンストラクタ＋代入禁止

public:
  FinderStudioTestProc();
  virtual ~FinderStudioTestProc();

  //============================================================================
  //GameProc
  //============================================================================
  //------------------------------------------------------------------
  /**
    * @brief   PROCの初期化を行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
    * @brief   PROCの終了を行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
    * @brief   PROCのメインを行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);

  /**
  * @brief	PROCの描画処理(下準備)
  * @param pManager   プロセス管理マネージャー
  * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
  * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
  */
  virtual void PreDrawFunc(gfl2::proc::Manager* pManager);

  /**
  * @brief	PROCの描画処理
  * @param pManager   プロセス管理マネージャー
  * @param displayNo  描画対象画面
  * 登録したPROCによる、フレームバッファへの描画を実行します。
  * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
  */
  virtual void DrawFunc(gfl2::proc::Manager* pManager,gfl2::gfx::CtrDisplayNo displayNo);

private:
  // ヒープ
  gfl2::heap::HeapBase* m_pHeap;
  //System::nijiAllocator* m_pPokeGLAllocator;

  // メインシステム
  App::FinderStudio::Viewer::FinderStudioViewer* m_pMainSystem;

  // 初期化用
  u32 m_InitSeq;
  // 終了用
  u32 m_EndSeq;

  // カメラチェンジモード
  u32 m_ChangeMode;

  // テスト用
  u32 m_MainSeq;
  u32 m_SubSeq;
  PokeTool::SimpleParam m_PokeSimpleParam;
  FormNo m_RealFormNo;
  const PokeTool::MODEL_ANIME* m_pAsingBattleMotionList;
  const PokeTool::MODEL_ANIME* m_pAsingKawaigariMotionList;
  int m_WaitCnt;
  u32 m_MotionListIdx;
  pml::personal::PersonalData* m_pPersonalData;

  App::FinderStudio::Viewer::PhotoParam m_PhotoParam;

  // ヒープ確保容量
  static const u32 HEAP_SIZE;
};

GFL_NAMESPACE_END(HaradaTest)
GFL_NAMESPACE_END(Test)

#endif // __FINDER_STUDIO_TESTPROC_H_INCLUDED__

#endif // PM_DEBUG