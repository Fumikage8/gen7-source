//===================================================================
/**
 *	@file	    BoxEvent.h
 *	@brief    ボックスの代表的なイベント
 *	@author	  k.ohno
 *	@date		  2015.7.15
 *
 */
//===================================================================
#ifndef __BOX_EVENT_H__
#define __BOX_EVENT_H__
#pragma once

#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameManager.h"

#include "App/Box/include/BoxAppParam.h"
#include "App/Box/include/BoxAppSetup.h"
#include "App/BoxSearch/include/BoxSearchAppParam.h"

#include "App/StrInput/include/StrInputProc.h"
#include "App/StrInput/include/StrInputAppParam.h"

#include "SaveData/include/BoxSave.h"
#include "SaveData/include/BoxPokemonSave.h"

#include "App/Status/include/StatusAppParam.h"
#include "App/Status/include/StatusProc.h"

#include "App/FieldBag/include/BagProc.h"

#include  "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase

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


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(P2PTrade)
class P2PTradeRequestClient;
GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetAppLib)


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

  class BoxEvent : public NetEvent::NetEventBase     //public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN( BoxEvent ); // コピーを禁止

  public:
    // このイベントを起動する関数 

    // このイベントを起動する関数 
#if PM_DEBUG
    static BoxEvent* StartEvent( GameSys::GameManager* p_gameman, bool bDebug = false);
#else
    static BoxEvent* StartEvent( GameSys::GameManager* p_gameman);
#endif

    //--------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     * @param   heap       workをとるためのメモリクラス
     */
    //--------------------------------------------------------------
    BoxEvent( gfl2::heap::HeapBase* heap);
    virtual ~BoxEvent();

#if PM_DEBUG
    void addDebugPoke(gfl2::heap::HeapBase* heap, GameSys::GameManager* game_manager);
#endif
    
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
     * @brief   スタートする際のタイプを指定する
     * @param   type       BoxAppParam.hにある BOX_CALL_TYPE
     * @param   page  ばとるトレイ
     */
    //--------------------------------------------------------------
    void SetBoxStartType(App::Box::BOX_CALL_TYPE type);
    void SetBoxStartType(App::Box::BOX_CALL_TYPE type, int page);

    //--------------------------------------------------------------
    /**
     * @brief   終了を得るリスナーをセット
     * @param   pBoxEventListener       BoxEventListener
     */
    //--------------------------------------------------------------
    void SetBoxEventListener(BoxEventListener* pBoxEventListener);
    //--------------------------------------------------------------
    /**
     * @brief   通信モジュールのセット
     * @param   P2PTradeRequestClient
     */
    //--------------------------------------------------------------
    void SetP2PTradeRequestClient(NetAppLib::P2PTrade::P2PTradeRequestClient* pP2PTradeRequestClient);

    //--------------------------------------------------------------
    /**
     * @brief   交換したいポケモンを設定
     * @param   monsno モンスター番号  何でもいい場合 MONSNO_NULL
     * @param   gender 性別       関係ない場合  pml::SEX_NUM
     * @param   level  レベル帯   関係ない場合０
     */
    //--------------------------------------------------------------
    void TradePokemon(u16 monsno, u8 gender, u8 level);

    //--------------------------------------------------------------
    /**
     * @brief   niji互換対応有効フラグ
     */
    //--------------------------------------------------------------
    void SetNijiCompatibility();

    void SetOutParam( void );

  private:

    void SetupStatusAppParam(GameSys::GameManager* game_manager);

#if PM_DEBUG
    debug::launcher::LauncherProc::CallParam m_pLauncherParam;
    static bool mbDebug;
#endif

    u32   mSeq;

    
    App::Box::APP_PARAM mParam;
    App::Box::Proc* mpProc;

    App::BoxSearch::APP_PARAM mBoxSearchAppParam;

    App::Box::BoxAppSetup* mpBoxAppSetup;
    
    BoxEventListener* mpBoxEventListener;
    
  };


GFL_NAMESPACE_END( Event )
GFL_NAMESPACE_END( App )

#endif // __BOX_EVENT_H__

