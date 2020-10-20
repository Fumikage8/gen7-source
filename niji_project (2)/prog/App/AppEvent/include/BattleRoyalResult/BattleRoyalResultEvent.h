// ============================================================================
/*
 * @file BattleRoyalResultEvent.h
 * @brief バトルロイヤルの結果アプリのイベント
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( BATTLE_ROYAL_RESULTEVENT_H_INCLUDE )
#define BATTLE_ROYAL_RESULTEVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "NetStatic/NetEvent/include/NetEventBase.h" // for NetEventBase

#include "App/BattleRoyalResult/include/BattleRoyalResultAppParam.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)

  class BattleRoyalResultProc;

GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)


  class BattleRoyalResultEvent : public NetEvent::NetEventBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleRoyalResultEvent );
  public:

    static void StartEvent( GameSys::GameManager* pGameManager, App::BattleRoyalResult::APP_PARAM* pAppParam );

    BattleRoyalResultEvent( gfl2::heap::HeapBase* pHeap );
    virtual~BattleRoyalResultEvent();

    virtual bool BootChk( GameSys::GameManager* pGameManager );
    virtual void InitFunc( GameSys::GameManager* pGameManager );
    virtual void EndFunc( GameSys::GameManager* pGameManager );
    virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  private:
    void _Clear()
    {
      m_pAppParam = NULL;
      m_pProc     = NULL;
    }

    App::BattleRoyalResult::APP_PARAM*                       m_pAppParam;

    App::BattleRoyalResult::BattleRoyalResultProc*           m_pProc;
  };


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif // BATTLE_ROYAL_RESULTEVENT_H_INCLUDE
