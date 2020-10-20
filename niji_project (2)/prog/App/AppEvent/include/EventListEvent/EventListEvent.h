#ifndef NIJI_PROJECT_APP_EVENTLIST_EVENT_H_INCLUDED
#define NIJI_PROJECT_APP_EVENTLIST_EVENT_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    EventListEvent.h
 * @author  k.ohno
 * @date    2015.09.15
 * @brief   ポケモンリスト画面イベント
 */
//==============================================================================

// niji
#include <AppLib/include/Util/app_util_heap.h>
#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameManager.h>

#include <App/EventList/include/EventListAppParam.h>

//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class Proc;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_BEGIN( Status )
class Proc;
GFL_NAMESPACE_END( Status )
GFL_NAMESPACE_BEGIN( Tool )
class TimeLimit;
GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( bag )
class BagProc;
GFL_NAMESPACE_END( bag )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( event )

//------------------------------------------------------------------------------
// @brief   ポケモンリスト画面イベント
//------------------------------------------------------------------------------
class EventListEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( EventListEvent );  //! コピーを禁止


public:
  debug::launcher::LauncherProc::CallParam m_LauncherParam;

public:
  //------------------------------------------------------------------
  /**
   * @brief   このイベントを起動する関数
   * @param   pGameMan    ゲームマネージャ
   * @param   index       項目インデックス
   * @param   debugFlag   デバッグフラグ(デフォルトはfalse)
   */
  //------------------------------------------------------------------
  static EventListEvent * StartEvent( GameSys::GameManager* pGameMan );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   pHeap    workをとるためのメモリクラス
   */
  //------------------------------------------------------------------
  EventListEvent( gfl2::heap::HeapBase* pHeap );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~EventListEvent( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //------------------------------------------------------------------
  virtual bool BootChk( GameSys::GameManager* /*gmgr*/ );

  //------------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //------------------------------------------------------------------
  virtual void InitFunc( GameSys::GameManager* /*gmgr*/ );

  //------------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   pGameMan         ゲームマネージャークラス
   * @return  GMEVENT_RESULT   イベント制御関数の戻り値
   */
  //------------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameMan );

  //------------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //------------------------------------------------------------------
  virtual void EndFunc( GameSys::GameManager* /*gmgr*/ );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリスト外部設定パラメータのセット
   * @param   context   ポケモンリスト外部設定パラメータ
   */
  //------------------------------------------------------------------
  void SetPokeListAppParam( App::PokeList::CONTEXT_PARAM * context );

private:
  //------------------------------------------------------------------
  /**
   * @brief   外部パラメータのセット
   * @param   gm    ゲームマネージャ
   */
  //------------------------------------------------------------------
  void SetAppParam( GameSys::GameManager* gm );

  //------------------------------------------------------------------
  /**
   * @brief   プロックの生成
   */
  //------------------------------------------------------------------
  void CreateProc( u32 procID );

  //------------------------------------------------------------------
  /**
   * @brief   プロックの終了チェック
   */
  //------------------------------------------------------------------
  void CheckEndProc( GameSys::GameManager* gm );

private:
  u32   m_Seq;


  gfl2::fs::AsyncFileManager*       m_pAsyncFileManager;      //!< ファイルマネージャー

  app::util::AppRenderingManager*   m_pAppRenderingManager;   //!< レンダリングマネージャー

  app::util::Heap*                  m_pAppHeap;

  App::EventList::APP_PARAM          m_EventListAppParam;


};


GFL_NAMESPACE_END( event )
GFL_NAMESPACE_END( app )

#endif  // NIJI_PROJECT_APP_EVENTLIST_EVENT_H_INCLUDED
