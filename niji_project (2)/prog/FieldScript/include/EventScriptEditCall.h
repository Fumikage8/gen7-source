//=============================================================================
/**
 *
 *  @file   EventScriptEditCall.h
 *  @brief  スクリプト エディットモード呼び出し
 *  @author Miyuki Iwasawa 
 *  @data   2015.10.24
 *
 */
//=============================================================================
#ifndef __EVNTSCRIPTEDITCALL_H__
#define __EVNTSCRIPTEDITCALL_H__
#pragma once

#if PM_DEBUG

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "GameSys/include/GameEvent.h"
#include "FieldScript/include/ScriptTypes.h"
#include "FieldScript/include/ScriptDebug.h"
#include "Field/FieldStatic/include/FieldLocation.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldActorBase.h"

namespace gfl2{
  namespace Fade{
    class FadeManager;
  }
}
namespace FieldScript{
  class FieldScriptSystem;
}

GFL_NAMESPACE_BEGIN( Field );

class IFieldActorBase;

namespace MoveModel{
  class FieldMoveModel;
}

class EventScriptEditCall : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( EventScriptEditCall );

public:

  //----------------------------------------------------------------------------
  /**
   *	@brief  スクリプトコール
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
  static GameSys::GameEvent* CallScriptEditEvent(
      GameSys::GameManager* p_gman, u32 scriptID,
      IFieldActorBase* p_actor = NULL,
      MoveModel::FieldMoveModel* p_model = NULL,
      u32 p0 = 0, u32 p1 = 0, u32 p2 = 0, u32 p3 = 0 );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief コンストラクタ
   */
  /* ------------------------------------------------------------------------- */
  EventScriptEditCall(  gfl2::heap::HeapBase* heap );
  virtual ~EventScriptEditCall(){}

  void SetTargetModel( MoveModel::FieldMoveModel* p_mdl ) { m_pTargetModel = p_mdl; }
  void SetScriptID( int scriptID ) { m_ScriptID = scriptID; }
  void SetInitialParam( GameSys::GameManager* p_gman, int scriptID, IFieldActorBase* p_actor, MoveModel::FieldMoveModel* p_mdl );

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
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 指定スクリプトのコール
   *
   * @param p_gman ゲームマネージャーへのポインタ
   */
  /* ------------------------------------------------------------------------- */
  int SeqSceneInit( GameSys::GameManager* p_gman );
  int SeqSceneWait( GameSys::GameManager* p_gman );
  int SeqScriptInit( GameSys::GameManager* p_gman );
  int SeqScriptWait( GameSys::GameManager* p_gman );
  int SeqScriptUpdate( GameSys::GameManager* p_gman );
  int SeqScriptEnd( GameSys::GameManager* p_gman );
  int SeqSceneReset( GameSys::GameManager* p_gman );

  void FadeOut(GameSys::GameManager* p_gman);
  void FadeIn(GameSys::GameManager* p_gman);
  bool FadeWait(GameSys::GameManager* p_gman);

private:
  enum{
    SEQ_SCENE_INIT,
    SEQ_SCENE_WAIT,
    SEQ_SCRIPT_INIT,
    SEQ_SCRIPT_UPDATE,
    SEQ_SCRIPT_END,
    SEQ_SCENE_RESET,
    SEQ_END,
  };
private:
  u32 m_Seq;
  u32 m_SubSeq;
  
  int m_ScriptID; //起動スクリプトID
  int m_SkipPoint;   //スキップポイント
  int m_SkipPointEnd;   //スキップポイントEnd
  int m_SkipPointMax;   //スキップポイントMax
  bool m_InitialFade;   //初期フェードステータス
  bool m_ExitFlag;   //終了フラグ

  Field::Location  m_StartLoc;  //起動場所の保存
  FieldScript::DebugSceneEditParam m_SceneEditParam;

  FieldScript::FieldScriptSystem* m_pScriptSys;
  MoveModel::FieldMoveModel* m_pTargetModel;
  u32 m_TargetActorID;
  FieldActorType m_TargetActorType;

  b8  m_ClearInterestViewFlag;          ///< trueなら自機が停止した際に注目を切る

  gfl2::Fade::FadeManager* m_pFadeManager;

};

GFL_NAMESPACE_END( Field );


#endif //PM_DEBUG


#endif // __EVNTSCRIPTEDITCALL_H__

