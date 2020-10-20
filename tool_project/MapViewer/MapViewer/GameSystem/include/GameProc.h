#ifndef __GAMEPROC_H__
#define __GAMEPROC_H__
//=============================================================================================
/**
 * @file   GameProc.h
 * @brief  ゲームのメインプロセス
 * @author k.ohno
 * @date   2011.3.31
 */
//=============================================================================================
#pragma once


#include <proc/include/gfl2_Proc.h>

#include "GameSystem/include/GameFrameMode.h"
#include "GameSystem/include/GameManager.h"

namespace GameSystem{

  class GameProcManager;


  class GameProc : public gfl2::proc::BaseProcess{
    GFL_FORBID_COPY_AND_ASSIGN(GameProc); //コピーコンストラクタ＋代入禁止

  protected:
    GameProcManager*                mpGameProcManager;  ///< プロセスマネージャ


  public:
    GameProc(void);  //コンストラクタ
    virtual ~GameProc(void){};  //デストラクタ


    //------------------------------------------------------------------
    /**
     * @brief   FrameModeの取得
     * @retval   FrameMode
     *
     * フレームモードを切り替えるときは、戻り値を変えてください。
     */
    //------------------------------------------------------------------
    virtual GameSystem::FrameMode GetFrameMode( void ) const { return GameSystem::FRAMEMODE_DEFAULT; }

    //------------------------------------------------------------------
    /**
     * @brief   PROCの初期化を行う関数
     * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
     */
    //------------------------------------------------------------------
    virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

    //------------------------------------------------------------------
    /**
     * @brief	  PROCのメイン更新を行う関数
     * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
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
     * @brief   PROCのメインを行う関数
     *  1/30フレームの②フレーム目の処理です。
     *  1/60フレーム動作時には、使われません。
     * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
     */
    //------------------------------------------------------------------
    virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);
    //------------------------------------------------------------------
    /**
     * @brief   PROCの終了を行う関数
     * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
     */
    //------------------------------------------------------------------
    virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);
    //------------------------------------------------------------------
    /**
     * @brief   PROCのイベント起動検知処理
     *
     *  以下のタイミングで行うべき、イベント起動処理
     *  　GameProc->EventCheckFunc
     *  　EventManager->Main
     *  　GameProc->Main
     */
    //------------------------------------------------------------------
    virtual void EventCheckFunc(GameManager* pManager);


    //------------------------------------------------------------------
    /**
     * @brief   GameProcManagerの設定
     * @param   GameProcManagerポインタ
     */
    //------------------------------------------------------------------
    inline void SetGameProcManager(GameProcManager* pGameProcManager){ mpGameProcManager = pGameProcManager; };
    //------------------------------------------------------------------
    /**
     * @brief   GameProcManagerの取得
     * @return  GameProcManagerポインタ
     */
    //------------------------------------------------------------------
    inline GameProcManager* GetGameProcManager(void){ return mpGameProcManager; };

    //----------------------------------------------------------------------------
    /**
     *	@brief  GameManagerの取得
     *	@return GameManagerポインタ
     */
    //-----------------------------------------------------------------------------
    GameManager* GetGameManager(void);


  };  //class GameProc


}//namespace GameSys

#endif //__GAMEPROC_H__
