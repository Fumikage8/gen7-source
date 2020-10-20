//=============================================================================
/**
 * @file    ShopSellEvent.h
 * @brief   ショップ売却画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2015.10.02
 */
//=============================================================================
#if !defined( SHOP_SELL_EVENT_H_INCLUDED )
#define SHOP_SELL_EVENT_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "heap/include/gfl2_Heap.h"
// niji
#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameManager.h"
#include "App/Bag/include/BagAppParam.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//=============================================================================
/**
 * @class ShopSellEvent
 * @brief ショップ売却画面イベントクラス
 * @date  2015.10.02
 */
//=============================================================================
class ShopSellEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(ShopSellEvent);   // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  ShopSellEvent( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~ShopSellEvent();

  //-----------------------------------------------------------------------------
  /**
   * @func    StartEvent
   * @brief   イベント起動
   * @date    2015.10.02
   *
   * @param   man     ゲームマネージャクラス
   *
   * @return  ShopSellEventクラス
   */
  //-----------------------------------------------------------------------------
  static ShopSellEvent * StartEvent( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @date    2015.10.02
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
   * @date    2015.10.02
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void InitFunc( GameSys::GameManager * gmgr);

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   呼び出される関数 純粋仮想関数
   * @date    2015.10.02
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
   * @date    2015.10.02
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void EndFunc( GameSys::GameManager * gmgr );


private:
  app::bag::APP_PARAM m_appParam;   //!< バッグ画面の外部設定パラメータ

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

#endif  // SHOP_SELL_EVENT_H_INCLUDED
