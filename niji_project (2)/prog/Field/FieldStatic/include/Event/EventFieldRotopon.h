//=============================================================================
/**
 * @file    EventFieldRotopon.h
 * @brief   ロトポン用イベント
 * @author  Hiroyuki Nakamura
 * @date    2016.12.08
 */
//=============================================================================
#if !defined( EVENT_FIELD_ROTOPON_H_INCLUDED )
#define EVENT_FIELD_ROTOPON_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "heap/include/gfl2_Heap.h"
// niji
#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameManager.h"
//#include "App/Bag/include/BagAppParam.h"
//#include "App/WazaOshie/include/WazaOshieAppParam.h"
//#include "NetStatic/NetEvent/include/EvolutionEvent.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)
class FieldMenuSubWindowProc;
GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(Field)

// 前方宣言
class EventXMenuListener;

//=============================================================================
/**
 * @class EventFieldRotopon
 * @brief ロトポン用イベントクラス
 * @date  2016.12.08
 */
//=============================================================================
class EventFieldRotopon
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventFieldRotopon);  // コピーコンストラクタ＋代入禁止

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  EventFieldRotopon( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~EventFieldRotopon();

  //-----------------------------------------------------------------------------
  /**
   * @func    StartEvent
   * @brief   イベント起動
   * @date    2016.12.08
   *
   * @param   gmgr  ゲームマネージャクラス
   *
   * @return  EventFieldRotoponクラス
   */
  //-----------------------------------------------------------------------------
  static EventFieldRotopon * StartEvent( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetupParam
   * @brief   パラメータ設定
   * @date    2016.12.08
   *
   * @param   proc      Proc
   * @param   listener  リスナー
   * @param   item      アイテム番号
   */
  //-----------------------------------------------------------------------------
  void SetupParam( App::FieldMenu::FieldMenuSubWindowProc * proc, EventXMenuListener * listener, u32 item );

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @date    2016.12.08
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
   * @date    2016.12.08
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void InitFunc( GameSys::GameManager * gmgr);

  //-----------------------------------------------------------------------------
  /**
   * @func    MainFunc
   * @brief   呼び出される関数 純粋仮想関数
   * @date    2016.12.08
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
   * @date    2016.12.08
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void EndFunc( GameSys::GameManager * gmgr );


private:
  App::FieldMenu::FieldMenuSubWindowProc * m_fieldMenuProc;   //!< フィールドメニューのProc
  EventXMenuListener * m_eventXMenuListener;                  //!< リスナー
  u32 m_itemNo;                                               //!< アイテム番号
  u32 m_mainSeq;                                              //!< メインシーケンス
};


GFL_NAMESPACE_END(Field)

#endif  // EVENT_FIELD_ROTOPON_H_INCLUDED
