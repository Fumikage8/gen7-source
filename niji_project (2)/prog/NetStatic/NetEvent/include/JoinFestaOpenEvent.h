//=================================================================================================
/**
 * @file  JoinFestaOpenEvent.h
 * @brief フィールドからジョインフェスタに遷移するイベント
 * @date 2015.09.11
 * @author ichiraku_katsuhiko
 */
//=================================================================================================
#ifndef __JOIN_FESTA_OPEN_EVENT_H__
#define __JOIN_FESTA_OPEN_EVENT_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"


#include "Field/FieldStatic/include/FieldLocation.h"
#include "Field/FieldStatic/include/Event/EventMapJump.h"

GFL_NAMESPACE_BEGIN(NetEvent)


/* -------------------------------------------------------------------------*/
/**
 * @brief フィールドからジョインフェスタに遷移するイベント
 */
/* -------------------------------------------------------------------------*/
class JoinFestaOpenEvent : public Field::EventMapJumpCore
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaOpenEvent ); // コピーを禁止

public:

  //--------------------------------------------------------------
  /**
   * @brief   フィールドからジョインフェスタに遷移するイベント呼び出し
   * 
   * @param   pGameManager  ゲームマネージャ
   * @param   isCall
   * @param   isRoom        城内かどうか(false:城外 true:城内）
   * @param   pPosition     遷移後の主人公位置(NULLでデフォルト位置）
   * @param   isNoFade      フェードの有無(デフォルトあり）
   */
  //--------------------------------------------------------------
  static GameSys::GameEvent* StartEvent( GameSys::GameManager* pGameManager, b32 isCall,
                                         const bool isRoom=false, gfl2::math::Vector3* pPosition=NULL,
                                         const bool isNoFade=false );

  //! コンストラクタ
  JoinFestaOpenEvent( gfl2::heap::HeapBase* pHeap );
  //! デストラクタ
  virtual ~JoinFestaOpenEvent();

private:

  virtual void OnInitialize( GameSys::GameManager* gmgr );

  virtual void OnFieldClose( GameSys::GameManager* gmgr );

};


GFL_NAMESPACE_END(NetEvent)


#endif // __JOIN_FESTA_OPEN_EVENT_H__
