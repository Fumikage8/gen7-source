//=================================================================================================
/**
 * @file  JoinFestaCloseEvent.h
 * @brief ジョインフェスタからフィールドに遷移するイベント
 * @date 2015.09.11
 * @author ichiraku_katsuhiko
 */
//=================================================================================================
#ifndef __JOIN_FESTA_CLOSE_EVENT_H__
#define __JOIN_FESTA_CLOSE_EVENT_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"

// field
#include "Field/FieldStatic/include/FieldLocation.h"
#include "Field/FieldStatic/include/Event/EventMapJump.h"

GFL_NAMESPACE_BEGIN(NetEvent)

/* -------------------------------------------------------------------------*/
/**
 * @brief ジョインフェスタからフィールドに遷移するイベント
 */
/* -------------------------------------------------------------------------*/
class JoinFestaCloseEvent : public Field::EventMapJumpCore
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaCloseEvent ); // コピーを禁止

public:

  //--------------------------------------------------------------
  /**
   * @brief   ジョインフェスタからフィールドに遷移するイベント呼び出し
   * 
   * @param   pGameManager  ゲームマネージャ
   * @param   isNoFade      フェードの有無(デフォルトあり）
   */
  //--------------------------------------------------------------
  static void StartEvent( GameSys::GameManager* pGameManager, const bool isNoFade = false );

  //! コンストラクタ
  JoinFestaCloseEvent( gfl2::heap::HeapBase* pHeap );
  //! デストラクタ
  virtual ~JoinFestaCloseEvent();

  //--------------------------------------------------------------
  /**
   * @brief   フィールド破棄直後に呼び出される関数
   * @param   gmgr       ゲームマネージャークラス
   * @note    フィールド遷移時に、何からの処理を挟みたい場合等にオーバライドして下さい
   */
  //--------------------------------------------------------------
  virtual void OnFieldClose( GameSys::GameManager* gmgr );

};


GFL_NAMESPACE_END(NetEvent)


#endif // __JOIN_FESTA_CLOSE_EVENT_H__
