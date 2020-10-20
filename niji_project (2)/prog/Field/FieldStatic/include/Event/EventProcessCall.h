//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file    EventProcessCall.h
 *  @brief  プロセスの終了を待つイベント
 *  @author  tomoya takahashi
 *  @date    2011.06.06
 *
 *  @niji_author saita_kazuki
 *  @date 2015.05.29
 */
//=============================================================================

#if !defined( EVENT_PROCESS_CALL_H_INCLUDED )
#define EVENT_PROCESS_CALL_H_INCLUDED
#pragma once

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <ro/include/gfl2_RoManager.h>

// gamesys
#include "GameSys/include/GameEvent.h"
#include <GameSys/include/GameProcManager.h>

GFL_NAMESPACE_BEGIN( Field )

//-----------------------------------------------------------------------------
/**
 *  @class プロセスの終了を待つイベント
*/
//-----------------------------------------------------------------------------
class EventProcessCall : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventProcessCall); // コピーコンストラクタ＋代入禁止

public:

  //===========================================================================================
  // 静的モジュール用PROC
  //===========================================================================================
  // プロセスコール　イベント
  template<class CallClass>
  static CallClass* CallProc( GameSys::GameManager* p_gman )
  {
    CallClass * p_proc = NIJI_PROC_CALL<GameSys::GameProcManager, CallClass>( p_gman->GetProcManager() );
    // プロセスの終了を待つイベントの生成
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventProcessCall>( p_gman->GetGameEventManager() );
    return p_proc;
  }

  // プロセスコール　イベント
  template<class CallClass, typename ArgClass>
  static CallClass* CallProc( GameSys::GameManager* p_gman, ArgClass* p_arg )
  {
    CallClass * p_proc = NIJI_PROC_CALL<GameSys::GameProcManager, CallClass, ArgClass>( p_gman->GetProcManager(), p_arg );
    // プロセスの終了を待つイベントの生成
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventProcessCall>( p_gman->GetGameEventManager() );
    return p_proc;
  }

  //===========================================================================================
  // 動的モジュール用PROC
  //===========================================================================================
//#if GFL_RO_ENABLE

  template<class CallClass>
  static void CallRoEventCall( GameSys::GameManager* p_gman )
  {
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventProcessCall>( p_gman->GetGameEventManager() );
  }

  template<class CallClass>
  static CallClass* CallRoProc( GameSys::GameManager* p_gman, const char* croFileName )
  {
    CallClass * p_proc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, CallClass>( p_gman->GetProcManager(), croFileName );
    // プロセスの終了を待つイベントの生成
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventProcessCall>( p_gman->GetGameEventManager() );
    return p_proc;
  }

  // プロセスコール　イベント
  template<class CallClass, typename ArgClass>
  static CallClass* CallRoProc( GameSys::GameManager* p_gman, ArgClass* p_arg, const char* croFileName )
  {
    CallClass * p_proc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, CallClass, ArgClass>( p_gman->GetProcManager(), p_arg, croFileName );
    // プロセスの終了を待つイベントの生成
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventProcessCall>( p_gman->GetGameEventManager() );
    return p_proc;
  }

  template<class CallClass>
  static CallClass* CallRoProc( GameSys::GameManager* p_gman, const char* croFileNameArr[] , const int roNum )
  {
    nn::ro::Module **roArr = GFL_NEW_LOW_ARRAY(p_gman->GetProcManager()->GetHeap())nn::ro::Module*[roNum];
    gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
    for( int i=0;i<roNum;i++ )
    {
      roArr[i] = roMgr->LoadModule(croFileNameArr[i]);
    }
    for( int i=0;i<roNum;i++ )
    {
      roMgr->StartModule(roArr[i]);
    }
    
    CallClass * p_proc = NIJI_PROC_CALL<GameSys::GameProcManager, CallClass>( p_gman->GetProcManager());
    // プロセスの終了を待つイベントの生成

    p_proc->CreateChildModulePtrBuffer( p_gman->GetProcManager()->GetHeap() , roNum );
    for( int i=0;i<roNum;i++ )
    {
      p_proc->SetChildModulePtr(i,roArr[i]);
    }
    
    GFL_DELETE_ARRAY roArr;
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventProcessCall>( p_gman->GetGameEventManager() );
    return p_proc;
  }

//#endif

  // コンストラクタデストラクタ
  EventProcessCall( gfl2::heap::HeapBase* heap ) :
    GameSys::GameEvent( heap ) {}

  virtual ~EventProcessCall(){}

  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   * @return  GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* p_gman);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* p_gman);

};

GFL_NAMESPACE_END( Field )

#endif  // EVENT_PROCESS_CALL_H_INCLUDED

