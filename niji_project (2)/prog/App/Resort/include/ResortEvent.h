//===================================================================
/**
 *	@file	    ResortEvent.h
 *	@brief    イベントのサンプル
 *	@author	  k.ohno
 *	@date		  2015.2.25
 *
 */
//===================================================================
#if 0
#ifndef __RESORT_EVENT_H__
#define __RESORT_EVENT_H__

#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameManager.h"

#include "App/Resort/include/ResortProc.h"


#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/StartMenu/include/StartLauncher.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )

class Proc;

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

  class ResortEvent : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN( ResortEvent ); // コピーを禁止

  public:
    debug::launcher::LauncherProc::CallParam m_pLauncherParam;
    // このイベントを起動する関数 

    static void StartEvent( GameSys::GameManager* p_gameman );

    //--------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     * @param   heap       workをとるためのメモリクラス
     */
    //--------------------------------------------------------------
    ResortEvent( gfl2::heap::HeapBase* heap);
    virtual ~ResortEvent();

    //--------------------------------------------------------------
    /**
     * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
     * @param   gmgr       ゲームマネージャークラス
     * @retval  true  起動してよい
     * @retval  false 起動しない
     */
    //--------------------------------------------------------------
    virtual bool BootChk( GameSys::GameManager* /*gmgr*/ );

    //--------------------------------------------------------------
    /**
     * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
     * @param   gmgr       ゲームマネージャークラス
     */
    //--------------------------------------------------------------
    virtual void InitFunc( GameSys::GameManager* /*gmgr*/ );

    //--------------------------------------------------------------
    /**
     * @brief   呼び出される関数 純粋仮想関数
     * @param   gmgr       ゲームマネージャークラス
     * @return  GMEVENT_RESULT  イベント制御関数の戻り値
     */
    //--------------------------------------------------------------
    virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* gmgr );

    //--------------------------------------------------------------
    /**
     * @brief   呼び出される関数 純粋仮想関数
     * @param   gmgr       ゲームマネージャークラス
     */
    //--------------------------------------------------------------
    virtual void EndFunc( GameSys::GameManager* /*gmgr*/ );

  private:
    void SetType(int type);

    int m_type;
    u32   mSeq;
    App::Resort::ResortProc* mpProc;
  };


GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

#endif // __RESORT_EVENT_H__
#endif
