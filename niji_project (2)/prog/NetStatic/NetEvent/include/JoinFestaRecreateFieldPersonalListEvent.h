// ============================================================================
/*
 * @file JoinFestaRecreateFieldPersonalListEvent.h
 * @brief ジョインフェスタのフィールド出現用パーソナルデータリストを再作成するイベント
 * @date 2015.12.14
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTA_RECREATE_PERSONALLIST_EVENT_H_INCLUDE )
#define JOINFESTA_RECREATE_PERSONALLIST_EVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"


GFL_NAMESPACE_BEGIN(NetEvent)


class JoinFestaRecreateFieldPersonalListEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaRecreateFieldPersonalListEvent );
public:

  JoinFestaRecreateFieldPersonalListEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaRecreateFieldPersonalListEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

};


GFL_NAMESPACE_END(NetEvent)

#endif // JOINFESTA_RECREATE_PERSONALLIST_EVENT_H_INCLUDE
