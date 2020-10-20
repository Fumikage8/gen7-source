//===================================================================
/**
 *	@file	    ListSampleEvent.h
 *	@brief    ListMenuのクラスの実験
 *	@author	  k.ohno
 *	@date		  2015.2.25
 *
 */
//===================================================================
#if PM_DEBUG

#ifndef __LIST_SAMPLE_EVENT_H__
#define __LIST_SAMPLE_EVENT_H__

#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameManager.h"

#include "Test/MenuFrameTest/include/ListSampleParam.h"


#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/StartMenu/include/StartLauncher.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( List )

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

  class ListSampleEvent : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN( ListSampleEvent ); // コピーを禁止

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
    ListSampleEvent( gfl2::heap::HeapBase* heap);
    virtual ~ListSampleEvent();

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
    u32   mSeq;
    APP_PARAM mAppParam;
		Test::List::Proc*  mpProc;
  };


GFL_NAMESPACE_END( List )
GFL_NAMESPACE_END( Test )

#endif // __LIST_SAMPLE_EVENT_H__

#endif //PM_DEBUG
