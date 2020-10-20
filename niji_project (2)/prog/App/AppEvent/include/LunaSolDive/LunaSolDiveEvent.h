//======================================================================
/**
 * @file LunaSolDiveEvent.h
 * @date 2016/11/22 19:22:40
 * @author saito_nozomu
 * @brief ルナソルダイブアプリ呼び出し
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if !defined( __APP_EVENT_LUNASOL_DIVE_H_INCLUDED__ )
#define __APP_EVENT_LUNASOL_DIVE_H_INCLUDED__
#pragma once
/// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "heap/include/gfl2_Heap.h"

#include <pml/include/pml_PokePara.h>

#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameManager.h>

#include "App/AppEvent/include/LunaSolDive/LunaSolDiveParam.h"
#include "App/LunaSolDive/include/DistortionType.h "

//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
/*
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(LunaSolDive)
class Proc;
GFL_NAMESPACE_END(LunaSolDive)
GFL_NAMESPACE_END(App)
*/

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//=============================================================================
/**
* @class LunaSolDiveEvent
* @brief ルナソルダイブイベントクラス
*/
//=============================================================================
class LunaSolDiveEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(LunaSolDiveEvent);   // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
  * @brief   コンストラクタ
  *
  * @param   heap  ヒープ
  */
  //-----------------------------------------------------------------------------
  LunaSolDiveEvent(gfl2::heap::HeapBase * heap);

  //-----------------------------------------------------------------------------
  /**
  * @brief   デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~LunaSolDiveEvent();

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
  static LunaSolDiveEvent * StartEvent(GameSys::GameManager * gmgr);

  //-----------------------------------------------------------------------------
  /**
  * @func    SetupAppParam
  * @brief   パラメータ設定
  * @date    2015.12.09
  *
  * @param   prm   外部設定パラメータ
  */
  //-----------------------------------------------------------------------------
  void SetupAppParam(LUNASOL_DIVE_PARAM * prm);

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
  static const u32 APP_MONS_NUM = 9;
  struct AppPokeData
  {
    u32 sys_flag;
    MonsNo monsno;
    MonsNo need_monsno[2];
    bool first_ng;
  };

  //-----------------------------------------------------------------------------
  /**
  * @brief   ホームゾーンへのセット
  */
  //-----------------------------------------------------------------------------
  void SetHomeZone(void);

  //-----------------------------------------------------------------------------
  /**
  * @brief   UBゾーンへのセット
  */
  //-----------------------------------------------------------------------------
  void SetUBZone(void);

  //-----------------------------------------------------------------------------
  /**
  * @brief   伝説、準伝ポケモンの抽選
  */
  //-----------------------------------------------------------------------------
  u32 DrawLotsMonster(GameSys::GameManager * gmgr);

  u32 DrawLotsLegendMonsNo(GameSys::GameManager * gmgr);

private:
  LUNASOL_DIVE_PARAM * m_pParam;
  u32 m_BgmFadeCounter;
};

GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif // __APP_EVENT_FIELD_LUNASOL_DIVE_H_INCLUDED__