//======================================================================
/**
 * @file PokeViewerProc.h
 * @date 2015/11/24 12:36:03
 * @author araki_syo
 * @brief PokeViewerのProc
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined POKEVIEWERPROC_H
#define POKEVIEWERPROC_H
#pragma once

#include "GameSys/include/GameProc.h"
#include <heap/include/gfl2_Heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>

#include "PokeViewerDataManager.h"
#include "PokeViewerFrame.h"
#include "PokeViewerView.h"
#include "PokeViewerPokeModelManager.h"

GFL_NAMESPACE_BEGIN(test)
  GFL_NAMESPACE_BEGIN(pokeviewer)

class HIOInjection;


/**
 * @class PokeViewerProc
 * @brief PokeViewerのProc
 */
class PokeViewerProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeViewerProc);

private:
//! シーケンス
  enum Step
  {
    STEP_NONE,
    STEP_CREATE,
    STEP_WORKING,
    STEP_DESTROY,
  };

public:

  /**
   * @fn ArakiTestProc()
   * @brief ctor
   */
  PokeViewerProc();

  /**
   * @fn ~ArakiTestProc()
   * @brief dtor
   */
  virtual ~PokeViewerProc();

  /**
   * @brief   FrameModeの取得
   * @retval  FrameMode
   *
   * @detail フレームモードを切り替えるときは、戻り値を変えてください。
   */
  virtual GameSys::FrameMode GetFrameMode( void ) const;

  /**
   * @brief   PROCの初期化を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

  /**
   * @brief   PROCのメイン更新を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);

  /**
   * @brief   PROCの描画処理(下準備)
   * @param   pManager   プロセス管理マネージャー
   * @detail  登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
   *          例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
   */
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );

  /**
   * @brief   PROCの描画処理
   * @param   pManager   プロセス管理マネージャー
   * @param   displayNo  描画対象画面
   * @detail  登録したPROCによる、フレームバッファへの描画を実行します。
   *          ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
   */
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

private:
  void UpdateFunc_None();
  void UpdateFunc_Create();
  void UpdateFunc_Working();
  void UpdateFunc_Destroy();

  void ChangeStep(Step step);

private:
  //! シーケンス
  Step m_Step;
  u32 m_StepCounter;

  PokeViewerDataManager* m_pDataManager;

  //! ヒープ
  gfl2::heap::HeapBase* m_pHeap;
  gfl2::heap::HeapBase* m_pFrameManagerHeap;
  app::util::Heap* m_pAppHeap;

  //! フレーム
  applib::frame::Manager* m_pFrameManager;
  PokeViewerFrameCell* m_pFrameCell;
  b32 m_frameCreated;
  b32 m_frameReqDestroy;

  app::util::AppRenderingManager* m_pAppRenderingManager;

  PokeViewerPokeModelManager* m_pPokeModelManager;

  /*  HIO  */
  HIOInjection*   m_pHIOInjection;
};

  GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // POKEVIEWERPROC_H

#endif // PM_DEBUG
