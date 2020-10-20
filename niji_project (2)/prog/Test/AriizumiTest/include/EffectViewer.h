//======================================================================
/**
 * @file  EffectViewer.h
 * @brief 15/04/16
 * @author  ariizumi
 * @data  エフェクトビュワー
 */
//======================================================================

#ifndef __EFFECT_VIEWER_H__
#define __EFFECT_VIEWER_H__
#pragma once
#if PM_DEBUG

#include <macro/include/gfl2_Macros.h>

#include "GameSys/include/GameProc.h"
#include "GameSys/include/GameManager.h"
#include "AppLib/include/Frame/AppFrameManager.h"

#include "EffectViewAppParam.h"
#include "AutoBattleProc.h"


#if PM_DEBUG
GFL_NAMESPACE_BEGIN(btl)
struct BTLV_DEBUG_PARAM;
GFL_NAMESPACE_END(btl)
#endif

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Ariizumi)

class DebugBattleViewSystem;



class EffectViewerProc : public GameSys::GameProc {
  GFL_FORBID_COPY_AND_ASSIGN(EffectViewerProc); //コピーコンストラクタ＋代入禁止

 public:
  EffectViewerProc(); //コンストラクタ
  virtual ~EffectViewerProc(void);  //デストラクタ

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
   * @brief     PROCの描画処理(下準備)
   * @param pManager   プロセス管理マネージャー
   * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
   * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
   */
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );

  /**
   * @brief     PROCの描画処理
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

#if PM_DEBUG
  void SetDebugParam(btl::BTLV_DEBUG_PARAM *param){mDebugParam = param;}
#endif



 private:

  void InitDebugMenu(void);
  void TermDebugMenu(void);
  bool UpdateMain(void);

  gfl2::heap::HeapBase* mMemHeap;
  gfl2::heap::HeapBase* mDevHeap;

  DebugBattleViewSystem *mBtlvSystem;

  int mMainSeq;
  int mSubSeq;
  int mTermSeq;
  bool mIsFinishSetup;

#if PM_DEBUG
  btl::BTLV_DEBUG_PARAM *mDebugParam;
#endif
  SDCardLog mSdLog;

  // ----------------------
  // 下画面関連
  // ----------------------
  void AppParamInit(gfl2::heap::HeapBase *devHeap);        // 下画面アプリ用のメモリ確保と初期化
  bool AppParamEnd(void);                                  // 下画面アプリ用のメモリの開放

  void CheckUiAction(void);
  void SetWazaTarget(void);
  void SetPokemon(void);

  applib::frame::Manager          *mpFrameManager;         // ファイルマネージャー
  app::util::AppRenderingManager  *mpAppRenderingManager;  // レンダリングマネージャー
  app::util::Heap                 *mpAppHeap;              // アプリ用ヒープ

  EFFECT_APP_PARAM                mEffectViewAppParam;     // インスタンスのポインタ、パラメータ等の受け渡し用構造体
  EffectViewerUiInfo              mEffectViewerUiInfo;     // パラメーター受け渡し用クラス

public:
  //DebugWinの項目からアクセスするためpublic
  bool mEnableAutoTest;
  int  mAutoStartWaza;
  int  mAutoEndWaza;
  
  int mSubIdx;  //全方向チェック用
  int mSubDirIdx;  //全方向チェック用
  int mAutoTestCmdBuf;
  //自動テスト用
  int  mAutoPlayWaza;
  bool mEnableAllDir;

  int mRenderPathCnt;
};  //class BattleProc
GFL_NAMESPACE_END(Ariizumi)
GFL_NAMESPACE_END(Test)


#endif  //PM_DEBUG
#endif  //__EFFECT_VIEWER_H__
