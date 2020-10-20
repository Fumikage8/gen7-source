//===================================================================
/**
 *	@file	    DebugBoxEvent.h
 *	@brief    ボックスの代表的なイベント
 *	@author	  k.ohno
 *	@date		  2015.7.15
 *
 */
//===================================================================
#if PM_DEBUG

#ifndef __DEBUG_BOX_EVENT_H__
#define __DEBUG_BOX_EVENT_H__
#pragma once

#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameManager.h"

#include "App/Box/include/BoxAppParam.h"
#include "App/Box/include/BoxAppSetup.h"
#include "App/BoxSearch/include/BoxSearchAppParam.h"

#include "App/Status/include/StatusAppParam.h"
#include "App/Status/include/StatusProc.h"

#include "SaveData/include/BoxSave.h"
#include "SaveData/include/BoxPokemonSave.h"

#include "App/FieldBag/include/BagProc.h"

#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/StartMenu/include/StartLauncher.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>



GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )
class Proc;
GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )
class Proc;
GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )





GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Box )

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

  class DebugBoxEvent : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN( DebugBoxEvent ); // コピーを禁止

  public:
    // このイベントを起動する関数 

    static void StartEvent( GameSys::GameManager* p_gameman, int mode);

    //--------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     * @param   heap       workをとるためのメモリクラス
     */
    //--------------------------------------------------------------
    DebugBoxEvent( gfl2::heap::HeapBase* heap);
    virtual ~DebugBoxEvent();

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


    void SetType(int type){ mParam.type = (App::Box::BOX_CALL_TYPE)type; };
    
  private:
    void addDebugPoke(gfl2::heap::HeapBase* heap,GameSys::GameManager* gmgr);
    void itemSetup( app::bag::BagProc * proc );

    
    u32   mSeq;
    App::Box::Proc* mpProc;

    App::Box::APP_PARAM mParam;

    App::BoxSearch::Proc *mpCallProc;

    debug::launcher::LauncherProc::CallParam m_pLauncherParam;

    App::BoxSearch::APP_PARAM mBoxSearchAppParam;
    App::Box::BoxAppSetup* mpBoxAppSetup;

    App::Status::Proc *mpStatusProc;

    App::Status::APP_PARAM mStatusAppParam;

    app::bag::APP_PARAM mParamBag;
    app::bag::BagProc* mpBagProc;

    App::PokeList::IntermediateData mPokemon;
    pml::PokeParty* mpParty;
    
  };


GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( Test )

#endif // __DEBUG_BOX_EVENT_H__

#endif // PM_DEBUG
