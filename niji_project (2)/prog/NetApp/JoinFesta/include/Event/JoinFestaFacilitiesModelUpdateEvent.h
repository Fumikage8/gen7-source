// ============================================================================
/*
 * @file JoinFestaFacilitiesModelUpdateEvent.h
 * @brief ジョインフェスタの施設を最新情報で更新するイベント
 * @date 2016.02.06
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTA_FACILITIES_MODEL_UPDATE_EVENT_H_INCLUDE )
#define JOINFESTA_FACILITIES_MODEL_UPDATE_EVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)


class JoinFestaFacilitiesModelUpdateEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaFacilitiesModelUpdateEvent );
public:

  JoinFestaFacilitiesModelUpdateEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaFacilitiesModelUpdateEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

};


GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTA_FACILITIES_MODEL_UPDATE_EVENT_H_INCLUDE
