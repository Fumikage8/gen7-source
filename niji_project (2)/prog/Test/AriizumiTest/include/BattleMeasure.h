//======================================================================
/**
 * @file  BattleMeasure.h
 * @brief 15/03/13
 * @author  ariizumi
 * @data  バトル処理負荷測定
 */
//======================================================================

#ifndef __BATTLE_MEASURE_H__
#define __BATTLE_MEASURE_H__
#pragma once
#if PM_DEBUG

#include <macro/include/gfl2_Macros.h>

#include "GameSys/include/GameProc.h"
#include "GameSys/include/GameManager.h"

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Ariizumi)

class DebugBattleViewSystem;

class BattleMeasureProc : public GameSys::GameProc {
  GFL_FORBID_COPY_AND_ASSIGN(BattleMeasureProc); //コピーコンストラクタ＋代入禁止

public:
  BattleMeasureProc(); //コンストラクタ
  virtual ~BattleMeasureProc(void);  //デストラクタ

  //------------------------------------------------------------------
  /**
    * @brief   PROCの初期化を行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);
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
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );

  /**
  * @brief	PROCの描画処理
  * @param pManager   プロセス管理マネージャー
  * @param displayNo  描画対象画面
  * 登録したPROCによる、フレームバッファへの描画を実行します。
  * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
  */
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

  //------------------------------------------------------------------
  /**
    * @brief   PROCの終了を行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

private:
  gfl2::heap::HeapBase* mMemHeap;
  gfl2::heap::HeapBase* mDevHeap;

  DebugBattleViewSystem *mBtlvSystem;

  int mMainSeq;

};  //class BattleProc
GFL_NAMESPACE_END(Ariizumi)
GFL_NAMESPACE_END(Test)



#endif  //PM_DEBUG
#endif  //__BATTLE_MEASURE_H__