//=============================================================================
/**
 * @file    TownMapEvent.h
 * @brief   タウンマップ画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2015.10.14
 */
//=============================================================================
#if !defined( TOWNMAP_EVENT_H_INCLUDED )
#define TOWNMAP_EVENT_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "heap/include/gfl2_Heap.h"
// niji
#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameManager.h"
#include "App/TownMap/include/TownMapAppParam.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

class TownMapEventListener;

//=============================================================================
/**
 * @class TownMapEvent
 * @brief タウンマップ画面イベントクラス
 * @date  2015.10.14
 */
//=============================================================================
class TownMapEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(TownMapEvent);   // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  TownMapEvent( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~TownMapEvent();

  //-----------------------------------------------------------------------------
  /**
   * @func    StartEvent
   * @brief   イベント起動
   * @date    2015.10.14
   *
   * @param   man     ゲームマネージャクラス
   *
   * @return  ShopSellEventクラス
   */
  //-----------------------------------------------------------------------------
  static TownMapEvent * StartEvent( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetParam
   * @brief   パラメータ設定
   * @date    2015.10.14
   *
   * @param   mode  呼び出しモード
   */
  //-----------------------------------------------------------------------------
  void SetParam( App::TownMap::CallMode mode );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetParam
   * @brief   パラメータ取得
   * @date    2015.10.14
   *
   * @return  パラメータ
   */
  //-----------------------------------------------------------------------------
  const App::TownMap::APP_PARAM * GetParam(void) const;

  //--------------------------------------------------------------
  /**
   * @brief   終了を得るリスナーをセット
   * @param   pTownMapEventListener       TownMapEventListener
   */
  //--------------------------------------------------------------
  void SetTownMapEventListener(TownMapEventListener* pTownMapEventListener);

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @date    2015.10.14
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
   * @date    2015.10.14
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void InitFunc( GameSys::GameManager * gmgr);

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   呼び出される関数 純粋仮想関数
   * @date    2015.10.14
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
   * @date    2015.10.14
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void EndFunc( GameSys::GameManager * gmgr );


private:
  App::TownMap::APP_PARAM m_appParam;   //!< 外部設定パラメータ
  TownMapEventListener* m_eventListener;

//=============================================================================
//  デバッグ用
//=============================================================================
#if PM_DEBUG
public:
  static void SetupLauncherCall( bool flg );
private:
  bool IsLauncherCall(void);
#endif  // PM_DEBUG

};


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif  // TOWNMAP_EVENT_H_INCLUDED
