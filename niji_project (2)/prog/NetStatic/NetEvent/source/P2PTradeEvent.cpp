// ============================================================================
/*
 * @file P2PTradeEvent.cpp
 * @brief P2P交換アプリのイベント
 * @date 2015.07.24
 * @author Akira endo
 */
// ============================================================================
#include "NetStatic/NetEvent/include/P2PTradeEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"
#include "App/AppEvent/include/Box/BoxEvent.h"
#include "App/AppDemo/include/AppDemoProc.h"
#include "NetApp/P2PTrade/include/P2PTradeProc.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(P2PTrade)


void P2PTradeEvent::StartEvent( GameSys::GameManager* pGameManager )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, P2PTradeEvent>( pGameEventManager );
}


P2PTradeEvent::P2PTradeEvent( gfl2::heap::HeapBase* pHeap ) :
  NetEvent::NetEventBase( pHeap ),
  m_pP2PTradeProc( NULL ),
  m_pP2PTradeRequestClient( NULL ),
  m_pDemoSendPokemon( NULL ),
  m_pDemoRecvPokemon( NULL ),
  m_OutJumpMode( 0 ),
  m_OutTray( 0 ),
  m_OutPos( 0 ),
  m_DemoParam(),
  m_TradeDemoEventResult()
{
}


P2PTradeEvent::~P2PTradeEvent()
{
}


bool P2PTradeEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void P2PTradeEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  gfl2::heap::HeapBase *pDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  m_pP2PTradeRequestClient = GFL_NEW( pDeviceHeap ) NetAppLib::P2PTrade::P2PTradeRequestClient( pDeviceHeap );

  m_pDemoSendPokemon = GFL_NEW( pDeviceHeap ) pml::pokepara::PokemonParam( pDeviceHeap );
  m_pDemoRecvPokemon = GFL_NEW( pDeviceHeap ) pml::pokepara::PokemonParam( pDeviceHeap );

  SetSeqNo( SEQUENCE_INITIALIZE );
}


void P2PTradeEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  GFL_SAFE_DELETE( m_pDemoRecvPokemon );
  GFL_SAFE_DELETE( m_pDemoSendPokemon );

  GFL_SAFE_DELETE( m_pP2PTradeRequestClient );
}


GameSys::GMEVENT_RESULT P2PTradeEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  switch( GetSeqNo() )
  {
    // --------------------------------------------------------------
    case SEQUENCE_INITIALIZE:
    {
      if( IsError() )
      {
        SetSeqNo( SEQUENCE_FINISH );
      }
      else if( m_pP2PTradeRequestClient->Initialize() )
      {
        NetLib::NijiNetworkSystem::SetP2PTradeRequestClient( m_pP2PTradeRequestClient );

        bootBoxEvent( pGameManager );
        SetSeqNo( SEQUENCE_BOX );
      }
    }
    break;

    // --------------------------------------------------------------
    case SEQUENCE_BOX:
    {
      if( IsError() )
      {
        SetSeqNo( SEQUENCE_FINISH );
      }
      else
      {
        switch( m_OutJumpMode )
        {
          case App::Box::BOXJUMP_CANCEL:
          {
            SetSeqNo( SEQUENCE_FINISH );
          }
          break;

          default:
          {
            bootP2PTradeProc( NetApp::P2PTrade::BOOT_MODE_WARNING, m_pP2PTradeRequestClient );
            SetSeqNo( SEQUENCE_P2PTRADE_WARNING );
          }
          break;
        }
      }
    }
    break;

    // --------------------------------------------------------------
    case SEQUENCE_P2PTRADE_WARNING:
    {
      if( pGameManager->GetProcManager()->IsRegisterProcess( m_pP2PTradeProc ) == false )
      {
        if( IsError() )
        {
          SetSeqNo( SEQUENCE_FINISH );
        }
        else
        {
          bootTradeDemoEvent( pGameManager );
          SetSeqNo( SEQUENCE_TRADE_DEMO );
        }
      }
    }
    break;

    // --------------------------------------------------------------
    case SEQUENCE_TRADE_DEMO:
    {
      if( IsError() )
      {
        SetSeqNo( SEQUENCE_FINISH );
      }
      else
      {
        if( m_TradeDemoEventResult.p2pTradeResult.bIsSyncSaveCancel )
        {
          SetSeqNo( SEQUENCE_FINISH );
        }
        else
        {
          bootBoxEvent( pGameManager );
          SetSeqNo( SEQUENCE_BOX );
        }
      }
    }
    break;

    // --------------------------------------------------------------
    case SEQUENCE_FINISH:
    {
      if( m_pP2PTradeRequestClient->CanDestroy() )
      {
        NetLib::NijiNetworkSystem::RemoveP2PTradeRequestClient();

        result = GameSys::GMEVENT_RES_FINISH;
      }
    }
    break;
  }

  return result;
}


void P2PTradeEvent::bootBoxEvent( GameSys::GameManager* pGameManager )
{
  App::Event::BoxEvent* pBoxEvent = App::Event::BoxEvent::StartEvent( pGameManager );
  pBoxEvent->SetBoxStartType( App::Box::BOX_CALL_TRADE );
  pBoxEvent->SetBoxEventListener( this );
  pBoxEvent->SetP2PTradeRequestClient( m_pP2PTradeRequestClient );

  // niji通信互換対応：相手がnijiだと互換対応ON
  {
    NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE( NetLib::P2P::P2pConnectionManager );
    if( pP2pConnectionManager->IsNijiConnect() )
    {
      pBoxEvent->SetNijiCompatibility();
    }
  }


  m_pP2PTradeRequestClient->ResetFlag();
}


void P2PTradeEvent::bootP2PTradeProc( u32 bootMode, NetAppLib::P2PTrade::P2PTradeRequestClient* pP2PTradeRequestClient )
{
  NetApp::P2PTrade::APP_PARAM  param;

  param.bootMode = bootMode;

  m_pP2PTradeProc = GameSys::CallP2PTradeProc( &param );

  m_pP2PTradeProc->SetP2PTradeRequestClient( pP2PTradeRequestClient );

  pP2PTradeRequestClient->StartSyncSaveRequest();
}


void P2PTradeEvent::bootTradeDemoEvent( GameSys::GameManager* pGameManager )
{
  NetEvent::TradeDemo::TradeDemoEventParam_MiracleTrade param;

  app::event::ZukanRegisterEventParam&        zukanRegisterEventParam  = m_pP2PTradeRequestClient->GetZukanRegisterEventParam();
  NetEvent::Evolution::EvolutionEventParam&   evolutionEventParam      = m_pP2PTradeRequestClient->GetEvolutionEventParam();
  pml::pokepara::PokemonParam&                recvPokemonParam         = m_pP2PTradeRequestClient->GetRecvPokemonParam();

  // デモ
  {
    NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

    int TradePersonNetID = 1 - pP2pConnectionManager->GetNetID();

    u8* pSendPokemonBuffer = m_pP2PTradeRequestClient->GetSendPokemonBuffer();
    pml::pokepara::PokemonParam& recvPokemonParam = m_pP2PTradeRequestClient->GetRecvPokemonParam();

    m_pDemoSendPokemon->Deserialize_Core( pSendPokemonBuffer );
    m_pDemoRecvPokemon->CopyFrom( recvPokemonParam );

    m_DemoParam.demoGARCID = GARC_demo_seq_script_sd9110_trade_all_BSEQ;

    m_DemoParam.pSendPokemonParam  = m_pDemoSendPokemon;
    m_DemoParam.pRecvPokemonParam  = m_pDemoRecvPokemon;
    m_DemoParam.pTradePersonStatus = pP2pConnectionManager->GetMyStatus( TradePersonNetID );
    m_DemoParam.pHeap              = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

    GFL_PRINT( "Send pokemon mons no = %d\n", m_pDemoSendPokemon->GetMonsNo() );
    GFL_PRINT( "Recv pokemon mons no = %d\n", m_pDemoRecvPokemon->GetMonsNo() );
  }

  // 図鑑
  {
    zukanRegisterEventParam.pp = &recvPokemonParam;
  }

  // 進化
  {
    GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

    evolutionEventParam.m_pTargetPokemon = &recvPokemonParam;
    evolutionEventParam.m_Reason         = NetApp::Evolution::REASON_Trade;
    evolutionEventParam.m_pPokeParty     = pGameData->GetPlayerParty();
    evolutionEventParam.m_pMyItem        = pGameData->GetMyItem();
  }

  bool& bCallZukanRegister          = m_pP2PTradeRequestClient->GetCallZukanRegister();
  bool& bCallEvolution              = m_pP2PTradeRequestClient->GetCallEvolution();

  param.pP2PTradeRequestClient      = m_pP2PTradeRequestClient;
  param.pMiracleTradeDemoParam      = &m_DemoParam;
  param.pZukanRegisterEventParam    = &zukanRegisterEventParam;
  param.pEvolutionEventParam        = &evolutionEventParam;
  param.p2pTradeParam.bootMode      = NetApp::P2PTrade::BOOT_MODE_SYNC_SAVE;
  param.bCallZukanRegister          = bCallZukanRegister;
  param.bCallEvolution              = bCallEvolution;

  bCallZukanRegister                = false;
  bCallEvolution                    = false;

  m_TradeDemoEventResult.p2pTradeResult.bIsSyncSaveCancel = false;

  NetEvent::TradeDemo::TradeDemoEvent::StartEvent( pGameManager, param, &m_TradeDemoEventResult );
}


void P2PTradeEvent::OnTermination( u8 outJumpMode, u8 outTray, u8 outPos )
{
  m_OutJumpMode = outJumpMode;

  if( outJumpMode == App::Box::BOXJUMP_CANCEL )
  {
    return;
  }

  m_OutTray     = outTray;
  m_OutPos      = outPos;

  switch( outTray )
  {
    case App::Box::RETURN_TEMOTI:
    {
      pml::PokeParty* pPokeParty = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
      pml::pokepara::PokemonParam* pPokemonParam = pPokeParty->GetMemberPointer( outPos );
      u8 buffer[ pml::pokepara::PokemonParam::DATASIZE ];
      pPokemonParam->Serialize_Core( buffer );
      m_pDemoSendPokemon->Deserialize_Core( buffer );
    }
    break;

    default:
    {
      gfl2::heap::HeapBase *pDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
      Savedata::BoxPokemon* pBoxPokemon = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();
      pml::pokepara::CoreParam coreParam( pDeviceHeap );
      pBoxPokemon->GetPokemon( &coreParam, outTray, outPos );
      u8 buffer[ pml::pokepara::CoreParam::DATASIZE ];
      coreParam.Serialize_Core( buffer );
      m_pDemoSendPokemon->Deserialize_Core( buffer );
    }
    break;
  }

}


GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetEvent)
