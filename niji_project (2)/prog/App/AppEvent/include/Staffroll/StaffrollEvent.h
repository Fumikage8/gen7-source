//======================================================================
/**
 * @file StaffrollEvent.h
 * @date 2017/03/21 20:16:18
 * @author saito_nozomu
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined( __APP_EVENT_STAFFROLL_H_INCLUDED__ )
#define __APP_EVENT_STAFFROLL_H_INCLUDED__
#pragma once

/// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "heap/include/gfl2_Heap.h"

#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameManager.h>

#include "Field/FieldStatic/include/FieldLocation.h"

//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//=============================================================================
/**
* @class LunaSolDiveEvent
* @brief ルナソルダイブイベントクラス
*/
//=============================================================================
class StaffrollEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(StaffrollEvent);   // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  *
  * @param   heap  ヒープ
  */
  //-----------------------------------------------------------------------------
  StaffrollEvent(gfl2::heap::HeapBase * heap);

  //-----------------------------------------------------------------------------
  /**
  * @brief   デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~StaffrollEvent();

  //-----------------------------------------------------------------------------
  /**
  * @func    StartEvent
  * @brief   イベント起動
  * @date    2015.06.11
  *
  * @param   man     ゲームマネージャクラス
  *
  * @return  LunaSolDiveEventクラス
  */
  //-----------------------------------------------------------------------------
  static StaffrollEvent * StartEvent(GameSys::GameManager * gmgr);

  //-----------------------------------------------------------------------------
  /**
  * @func    BookChk
  * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
  * @date    2015.06.11
  *
  * @param   gmgr  ゲームマネージャークラス
  *
  * @retval  true  = 起動してよい
  * @retval  false = 起動しない
  */
  //-----------------------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager * gmgr);

  //-----------------------------------------------------------------------------
  /**
  * @func    BookChk
  * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
  * @date    2015.06.11
  *
  * @param   gmgr  ゲームマネージャークラス
  */
  //-----------------------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager * gmgr);

  //-----------------------------------------------------------------------------
  /**
  * @func    BookChk
  * @brief   呼び出される関数 純粋仮想関数
  * @date    2015.06.11
  *
  * @param   gmgr  ゲームマネージャークラス
  *
  * @return  イベント制御関数の戻り値
  */
  //-----------------------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager * gmgr);

  //-----------------------------------------------------------------------------
  /**
  * @func    BookChk
  * @brief   呼び出される関数 純粋仮想関数
  * @date    2015.06.11
  *
  * @param   gmgr  ゲームマネージャークラス
  */
  //-----------------------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager * gmgr);
  
private:
  Field::Location m_playerLocation;
};

GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif // // __APP_EVENT_FIELD_STAFFROLL_H_INCLUDED__