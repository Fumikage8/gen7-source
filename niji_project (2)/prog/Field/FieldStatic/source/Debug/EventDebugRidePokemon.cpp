// ============================================================================
/*
 * @file   EventDebugRidePokemon.cpp
 * @brief  ライドのデバッグイベント
 * @date   2016.05.20
 * @author yuya_ikeuchi
 */
// ============================================================================
#if PM_DEBUG

#include "Field/FieldStatic/include/Debug/EventDebugRidePokemon.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"
#include "Field/FieldRo/include/Event/FieldEventPokemonRide.h"

namespace Field{ namespace Debug {

void EventDebugRidePokemon::StartEvent( GameSys::GameManager* pGameManager,u32 continueCnt )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  EventDebugRidePokemon* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventDebugRidePokemon>( pGameEventManager );
  pEvent->SetKisekaeContinueCnt(continueCnt);
}

EventDebugRidePokemon::EventDebugRidePokemon()
: GameSys::GameEvent( NULL )
, m_continueCnt     ( 0 )
, m_loopCnt         ( 0 )
{
}

EventDebugRidePokemon::EventDebugRidePokemon( gfl2::heap::HeapBase* pHeap )
: GameSys::GameEvent( pHeap )
, m_continueCnt     ( 0 )
, m_loopCnt         ( 0 )
{
}

EventDebugRidePokemon::~EventDebugRidePokemon()
{
}

bool EventDebugRidePokemon::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}

void EventDebugRidePokemon::InitFunc( GameSys::GameManager* pGameManager )
{
}

void EventDebugRidePokemon::EndFunc( GameSys::GameManager* pGameManager )
{
}

GameSys::GMEVENT_RESULT EventDebugRidePokemon::MainFunc( GameSys::GameManager* pGameManager )
{
  enum
  {
    SEQ_RIDE_ON,
    SEQ_END,
  };

  switch( this->GetSeqNo() )
  {
    case SEQ_RIDE_ON:
    {
      u32 ridePokemonIndex;
      u32 attr = pGameManager->GetFieldmap()->GetPlayerCharacter()->GetGroundAttribute();

      if( Attribute::IsWater( attr ) )
      {
        u32 randIndex = System::GflUse::GetPublicRand(2);
        if     ( randIndex == 0 ){ ridePokemonIndex = RIDE_SAMEHADA;   }
        else if( randIndex == 1 ){ ridePokemonIndex = RIDE_LAPLACE;    }
        else                     { return GameSys::GMEVENT_RES_FINISH; }
      }
      else if( !Attribute::IsRoba2Ground( attr ) && !pGameManager->GetFieldmap()->GetPlayerCharacter()->IsHitRoba2WallGroundRide() )
      {
        u32 randIndex = System::GflUse::GetPublicRand(4);
        if     ( randIndex == 0 ){ ridePokemonIndex = RIDE_KENTAROSU;  }
        else if( randIndex == 1 ){ ridePokemonIndex = RIDE_MUURANDO;   }
        else if( randIndex == 2 ){ ridePokemonIndex = RIDE_KAIRIKY;    }
        else if( randIndex == 3 ){ ridePokemonIndex = RIDE_ROBA2;      }
        else                     { return GameSys::GMEVENT_RES_FINISH; }
      }
      else
      {
        return GameSys::GMEVENT_RES_FINISH;
      }

      RIDE_CHECK_RESULT result = EventPokemonRideOn::IsPokemonRideOnEnable( pGameManager, static_cast<Field::RIDE_POKEMON_ID>(ridePokemonIndex) );
      if( result != RCR_OK )
      {
        return GameSys::GMEVENT_RES_FINISH;
      }

      Field::EventPokemonRideOn* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventPokemonRideOn>( pGameManager->GetGameEventManager() );
      pEvent->SetPokemonRideOnID( pGameManager, static_cast<Field::RIDE_POKEMON_ID>(ridePokemonIndex) );
      this->AddSeqNo();
      break;
    }
    case SEQ_END:
    {
      if( m_loopCnt < m_continueCnt )
      {
        m_loopCnt++;
        SetSeqNo(SEQ_RIDE_ON);
      }
      else
      {
        return GameSys::GMEVENT_RES_FINISH;
      }
    }
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

}}

#endif // PM_DEBUG