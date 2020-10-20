//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file    EventProcessCall.cpp
 *  @brief  プロセスの終了を待つイベント
 *  @author  tomoya takahashi
 *  @date    2011.06.06
 *
 *  @niji_author saita_kazuki
 *  @date 2015.05.29
 */
//=============================================================================

#include "Field/FieldStatic/include/Event/EventProcessCall.h"
#include "GameSys/include/GameManager.h"

GFL_NAMESPACE_BEGIN( Field )

//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool EventProcessCall::BootChk(GameSys::GameManager* /*p_gman*/)
{
  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventProcessCall::InitFunc(GameSys::GameManager* /*p_gman*/)
{
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @return  GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventProcessCall::MainFunc(GameSys::GameManager* p_gman)
{
  // プロセスがいなくなったら元に戻る。
  if( p_gman->IsProcessExists() == false ){
    return GameSys::GMEVENT_RES_FINISH;
  }
  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventProcessCall::EndFunc(GameSys::GameManager* /*p_gman*/)
{
}


GFL_NAMESPACE_END( Field )
