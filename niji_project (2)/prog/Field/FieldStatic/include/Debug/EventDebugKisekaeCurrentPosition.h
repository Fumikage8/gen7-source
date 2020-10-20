// ============================================================================
/*
 * @file   EventDebugKisekaeCurrentPosition.cpp
 * @brief  きせかえのデバッグイベント(暗転なし)
 * @date   2016.05.20
 * @author yuya_ikeuchi
 */
// ============================================================================
#if PM_DEBUG

#if !defined( EVENT_KISEKASE_CURRENT_POSITION_DEBUG_H_INCLUDE )
#define EVENT_KISEKASE_CURRENT_POSITION_DEBUG_H_INCLUDE

//  イベント起動に必要な物
#include <System/include/DressUp.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"

namespace Field{ namespace Debug {

class EventDebugKisekaeCurrentPosition : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( EventDebugKisekaeCurrentPosition );

public:

  static void StartEvent( GameSys::GameManager* pGameManager,u32 continueCnt );

  EventDebugKisekaeCurrentPosition();
  EventDebugKisekaeCurrentPosition( gfl2::heap::HeapBase* pHeap );
  virtual~EventDebugKisekaeCurrentPosition();
  virtual bool                    BootChk ( GameSys::GameManager* pGameManager );
  virtual void                    InitFunc( GameSys::GameManager* pGameManager );
  virtual void                    EndFunc ( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );
  void                            SetKisekaeContinueCnt(u32 continueCnt){ m_continueCnt = continueCnt; }

private:

  u32   m_continueCnt;
  u32   m_loopCnt;
  poke_3d::model::DressUpParam m_dressUpParam;
};

}}

#endif // EVENT_KISEKASE_CURRENT_POSITION_DEBUG_H_INCLUDE
#endif // PM_DEBUG