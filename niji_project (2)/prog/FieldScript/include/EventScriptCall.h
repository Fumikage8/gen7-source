//============================================================================
/**
 *
 *	@file		EventScriptCall.h
 *	@brief  スクリプト呼び出し
 *	@author hosaka genya
 *	@data		2011.08.24
 *
 */
//============================================================================
#ifndef __EVENTSCRIPTCALL_H__
#define __EVENTSCRIPTCALL_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "GameSys/include/GameEvent.h"
#include "FieldScript/include/ScriptTypes.h"

GFL_NAMESPACE_BEGIN( Field );

class IFieldActorBase;

namespace MoveModel{
  class FieldMoveModel;
}
namespace EventList{
  class EventListDataManager;
}


class EventScriptCall : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( EventScriptCall );

public:

  //----------------------------------------------------------------------------
  /**
   *	@brief  スクリプトコール
   *
   *	@param	p_gman    ゲームマネージャ
   *	@param  scriptID  スクリプトID
   *	@param  p_actor   ターゲットアクター(アクターが無関係の場合はNULLを代入)  
   *	@param  p_model   会話相手  
   *	@param  p0        引き渡すパラメータ(SCWK_PARAM0に入る）
   *	@param  p1        引き渡すパラメータ(SCWK_PARAM1に入る）
   *	@param  p2        引き渡すパラメータ(SCWK_PARAM2に入る）
   *	@param  p3        引き渡すパラメータ(SCWK_PARAM3に入る）
   *
   *	@return 生成したイベント
   */
  //-----------------------------------------------------------------------------
  static GameSys::GameEvent* CallScript(
      GameSys::GameManager* p_gman, u32 scriptID,
      IFieldActorBase* p_actor, 
      MoveModel::FieldMoveModel* p_model = NULL,
      u32 p0 = 0, u32 p1 = 0, u32 p2 = 0, u32 p3 = 0 );

  //----------------------------------------------------------------------------
  /**
   *	@brief  スクリプトコール ForceStopしない版
   *
   *	@param	p_gman    ゲームマネージャ
   *	@param  scriptID  スクリプトID
   *	@param  p_model   会話相手  
   *	@param  p0        引き渡すパラメータ(SCWK_PARAM0に入る）
   *	@param  p1        引き渡すパラメータ(SCWK_PARAM1に入る）
   *	@param  p2        引き渡すパラメータ(SCWK_PARAM2に入る）
   *	@param  p3        引き渡すパラメータ(SCWK_PARAM3に入る）
   *
   *	@return 生成したイベント
   */
  //-----------------------------------------------------------------------------
  static GameSys::GameEvent* CallScriptNotForceStop( GameSys::GameManager* p_gman, u32 scriptID,
      IFieldActorBase* p_actor = NULL, 
      MoveModel::FieldMoveModel* p_model = NULL, 
      u32 p0 = 0, u32 p1 = 0, u32 p2 = 0, u32 p3 = 0 );

  //----------------------------------------------------------------------------
  /**
   *	@brief  スクリプトコール BG
   *
   *	@param	p_gman    ゲームマネージャ
   *	@param  scriptID  スクリプトID
   *	@param  bgid      BGのID
   *
   *	@return 生成したイベント
   */
  //-----------------------------------------------------------------------------
  static GameSys::GameEvent* CallScriptBG( GameSys::GameManager* p_gman, u32 scriptID, u32 bgid );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  スクリプトコール POS
   *
   *	@param	p_gman    ゲームマネージャ
   *	@param  scriptID  スクリプトID
   *	@param  posid      POSのID
   *
   *	@return 生成したイベント
   */
  //-----------------------------------------------------------------------------
  static GameSys::GameEvent* CallScriptPos( GameSys::GameManager* p_gman, u32 scriptID, u32 posid );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  スクリプトコール シーン
   *
   *	@param	p_gman    ゲームマネージャ
   *	@param  scriptID  スクリプトID
   *	@param  p0        引き渡すパラメータ(SCWK_PARAM0に入る）
   *	@param  p1        引き渡すパラメータ(SCWK_PARAM1に入る）
   *	@param  p2        引き渡すパラメータ(SCWK_PARAM2に入る）
   *	@param  p3        引き渡すパラメータ(SCWK_PARAM3に入る）
   *
   *	@return 生成したイベント
   */
  //-----------------------------------------------------------------------------
  static GameSys::GameEvent* CallScriptScene(
      GameSys::GameManager* p_gman, u32 scriptID,
      u32 p0 = 0, u32 p1 = 0, u32 p2 = 0, u32 p3 = 0 );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief コンストラクタ
   */
  /* ------------------------------------------------------------------------- */
  EventScriptCall(  gfl2::heap::HeapBase* heap ) : GameSys::GameEvent( heap ), m_Seq(0), m_bForceStop(true), m_InitPauseCounter(0), m_pTargetModel(NULL), m_pTargetActor(NULL), m_ClearInterestViewFlag(false), m_DrawModelCheckCounter(0) { }
  virtual ~EventScriptCall(){}

  void SetTarget( IFieldActorBase* p_actor, MoveModel::FieldMoveModel* p_mdl ) { m_pTargetActor = p_actor; m_pTargetModel = p_mdl; }
  void SetScriptID( u32 scriptID ) { m_ScriptID = scriptID; }
  void SetForceStop( bool is_force_stop ) { m_bForceStop = is_force_stop; }
  void SetClearInterestViewFlag( b32 flag ) { m_ClearInterestViewFlag = flag; }

private:

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

  typedef bool (*SCR_END_CHECK_FUNC)( GameSys::GameManager* p_gman );
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 終了時のチェック
   *
   * @return TRUE：終了
   */
  /* ------------------------------------------------------------------------- */
  bool EndCheck( GameSys::GameManager* p_gman );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief スクリプト終了時のWordsetバッファークリア処理
   *
   * @param p_gman ゲームマネージャーへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  void WordsetBufClear( GameSys::GameManager* p_gman );
  
private:
  u32 m_Seq;
  u32 m_ScriptID;
  bool m_bForceStop;
  bool m_bFieldDLLRecover;
  int m_InitPauseCounter;
  MoveModel::FieldMoveModel* m_pTargetModel;
  IFieldActorBase* m_pTargetActor;

  b8  m_ClearInterestViewFlag;          ///< trueなら自機が停止した際に注目を切る
  u32 m_DrawModelCheckCounter;

  EventList::EventListDataManager* pEventListDataManager;
  
};

GFL_NAMESPACE_END( Field );


#endif // __EVENTSCRIPTCALL_H__

