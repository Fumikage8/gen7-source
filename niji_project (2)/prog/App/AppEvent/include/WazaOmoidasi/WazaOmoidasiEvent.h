//=============================================================================
/**
 * @file    WazaOmoidasiEvent.h
 * @brief   技思い出し画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2015.12.22
 */
//=============================================================================
#if !defined( WAZA_OMOIDASI_EVENT_H_INCLUDED )
#define WAZA_OMOIDASI_EVENT_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "heap/include/gfl2_Heap.h"
// niji
#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameManager.h"
#include "App/WazaOmoidasi/include/WazaOmoidasiAppParam.h"
#include "App/WazaOshie/include/WazaOshieAppParam.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//=============================================================================
/**
 * @class WazaOmoidasiEvent
 * @brief 技思い出し画面イベントクラス
 * @date  2015.12.22
 */
//=============================================================================
class WazaOmoidasiEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(WazaOmoidasiEvent);   // コピーコンストラクタ＋代入禁止

public:
  //! イベントの終了パラメータ
  struct RESULT_PARAM
  {
    WazaNo set_waza;  //!< 思い出した技（キャンセル時はWAZANO_NULL）
  };


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  WazaOmoidasiEvent( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~WazaOmoidasiEvent();

  //-----------------------------------------------------------------------------
  /**
   * @func    StartEvent
   * @brief   イベント起動
   * @date    2015.12.22
   *
   * @param   man     ゲームマネージャクラス
   *
   * @return  WazaOmoidasiEventクラス
   */
  //-----------------------------------------------------------------------------
  static WazaOmoidasiEvent * StartEvent( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetParam
   * @brief   パラメータ設定
   * @date    2015.12.22
   *
   * @param   party_index 手持ちポケモンの並び位置
   * @param   result      イベント終了パラメータ
   */
  //-----------------------------------------------------------------------------
  void SetParam( u32 party_index, RESULT_PARAM * result );

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @date    2015.12.22
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
   * @date    2015.12.22
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void InitFunc( GameSys::GameManager * gmgr);

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   呼び出される関数 純粋仮想関数
   * @date    2015.12.22
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
   * @date    2015.12.22
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void EndFunc( GameSys::GameManager * gmgr );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    SetWazaOshieParam
   * @brief   技教え画面パラメータ設定
   * @date    2015.12.22
   */
  //-----------------------------------------------------------------------------
  void SetWazaOhsieParam(void);


private:
  RESULT_PARAM * m_resultParam;   //!< イベントの終了パラメータ

  App::WazaOmoidasi::APP_PARAM m_appParam;  //!< 技思い出し画面パラメータ

  App::WazaOshie::APP_PARAM m_wazaOshieParam;   //!< 技教え画面パラメータ

  u32 m_partyIndex;   //!< 対象のインデックス


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

#endif  // WAZA_OMOIDASI_EVENT_H_INCLUDED
