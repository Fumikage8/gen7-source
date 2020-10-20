//======================================================================
/**
 * @file EventDebugAppReturn.h
 * @date 2016/06/17 16:14:05
 * @author takahashi_tomoya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG

#if !defined __EVENT_DEBUG_APP_RETURN_H_INCLUDED__
#define __EVENT_DEBUG_APP_RETURN_H_INCLUDED__
#pragma once


#include "Field/FieldStatic/include/Event/EventMapJump.h"

GFL_NAMESPACE_BEGIN(Field)

/**
 *  @class アプリケーションジャンプをして戻ってくるイベント
 */
class DebugAppReturn : public GameSys::GameEvent
{
public:
  enum Mode
  {
    MODE_APP_KEEP,
    MODE_APP,
    MODE_JF,
    MODE_MAX
  };

public:
  DebugAppReturn( gfl2::heap::HeapBase* heap ) : GameSys::GameEvent(heap), m_Mode(MODE_APP_KEEP){}

  virtual ~DebugAppReturn(){}

  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* gmgr);

  /**
   *  @brief モードを設定
   */
  void SetMode( Mode mode )
  {
    m_Mode = mode;
  }

private:

  u32 m_Mode;
  Location m_playerLocation;
};


GFL_NAMESPACE_END(Field)

#endif // __EVENT_DEBUG_APP_RETURN_H_INCLUDED__


#endif // PM_DEBUG