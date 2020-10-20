//=============================================================================
/**
 * @file    TrainerPassEvent.h
 * @brief   トレーナーパス画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2015.11.06
 */
//=============================================================================
#if !defined( TRAINER_PASS_EVENT_H_INCLUDED )
#define TRAINER_PASS_EVENT_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "heap/include/gfl2_Heap.h"
// niji
#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameManager.h"
#include "App/TrainerPass/include/TrainerPassAppParam.h"
#include "App/TrainerPhoto/include/TrainerPhotoProc.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//=============================================================================
/**
 * @class TrainerPassEvent
 * @brief トレーナーパス画面イベントクラス
 * @date  2015.11.06
 */
//=============================================================================
class TrainerPassEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(TrainerPassEvent);   // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  TrainerPassEvent( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~TrainerPassEvent();

  //-----------------------------------------------------------------------------
  /**
   * @func    StartEvent
   * @brief   イベント起動
   * @date    2015.11.06
   *
   * @param   man     ゲームマネージャクラス
   *
   * @return  TrainerPassEventクラス
   */
  //-----------------------------------------------------------------------------
  static TrainerPassEvent * StartEvent( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetStampMode
   * @brief   スタンプ用の設定
   * @date    2015.11.06
   *
   * @param   stamp   スタンプ番号
   */
  //-----------------------------------------------------------------------------
  void SetStampMode( u32 stamp );

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


private:
  App::TrainerPass::APP_PARAM m_appParam;   //!< トレーナーパス画面の外部設定パラメータ
  app::photo::APP_PARAM m_photoAppParam;    //!< 証明写真画面の外部設定パラメータ



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

#endif  // TRAINER_PASS_EVENT_H_INCLUDED
