#ifndef __GAMEPROCMANAGER_H__
#define __GAMEPROCMANAGER_H__
//=============================================================================================
/**
 * @file   GameProcManager.h
 * @brief  ゲームのプロセスマネージャー
 * @author k.ohno
 * @date   2011.4.19
 */
//=============================================================================================
#pragma once

#include <proc/include/gfl2_Proc.h>
#include <ro/include/gfl2_RoManager.h>

#include "GameFrameMode.h"
#include "GameProc.h"

/**
  呼び出しにはこちらを使ってください。
  NEWは外部 DELETEは内部で行う設計となってしまったので
  define定義で隠す事で違和感を緩和するねらいです
 */

//===========================================================================================
// 静的モジュール用PROC
//===========================================================================================
#if 0
//!PROCの呼び出し（下位コール）
#define NIJI_PROC_CALL( pManager, CallClass )            \
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass; \
  pCC->SetGameProcManager( pManager ); \
  pManager->CallProc( pCC )

//!PROCの切り替え
#define NIJI_PROC_SETNEXT( pManager, NextClass )            \
  NextClass* pCC = GFL_NEW(pManager->GetHeap()) NextClass;  \
  pCC->SetGameProcManager( pManager );                      \
  pManager->SetNextProc( pCC )
#endif

//!PROCの生成   NIJI_PROC_CALL<ManagerClass名, CallClass名>(ManagerClass*)
template <class ManagerClass, class CallClass> CallClass* NIJI_PROC_CREATE(ManagerClass* pManager)
{
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass;
  pCC->SetGameProcManager( pManager );
  return pCC;
}
//!PROCの生成( 引き数付きコンストラクタ版 )   NIJI_PROC_CALL<ManagerClass名, CallClass名, ArgClass名>(ManagerClass*, ArgClass*)
template <class ManagerClass, class CallClass, class ArgClass> CallClass* NIJI_PROC_CREATE(ManagerClass* pManager, ArgClass* pArgument)
{
  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass( pArgument );
  pCC->SetGameProcManager( pManager );
  return pCC;
}

//!PROCの呼び出し   NIJI_PROC_CALL<ManagerClass名, CallClass名>(ManagerClass*)
template <class ManagerClass, class CallClass> CallClass* NIJI_PROC_CALL(ManagerClass* pManager)
{
  CallClass* pCC = NIJI_PROC_CREATE<ManagerClass,CallClass>(pManager);
  pCC->SetGameProcManager( pManager );
  pManager->CallProc( pCC );
  return pCC;
}
//!PROCの呼び出し( 引き数付きコンストラクタ版 )   NIJI_PROC_CALL<ManagerClass名, CallClass名, ArgClass名>(ManagerClass*, ArgClass*)
template <class ManagerClass, class CallClass, class ArgClass> CallClass* NIJI_PROC_CALL(ManagerClass* pManager, ArgClass* pArgument)
{
  CallClass* pCC = NIJI_PROC_CREATE<ManagerClass,CallClass,ArgClass>(pManager,pArgument);
  pCC->SetGameProcManager( pManager );
  pManager->CallProc( pCC );
  return pCC;
}

//!PROCの切り替え（下位コール）   NIJI_PROC_SETNEXT<ManagerClass名, CallClass名>(ManagerClass*)
template <typename ManagerClass, typename CallClass> CallClass* NIJI_PROC_SETNEXT(ManagerClass* pManager)
{
  CallClass* pCC = NIJI_PROC_CREATE<ManagerClass,CallClass>(pManager);
  pCC->SetGameProcManager( pManager );
  pManager->SetNextProc( pCC );
  return pCC;
}
//!PROCの切り替え（下位コール, 引き数付きコンストラクタ版）   NIJI_PROC_SETNEXT<ManagerClass名, CallClass名, ArgClass名>(ManagerClass*, ArcClass*)
template <typename ManagerClass, typename CallClass, typename ArgClass> CallClass* NIJI_PROC_SETNEXT(ManagerClass* pManager, ArgClass* pArgument)
{
  CallClass* pCC = NIJI_PROC_CREATE<ManagerClass,CallClass,ArgClass>(pManager,pArgument);
  pCC->SetGameProcManager( pManager );
  pManager->SetNextProc( pCC );
  return pCC;
}


//===========================================================================================
// 動的モジュール用PROC
//===========================================================================================
//!PROCの生成   NIJI_PROC_CALL<ManagerClass名, CallClass名>(ManagerClass*)
template <class ManagerClass , class CallClass> CallClass* NIJI_RO_PROC_CREATE(ManagerClass* pManager, gfl2::fs::AsyncFileManager *fileMng , const char* croFileName)
{
  gfl2::ro::Module* pModule = gfl2::ro::StartModuleFunc( fileMng , croFileName );

  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass;
  pCC->SetModulePtr( pModule );
  pCC->SetGameProcManager( pManager );

  return pCC;
}

//!PROCの生成( 引き数付きコンストラクタ版 )   NIJI_PROC_CALL<ManagerClass名, CallClass名, ArgClass名>(ManagerClass*, ArgClass*)
template <class ManagerClass , class CallClass, class ArgClass> CallClass* NIJI_RO_PROC_CREATE(ManagerClass* pManager, ArgClass* pArgument, gfl2::fs::AsyncFileManager *fileMng , const char* croFileName )
{
  gfl2::ro::Module* pModule = gfl2::ro::StartModuleFunc( fileMng , croFileName );

  CallClass* pCC = GFL_NEW(pManager->GetHeap()) CallClass( pArgument );
  pCC->SetGameProcManager( pManager );
  pCC->SetModulePtr( pModule );

  return pCC;
}

//!PROCの呼び出し   NIJI_PROC_CALL<ManagerClass名, CallClass名>(ManagerClass*)
template <class ManagerClass , class CallClass> CallClass* NIJI_RO_PROC_CALL(ManagerClass* pManager, gfl2::fs::AsyncFileManager *fileMng , const char* croFileName)
{
  CallClass* pCC = NIJI_RO_PROC_CREATE<ManagerClass,CallClass>(pManager, fileMng , croFileName);
  pCC->SetGameProcManager( pManager );
  pManager->CallProc( pCC );
  return pCC;
}

//! PROCの呼び出し( 引き数付きコンストラクタ版 )   NIJI_PROC_CALL<ManagerClass名, CallClass名, ArgClass名>(ManagerClass*, ArgClass*)
template <class ManagerClass , class CallClass, class ArgClass> CallClass* NIJI_RO_PROC_CALL(ManagerClass* pManager, ArgClass* pArgument, gfl2::fs::AsyncFileManager *fileMng , const char* croFileName )
{
  CallClass* pCC = NIJI_RO_PROC_CREATE<ManagerClass,CallClass,ArgClass>(pManager, pArgument, fileMng , croFileName);
  pCC->SetGameProcManager( pManager );
  pManager->CallProc( pCC );

  return pCC;
}

//! PROCの切り替え（下位コール）   NIJI_PROC_SETNEXT<ManagerClass名, CallClass名>(ManagerClass*)
template <typename ManagerClass, typename CallClass> CallClass* NIJI_RO_PROC_SETNEXT(ManagerClass* pManager, gfl2::fs::AsyncFileManager *fileMng , const char* croFileName)
{
  CallClass* pCC = NIJI_RO_PROC_CREATE<ManagerClass,CallClass>(pManager, fileMng , croFileName);
  pCC->SetGameProcManager( pManager );
  pManager->SetNextProc( pCC );
  return pCC;
}

//! PROCの切り替え( 引き数付きコンストラクタ版 )   NIJI_PROC_SETNEXT<ManagerClass名, CallClass名, ArgClass名>(ManagerClass*, ArgClass*)
template <class ManagerClass , class CallClass, class ArgClass> CallClass* NIJI_RO_PROC_SETNEXT(ManagerClass* pManager, ArgClass* pArgument, gfl2::fs::AsyncFileManager *fileMng , const char* croFileName )
{
  CallClass* pCC = NIJI_RO_PROC_CREATE<ManagerClass,CallClass,ArgClass>(pManager, pArgument, fileMng , croFileName);
  pCC->SetGameProcManager( pManager );
  pManager->SetNextProc( pCC );

  return pCC;
}

namespace GameSys{

  class GameManager;
  class GameProc;

  class GameProcManager : public gfl2::proc::Manager{
    GFL_FORBID_COPY_AND_ASSIGN(GameProcManager); //コピーコンストラクタ＋代入禁止

  private:
    GameManager* mpGameManager;     ///< ゲームマネージャー
    b32          m_IsCallNextFrame; ///< NextFrame動作を行うかのフラグ

  public:
    GameProcManager(gfl2::heap::HeapBase* pBase, GameManager* pGameManager);  //コンストラクタ
    virtual ~GameProcManager(void){};  //デストラクタ


    //------------------------------------------------------------------
    /**
     * @brief	PROCのイベント検知処理
     * 登録したPROCのEventCheckFunc処理を行います。 適切な位置で呼んでください。
     */
    //------------------------------------------------------------------
    void EventCheck(void);


    //------------------------------------------------------------------
    /**
     * @brief	PROCの処理
     * 登録したPROCの処理を行います。 適切な位置で呼んでください。
     * @return MainStatus ステータスの動きが帰ってきます
     */
    //------------------------------------------------------------------
    gfl2::proc::MainStatus Main(void);

    //------------------------------------------------------------------
    /**
     * @brief	PROCの処理
     * 登録したPROCのNextFrameMainFunc処理を行います。 適切な位置で呼んでください。
     * @return MainStatus ステータスの動きが帰ってきます
     */
    //------------------------------------------------------------------
    gfl2::proc::MainStatus NextFrameMain(void);

    //------------------------------------------------------------------
    /**
     * @brief   GameManagerを得る
     * @return
     */
    //------------------------------------------------------------------
    inline GameManager* GetGameManager(void){ return mpGameManager; }

  };  //class GameProc


}//namespace GameSys 

#endif //__GAMEPROC_H__
