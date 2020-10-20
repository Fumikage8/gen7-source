// ============================================================================
/*
 * @file JoinFestaAttractionRetireEvent.h
 * @brief アトラクションリタイアのイベント
 * @date 2015.12.02
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONRETIREEVENT_H_INCLUDE )
#define JOINFESTAATTRACTIONRETIREEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"


GFL_NAMESPACE_BEGIN(NetEvent)


class JoinFestaAttractionRetireEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionRetireEvent );
public:

  JoinFestaAttractionRetireEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaAttractionRetireEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

};


GFL_NAMESPACE_END(NetEvent)

#endif // JOINFESTAATTRACTIONRETIREEVENT_H_INCLUDE
