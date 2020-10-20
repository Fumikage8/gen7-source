//===================================================================
/**
 *	@file	    MovieEvent.h
 *	@brief    ムービーのイベント
 *	@author	  k.ohno
 *	@date		  2015.10.13
 *
 */
//===================================================================
#ifndef __MOVIE_EVENT_H__
#define __MOVIE_EVENT_H__
#pragma once

#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameManager.h"


#include "MovieAppParam.h"
#include "MovieEventListener.h"


#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/StartMenu/include/StartLauncher.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>



GFL_NAMESPACE_BEGIN( movie )
class MovieProc;
GFL_NAMESPACE_END( movie )




GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Event )

class BoxEventListener;

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

  class MovieEvent : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN( MovieEvent ); // コピーを禁止

  public:
    // このイベントを起動する関数 
#if PM_DEBUG
    static MovieEvent* MovieEvent::StartEvent( GameSys::GameManager* p_gameman, bool bDebug, u32 return_bgm_id );
#else
    static MovieEvent* MovieEvent::StartEvent( GameSys::GameManager* p_gameman, u32 return_bgm_id );
#endif

    //--------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     * @param   heap       workをとるためのメモリクラス
     */
    //--------------------------------------------------------------
    MovieEvent( gfl2::heap::HeapBase* heap);
    virtual ~MovieEvent();
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
    //--------------------------------------------------------------
    /**
     * @brief   終了を得るリスナーをセット
     * @param   pMovieEventListener       MovieEventListener
     */
    //--------------------------------------------------------------
    void SetMovieEventListener(MovieEventListener* pMovieEventListener);

    //--------------------------------------------------------------
    /**
     * @brief   初期化データをセット
     * @param   pMovieEventListener       MovieEventListener
     */
    //--------------------------------------------------------------
    void SetAppParam(const APP_PARAM* pAppParam);

    void SetReturnBGMID(u32 return_bgm_id){ mReturnBGMID = return_bgm_id; }
    
  private:
#if PM_DEBUG
    debug::launcher::LauncherProc::CallParam m_pLauncherParam;
    static bool mbDebug;
#endif

    u32   mSeq;
    u32   mReturnBGMID;

    movie::MovieProc* mpProc;

    MovieEventListener* mpMovieEventListener;

    APP_PARAM mAppParam;  //初期化に必要な要素
    
  };


GFL_NAMESPACE_END( Event )
GFL_NAMESPACE_END( App )

#endif // __MOVIE_EVENT_H__

