//======================================================================
/**
 * @file MantainSurfEvent.h
 * @date 2016/11/22 19:22:10
 * @author saito_nozomu
 * @brief マンタインサーフアプリ呼び出し
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if !defined( __APP_EVENT_MANTAIN_SURF_H_INCLUDED__ )
#define __APP_EVENT_MANTAIN_SURF_H_INCLUDED__
#pragma once
// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "heap/include/gfl2_Heap.h"

#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameManager.h>

#include "App/AppEvent/include/MantainSurf/MantainSurfParam.h"

//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
/*
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
class Proc;
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
*/

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//=============================================================================
/**
* @class MantainSurfEvent
* @brief マンタインサーフイベントクラス
*/
//=============================================================================
class MantainSurfEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(MantainSurfEvent);   // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  *
  * @param   heap  ヒープ
  */
  //-----------------------------------------------------------------------------
  MantainSurfEvent(gfl2::heap::HeapBase * heap);

  //-----------------------------------------------------------------------------
  /**
  * @brief   デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~MantainSurfEvent();

  //-----------------------------------------------------------------------------
  /**
  * @func    StartEvent
  * @brief   イベント起動
  * @date    2015.06.11
  *
  * @param   man     ゲームマネージャクラス
  *
  * @return  MantainSurfEventクラス
  */
  //-----------------------------------------------------------------------------
  static MantainSurfEvent * StartEvent(GameSys::GameManager * gmgr);

  //-----------------------------------------------------------------------------
  /**
  * @func    SetupAppParam
  * @brief   パラメータ設定
  * @date    2015.12.09
  *
  * @param   prm   外部設定パラメータ
  */
  //-----------------------------------------------------------------------------
  void SetupAppParam(MANTAIN_SURF_PARAM * prm);

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
  MANTAIN_SURF_PARAM * m_pParam;
  u32 m_BgmFadeCounter;
};

GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif // __APP_EVENT_FIELD_MANTAIN_SURF_H_INCLUDED__