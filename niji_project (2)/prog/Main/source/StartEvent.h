//===================================================================
/**
 *	@file	    StartEvent.h
 *	@brief    スタートイベント
 *	@author	  k.ohno
 *	@date		  2015.5.31
 *
 */
//===================================================================
#ifndef __START_EVENT_H__
#define __START_EVENT_H__
#pragma once

#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameManager.h"

#include "Test/MenuFrameTest/include/MenuSampleAppParam.h"


#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/StartMenu/include/StartLauncher.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>

#include "App/Title/include/TitleProc.h"
#include <App/TitleMenu/include/TitleMenuAppParam.h>
#include <App/LangSelect/include/LangSelectAppParam.h>
#include <App/HeroSelect/include/HeroSelectAppParam.h>


//-----------------------------------------------------------------------------
/**
 *					前方宣言
*/
//-----------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( Main )
class Proc;
GFL_NAMESPACE_END( Main )


GFL_NAMESPACE_BEGIN( Main )


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

  class StartEvent : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN( StartEvent ); // コピーを禁止

  public:
#if PM_DEBUG
    debug::launcher::LauncherProc::CallParam m_pLauncherParam;
#endif
    // このイベントを起動する関数 
    static void FirstEventStart( GameSys::GameManager* p_gameman );

    //--------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     * @param   heap       workをとるためのメモリクラス
     */
    //--------------------------------------------------------------
    StartEvent( gfl2::heap::HeapBase* heap);
    virtual ~StartEvent();

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
    //--------------------------------------------------------------
    /**
     * @brief   Procの開始
     */
    //--------------------------------------------------------------
    void StartProc( void );

    //--------------------------------------------------------------
    /**
     * @brief   次の遷移へ
     * @param   procMan   GameProcManager
     */
    //--------------------------------------------------------------
    GameSys::GMEVENT_RESULT Next( GameSys::GameProcManager* procMan );

    //--------------------------------------------------------------
    /**
     * @brief   次の遷移へ：言語選択
     * @param   procMan   GameProcManager
     */
    //--------------------------------------------------------------
    GameSys::GMEVENT_RESULT Next_LangSelect( GameSys::GameProcManager* procMan );

    //--------------------------------------------------------------
    /**
     * @brief   次の遷移へ：タイトル
     * @param   procMan   GameProcManager
     */
    //--------------------------------------------------------------
    GameSys::GMEVENT_RESULT Next_Title( GameSys::GameProcManager* procMan );

    //--------------------------------------------------------------
    /**
     * @brief   次の遷移へ：タイトルメニュー
     * @param   procMan   GameProcManager
     */
    //--------------------------------------------------------------
    GameSys::GMEVENT_RESULT Next_TitleMenu( GameSys::GameProcManager* procMan );

    //--------------------------------------------------------------
    /**
     * @brief   次の遷移へ：イントロデモ：前半
     * @param   procMan   GameProcManager
     */
    //--------------------------------------------------------------
    GameSys::GMEVENT_RESULT Next_IntroDemoFirst( GameSys::GameProcManager* procMan );

    //--------------------------------------------------------------
    /**
     * @brief   次の遷移へ：容姿・性別選択＋名前入力
     * @param   procMan   GameProcManager
     */
    //--------------------------------------------------------------
    GameSys::GMEVENT_RESULT Next_HeroSelect( GameSys::GameProcManager* procMan );

    //--------------------------------------------------------------
    /**
     * @brief   次の遷移へ：イントロデモ：後半
     * @param   procMan   GameProcManager
     */
    //--------------------------------------------------------------
    GameSys::GMEVENT_RESULT Next_IntroDemoSecond( GameSys::GameProcManager* procMan );

    //--------------------------------------------------------------
    /**
     * @brief   次の遷移へ：イントロデモ：オープニング
     * @param   procMan   GameProcManager
     */
    //--------------------------------------------------------------
    GameSys::GMEVENT_RESULT Next_IntroDemoThird( GameSys::GameProcManager* procMan );
  private:
    bool IsTitleReturnCodeDecide();
    bool IsTitleReturnCodeDeleteSave();
    bool IsTitleReturnCodeDebugMode();
    int LanguageToMovieNo();

  private:
    int m_titleReturnCode;

    App::TitleMenu::APP_PARAM     mTitleMenuAppParam;

    App::LangSelect::APP_PARAM    mLangSelectAppParam;

    App::HeroSelect::APP_PARAM    m_heroSelectParam;

    u32   mSeq;

    u32   mProcIndex;

#if !defined(GF_PLATFORM_CTR)
    app::title::APP_PARAM mTitleAppParam;
#endif
  };


GFL_NAMESPACE_END( Main )

#endif // __START_EVENT_H__

