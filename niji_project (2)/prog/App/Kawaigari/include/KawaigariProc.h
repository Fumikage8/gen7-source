//======================================================================
/**
 * @file  KawaigariProc.cpp
 * @brief 15/08/31
 * @author  ariizumi
 * @data  かわいがりProc
 */
//======================================================================

#ifndef __KAWAIGARI_PROC_H__
#define __KAWAIGARI_PROC_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include "GameSys/include/GameProc.h"
#include "GameSys/include/GameManager.h"

#include "PokeTool/include/PokeModelSystem.h"
#include <Battle/Background/include/BgSystem.h>


GFL_NAMESPACE_BEGIN(btl)
  class BgSystem;
GFL_NAMESPACE_END(btl)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kawaigari)

enum
{
  KAWAIGARI_EXITCODE_NORMAL = 0,
  KAWAIGARI_EXITCODE_POKEMONSELECT,
  KAWAIGARI_EXITCODE_INVALID_POKEMON,
};

enum
{
  KAWAIGARI_MODE_NORMAL = 0,
  KAWAIGARI_MODE_FIELD,
  KAWAIGARI_MODE_FESTIVAL_PLAYER,
  KAWAIGARI_MODE_FESTIVAL_EVENT,
  KAWAIGARI_MODE_FESTIVAL_NPC,
  KAWAIGARI_MODE_RESORT,
  KAWAIGARI_MODE_FORCE_IDLE,
  KAWAIGARI_MODE_END,
#if PM_DEBUG
  KAWAIGARI_MODE_DEBUG,
#endif
};

//Proc初期化パラメーター
typedef struct KawaigariProcParam
{
  btl::BgSystem::SETUP_PARAM bgParam;
  pml::pokepara::PokemonParam *pokemonParam;
  u32 exitCode;
  int mode;
}KawaigariProcParam;

class KawaigariMain;

class KawaigariProc : public GameSys::GameProc 
{
  GFL_FORBID_COPY_AND_ASSIGN(KawaigariProc); //コピーコンストラクタ＋代入禁止

public:
  KawaigariProc(KawaigariProcParam *param); //コンストラクタ
  virtual ~KawaigariProc();  //デストラクタ

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
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );

  /**
  * @brief	PROCの描画処理
  * @param pManager   プロセス管理マネージャー
  * @param displayNo  描画対象画面
  * 登録したPROCによる、フレームバッファへの描画を実行します。
  * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
  */
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

private:
  KawaigariProcParam mInitParam;

  KawaigariProcParam *mParam; // Original Parameters

  gfl2::heap::HeapBase *mHeap;
  KawaigariMain *mMainSystem;

  int mInitSeq;
  int mExitSeq;
};
GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif  //__KAWAIGARI_PROC_H__