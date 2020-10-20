// ============================================================================
/*
 * @file   EventDebugKisekae.cpp
 * @brief  きせかえのデバッグイベント
 * @date   2016.05.20
 * @author yuya_ikeuchi
 */
// ============================================================================
#if PM_DEBUG

#if !defined( EVENT_KISEKASE_DEBUG_H_INCLUDE )
#define EVENT_KISEKASE_DEBUG_H_INCLUDE

//  イベント起動に必要な物
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"

namespace Field{ namespace Debug {

class EventDebugKisekae : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( EventDebugKisekae );

public:

  static void StartEvent( GameSys::GameManager* pGameManager,u32 continueCnt );

  EventDebugKisekae( gfl2::heap::HeapBase* pHeap );
  virtual~EventDebugKisekae();
  virtual bool                    BootChk ( GameSys::GameManager* pGameManager );
  virtual void                    InitFunc( GameSys::GameManager* pGameManager );
  virtual void                    EndFunc ( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );
  void                            SetKisekaeContinueCnt(u32 continueCnt){ m_continueCnt = continueCnt; }

private:

  u32   m_continueCnt;
  u32   m_loopCnt;
};

}}

#endif // EVENT_KISEKASE_DEBUG_H_INCLUDE
#endif // PM_DEBUG