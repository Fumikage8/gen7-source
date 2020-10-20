//=============================================================================
/**
 * @file    RideDemoEvent.h
 * @brief   ライド選択画面の説明デモイベント
 * @author  Hiroyuki Nakamura
 * @date    2016.03.30
 */
//=============================================================================
#if !defined( RIDEDEMO_EVENT_H_INCLUDED )
#define RIDEDEMO_EVENT_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
// niji
#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameManager.h"
#include "Savedata/include/FieldMenuSave.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//=============================================================================
/**
 * @class RideDemoEvent
 * @brief ライド説明デモイベント
 * @date  2016.03.30
 */
//=============================================================================
class RideDemoEvent
  : public GameSys::GameEvent
{
public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  RideDemoEvent( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~RideDemoEvent();

  //-----------------------------------------------------------------------------
  /**
   * @func    StartEvent
   * @brief   イベント起動
   * @date    2016.03.31
   *
   * @param   man     ゲームマネージャクラス
   *
   * @return  RideDemoEventクラス
   */
  //-----------------------------------------------------------------------------
  static RideDemoEvent * StartEvent( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetupParam
   * @brief   パラメータ設定
   * @date    2016.03.31
   *
   * @param   id  ライドID
   */
  //-----------------------------------------------------------------------------
  void SetupParam( Savedata::FieldMenu::RideID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @date    2016.03.31
   *
   * @param   gmgr  ゲームマネージャークラス
   *
   * @retval  true  = 起動してよい
   * @retval  false = 起動しない
   */
  //-----------------------------------------------------------------------------
  virtual bool BootChk( GameSys::GameManager * /*gmgr*/ );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFunc
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @date    2016.03.31
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void InitFunc( GameSys::GameManager * /*gmgr*/ );

  //-----------------------------------------------------------------------------
  /**
   * @func    MainFunc
   * @brief   呼び出される関数 純粋仮想関数
   * @date    2016.03.31
   *
   * @param   gmgr  ゲームマネージャークラス
   *
   * @return  イベント制御関数の戻り値
   */
  //-----------------------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    EndFunc
   * @brief   呼び出される関数 純粋仮想関数
   * @date    2016.03.31
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void EndFunc( GameSys::GameManager * gmgr );


private:
  Savedata::FieldMenu::RideID m_rideID;   //!< ライドID
};


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif  // RIDEDEMO_EVENT_H_INCLUDED
