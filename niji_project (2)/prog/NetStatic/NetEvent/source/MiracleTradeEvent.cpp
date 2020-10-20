// ============================================================================
/*
 * @file MiracleTradeEvent.cpp
 * @brief ミラクル交換イベント
 * @date 2015.05.25
 */
// ============================================================================
#include "NetStatic/NetEvent/include/MiracleTradeEvent.h"
#include "NetApp/MiracleTrade/include/MiracleTradeProc.h"
#include "NetApp/MiracleTrade/source/MiracleTradeFrameID.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"
#include "App/AppEvent/include/Box/BoxEvent.h"
#include "Savedata/include/BoxPokemonSave.h"
#include "pml/include/pml_PokeParty.h"
#include "App/AppDemo/include/AppDemoProc.h"
#include "Sound/include/Sound.h"
#include "NetStatic/NetEvent/include/TradeDemoEvent.h"


GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(MiracleTrade)


void MiracleTradeEvent::StartEvent( GameSys::GameManager* pGameManager )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, MiracleTradeEvent>( pGameEventManager );
}


MiracleTradeEvent::MiracleTradeEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_pWorkHeap( NULL ),
 m_AppParam(),
 m_pMiracleTradeWork( NULL ),
 m_pMiracleTradeProc( NULL ),
 m_pDemoSendPokemon( NULL ),
 m_pDemoRecvPokemon( NULL ),
 m_DemoParam()
{
}


MiracleTradeEvent::~MiracleTradeEvent()
{
}


bool MiracleTradeEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void MiracleTradeEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  Sound::StartBGMReq( STRM_BGM_MIRACLE_TRADE, Sound::BGM_FADE_VERY_FAST );

	gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  m_pDemoSendPokemon = GFL_NEW( pDevHeap ) pml::pokepara::PokemonParam( pDevHeap );
  m_pDemoRecvPokemon = GFL_NEW( pDevHeap ) pml::pokepara::PokemonParam( pDevHeap );

  m_pWorkHeap = GFL_CREATE_LOCAL_HEAP( pDevHeap, NetApp::MiracleTrade::MIRACLETRADE_APP_WORK_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );

  bootMiracleTradeProc( NetApp::MiracleTrade::MIRACLETRADE_FRAME_ID_START_CONFIRM );
  SetSeqNo( SEQUENCE_MIRACLE_TRADE );
}


void MiracleTradeEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  GFL_SAFE_DELETE( m_pMiracleTradeWork );

  gfl2::heap::Manager::DeleteHeap( m_pWorkHeap );

  GFL_SAFE_DELETE( m_pDemoRecvPokemon );
  GFL_SAFE_DELETE( m_pDemoSendPokemon );
}


void MiracleTradeEvent::bootMiracleTradeProc( NetAppLib::System::FrameID startFrameID )
{
  m_AppParam.startFrameID = startFrameID;

  m_pMiracleTradeProc = GameSys::CallMiracleTradeProc( &m_AppParam );
  
  if( m_pMiracleTradeWork == NULL )
  {
    m_pMiracleTradeWork = GFL_NEW( m_pWorkHeap ) NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork( m_pWorkHeap );
  }

  m_pMiracleTradeWork->CreateHeap( HEAPID_APP_DEVICE, NetApp::MiracleTrade::MIRACLETRADE_APP_HEAP_SIZE, NetApp::MiracleTrade::MIRACLETRADE_APP_DEVICE_HEAP_SIZE );
  
  m_pMiracleTradeProc->SetMiracleTradeWork( m_pMiracleTradeWork );
}


void MiracleTradeEvent::bootBoxEvent( GameSys::GameManager* pGameManager )
{
  if( m_pMiracleTradeWork )
  {
    m_pMiracleTradeWork->DeleteHeap();
  }

  App::Event::BoxEvent* pBoxEvent = App::Event::BoxEvent::StartEvent( pGameManager );
  pBoxEvent->SetBoxStartType( App::Box::BOX_CALL_MIRACLE );
  pBoxEvent->SetBoxEventListener( this );
  // niji通信互換対応：momiji新要素は出せない
  pBoxEvent->SetNijiCompatibility();
}


void MiracleTradeEvent::bootTradeDemoEvent( GameSys::GameManager* pGameManager )
{
  if( m_pMiracleTradeWork )
  {
    m_pMiracleTradeWork->DeleteHeap();
  }

  NetEvent::TradeDemo::TradeDemoEventParam_MiracleTrade param;

  app::event::ZukanRegisterEventParam&         zukanRegisterEventParam  = m_pMiracleTradeWork->GetZukanRegisterEventParam();
  NetEvent::Evolution::EvolutionEventParam&    evolutionEventParam      = m_pMiracleTradeWork->GetEvolutionEventParam();

  // デモ
  {
    pml::pokepara::CoreParam&  sendPokemon       = m_pMiracleTradeWork->GetSelectPokemonCoreParam();
    pml::pokepara::CoreParam&  recvPokemon       = m_pMiracleTradeWork->GetReceivedPokemonCoreParam();
    Savedata::MyStatus&        tradePersonStatus = m_pMiracleTradeWork->GetTradePersonStatus();

    m_pDemoSendPokemon->CoreParam::CopyFrom( sendPokemon );
    m_pDemoRecvPokemon->CoreParam::CopyFrom( recvPokemon );


    m_DemoParam.demoGARCID = GARC_demo_seq_script_sd9110_trade_all_BSEQ;

    m_DemoParam.pSendPokemonParam  = m_pDemoSendPokemon;
    m_DemoParam.pRecvPokemonParam  = m_pDemoRecvPokemon;
    m_DemoParam.pTradePersonStatus = &tradePersonStatus;
    m_DemoParam.pHeap              = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

    GFL_PRINT( "Send pokemon mons no = %d\n", m_pDemoSendPokemon->GetMonsNo() );
    GFL_PRINT( "Recv pokemon mons no = %d\n", m_pDemoRecvPokemon->GetMonsNo() );
  }

  // 図鑑 & 進化
  {
    pml::pokepara::CoreParam& receivedPokemonCoreParam = m_pMiracleTradeWork->GetReceivedPokemonCoreParam();
    pml::pokepara::PokemonParam& receivedPokemonParam = m_pMiracleTradeWork->GetReceivedPokemonParam();

    u8 buffer[ pml::pokepara::CoreParam::DATASIZE ];
    receivedPokemonCoreParam.Serialize_Core( buffer );
    receivedPokemonParam.Deserialize_Core( buffer );

    // 図鑑
    {
      zukanRegisterEventParam.pp = &receivedPokemonParam;
    }

    // 進化
    {
      GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

      evolutionEventParam.m_pTargetPokemon = &receivedPokemonParam;
      evolutionEventParam.m_Reason         = NetApp::Evolution::REASON_Trade;
      evolutionEventParam.m_pPokeParty     = pGameData->GetPlayerParty();
      evolutionEventParam.m_pMyItem        = pGameData->GetMyItem();
    }
  }

  bool& bCallZukanRegister          = m_pMiracleTradeWork->GetCallZukanRegister();
  bool& bCallEvolution              = m_pMiracleTradeWork->GetCallEvolution();

  param.pP2PTradeRequestClient      = NULL;
  param.pMiracleTradeDemoParam      = &m_DemoParam;
  param.pZukanRegisterEventParam    = &zukanRegisterEventParam;
  param.pEvolutionEventParam        = &evolutionEventParam;
  param.p2pTradeParam.bootMode      = NetApp::P2PTrade::BOOT_MODE_SAVE;
  param.bCallZukanRegister          = bCallZukanRegister;
  param.bCallEvolution              = bCallEvolution;

  bCallZukanRegister                = false;
  bCallEvolution                    = false;

  NetEvent::TradeDemo::TradeDemoEvent::StartEvent( pGameManager, param );
}


GameSys::GMEVENT_RESULT MiracleTradeEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  switch( GetSeqNo() )
  {
    // ==========================================================================================
    case SEQUENCE_MIRACLE_TRADE:
    {
      if( pGameManager->GetProcManager()->IsRegisterProcess( m_pMiracleTradeProc ) == false )
      {
        if( IsError() )
        {
          result = GameSys::GMEVENT_RES_FINISH;
        }
        else
        {
          NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork::BootProcID eBootProcID = m_pMiracleTradeWork->GetBootProcID();
          if( eBootProcID != NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork::BOOT_PROC_ID_NONE )
          {
            switch( eBootProcID )
            {
              case NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork::BOOT_PROC_ID_BOX:
              {
                bootBoxEvent( pGameManager );
                SetSeqNo( SEQUENCE_BOX_SELECT_WAIT );
              }
              break;

              case NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork::BOOT_PROC_ID_DEMO:
              {
                bootTradeDemoEvent( pGameManager );
                SetSeqNo( SEQUENCE_TRADE_DEMO );
              }
              break;
            }

            m_pMiracleTradeWork->SetBootProcID( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork::BOOT_PROC_ID_NONE );

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
    case SEQUENCE_BOX_SELECT_END:
    {
      if( IsError() )
      {
        result = GameSys::GMEVENT_RES_FINISH;
      }
      else
      {
        switch( m_pMiracleTradeWork->GetLastExecuteFrameID() )
        {
          // --------------------------------------------------------------------------------------
          case NetApp::MiracleTrade::MIRACLETRADE_FRAME_ID_START_CONFIRM:
          {
            switch( m_AppParam.outJumpMode )
            {
              case App::Box::BOXJUMP_CANCEL:
                bootMiracleTradeProc( NetApp::MiracleTrade::MIRACLETRADE_FRAME_ID_START_CONFIRM );
                SetSeqNo( SEQUENCE_MIRACLE_TRADE );
              break;

              default:
                bootMiracleTradeProc( NetApp::MiracleTrade::MIRACLETRADE_FRAME_ID_MATCHING );
                SetSeqNo( SEQUENCE_MIRACLE_TRADE );
              break;
            }
          }
          break;
        }
      }
    }
    break;

    // ==========================================================================================
    case SEQUENCE_TRADE_DEMO:
    {
      //result = GameSys::GMEVENT_RES_FINISH;
      // momiji要望変更：ミラクル交換終了後にミラクル交換アプリに戻るように修正
      bootMiracleTradeProc( NetApp::MiracleTrade::MIRACLETRADE_FRAME_ID_START_CONFIRM );
      SetSeqNo( SEQUENCE_MIRACLE_TRADE );
    }
    break;

  }

  return result;
}


void MiracleTradeEvent::OnTermination( u8 outJumpMode, u8 outTray,u8 outPos )
{
  SetSeqNo( SEQUENCE_BOX_SELECT_END );

  m_AppParam.outJumpMode = outJumpMode;

  if( outJumpMode == App::Box::BOXJUMP_CANCEL )
  {
    return;
  }

  m_AppParam.outTray = outTray;
  m_AppParam.outPos = outPos;

  if( m_pMiracleTradeWork )
  {
    NetApp::MiracleTrade::ApplicationSystem::PokemonSelectInfo& pokemonSelectInfo = m_pMiracleTradeWork->GetPokemonSelectInfo();
    pml::pokepara::CoreParam& selectPokemonCoreParam = m_pMiracleTradeWork->GetSelectPokemonCoreParam();
    
    pokemonSelectInfo.tray = static_cast<u32>( outTray );
    pokemonSelectInfo.pos = static_cast<u32>( outPos );

    switch( m_AppParam.outTray )
    {
      case App::Box::RETURN_TEMOTI:
      {
        pml::PokeParty* pPokeParty = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
        pml::pokepara::PokemonParam* pPokemonParam = pPokeParty->GetMemberPointer( m_AppParam.outPos );
        u8 buffer[ pml::pokepara::CoreParam::DATASIZE ];
        pPokemonParam->Serialize_Core( buffer );
        selectPokemonCoreParam.Deserialize_Core( buffer );
        pokemonSelectInfo.inBox = false;
      }
      break;

      default:
      {
        Savedata::BoxPokemon* pBoxPokemon = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();
        pBoxPokemon->GetPokemon( &selectPokemonCoreParam, m_AppParam.outTray, m_AppParam.outPos );
        pokemonSelectInfo.inBox = true;
      }
      break;
    }
  }
}


GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetEvent)
