#ifndef __BATTTLE_PROC_H__
#define __BATTTLE_PROC_H__
//=============================================================================================
/**
 * @file   battle_proc.h
 * @brief  バトルのプロセス(移植用
 * @author Ariizumi Nobuhiko
 * @date   2014.9.10
 */
//=============================================================================================
#include <heap/include/gfl2_heap.h>


#include "GameSys/include/GameProc.h"
#include "GameSys/include/GameManager.h"

#include "battle_SetupParam.h"


namespace btl{

  struct BATTLE_PROC_PARAM
  {
    gfl2::heap::HeapBase* pAppMemory;
    gfl2::heap::HeapBase* pAppDeviceMemory;
	  GameSys::GameManager* pGameManager;
    BATTLE_SETUP_PARAM* pSetupParam;
  };

  
  class MainModule;

  class BattleProc : public GameSys::GameProc {
    GFL_FORBID_COPY_AND_ASSIGN(BattleProc); //コピーコンストラクタ＋代入禁止

  private:
    BATTLE_PROC_PARAM  mInitParam;

    MainModule *m_mainModule;
    BATTLE_SETUP_PARAM *m_setupParam;

    int m_mainSeq;
    int m_subSeq;

    gfl2::heap::HeapBase *m_heap_total;
    gfl2::heap::HeapBase *m_heap_system;
    gfl2::heap::HeapBase *m_heap_view;
    gfl2::heap::HeapBase *m_heap_net;

  public:
    BattleProc(BATTLE_PROC_PARAM* pParam); //コンストラクタ
    virtual ~BattleProc(void){};  //デストラクタ

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

#if PM_DEBUG
    void CheckSetupValidity( const BATTLE_SETUP_PARAM& setupParam ) const;
#endif

  };  //class BattleProc

}//namespace battle

#endif //__BATTTLE_PROC_H__
