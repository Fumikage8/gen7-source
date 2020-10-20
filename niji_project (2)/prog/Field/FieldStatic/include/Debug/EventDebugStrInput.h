//======================================================================
/**
 * @file EventDebugStrInput.h
 * @date 2016/01/28 15:33:35
 * @author miyachi_soichi
 * @brief デバッグ：キャラクタ名入力
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __EVENT_DEBUG_STRINPUT_H_INCLUDED__
#define __EVENT_DEBUG_STRINPUT_H_INCLUDED__
#pragma once
#if PM_DEBUG

// gfl2
#include <heap/include/gfl2_heap.h>
#include <str/include/gfl2_Str.h>
// event
#include "GameSys/include/GameEvent.h"
// location
#include "Field/FieldStatic/include/FieldLocation.h"
// param
#include "App/StrInput/include/StrInputAppParam.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 *  @class  EventDebugStrInput
 *  @brief デバッグ：キャラクタ名入力
 */
class EventDebugStrInput : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventDebugStrInput);

public:
  /**
   *  @brief  コンストラクタ
   *  @param  pHeap   workようヒープ
   */
  EventDebugStrInput( gfl2::heap::HeapBase *pHeap );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EventDebugStrInput( void ){}

public:
  /**
   *  @brief  イベントを動かしてよいかどうかの判定関数
   *  @param  pGameManager  ゲームマネージャ
   *  @retval true  起動して良い
   *  @retval false 起動してはいけない
   */
  virtual bool BootChk( GameSys::GameManager *pGameManager );

  /**
   *  @brief  初期化関数
   *  @param  pGameManager  ゲームマネージャ
   */
  virtual void InitFunc( GameSys::GameManager *pGameManager );

  /**
   *  @brief  メイン関数
   *  @param  pGameManager    ゲームマネージャ
   *  @return GMEVENT_RESUTLT イベント制御関数の戻り値
   */
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager *pGameManager );

  /**
   *  @brief  終了関数
   *  @param  pGameManager    ゲームマネージャ
   */
  virtual void EndFunc( GameSys::GameManager *pGameManager );

private:
  enum SeqNo
  {
    SEQ_NO_CloseField,
    SEQ_NO_CreateProc,
    SEQ_NO_WaitProc,
    SEQ_NO_OpenField,
    SEQ_NO_End,
  };

  App::StrInput::StrInputParam *m_pAppParam;
  Field::Location m_player_location;
  gfl2::str::StrBuf name;

}; // class EventDebugStrInput : public GameSys::GameEvent

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
#endif // __EVENT_DEBUG_STRINPUT_H_INCLUDED__
