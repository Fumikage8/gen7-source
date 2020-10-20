// ============================================================================
/*
 * @file TradeDemoEvent.cpp
 * @brief 交換デモ専用のイベント
 * @date 2016.01.05
 * @author Akira endo
 */
// ============================================================================
#include "NetStatic/NetEvent/include/TradeDemoEvent.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"
#include "App/AppDemo/include/AppDemoProc.h"
#include "NetApp/P2PTrade/include/P2PTradeProc.h"
#include "App/Box/include/BoxAppParam.h"


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(TradeDemo)

static const int SEQUENCE_GTS_TRADE_DEMO        = 0;
static const int SEQUENCE_MIRACLE_TRADE_DEMO    = 1;
static const int SEQUENCE_ZUKAN_REGISTER        = 2;
static const int SEQUENCE_EVOLUTION             = 3;
static const int SEQUENCE_SAVE                  = 4;

void TradeDemoEvent::StartEvent( GameSys::GameManager* pGameManager, TradeDemoEventParam_GTS& param, TradeDemoEventResult* pResult )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  TradeDemoEvent* pTradeDemoEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, TradeDemoEvent>( pGameEventManager );
  pTradeDemoEvent->SetParam( param );
  pTradeDemoEvent->SetResult( pResult );
}


void TradeDemoEvent::StartEvent( GameSys::GameManager* pGameManager, TradeDemoEventParam_MiracleTrade& param, TradeDemoEventResult* pResult )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  TradeDemoEvent* pTradeDemoEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, TradeDemoEvent>( pGameEventManager );
  pTradeDemoEvent->SetParam( param );
  pTradeDemoEvent->SetResult( pResult );
}


TradeDemoEvent::TradeDemoEvent( gfl2::heap::HeapBase* pHeap ) :
  GameSys::GameEvent( pHeap ),
  m_pP2PTradeProc( NULL ),
  m_pGTSDemoProc( NULL ),
  m_pMiracleTradeDemoProc( NULL ),
  m_pGTSTradeDemoParam( NULL ),
  m_pMiracleTradeDemoParam( NULL ),
  m_pZukanRegisterEventParam( NULL ),
  m_pEvolutionEventParam( NULL ),
  m_P2PTradeParam(),
  m_pTradeDemoEventResult( NULL ),
  m_pP2PTradeRequestClient( NULL ),
  m_bCallZukanRegister( false ),
  m_bCallEvolution( false )
{
}


TradeDemoEvent::~TradeDemoEvent()
{
}


void TradeDemoEvent::SetParam( TradeDemoEventParam_GTS& param )
{
  m_pGTSTradeDemoParam         = param.pGTSTradeDemoParam;
  m_pZukanRegisterEventParam   = param.pZukanRegisterEventParam;
  m_pEvolutionEventParam       = param.pEvolutionEventParam;
  m_P2PTradeParam              = param.p2pTradeParam;
  m_bCallZukanRegister         = param.bCallZukanRegister;
  m_bCallEvolution             = param.bCallEvolution;
}


void TradeDemoEvent::SetParam( TradeDemoEventParam_MiracleTrade& param )
{
  m_pP2PTradeRequestClient     = param.pP2PTradeRequestClient;
  m_pMiracleTradeDemoParam     = param.pMiracleTradeDemoParam;
  m_pZukanRegisterEventParam   = param.pZukanRegisterEventParam;
  m_pEvolutionEventParam       = param.pEvolutionEventParam;
  m_P2PTradeParam              = param.p2pTradeParam;
  m_bCallZukanRegister         = param.bCallZukanRegister;
  m_bCallEvolution             = param.bCallEvolution;
}


void TradeDemoEvent::SetResult( TradeDemoEventResult* pResult )
{
  m_pTradeDemoEventResult = pResult;
}


void TradeDemoEvent::copy( pml::pokepara::PokemonParam& dest, pml::pokepara::PokemonParam& src )
{
  u8 buffer[ pml::pokepara::PokemonParam::DATASIZE ];
  
  src.Serialize_Core( buffer );

  dest.Deserialize_Core( buffer );
}


bool TradeDemoEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void TradeDemoEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  bootDemoProc();
}


void TradeDemoEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT TradeDemoEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  switch( GetSeqNo() )
  {
    // --------------------------------------------------------------------------------------------
    case SEQUENCE_GTS_TRADE_DEMO:
    {
      if( pGameManager->GetProcManager()->IsRegisterProcess( m_pGTSDemoProc ) == false )
      {
        if( m_bCallZukanRegister )
        {
          bootZukanRegisterEvent( pGameManager );
        }
        else
        {
          if( m_bCallEvolution )
          {
            bootEvolutionEvent( pGameManager );
          }
          else
          {
            bool bBootSaveProc = true;

            if( m_pGTSTradeDemoParam )
            {
              switch( m_pGTSTradeDemoParam->demoGARCID )
              {
                case GARC_demo_seq_script_sd9110_trade_up_BSEQ:
                {
                  bBootSaveProc = false;
                }
                break;

                case GARC_demo_seq_script_sd9110_trade_all_BSEQ:
                {
                  bBootSaveProc = true;
                }
                break;

                case GARC_demo_seq_script_sd9110_trade_down_BSEQ:
                {
                  if( m_pGTSTradeDemoParam->bDownloadTrade )
                  {
                    bBootSaveProc = true;
                  }
                  else
                  {
                    bBootSaveProc = false;
                  }
                }
                break;
              }
            }
            
            if( bBootSaveProc )
            {
              bootSaveProc();
            }
            else
            {
              result = GameSys::GMEVENT_RES_FINISH;
            }
          }
        }
      }
    }
    break;

    // --------------------------------------------------------------------------------------------
    case SEQUENCE_MIRACLE_TRADE_DEMO:
    {
      if( pGameManager->GetProcManager()->IsRegisterProcess( m_pMiracleTradeDemoProc ) == false )
      {
        if( m_pMiracleTradeDemoParam->pRecvPokemonParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
        {
          bootSaveProc();
        }
        else
        {
          if( m_bCallZukanRegister )
          {
            bootZukanRegisterEvent( pGameManager );
          }
          else
          {
            if( m_bCallEvolution )
            {
              bootEvolutionEvent( pGameManager );
            }
            else
            {
              bootSaveProc();
            }
          }
        }
      }
    }
    break;

    // --------------------------------------------------------------------------------------------
    case SEQUENCE_ZUKAN_REGISTER:
    {
      if( m_bCallEvolution )
      {
        bootEvolutionEvent( pGameManager );
      }
      else
      {
        bootSaveProc();
      }
    }
    break;

    // --------------------------------------------------------------------------------------------
    case SEQUENCE_EVOLUTION:
    {
      u32 pos0 = m_pZukanRegisterEventParam->returnParam.pos0;
      u32 pos1 = m_pZukanRegisterEventParam->returnParam.pos1;

      if( m_pZukanRegisterEventParam->returnParam.putPlace == PokeTool::PokeGetUtil::PutPlace::PARTY )
      {
        pml::PokeParty* pPokeParty = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
        pPokeParty->ReplaceMember( pos0, *m_pEvolutionEventParam->m_pTargetPokemon );
      }
      else
      if( m_pZukanRegisterEventParam->returnParam.putPlace == PokeTool::PokeGetUtil::PutPlace::BOX )
      {
        gfl2::heap::HeapBase *pDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
        Savedata::BoxPokemon* pBoxPokemon = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();
        pBoxPokemon->ClearPokemon( pos0, pos1, pDeviceHeap );
        pBoxPokemon->PutPokemon( m_pEvolutionEventParam->m_pTargetPokemon, pos0, pos1, pDeviceHeap );
      }
      else
      {
        GFL_ASSERT( 0 ); // @fix
      }

      bootSaveProc();
    }
    break;

    // --------------------------------------------------------------------------------------------
    case SEQUENCE_SAVE:
    {
      if( pGameManager->GetProcManager()->IsRegisterProcess( m_pP2PTradeProc ) == false )
      {
        result = GameSys::GMEVENT_RES_FINISH;
      }
    }
    break;
  }

  return result;
}


void TradeDemoEvent::bootDemoProc()
{
  if( m_pGTSTradeDemoParam )
  {
    m_pGTSDemoProc = GameSys::CallAppDemoProc_GTS( m_pGTSTradeDemoParam );

    SetSeqNo( SEQUENCE_GTS_TRADE_DEMO );
  }
  else
  if( m_pMiracleTradeDemoParam )
  {
    m_pMiracleTradeDemoProc = GameSys::CallAppDemoProc_MiracleTrade( m_pMiracleTradeDemoParam );

    SetSeqNo( SEQUENCE_MIRACLE_TRADE_DEMO );
  }
}


void TradeDemoEvent::bootZukanRegisterEvent( GameSys::GameManager* pGameManager )
{
  app::event::ZukanRegisterEvent::StartEvent( pGameManager, m_pZukanRegisterEventParam );

  SetSeqNo( SEQUENCE_ZUKAN_REGISTER );
}


void TradeDemoEvent::bootEvolutionEvent( GameSys::GameManager* pGameManager )
{
  NetEvent::Evolution::EvolutionEvent::StartEvent( pGameManager, *m_pEvolutionEventParam );

  SetSeqNo( SEQUENCE_EVOLUTION );
}


void TradeDemoEvent::bootSaveProc()
{
  GFL_PRINT( "TradeDemoEvent::bootSaveProc() \n" );

  m_pP2PTradeProc = GameSys::CallP2PTradeProc( &m_P2PTradeParam );

  if( m_pTradeDemoEventResult )
  {
    GFL_PRINT( "m_pP2PTradeProc->SetAppResult( &m_pTradeDemoEventResult->p2pTradeResult );\n" );

    m_pP2PTradeProc->SetAppResult( &m_pTradeDemoEventResult->p2pTradeResult );
  }

  if( m_pP2PTradeRequestClient )
  {
    GFL_PRINT( "m_pP2PTradeProc->SetP2PTradeRequestClient( m_pP2PTradeRequestClient );\n" );
    
    m_pP2PTradeProc->SetP2PTradeRequestClient( m_pP2PTradeRequestClient );
  }

  SetSeqNo( SEQUENCE_SAVE );
}


GFL_NAMESPACE_END(TradeDemo)
GFL_NAMESPACE_END(NetEvent)
