// ============================================================================
/*
 * @file GTSEvent.cpp
 * @brief GTSイベント
 * @date 2015.03.17
 */
// ============================================================================
#include "NetStatic/NetEvent/include/GTSEvent.h"
#include "NetApp/GTS/include/GTSProc.h"
#include "NetApp/GTS/source/GTSFrameID.h"
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
#include "App/AppEvent/include/FieldStrInput/FieldStrInputEvent.h"
#include "App/StrInput/include/StrInputFrame.h"


#if PM_DEBUG
#include "App/Zukan/source/ZukanDebugWin.h"
#endif // PM_DEBUG

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(GTS)


void GTSEvent::StartEvent( GameSys::GameManager* pGameManager )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, GTSEvent>( pGameEventManager );
}


GTSEvent::GTSEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_pBoxModule( NULL ),
 m_pRaderChartModule( NULL ),
#if PM_DEBUG
 m_pZukanModule( NULL ),
 m_pZukanDebugWin( NULL ),
#endif // PM_DEBUG
 m_pWorkHeap( NULL ),
 m_AppParam(),
 m_pGTSWork( NULL ),
 m_pGTSProc( NULL ),
 m_pDemoSendPokemon( NULL ),
 m_pDemoRecvPokemon( NULL ),
 m_DemoParam(),
 m_StrInputAppParam()
{
}


GTSEvent::~GTSEvent()
{
}


bool GTSEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void GTSEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  Sound::StartBGMReq( STRM_BGM_GTS, Sound::BGM_FADE_VERY_FAST );
  
  gfl2::heap::HeapBase* pDevHeap  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  m_pDemoSendPokemon = GFL_NEW( pDevHeap ) pml::pokepara::PokemonParam( pDevHeap );
  m_pDemoRecvPokemon = GFL_NEW( pDevHeap ) pml::pokepara::PokemonParam( pDevHeap );

  m_pWorkHeap = GFL_CREATE_LOCAL_HEAP( pDevHeap, NetApp::GTS::GTS_APP_WORK_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );

  bootGTSProc( NetApp::GTS::GTS_FRAME_ID_STATE_CONFIRM );
  SetSeqNo( SEQUENCE_GTS );
}


void GTSEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  shutdownGTSProc();

  GFL_SAFE_DELETE( m_pGTSWork );

  gfl2::heap::Manager::DeleteHeap( m_pWorkHeap );

  GFL_SAFE_DELETE( m_pDemoRecvPokemon );
  GFL_SAFE_DELETE( m_pDemoSendPokemon );

}


void GTSEvent::bootGTSProc( NetAppLib::System::FrameID startFrameID )
{
  m_AppParam.startFrameID = startFrameID;

  // @note : 最初にdllを読み込まないとdll内のクラスをnew出来ない。要注意
  m_pGTSProc = GameSys::CallGTSProc( &m_AppParam );


  // Boxの上画面に必要なROモジュールの読み込み
  {
    gfl2::fs::AsyncFileManager* pAsyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
    gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

    if( m_pBoxModule == NULL )
    {
      m_pBoxModule = roMgr->LoadModule( pAsyncFileManager, "Box.cro" );
      roMgr->StartModule( m_pBoxModule );
    }

    if( m_pRaderChartModule == NULL )
    {
      m_pRaderChartModule = roMgr->LoadModule( pAsyncFileManager, "RaderChart.cro" );
      roMgr->StartModule( m_pRaderChartModule );
    }

#if PM_DEBUG
    if( m_pZukanModule == NULL )
    {
      m_pZukanModule = roMgr->LoadModule( pAsyncFileManager, "Zukan.cro" );
      roMgr->StartModule( m_pZukanModule );
    }
#endif // PM_DEBUG
  }


  if( m_pGTSWork == NULL )
  {
    m_pGTSWork = GFL_NEW( m_pWorkHeap ) NetApp::GTS::ApplicationSystem::GTSWork( m_pWorkHeap );
  }

  m_pGTSWork->CreateHeap( HEAPID_APP_DEVICE, NetApp::GTS::GTS_APP_HEAP_SIZE, NetApp::GTS::GTS_APP_DEVICE_HEAP_SIZE );

  m_pGTSProc->SetGTSWork( m_pGTSWork );

  GFL_PRINT( "----------------------------------------------\n" );
  GFL_PRINT( "GTS memory debug\n" );
  GFL_PRINT( "----- Work Heap -----\n" );
  GFL_PRINT( "Total size                = %d\n", m_pWorkHeap->GetTotalSize() );
  GFL_PRINT( "Total free size           = %d\n", m_pWorkHeap->GetTotalFreeSize() );
  GFL_PRINT( "Total allocatable size    = %d\n", m_pWorkHeap->GetTotalAllocatableSize() );
  GFL_PRINT( "Memory fragmentation size = %d\n", m_pWorkHeap->GetTotalFreeSize() - m_pWorkHeap->GetTotalAllocatableSize() );
  GFL_PRINT( "Use memory size           = %d\n", m_pWorkHeap->GetTotalSize() - m_pWorkHeap->GetTotalFreeSize() );
  GFL_PRINT( "----------------------------------------------\n" );

#if PM_DEBUG
  m_pZukanDebugWin = GFL_NEW( m_pGTSWork->GetDeviceHeap() ) app::zukan::ZukanDebugWin( m_pGTSWork->GetDeviceHeap(), NULL );
#endif // PM_DEBUG

}


void GTSEvent::shutdownGTSProc()
{
#if PM_DEBUG
  GFL_SAFE_DELETE( m_pZukanDebugWin );
#endif // PM_DEBUG

  // 読み込んだROモジュールの解放
  {
    gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

#if PM_DEBUG
    if( m_pZukanModule )
    {
      roMgr->DisposeModule( m_pZukanModule );
      m_pZukanModule = NULL;
    }
#endif // PM_DEBUG

    if( m_pRaderChartModule )
    {
      roMgr->DisposeModule( m_pRaderChartModule );
      m_pRaderChartModule = NULL;
    }

    if( m_pBoxModule )
    {
      roMgr->DisposeModule( m_pBoxModule );
      m_pBoxModule = NULL;
    }
  }

  if( m_pGTSWork )
  {
    m_pGTSWork->DeleteHeap();
  }
}


void GTSEvent::bootBoxEvent( GameSys::GameManager* pGameManager )
{
  shutdownGTSProc();

  App::Event::BoxEvent* pBoxEvent = App::Event::BoxEvent::StartEvent( pGameManager );
  pBoxEvent->SetBoxStartType( App::Box::BOX_CALL_GTS );
  pBoxEvent->SetBoxEventListener( this );
  
  // 交換相手を選択する画面からボックスを起動する時
  if( m_pGTSWork->GetLastExecuteFrameID() == NetApp::GTS::GTS_FRAME_ID_TRADE_PERSON_SELECT )
  {
    const NetApp::GTS::GtsSearchResultInfo& tradePartnerInfo = m_pGTSWork->GetTradePartnerInfo();
    
    u8 gender = pml::SEX_NUM;

    switch( tradePartnerInfo.indexData.PokeInfo.requestGender )
    {
      case NetApp::GTS::GENDER_MALE:  gender = pml::SEX_MALE;   break;
      case NetApp::GTS::GENDER_FMALE: gender = pml::SEX_FEMALE; break;
    }

    u8 level = 0;

    switch( tradePartnerInfo.indexData.PokeInfo.requestLevelBand )
    {
      case NetApp::GTS::LEVEL_BAND_NONE:   level =  0; break;
      case NetApp::GTS::LEVEL_BAND_1_10:   level =  1; break;
      case NetApp::GTS::LEVEL_BAND_11_20:  level = 11; break;
      case NetApp::GTS::LEVEL_BAND_21_30:  level = 21; break;
      case NetApp::GTS::LEVEL_BAND_31_40:  level = 31; break;
      case NetApp::GTS::LEVEL_BAND_41_50:  level = 41; break;
      case NetApp::GTS::LEVEL_BAND_51_60:  level = 51; break;
      case NetApp::GTS::LEVEL_BAND_61_70:  level = 61; break;
      case NetApp::GTS::LEVEL_BAND_71_80:  level = 71; break;
      case NetApp::GTS::LEVEL_BAND_81_90:  level = 81; break;
      case NetApp::GTS::LEVEL_BAND_91_100: level = 91; break;
    }

    // 相手が欲しいポケモンだけを選択できるようにする
    pBoxEvent->TradePokemon( tradePartnerInfo.indexData.PokeInfo.requestKind, gender, level );

    // 相手がnijiの場合は互換モードにする
    ICHI_PRINT( ">ICHI GTSインデックスデータのROMコード %d\n",tradePartnerInfo.indexData.romCode );
    if( tradePartnerInfo.indexData.romCode == VERSION_SUN ||
        tradePartnerInfo.indexData.romCode == VERSION_MOON )
    {
      pBoxEvent->SetNijiCompatibility();
    }
  }
}


void GTSEvent::bootTradeDemoEvent( GameSys::GameManager* pGameManager )
{
  shutdownGTSProc();

  NetEvent::TradeDemo::TradeDemoEventParam_GTS param;

  app::event::ZukanRegisterEventParam& zukanRegisterEventParam = m_pGTSWork->GetZukanRegisterEventParam();
  NetEvent::Evolution::EvolutionEventParam& evolutionEventParam = m_pGTSWork->GetEvolutionEventParam();

  // デモ
  {
    NetApp::GTS::ApplicationSystem::GTSWork::BootProcID eBootProcID = m_pGTSWork->GetBootProcID();

    switch( eBootProcID )
    {
      case NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_DEMO_UPLOAD:         m_DemoParam.demoGARCID = GARC_demo_seq_script_sd9110_trade_up_BSEQ;    break;
      case NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_DEMO_TRADE:          m_DemoParam.demoGARCID = GARC_demo_seq_script_sd9110_trade_all_BSEQ;   break;
      case NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_DEMO_DOWNLOAD_TRADE: m_DemoParam.demoGARCID = GARC_demo_seq_script_sd9110_trade_down_BSEQ;  break;
      case NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_DEMO_DOWNLOAD:       m_DemoParam.demoGARCID = GARC_demo_seq_script_sd9110_trade_down_BSEQ;  break;
      default: GFL_ASSERT_MSG( NULL, "Error !! : eBootProcID = %d\n", eBootProcID ); break;
    }

    pml::pokepara::CoreParam& sendPokemon = m_pGTSWork->GetDemoSendPokemon();
    pml::pokepara::CoreParam& recvPokemon = m_pGTSWork->GetDemoRecvPokemon();

    m_pDemoSendPokemon->CoreParam::CopyFrom( sendPokemon );
    m_pDemoRecvPokemon->CoreParam::CopyFrom( recvPokemon );

    m_DemoParam.pSendPokemonParam = m_pDemoSendPokemon;
    m_DemoParam.pRecvPokemonParam = m_pDemoRecvPokemon;
    m_DemoParam.bDownloadTrade    = ( eBootProcID == NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_DEMO_DOWNLOAD_TRADE ) ? true : false;
  }

  // 図鑑 & 進化
  {
    pml::pokepara::CoreParam& demoRecvPokemon = m_pGTSWork->GetDemoRecvPokemon();
    pml::pokepara::PokemonParam& receivedPokemonParam = m_pGTSWork->GetReceivedPokemonParam();

    u8 buffer[ pml::pokepara::CoreParam::DATASIZE ];
    demoRecvPokemon.Serialize_Core( buffer );
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

  bool& bCallZukanRegister          = m_pGTSWork->GetCallZukanRegister();
  bool& bCallEvolution              = m_pGTSWork->GetCallEvolution();

  param.pGTSTradeDemoParam          = &m_DemoParam;
  param.pZukanRegisterEventParam    = &zukanRegisterEventParam;
  param.pEvolutionEventParam        = &evolutionEventParam;
  param.p2pTradeParam.bootMode      = NetApp::P2PTrade::BOOT_MODE_SAVE;
  param.bCallZukanRegister          = bCallZukanRegister;
  param.bCallEvolution              = bCallEvolution;

  bCallZukanRegister                = false;
  bCallEvolution                    = false;

  NetEvent::TradeDemo::TradeDemoEvent::StartEvent( pGameManager, param );
}


void GTSEvent::bootNameInputEvent( GameSys::GameManager* pGameManager )
{
  shutdownGTSProc();

  m_StrInputAppParam.mpGameProcManager     = pGameManager->GetProcManager();
  m_StrInputAppParam.pSaveMisc             = pGameManager->GetGameData()->GetMisc();
  m_StrInputAppParam.type                  = App::StrInput::TYPE_POKE_WANT;
  m_StrInputAppParam.param                 = 0;
  m_StrInputAppParam.cp                    = NULL;
  m_StrInputAppParam.isFieldSubProc        = false;

  m_StrInputAppParam.language              = System::GetLang();
  m_StrInputAppParam.strBuf                = &m_pGTSWork->GetInputName();
  m_StrInputAppParam.strCode               = NULL;

  m_StrInputAppParam.changeFlag            = false;
  m_StrInputAppParam.isInputCancel         = false;

  m_StrInputAppParam.skipInitGraphicMemory = true;

  m_StrInputAppParam.mpHeap                = m_pGTSWork->GetAppHeap();
  m_StrInputAppParam.mpAppRenderingManager = m_pGTSWork->GetAppRenderingManager();

  app::event::FieldStrInputEvent* pFieldStrInputEvent = app::event::FieldStrInputEvent::StartEvent( pGameManager );

  pFieldStrInputEvent->SetAppParam( &m_StrInputAppParam );
}


GameSys::GMEVENT_RESULT GTSEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result = GameSys::GMEVENT_RES_CONTINUE;

  switch( GetSeqNo() )
  {
    // ==========================================================================================
    case SEQUENCE_GTS:
    {
      if( pGameManager->GetProcManager()->IsRegisterProcess( m_pGTSProc ) == false )
      {
        if( IsError() )
        {
          result = GameSys::GMEVENT_RES_FINISH;
        }
        else
        {
          NetApp::GTS::ApplicationSystem::GTSWork::BootProcID eBootProcID = m_pGTSWork->GetBootProcID();
          if( eBootProcID != NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_NONE )
          {
            switch( eBootProcID )
            {
              case NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_BOX:
              {
                bootBoxEvent( pGameManager );
                SetSeqNo( SEQUENCE_BOX_SELECT_WAIT );
              }
              break;
            
              case NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_DEMO_UPLOAD:
              case NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_DEMO_TRADE:
              case NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_DEMO_DOWNLOAD_TRADE:
              case NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_DEMO_DOWNLOAD:
              {
                bootTradeDemoEvent( pGameManager );
                SetSeqNo( SEQUENCE_TRADE_DEMO );
              }
              break;

              case NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_NAME_INPUT:
              {
                bootNameInputEvent( pGameManager );
                SetSeqNo( SEQUENCE_NAME_INPUT );
              }
              break;
            }

            m_pGTSWork->SetBootProcID( NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_NONE );
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
        switch( m_pGTSWork->GetLastExecuteFrameID() )
        {
          // --------------------------------------------------------------------------------------
          case NetApp::GTS::GTS_FRAME_ID_TOP_MENU:          // トップメニューの画面
          case NetApp::GTS::GTS_FRAME_ID_UPLOAD_POKEMON:    // ポケモンを預ける画面
          {
            switch( m_AppParam.outJumpMode )
            {
              case App::Box::BOXJUMP_CANCEL:
                bootGTSProc( NetApp::GTS::GTS_FRAME_ID_TOP_MENU );
                SetSeqNo( SEQUENCE_GTS );
              break;

              default:
                bootGTSProc( NetApp::GTS::GTS_FRAME_ID_UPLOAD_POKEMON );
                SetSeqNo( SEQUENCE_GTS );
              break;

            }
          }
          break;

          // --------------------------------------------------------------------------------------
          case NetApp::GTS::GTS_FRAME_ID_TRADE_PERSON_SELECT: // 交換相手を選択する画面
          {
            switch( m_AppParam.outJumpMode )
            {
              case App::Box::BOXJUMP_CANCEL:
                bootGTSProc( NetApp::GTS::GTS_FRAME_ID_TRADE_PERSON_SELECT );
                SetSeqNo( SEQUENCE_GTS );
              break;

              default:
                bootGTSProc( NetApp::GTS::GTS_FRAME_ID_TRADE_POKEMON );
                SetSeqNo( SEQUENCE_GTS );
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
      if( IsError() )
      {
        result = GameSys::GMEVENT_RES_FINISH;
      }
      else
      {
        bootGTSProc( NetApp::GTS::GTS_FRAME_ID_TOP_MENU );
        SetSeqNo( SEQUENCE_GTS );
      }
    }
    break;

    // ==========================================================================================
    case SEQUENCE_NAME_INPUT:
    {
      if( IsError() )
      {
        result = GameSys::GMEVENT_RES_FINISH;
      }
      else
      {
        if( m_pGTSWork->GetInputName().GetLength() > 0 )
        {
          m_pGTSWork->SetNameInputFlag( true );
        }
        else
        {
          m_pGTSWork->SetRequestPokemonMonsNo( MONSNO_NULL );
          m_pGTSWork->SetSearchPokemonMonsNo( MONSNO_NULL );
          m_pGTSWork->SetNameInputFlag( false );
        }

        bootGTSProc( m_pGTSWork->GetLastExecuteFrameID() );
        SetSeqNo( SEQUENCE_GTS );
      }
    }
    break;
  }

  return result;
}


void GTSEvent::OnTermination( u8 outJumpMode, u8 outTray, u8 outPos )
{
  SetSeqNo( SEQUENCE_BOX_SELECT_END );

  m_AppParam.outJumpMode = outJumpMode;

  if( outJumpMode == App::Box::BOXJUMP_CANCEL )
  {
    return;
  }

  m_AppParam.outTray = outTray;
  m_AppParam.outPos = outPos;

  if( m_pGTSWork )
  {
    NetApp::GTS::ApplicationSystem::PokemonSelectInfo& pokemonSelectInfo = m_pGTSWork->GetPokemonSelectInfo();
    pml::pokepara::CoreParam& selectPokemonCoreParam = m_pGTSWork->GetSelectPokemonCoreParam();
    
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


GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetEvent)
