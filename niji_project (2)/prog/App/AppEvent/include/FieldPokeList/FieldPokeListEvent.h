#if !defined( __APP_EVENT_FIELD_POKELIST_EVENT_H__ )
#define __APP_EVENT_FIELD_POKELIST_EVENT_H__
#pragma once
//======================================================================
/**
 * @file    FieldPokeListEvent.h
 * @date    2015/10/01 18:34:41
 * @author  fukushima_yuya
 * @brief   アプリイベント：フィールドポケリスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/PokeList/include/PokeListAppParam.h>
#include <App/PokeList/include/PokeListSetupParam.h>
#include <App/Status/include/StatusAppParam.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameManager.h>



//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(PokeList)
class Proc;
GFL_NAMESPACE_END(PokeList)
GFL_NAMESPACE_BEGIN(Status)
class Proc;
GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( event )

//------------------------------------------------------------------------------
// @brief   ポケモンリスト対象選択画面イベント
//------------------------------------------------------------------------------
class FieldPokeListEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( FieldPokeListEvent );  //! コピーを禁止

public:
  //------------------------------------------------------------------
  /**
   * @brief   このイベントを起動する関数
   * @param   pGameMan    ゲームマネージャ
   * @param   index       項目インデックス
   * @param   debugFlag   デバッグフラグ(デフォルトはfalse)
   */
  //------------------------------------------------------------------
  static FieldPokeListEvent * StartEvent( GameSys::GameManager* pGameMan );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   pHeap    workをとるためのメモリクラス
   */
  //------------------------------------------------------------------
  FieldPokeListEvent( gfl2::heap::HeapBase* pHeap );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~FieldPokeListEvent( void ) { ; }

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

private:
  //------------------------------------------------------------------
  /**
   * @brief   App::PokeList::Procの生成
   * @param   gm    GameSys::GameManager
   */
  //------------------------------------------------------------------
  void CreatePokeListProc( GameSys::GameManager* gm );

  //------------------------------------------------------------------
  /**
   * @brief   App::Status::Procの生成
   */
  //------------------------------------------------------------------
  void CreateStatusProc( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリスト外部設定パラメータのセット
   * @param   context   ポケモンリスト外部設定パラメータ
   */
  //------------------------------------------------------------------
  void SetPokeListAppParam( App::PokeList::CONTEXT_PARAM * context );

private:
  u32   m_Seq;

private:
  App::PokeList::Proc*            m_pPokeListProc;

  App::PokeList::CONTEXT_PARAM*   m_pContextParam;

private:
  App::Status::Proc*      m_pStatusProc;

  App::Status::APP_PARAM  m_StatusAppParam;
};

GFL_NAMESPACE_END( event )
GFL_NAMESPACE_END( app )

#endif // __APP_EVENT_FIELD_POKELIST_EVENT_H__