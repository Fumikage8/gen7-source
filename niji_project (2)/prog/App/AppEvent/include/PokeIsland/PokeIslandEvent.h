
//===================================================================
/**
 *	@file	    PokeIslandEvent.h
 *	@brief    ポケモンアイランドのイベント
 *	@author	  k.ohno
 *	@date		  2015.6.13
 *
 */
//===================================================================
#ifndef __POKE_ISLAND_EVENT_H__
#define __POKE_ISLAND_EVENT_H__
#pragma once

#include "System/include/DressUp.h"

#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameManager.h"

#include "App/Resort/include/ResortProc.h"
#include "App/Resort/include/ResortAppParam.h"

#include "App/Box/include/BoxAppParam.h"
#include "App/Box/include/BoxAppSetup.h"

#include "App/BoxSearch/include/BoxSearchAppParam.h"
#include "App/Status/include/StatusAppParam.h"
#include "App/Status/include/StatusProc.h"
#include "App/FieldBag/include/BagProc.h"

#include "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"


#if PM_DEBUG
#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/StartMenu/include/StartLauncher.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>
#endif


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )
class ResortProc;
GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )
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

  class PokeIslandEvent : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN( PokeIslandEvent ); // コピーを禁止

  public:
    // このイベントを起動する関数 
#if PM_DEBUG
    static void StartEvent( GameSys::GameManager* p_gameman, bool bDebug = false);
#else
    static void StartEvent( GameSys::GameManager* p_gameman);
#endif

    //--------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     * @param   heap       workをとるためのメモリクラス
     */
    //--------------------------------------------------------------
    PokeIslandEvent( gfl2::heap::HeapBase* heap);
    virtual ~PokeIslandEvent();

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
 //   void SetType(int type);
    void itemSetup( app::bag::BagProc * proc );

    void ResultBoxHotSpa(void);
    //--------------------------------------------------------------
    // GIMポケモン設定処理
    //--------------------------------------------------------------
    void ResultBoxGim(bool binbox );
    //--------------------------------------------------------------
    // ケルディオ設定処理
    //--------------------------------------------------------------
    void ResultBoxGimKerudhio( GameSys::GameData * gd );


    
#if PM_DEBUG
    debug::launcher::LauncherProc::CallParam m_pLauncherParam;
    static bool mbDebug;
#endif
    //  int m_type;

    gfl2::heap::HeapBase* mpHeap;

    u32   mSeq;
    App::Resort::ResortProc* mpProc;

    App::Resort::APP_PARAM mParam;

    App::Box::APP_PARAM mBoxParam;

    App::Box::Proc* mpBoxProc;
    
    App::Box::BoxAppSetup* mpBoxAppSetup;


    App::BoxSearch::APP_PARAM mBoxSearchAppParam;
    App::BoxSearch::Proc *mpCallProc;

    App::Status::Proc *mpStatusProc;
    App::Status::APP_PARAM mStatusAppParam;
    
    app::bag::APP_PARAM mParamBag;
    app::bag::BagProc* mpBagProc;

    App::PokeList::IntermediateData mPokemon;
    pml::PokeParty* mpParty;


    app::event::ZukanRegisterEventParam mZukanRegisterEventParam;
    
    poke_3d::model::DressUpParam m_DressUpParam;

  };


GFL_NAMESPACE_END( Event )
GFL_NAMESPACE_END( App )

#endif // __POKE_ISLAND_EVENT_H__

