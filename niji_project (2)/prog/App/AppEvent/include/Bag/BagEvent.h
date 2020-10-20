//=============================================================================
/**
 * @file    BagEvent.h
 * @brief   バッグ画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2015.03.04
 */
//=============================================================================
#if !defined( BAG_EVENT_H_INCLUDED )
#define BAG_EVENT_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "heap/include/gfl2_Heap.h"
// niji
#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameManager.h"
#include "App/Bag/include/BagAppParam.h"
#include "App/WazaOshie/include/WazaOshieAppParam.h"
#include "NetStatic/NetEvent/include/EvolutionEvent.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//=============================================================================
/**
 * @class BagEvent
 * @brief バッグ画面イベントクラス
 * @date  2015.03.04
 */
//=============================================================================
class BagEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(BagEvent);   // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  BagEvent( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~BagEvent();

  //-----------------------------------------------------------------------------
  /**
   * @func    StartEvent
   * @brief   イベント起動
   * @date    2015.06.11
   *
   * @param   gmgr  ゲームマネージャークラス
   *
   * @return  BagEventクラス
   */
  //-----------------------------------------------------------------------------
  static BagEvent * StartEvent( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetupAppParam
   * @brief   パラメータ設定
   * @date    2015.12.09
   *
   * @param   prm   バッグ画面の外部設定パラメータ
   */
  //-----------------------------------------------------------------------------
  void SetupAppParam( app::bag::APP_PARAM * prm );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetupFieldParam
   * @brief   フィールド関連のパラメータ設定
   * @date    2015.12.09
   *
   * @param   work      アイテム使用ワーク
   * @param   weather   天候
   *
   * @note  APP_PARAMを外部指定する場合、SetupAppParam()の後に呼ぶこと！
   */
  //-----------------------------------------------------------------------------
  void SetupFieldParam( Field::FieldItemUse::ITEM_USE_CHECK_WORK * work, Field::weather::WeatherKind weather );

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
  virtual bool BootChk( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitFunc
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @date    2015.06.11
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void InitFunc( GameSys::GameManager * gmgr);

  //-----------------------------------------------------------------------------
  /**
   * @func    MainFunc
   * @brief   呼び出される関数 純粋仮想関数
   * @date    2015.06.11
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
   * @date    2015.06.11
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void EndFunc( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetWazaOshieParam
   * @brief   技教え画面パラメータ設定
   * @date    2015.11.25
   *
   * @param   bag_param   バッグの外部設定パラメータ
   * @param   wo_param    技教えのパラメータ設定場所
   */
  //-----------------------------------------------------------------------------
  static void SetWazaOhsieParam( app::bag::APP_PARAM * bag_param, App::WazaOshie::APP_PARAM * wo_param );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetEvolutionDemoParam
   * @brief   進化デモ用パラメータ設定
   * @date    2015.11.25
   *
   * @param   gd          ゲームデータ
   * @param   bag_param   バッグの外部設定パラメータ
   * @param   evo_param   進化デモのパラメータ設定場所
   */
  //-----------------------------------------------------------------------------
  static void SetEvolutionDemoParam( GameSys::GameData * gd, app::bag::APP_PARAM * bag_param, NetEvent::Evolution::EvolutionEventParam * evo_param );


private:
  app::bag::APP_PARAM m_defAppParam;    //!< バッグ画面の外部設定パラメータ（デフォルト）
  app::bag::APP_PARAM * m_appParam;     //!< バッグ画面の外部設定パラメータ（実際にアクセスするポインタ）
  App::PokeList::IntermediateData m_i;

  App::WazaOshie::APP_PARAM m_wazaOshieParam;   //!< 技教え画面外部設定ワーク

  NetEvent::Evolution::EvolutionEventParam m_evolutionParam;  //!< 進化デモ用ワーク



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

#endif  // BAG_EVENT_H_INCLUDED
