#if PM_DEBUG
//=============================================================================
/**
 * @file    EventDebugItemMake.h
 * @brief   デバッグ道具生成画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2015.11.17
 */
//=============================================================================
#if !defined( EVENT_DEBUG_ITEMMAKE_H_INCLUDED )
#define EVENT_DEBUG_ITEMMAKE_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "heap/include/gfl2_Heap.h"
// niji
#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameManager.h"
#include "Debug/DebugItemMake/include/ItemMakeProc.h"


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

//=============================================================================
/**
 * @class EventDebugItemMake
 * @brief デバッグ道具生成画面イベントクラス
 * @date  2015.11.17
 */
//=============================================================================
class EventDebugItemMake
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventDebugItemMake);   // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  EventDebugItemMake( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~EventDebugItemMake();

  //-----------------------------------------------------------------------------
  /**
   * @func    StartEvent
   * @brief   イベント起動
   * @date    2015.11.06
   *
   * @param   man     ゲームマネージャクラス
   *
   * @return  EventDebugItemMakeクラス
   */
  //-----------------------------------------------------------------------------
  static EventDebugItemMake * StartEvent( GameSys::GameManager * gmgr, bool flag );

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @date    2015.11.06
   *
   * @param   gmgr  ゲームマネージャークラス
   *
   * @retval  true  = 起動してよい
   * @retval  false = 起動しない
   */
  //-----------------------------------------------------------------------------
  virtual bool BootChk( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @date    2015.11.06
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void InitFunc( GameSys::GameManager * gmgr);

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   呼び出される関数 純粋仮想関数
   * @date    2015.11.06
   *
   * @param   gmgr  ゲームマネージャークラス
   *
   * @return  イベント制御関数の戻り値
   */
  //-----------------------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   呼び出される関数 純粋仮想関数
   * @date    2015.11.06
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void EndFunc( GameSys::GameManager * gmgr );


  void SetFieldControlFlag( bool flag );


private:
  ::Debug::ItemMakeProc * m_pProc;
  bool m_fieldControlFlag;
};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif  // EVENT_DEBUG_ITEMMAKE_H_INCLUDED

#endif  // PM_DEBUG
