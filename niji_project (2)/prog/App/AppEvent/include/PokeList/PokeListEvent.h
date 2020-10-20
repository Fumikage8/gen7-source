#ifndef NIJI_PROJECT_APP_POKELIST_EVENT_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_EVENT_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListTestEvent.h
 * @author  fukushima_yuya
 * @date    2015.03.03
 * @brief   ポケモンリスト画面イベント
 */
//==============================================================================

// niji
#include <App/PokeList/include/PokeListAppParam.h>
#include <App/PokeList/include/PokeListSetupParam.h>
#include <App/Status/include/StatusAppParam.h>
#include <App/Bag/include/BagAppParam.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <Debug/Launcher/include/LauncherProc.h>
#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameManager.h>
#include <App/WazaOshie/include/WazaOshieAppParam.h>
#include <NetStatic/NetEvent/include/EvolutionEvent.h>


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
class PokeListEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeListEvent );  //! コピーを禁止

public:
  enum
  {
    FRAME_ID_POKELIST_FIELD = 0,
    FRAME_ID_POKELIST_JOIN,
    FRAME_ID_POKELIST_SIMPLE,
    FRAME_ID_POKELIST_SIMPLE2,
    FRAME_ID_POKELIST_SODATEYA,
    FRAME_ID_POKELIST_WAZAOSHIE,
    FRAME_ID_POKELIST_FIELD_DEBUG,
    FRAME_ID_MAX,
  };

public:
#if PM_DEBUG
  debug::launcher::LauncherProc::CallParam m_LauncherParam;
#endif // PM_DEBUG

public:
  //------------------------------------------------------------------
  /**
   * @brief   このイベントを起動する関数
   * @param   pGameMan    ゲームマネージャ
   * @param   index       項目インデックス
   * @param   debugFlag   デバッグフラグ(デフォルトはfalse)
   */
  //------------------------------------------------------------------
  static PokeListEvent * StartEvent( GameSys::GameManager* pGameMan, u32 index = 0, bool debugFlag = false );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   pHeap    workをとるためのメモリクラス
   */
  //------------------------------------------------------------------
  PokeListEvent( gfl2::heap::HeapBase* pHeap );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeListEvent( void );

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

private:
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

  u32   m_ProcSeq;

  bool  m_isInit;

  App::PokeList::Proc*              m_pPokeListProc;

  app::bag::BagProc*                m_pBagProc;

  App::Status::Proc*                m_pStatusProc;

  gfl2::fs::AsyncFileManager*       m_pAsyncFileManager;      //!< ファイルマネージャー

  app::util::AppRenderingManager*   m_pAppRenderingManager;   //!< レンダリングマネージャー

  app::util::Heap*                  m_pAppHeap;

  App::PokeList::APP_PARAM          m_PokeListAppParam;

  App::PokeList::CONTEXT_PARAM      m_PokeListContext;

  App::Status::APP_PARAM            m_StatusAppParam;

  app::bag::APP_PARAM               m_BagAppParam;

  App::WazaOshie::APP_PARAM         m_WazaOshieParam;         //!< 技教え画面外部設定ワーク

  NetEvent::Evolution::EvolutionEventParam m_EvolutionParam;  //!< 進化デモ用ワーク

  static u32    m_Index;

  static bool   m_DebugFlag;
};


GFL_NAMESPACE_END( event )
GFL_NAMESPACE_END( app )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_EVENT_H_INCLUDED
