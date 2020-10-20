//=============================================================================
/**
 *
 *	@file		EventBattleDll.h
 *	@brief  バトルDLL非同期読み込み
 *	@author	tamada
 *	@date		2013.03.22
 *
    2015.12.13  sangoから移植開始
 */
//=============================================================================

#if !defined __EVENT_BATTLE_DLL_H__
#define __EVENT_BATTLE_DLL_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

// gamesys
#include "GameSys/include/GameEvent.h"

#include "Battle/include/battle_proc.h"

namespace GameSys { class DllReadThread; }

GFL_NAMESPACE_BEGIN( Field )

  //--------------------------------------------------------------
  /**
   * @class EventBattleDll
   * @brief バトル用DLLのスレッド読み込みをラップするイベント
   */
  //--------------------------------------------------------------
  class EventBattleDll : public GameSys::GameEvent
  {
      GFL_FORBID_COPY_AND_ASSIGN(EventBattleDll);

  public:

  // コンストラクタデストラクタ
  EventBattleDll( gfl2::heap::HeapBase* heap ) : 
    GameSys::GameEvent( heap ),
    m_Seq(0),
    m_isBattleFes(false)
  {}

  virtual ~EventBattleDll(){}

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

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   p_gman       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  void SetParameter( btl::BATTLE_PROC_PARAM * p_param ) { m_Param = *p_param; }

  //--------------------------------------------------------------
  /**
  * @brief   バトルフェスの呼び出しか否か
  * @param   bool 
  */
  //--------------------------------------------------------------
  void SetIsBattleFes(bool isBattleFes) { m_isBattleFes = isBattleFes; }

  private:
    int m_Seq;
    GameSys::DllReadThread * m_BattleDllRead;
#if PM_DEBUG
    GameSys::DllReadThread * m_BattleDllReadDebug;
#endif // PM_DEBUG
    btl::BATTLE_PROC_PARAM   m_Param;
    bool                     m_isBattleFes;
  };

GFL_NAMESPACE_END( Field )

#endif // __EVENT_BATTLE_DLL_H__