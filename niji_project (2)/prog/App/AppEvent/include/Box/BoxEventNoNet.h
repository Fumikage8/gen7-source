//===================================================================
/**
 *	@file	    BoxEventNoNet.h
 *	@brief    ボックスのイベント  通信非対応部分
 *	@author	  k.ohno
 *	@date		  2016.1.25
 *
 */
//===================================================================
#ifndef __BOX_EVENT_NONET_H__
#define __BOX_EVENT_NONET_H__
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

  class BoxEventNoNet : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN( BoxEventNoNet ); // コピーを禁止

  public:
    // このイベントを起動する関数 
    static BoxEventNoNet* StartEvent( GameSys::GameManager* p_gameman);

    //--------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     * @param   heap       workをとるためのメモリクラス
     */
    //--------------------------------------------------------------
    BoxEventNoNet( gfl2::heap::HeapBase* heap);
    virtual ~BoxEventNoNet();

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
     * @brief   ボックスイベントからセットアッパーをもらう
     * @param   BoxAppSetupのポインタ
     */
    //--------------------------------------------------------------
    void SetBoxAppSetup( App::Box::BoxAppSetup* pAppSetup ,App::Box::APP_PARAM* pParam,App::BoxSearch::APP_PARAM* mpBoxSearchParam);

    //--------------------------------------------------------------
    /**
     * @brief   交換したいポケモンを設定
     * @param   monsno モンスター番号  何でもいい場合 MONSNO_NULL
     * @param   gender 性別       関係ない場合  pml::SEX_NUM
     * @param   level  レベル帯   関係ない場合０
     */
    //--------------------------------------------------------------
    void TradePokemon(u16 monsno, u8 gender, u8 level);

    void SetOutParam( void );

  private:


    void setTeamParty(int tray);
    void setGimParty(int tray);
    void setHotSpaParty(int tray);



    
    void SetupStatusAppParam(GameSys::GameManager* game_manager);


    void itemSetup( app::bag::BagProc * proc );

    u32   mSeq;

    gfl2::str::StrBuf * mpNameStrBuf;

    
    App::Box::APP_PARAM* mpParam;
//    App::Box::Proc* mpProc;

    App::BoxSearch::APP_PARAM* mpBoxSearchAppParam;
    App::BoxSearch::Proc *mpCallProc;

    App::Status::APP_PARAM mStatusAppParam;
    App::Status::Proc *mpStatusProc;

    App::Box::BoxAppSetup* mpBoxAppSetup;
    
    app::bag::APP_PARAM mParamBag;
    app::bag::BagProc* mpBagProc;

    App::StrInput::StrInputParam mParamStrInput;
    App::StrInput::Proc *mpStrProc;

    
    App::PokeList::IntermediateData mPokemon;
    pml::PokeParty* mpParty;
    
    
  };


GFL_NAMESPACE_END( Event )
GFL_NAMESPACE_END( App )

#endif // __BOX_EVENT_NONET_H__

