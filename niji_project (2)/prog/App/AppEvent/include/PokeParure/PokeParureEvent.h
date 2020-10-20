//=============================================================================
/**
 * @file    PokeParureEvent.h
 * @brief   ポケパルレ画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2015.10.21
 */
//=============================================================================
#if !defined( POKEPARURE_EVENT_H_INCLUDED )
#define POKEPARURE_EVENT_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "heap/include/gfl2_Heap.h"
// niji
#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameManager.h"
#include "App/Kawaigari/include/KawaigariProc.h"
#include "App/ParurePokeList/include/ParurePokeListAppParam.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//=============================================================================
/**
 * @class PokeParureEvent
 * @brief ポケパルレ画面イベントクラス
 * @date  2015.10.21
 */
//=============================================================================
class PokeParureEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeParureEvent);  // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  PokeParureEvent( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~PokeParureEvent();

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
  static PokeParureEvent * StartEvent( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetupParam
   * @brief   パラメータ設定
   * @date    2015.12.08
   *
   * @param   prm     メイン画面の外部設定パラメータ
   * @param   index   ポケモンの並び位置
   *
   * @note  入れ替えがない場合はindex = pml::PokeParty::MAX_MEMBERS
   */
  //-----------------------------------------------------------------------------
  void SetupParam( const App::Kawaigari::KawaigariProcParam * prm, u32 index );

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

  //-----------------------------------------------------------------------------
  /**
   * @func    IsNoHungryPokemon
   * @brief   満腹度を無視するポケモンかをチェック
   * @date    2016.05.23
   *
   * @param   mons	ポケモン番号
   *
   * @retval  true  = 満腹度を無視する
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  static bool IsNoHungryPokemon( MonsNo mons );


private:
  App::Kawaigari::KawaigariProcParam m_kawaigariProcParam;  //!< かわいがりメイン画面のパラメータ
  App::PokeParure::APP_PARAM m_pokeListParam;   //!< ポケモン選択画面のパラメータ
  u32 m_pokemonIndex;   //!< かわいがり対象のインデックス


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

#endif  // POKEPARURE_EVENT_H_INCLUDED
