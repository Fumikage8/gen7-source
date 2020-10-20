//===================================================================
/**
 *	@file	    CongratsEvent.h
 *	@brief    おめでとうえんしゅつイベント
 *	@author	  k.ohno
 *	@date		  2016.1.19
 *
 */
//===================================================================
#ifndef __CONGRATS_EVENT_H__
#define __CONGRATS_EVENT_H__
#pragma once

#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameManager.h"

#include <App/AppDemo/include/AppDemoProcParam.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( AppDemo )
class AppDemoProc_Congrats;
GFL_NAMESPACE_END( AppDemo )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( dendou_demo )
class Proc;
struct APP_PARAM;
GFL_NAMESPACE_END( dendou_demo )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Event )

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

  class CongratsEvent : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN( CongratsEvent ); // コピーを禁止

  public:
    // このイベントを起動する関数 
    static CongratsEvent* StartEvent( GameSys::GameManager* p_gameman, bool isAlreadyClear );

    //--------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     * @param   heap       workをとるためのメモリクラス
     */
    //--------------------------------------------------------------
    CongratsEvent( gfl2::heap::HeapBase* heap);
    virtual ~CongratsEvent();

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
    void SetParams( bool isAlreadyClear ) { mIsAlreadyClear = isAlreadyClear; }

  private:
		App::AppDemo::AppDemoProc_Congrats* mpProc;
    App::AppDemo::AppDemoProcParam_Congrats* mpParam;
    app::dendou_demo::Proc* mpBPartProc;
    app::dendou_demo::APP_PARAM* mpBPartParam;
    u32   mSeq;
    bool mIsAlreadyClear;
  };


GFL_NAMESPACE_END( Event )
GFL_NAMESPACE_END( App )

#endif // __CONGRATS_EVENT_H__

