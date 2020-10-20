// ============================================================================
/*
 * @file SuperTrainingEvent.h
 * @brief すごい特訓アプリのイベント
 * @date 2015.12.04
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( SUPER_TRAINIGEVENT_H_INCLUDE )
#define SUPER_TRAINIGEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"
#include "App/SuperTraining/include/SuperTrainingAppParam.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)

  class SuperTrainingProc;

GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)


  class SuperTrainingEvent : public GameSys::GameEvent
  {
    GFL_FORBID_COPY_AND_ASSIGN( SuperTrainingEvent );
  public:

    static void StartEvent( GameSys::GameManager* pGameManager, App::SuperTraining::APP_PARAM* pAppParam );

    SuperTrainingEvent( gfl2::heap::HeapBase* pHeap );
    virtual~SuperTrainingEvent();

    virtual bool BootChk( GameSys::GameManager* pGameManager );
    virtual void InitFunc( GameSys::GameManager* pGameManager );
    virtual void EndFunc( GameSys::GameManager* pGameManager );
    virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  private:

    App::SuperTraining::APP_PARAM*                        m_pAppParam;
    App::SuperTraining::SuperTrainingProc*                m_pProc;
  };


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif // SUPER_TRAINIGEVENT_H_INCLUDE
