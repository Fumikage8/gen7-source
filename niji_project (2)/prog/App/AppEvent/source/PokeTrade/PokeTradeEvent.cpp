// ============================================================================
/*
 * @file PokeTradeEvent.cpp
 * @brief ポケモン交換アプリのイベントです
 * @date 2015.11.25
 * @author endo_akira
 */
// ============================================================================
#include "App/AppEvent/include/PokeTrade/PokeTradeEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"
#include "App/AppDemo/include/AppDemoProc.h"
#include "App/Box/include/BoxAppParam.h"
#include "NetStatic/NetAppLib/include/Util/NetAppCommonTradeUtility.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)


void PokeTradeEvent::StartEvent( GameSys::GameManager* pGameManager, App::Event::PokeTradeEventParam* pPokeTradeEventParam )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  PokeTradeEvent* pPokeTradeEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, PokeTradeEvent>( pGameEventManager );
  pPokeTradeEvent->setPokeTradeEventParam( pPokeTradeEventParam );
}


PokeTradeEvent::PokeTradeEvent( gfl2::heap::HeapBase* pHeap ) :
  GameSys::GameEvent( pHeap ),
  m_pDemoProc( NULL ),
  m_DemoSendPokemon( pHeap ),
  m_DemoRecvPokemon( pHeap ),
  m_Tray( 0 ),
  m_Pos( 0 ),
  m_bInBox( false ),
  m_bCallZukanRegister( false ),
  m_ZukanRegisterEventParam(),
  m_bCallEvolution( false ),
  m_EvolutionEventParam(),
  m_TradePersonStatus()
{
}


PokeTradeEvent::~PokeTradeEvent()
{
}


bool PokeTradeEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void PokeTradeEvent::InitFunc( GameSys::GameManager* pGameManager )
{
}


void PokeTradeEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT PokeTradeEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  switch( GetSeqNo() )
  {
    // ==========================================================================================
    case SEQUENCE_DEMO_START:
    {
      updateSaveData();
      bootDemoProc();
      SetSeqNo( SEQUENCE_DEMO_WAIT );
    }
    break;

    // ==========================================================================================
    case SEQUENCE_DEMO_WAIT:
    {
      if( pGameManager->GetProcManager()->IsRegisterProcess( m_pDemoProc ) == false )
      {
        if( m_bCallZukanRegister )
        {
          bootZukanRegisterEvent( pGameManager );
          SetSeqNo( SEQUENCE_ZUKAN_REGISTER );
        }
        else
        {
          if( m_bCallEvolution )
          {
            bootEvolutionEvent( pGameManager );
            SetSeqNo( SEQUENCE_EVOLUTION );
          }
          else
          {
            result = GameSys::GMEVENT_RES_FINISH;
          }
        }
      }
    }
    break;

    // ==========================================================================================
    case SEQUENCE_ZUKAN_REGISTER:
    {
      if( m_bCallEvolution )
      {
        bootEvolutionEvent( pGameManager );
        SetSeqNo( SEQUENCE_EVOLUTION );
      }
      else
      {
        result = GameSys::GMEVENT_RES_FINISH;
      }
    }
    break;

    // ==========================================================================================
    case SEQUENCE_EVOLUTION:
    {
      result = GameSys::GMEVENT_RES_FINISH;
    }
    break;
  }

  return result;
}


void PokeTradeEvent::setPokeTradeEventParam( App::Event::PokeTradeEventParam* pPokeTradeEventParam )
{
  m_DemoRecvPokemon.CopyFrom( *pPokeTradeEventParam->pRecvPokemon );
  m_Tray = pPokeTradeEventParam->tray;
  m_Pos  = pPokeTradeEventParam->pos;

  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  gfl2::str::StrBuf npcName( pPokeTradeEventParam->npcName, pHeap );

  m_TradePersonStatus.SetNameString( &npcName );
}


void PokeTradeEvent::updateSaveData()
{
  switch( m_Tray )
  {
    case App::Box::RETURN_TEMOTI:
    {
      pml::PokeParty* pPokeParty = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
      pml::pokepara::PokemonParam* pPokemonParam = pPokeParty->GetMemberPointer( m_Pos );
      u8 buffer[ pml::pokepara::CoreParam::DATASIZE ];
      pPokemonParam->Serialize_Core( buffer );
      m_DemoSendPokemon.Deserialize_Core( buffer );
      m_bInBox = false;
    }
    break;

    default:
    {
      Savedata::BoxPokemon* pBoxPokemon = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();
      pBoxPokemon->GetPokemon( &m_DemoSendPokemon, m_Tray, m_Pos );
      m_bInBox = true;
    }
    break;
  }

  gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  NetAppLib::Util::NetAppCommonTradeUtility::UpdateSaveData( pHeap, NetAppLib::Util::TRADE_ID_NPC, m_bInBox, m_Tray, m_Pos, &m_DemoRecvPokemon, &m_DemoSendPokemon, m_bCallZukanRegister, m_ZukanRegisterEventParam, m_bCallEvolution, m_EvolutionEventParam );

}


void PokeTradeEvent::bootDemoProc()
{
  App::AppDemo::AppDemoProcParam_MiracleTrade param;

  param.demoGARCID = GARC_demo_seq_script_sd9110_trade_all_BSEQ;

  param.pSendPokemonParam  = &m_DemoSendPokemon;
  param.pRecvPokemonParam  = &m_DemoRecvPokemon;
  param.pTradePersonStatus = &m_TradePersonStatus;
  param.pHeap              = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  m_pDemoProc = GameSys::CallAppDemoProc_MiracleTrade( &param );
}


void PokeTradeEvent::bootZukanRegisterEvent( GameSys::GameManager* pGameManager )
{
  m_ZukanRegisterEventParam.pp = &m_DemoRecvPokemon;

  app::event::ZukanRegisterEvent::StartEvent( pGameManager, &m_ZukanRegisterEventParam );
}


void PokeTradeEvent::bootEvolutionEvent( GameSys::GameManager* pGameManager )
{
  GameSys::GameData*      pGameData   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  pml::PokeParty*         pPokeParty  = pGameData->GetPlayerParty();
  Savedata::MyItem*       pMyItem     = pGameData->GetMyItem();

  m_EvolutionEventParam.m_pTargetPokemon = &m_DemoRecvPokemon;
  m_EvolutionEventParam.m_Reason         = NetApp::Evolution::REASON_Trade;
  m_EvolutionEventParam.m_pPokeParty     = pPokeParty;
  m_EvolutionEventParam.m_pMyItem        = pMyItem;

  NetEvent::Evolution::EvolutionEvent::StartEvent( pGameManager, m_EvolutionEventParam );
}


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)
