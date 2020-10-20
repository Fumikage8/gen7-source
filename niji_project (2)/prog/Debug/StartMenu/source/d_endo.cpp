#if PM_DEBUG
//=============================================================================
/**
 * @file    d_endo.cpp
 * @brief   遠藤用ランチャーリスト処理
 * @author  Akira Endo
 * @date    2015.03.02
 */
//=============================================================================
#include <Debug/StartMenu/include/StartLauncher.h>
#include <Debug/StartMenu/include/d_endo.h>

#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_endo.h>

#include <GameSys/include/GameProcManager.h>

#include "Test/CecTest/include/CecTestProc.h"
#include "Test/P2PTradeTest/include/P2PTradeTestProc.h"
#include "Test/StlPerformanceCheck/include/StlPerformanceCheckProc.h"
#include "Test/QRCodeTest/include/QRCodeTestProc.h"
#include "Test/CryptoTest/include/CryptoTestProc.h"
#include "Test/SaveTest/include/SaveTestProc.h"
#include "NetStatic/NetEvent/include/GTSEvent.h"
#include "NetStatic/NetEvent/include/MiracleTradeEvent.h"
#include "NetStatic/NetEvent/include/RegulationSelectEvent.h"
#include "NetStatic/NetEvent/include/ConnectionEvent.h"
#include "NetStatic/NetEvent/include/P2PTradeEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaPlayerListSelectEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaPlayerListSelectMultiEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaPlayerListShowEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaPlayerListSelectForScriptEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaPlayerDetailEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaOpenListEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaAttractionListSelectEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaAttractionResultEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaProfileEvent.h"
#include "NetStatic/NetEvent/include/GameSyncEvent.h"
#include "App/AppEvent/include/PokeTrade/PokeTradeEvent.h"
#include "NetStatic/NetEvent/include/FatalErrorEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaGlobalAttractionEvent.h"

#include <Heap/include/gfl2_Heap.h>
#include <System/include/HeapDefine.h>
#include <System/include/HeapSize.h>
#include <NetStatic/NetLib/include/Wifi/WifiConnectRunner.h>
#include <NetStatic/NetLib/include/NijiNetworkSystem.h>
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

// 項目文字列データテーブル
const u32 LauncherEndo::ItemList[] =
{
  EndoDebugMenuItem_CecTest,
  EndoDebugMenuItem_StlPerformanceCheck,
  EndoDebugMenuItem_QRCodeTest,
  EndoDebugMenuItem_CryptoTest,
  EndoDebugMenuItem_GTS,
  EndoDebugMenuItem_MiracleTrade,
  EndoDebugMenuItem_RegulationSelect,
  EndoDebugMenuItem_RegulationSelect2,
  EndoDebugMenuItem_RegulationSelect3,
  EndoDebugMenuItem_RegulationSelect4,
  EndoDebugMenuItem_P2PTradeTest,
  EndoDebugMenuItem_P2PTrade,
  EndoDebugMenuItem_Connection,
  EndoDebugMenuItem_SaveTest,
  EndoDebugMenuItem_JFPL,
  EndoDebugMenuItem_JFPL_SelectMulti,
  EndoDebugMenuItem_JFPL_Show,
  EndoDebugMenuItem_JFPL_SelectForScript,
  EndoDebugMenuItem_JFPL_Detail,
  EndoDebugMenuItem_JF_OpenList,
  EndoDebugMenuItem_JF_AttractionListSelect,
  EndoDebugMenuItem_JF_AttractionResult,
  EndoDebugMenuItem_JF_Profile,
  EndoDebugMenuItem_GameSync,
  EndoDebugMenuItem_PokeTrade,
  EndoDebugMenuItem_GlobalAttraction
};

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
LauncherEndo::LauncherEndo(void) :
  m_RegulationDrawInfo( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE ) )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
LauncherEndo::~LauncherEndo()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetArcDataID
 * @brief   アークデータID取得
 *
 * @return  アークデータID
 */
//-----------------------------------------------------------------------------
gfl2::fs::ArcFile::ARCDATID LauncherEndo::GetArcDataID(void)
{
  return GARC_debug_message_d_endo_DAT;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブル取得
 *
 * @return  文字列データテーブル
 */
//-----------------------------------------------------------------------------
const u32 * LauncherEndo::GetListTable(void)
{
  return ItemList;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブルの要素数取得
 *
 * @return  文字列データテーブルの要素数
 */
//-----------------------------------------------------------------------------
u32 LauncherEndo::GetListTableSize(void)
{
  return GFL_NELEMS( ItemList );
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectListener
 * @brief   決定時のリスナー
 *
 * @param   pos       カーソル位置 ( キャンセル時はCURSOR_POS_CANCEL )
 * @param   launcher  呼び出し元のランチャークラス
 *
 * @return  ランチャーへのリクエストID
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherEndo::SelectListener( u32 pos, launcher::LauncherProc * launcher )
{
  if( pos == launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<StartLauncher>();
    return launcher::ListData::REQID_RESET;
  }

  bool isInitializeWifiNetworkLibrary = true;

  switch( ItemList[pos] )
  {
    case EndoDebugMenuItem_CecTest:
    {
      NIJI_PROC_CALL< GameSys::GameProcManager, Test::CecTest::CecTestProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );

      return launcher::ListData::REQID_CALL;
    }

    case EndoDebugMenuItem_StlPerformanceCheck:
    {
      NIJI_PROC_CALL< GameSys::GameProcManager, Test::StlPerformanceCheck::StlPerformanceCheckProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );

      return launcher::ListData::REQID_CALL;
    }

    case EndoDebugMenuItem_QRCodeTest:
    {
      NIJI_PROC_CALL< GameSys::GameProcManager, Test::QRCodeTest::QRCodeTestProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );

      return launcher::ListData::REQID_CALL;
    }

    case EndoDebugMenuItem_CryptoTest:
    {
      NIJI_PROC_CALL< GameSys::GameProcManager, Test::CryptoTest::CryptoTestProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );

      return launcher::ListData::REQID_CALL;
    }

    case EndoDebugMenuItem_GTS:
    {
      isInitializeWifiNetworkLibrary = initializeWifiNetworkLibrary();
      if( isInitializeWifiNetworkLibrary )
      {
        NetEvent::GTS::GTSEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ) );
      }
    }
    break;

    case EndoDebugMenuItem_MiracleTrade:
    {
      isInitializeWifiNetworkLibrary = initializeWifiNetworkLibrary();
      if( isInitializeWifiNetworkLibrary )
      {
        NetEvent::MiracleTrade::MiracleTradeEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ) );
      }
    }
    break;

    case EndoDebugMenuItem_RegulationSelect:
    {
      isInitializeWifiNetworkLibrary = initializeWifiNetworkLibrary();
      if( isInitializeWifiNetworkLibrary )
      {
        NetApp::RegulationSelect::APP_PARAM appParam;
        appParam.bootMode = NetApp::RegulationSelect::BOOT_MODE_SELECT_P2P;
        appParam.battleRule = Regulation::BATTLE_RULE_SINGLE;
        NetEvent::RegulationSelect::RegulationSelectEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), appParam );
      }
    }
    break;

    case EndoDebugMenuItem_RegulationSelect2:
    {
      isInitializeWifiNetworkLibrary = initializeWifiNetworkLibrary();
      if( isInitializeWifiNetworkLibrary )
      {
        NetApp::RegulationSelect::APP_PARAM appParam;
        appParam.bootMode = NetApp::RegulationSelect::BOOT_MODE_SELECT_BATTLE_CENTER;
        appParam.battleRule = Regulation::BATTLE_RULE_SINGLE;
        appParam.bSingleNormalClearFlag = true;
        appParam.bDoubleNormalClearFlag = true;
        appParam.bMultiNormalClearFlag = false;
        NetEvent::RegulationSelect::RegulationSelectEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), appParam );
      }
    }
    break;

    case EndoDebugMenuItem_RegulationSelect3:
    {
      isInitializeWifiNetworkLibrary = initializeWifiNetworkLibrary();
      if( isInitializeWifiNetworkLibrary )
      {
        NetApp::RegulationSelect::APP_PARAM appParam;
        appParam.bootMode = NetApp::RegulationSelect::BOOT_MODE_SELECT_BATTLE_ROYAL;
        appParam.battleRule = Regulation::BATTLE_RULE_ROYAL;
        appParam.bBattleRoyalNormalClearFlag = true;
        appParam.bBattleRoyalSuperClearFlag = true;
        appParam.bBattleRoyalHyperClearFlag = false;
        NetEvent::RegulationSelect::RegulationSelectEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), appParam );
      }
    }
    break;

    case EndoDebugMenuItem_RegulationSelect4:
    {
      isInitializeWifiNetworkLibrary = initializeWifiNetworkLibrary();
      if( isInitializeWifiNetworkLibrary )
      {
        NetApp::RegulationSelect::APP_PARAM appParam;
        appParam.bootMode = NetApp::RegulationSelect::BOOT_MODE_DOWNLOAD_P2P;
        NetEvent::RegulationSelect::RegulationSelectEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), appParam );
      }
    }
    break;

    case EndoDebugMenuItem_P2PTradeTest:
    {
      NIJI_PROC_CALL< GameSys::GameProcManager, Test::P2PTradeTest::P2PTradeTestProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );

      return launcher::ListData::REQID_CALL;
    }
    break;

    case EndoDebugMenuItem_P2PTrade:
    {
      if( initializeLocalNetworkLibrary() )
      {
        NetEvent::P2PTrade::P2PTradeEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ) );
      }

      return launcher::ListData::REQID_CALL;
    }
    break;

    case EndoDebugMenuItem_Connection:
    {
#if defined(GF_PLATFORM_CTR)

#if 0

      NetApp::Connection::APP_PARAM appParam;
      if( NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_OFFLINE )
      {
//        appParam.bootMode = NetApp::Connection::BOOT_MODE_INTERNET_CONNECTION;
        appParam.bootMode = NetApp::Connection::BOOT_MODE_CHANGE_INTERNET_MODE;
      }
      else
      {
//        appParam.bootMode = NetApp::Connection::BOOT_MODE_INTERNET_DISCONNECTION;
        appParam.bootMode = NetApp::Connection::BOOT_MODE_CHANGE_LOCAL_MODE;
      }
      NetEvent::Connection::ConnectionEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), appParam );

#else

      NetEvent::FatalError::EventParam eventParam;
      eventParam.eFatalErrorNo = NetEvent::FatalError::FATAL_ERROR_NO_SD_CARD_NOT_RECOGNIZED;
      NetEvent::FatalError::FatalErrorEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), eventParam );

#endif

#endif // defined(GF_PLATFORM_CTR)
    }
    break;

    case EndoDebugMenuItem_SaveTest:
    {
      NIJI_PROC_CALL< GameSys::GameProcManager, Test::SaveTest::SaveTestProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );

      return launcher::ListData::REQID_CALL;
    }
    break;

    case EndoDebugMenuItem_JFPL:
    {
      NetApp::JoinFestaPlayerListSelect::APP_PARAM appParam;
      NetEvent::JoinFestaPlayerListSelect::JoinFestaPlayerListSelectEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), appParam, NULL );
    }
    break;

    case EndoDebugMenuItem_JFPL_SelectMulti:
    {
      NetApp::JoinFestaPlayerListSelectMulti::APP_PARAM appParam;
      NetEvent::JoinFestaPlayerListSelectMulti::JoinFestaPlayerListSelectMultiEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), appParam, NULL );
    }
    break;

    case EndoDebugMenuItem_JFPL_Show:
    {
      NetEvent::JoinFestaPlayerListShow::JoinFestaPlayerListShowEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ) );
    }
    break;

    case EndoDebugMenuItem_JFPL_SelectForScript:
    {
      NetApp::JoinFestaPlayerListSelectForScript::APP_PARAM appParam;
      NetEvent::JoinFestaPlayerListSelectForScript::JoinFestaPlayerListSelectForScriptEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), appParam, NULL );
    }
    break;

    case EndoDebugMenuItem_JFPL_Detail:
    {
      NetApp::JoinFestaPlayerDetail::APP_PARAM appParam;
      {
        NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData coreData;
        gfl2::str::StrNCopy( coreData.m_personalInfo.name, L"dummy", 5 );
        appParam.playerData.Copy( coreData );
      }
      NetEvent::JoinFestaPlayerDetail::JoinFestaPlayerDetailEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), appParam );
    }
    break;

    case EndoDebugMenuItem_JF_OpenList:
    {
      NetEvent::JoinFestaOpenList::JoinFestaOpenListEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), NULL );
    }
    break;

    case EndoDebugMenuItem_JF_AttractionListSelect:
    {
      NetEvent::JoinFestaAttractionListSelect::JoinFestaAttractionListSelectEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), NULL );
    }
    break;

    case EndoDebugMenuItem_JF_AttractionResult:
    {
      NetApp::JoinFestaAttractionResult::APP_PARAM appParam;
      NetEvent::JoinFestaAttractionResult::JoinFestaAttractionResultEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), appParam );
    }
    break;

    case EndoDebugMenuItem_JF_Profile:
    {
      NetEvent::JoinFestaProfile::JoinFestaProfileEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ) );
    }
    break;

    case EndoDebugMenuItem_GameSync:
    {
      NetEvent::GameSync::GameSyncEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ) );
    }
    break;

    case EndoDebugMenuItem_PokeTrade:
    {
      gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      pml::pokepara::PokemonParam pokemonParam( pHeap, static_cast<MonsNo>( MONSNO_001 ), static_cast<u16>( 3 ), static_cast<u64>( 33 ) );

      App::Event::PokeTradeEventParam param;
      param.pRecvPokemon = &pokemonParam;
      param.tray = 0;
      param.pos = 0;
      gfl2::str::StrNCopy( param.npcName, L"npc", 3 );
      App::Event::PokeTradeEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ), &param );
    }
    break;

    case EndoDebugMenuItem_GlobalAttraction:
    {
      isInitializeWifiNetworkLibrary = initializeWifiNetworkLibrary();
      if( isInitializeWifiNetworkLibrary )
      {
        NetEvent::JoinFestaGlobalAttraction::JoinFestaGlobalAttractionEvent::StartEvent( GFL_SINGLETON_INSTANCE( GameSys::GameManager ) );
      }
    }
    break;
  }

  if( isInitializeWifiNetworkLibrary == false )
  {
#if defined(GF_PLATFORM_CTR)
    NetLib::NijiNetworkSystem::TerminateNetwork();
#endif
    launcher->CreateListData<StartLauncher>();
    return launcher::ListData::REQID_RESET;
  }

  return launcher::ListData::REQID_NONE;
}


bool LauncherEndo::initializeWifiNetworkLibrary()
{
#if defined(GF_PLATFORM_CTR)
  gfl2::heap::HeapBase*  heapDev  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  NetLib::NijiNetworkSystem::InitializeWifiNetwork();

  NetLib::Wifi::WifiConnectRunner::E_EXEC_MODE mode = NetLib::Wifi::WifiConnectRunner::E_MODE_CONNECT_AND_CREATE_NEXID;
  NetLib::Wifi::WifiConnectRunner* pWifiConnect = GFL_NEW( heapDev ) NetLib::Wifi::WifiConnectRunner( mode );

  NetLib::Wifi::WifiConnectRunner::E_RET_CODE ret;

  while(true)
  {
    ret = pWifiConnect->Update();
    if( ret != NetLib::Wifi::WifiConnectRunner::E_RET_CONTINUE )
    {
      break;
    }
    GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
    gfl2::thread::ThreadStatic::CurrentSleep(1);
  }

  GFL_SAFE_DELETE( pWifiConnect );

  if( ret == NetLib::Wifi::WifiConnectRunner::E_RET_SUCCESS )
  {
    return true;
  }

#endif // defined(GF_PLATFORM_CTR)

  return false;
}


bool LauncherEndo::initializeLocalNetworkLibrary()
{
#if defined(GF_PLATFORM_CTR)

  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
 
  NetLib::NijiNetworkSystem::InitializeLocalP2PNetwork();

  pP2pConnectionManager->Initialize( NetLib::P2P::NIJI_P2P_TRADE_L );

  while(true)
  {
    pP2pConnectionManager->Update();
    if( pP2pConnectionManager->ConnectStart( gflnet2::p2p::AUTO ) )
    {
      break;
    }
    GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
    gfl2::thread::ThreadStatic::CurrentSleep(1);
  }

  while(true)
  {
    pP2pConnectionManager->Update();
    if( pP2pConnectionManager->IsConnectSuccess() )
    {
      break;
    }
    GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
    gfl2::thread::ThreadStatic::CurrentSleep(1);
  }

  return true;

#endif // defined(GF_PLATFORM_CTR)

  return false;
}


GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif  // PM_DEBUG
