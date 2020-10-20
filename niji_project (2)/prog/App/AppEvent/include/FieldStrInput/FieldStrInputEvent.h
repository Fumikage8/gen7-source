#if !defined( __APP_EVENT_FIELD_STRINPUT_H_INCLUDED__ )
#define __APP_EVENT_FIELD_STRINPUT_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FieldStrInputEvent.h
 * @date    2015/12/21 15:00:44
 * @author  fukushima_yuya
 * @brief   文字入力イベント：フィールド用
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/StrInput/include/StrInputAppParam.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameManager.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(StrInput)
class Proc;
GFL_NAMESPACE_END(StrInput)
GFL_NAMESPACE_END(App )


GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( event )

//------------------------------------------------------------------------------
// @brief   ポケモンリスト対象選択画面イベント
//------------------------------------------------------------------------------
class FieldStrInputEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( FieldStrInputEvent );  //! コピーを禁止

public:
  //------------------------------------------------------------------
  /**
   * @brief   このイベントを起動する関数
   * @param   pGameMan    ゲームマネージャ
   * @param   index       項目インデックス
   * @param   debugFlag   デバッグフラグ(デフォルトはfalse)
   */
  //------------------------------------------------------------------
  static FieldStrInputEvent * StartEvent( GameSys::GameManager* pGameMan );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   pHeap    workをとるためのメモリクラス
   */
  //------------------------------------------------------------------
  FieldStrInputEvent( gfl2::heap::HeapBase* pHeap );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~FieldStrInputEvent( void ) { ; }

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
   * @brief   Procの生成
   * @param   gm    GameSys::GameManager
   */
  //------------------------------------------------------------------
  void CreateProc( GameSys::GameManager* gm );

public:
  //------------------------------------------------------------------
  /**
   * @brief   外部設定パラメータのセット
   * @param   param   外部設定パラメータ
   */
  //------------------------------------------------------------------
  void SetAppParam( App::StrInput::APP_PARAM * param );

private:
  u32   m_Seq;

  App::StrInput::Proc*        m_pProc;

  App::StrInput::APP_PARAM*   m_pAppParam;

  App::StrInput::APP_PARAM    m_StrInputAppParam;
};

GFL_NAMESPACE_END( event )
GFL_NAMESPACE_END( app )

#endif // __APP_EVENT_FIELD_STRINPUT_H_INCLUDED__