#if PM_DEBUG
//=============================================================================
/**
 * @file    DebugBattleProc.cpp
 * @brief   デバッグバトルProc
 * @author  obata_toshihiro
 * @data    2015.2.25
 * @note    sango_projectから移植
 */
//=============================================================================
#include <debug/include/gfl2_DebugPrint.h>
#include <fade/include/gfl2_FadeManager.h>

#include <System/include/HeapDefine.h>
#include <System/include/GflUse.h>
#include <System/include/PokemonVersion.h>
#include <Savedata/include/MyStatus.h>
#include <Savedata/include/MiscSave.h>
#include <Savedata/include/ConfigSave.h>
#include <ExtSavedata/include/BattleRecorderSaveData.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/DllProc.h>
#include <Battle/include/battle_proc.h>
#include <Battle/Background/include/BgSetupUtil.h>
#include <Battle/Regulation/include/Regulation.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_AppLytAccessor.h>
#include <Sound/include/Sound.h>
#include <Debug/DebugBattle/include/DebugBattleProc.h>


#include <arc_def_index/arc_def.h>
#include <arc_index/debug_battle.gaix>
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_battle.h>
#include <niji_conv_header/app/debug/debug_battle/res2d/debug_battle.h>
#include <niji_conv_header/app/debug/debug_battle/res2d/debug_battle_pane.h>

//通信用
#ifdef  GF_PLATFORM_CTR
#include <NetStatic/NetLib/include/NijiNetworkSystem.h>
#include <NetStatic/NetLib/include/Wifi/WifiConnectRunner.h>
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>
#endif

// フェスファン名用
#include <arc_index/message.gaix>

namespace
{
  static const u32 HEAP_SIZE_SYSTEM = 0x200000;
  static const u32 HEAP_SIZE_DEVICE = 0x100000;

  static const u32 BGM_TABLE[] = 
  {
    STRM_BGM_VS_NORAPOKE_NJ,
    STRM_BGM_VS_NORAPOKE_02,
    STRM_BGM_VS_TRAINER_NJ,
    STRM_BGM_VS_GIVER,
    STRM_BGM_VS_FRIEND_NJ,
    STRM_BGM_VS_RIVAL_NJ,
    STRM_BGM_VS_SKULL,
    STRM_BGM_VS_SKULLELITE,
    STRM_BGM_VS_SKULLBOSS,
    STRM_BGM_VS_ETHER,
    STRM_BGM_VS_MOTHER,
    STRM_BGM_VS_MOTHERBEAST,
    STRM_BGM_VS_BEAST,
    STRM_BGM_VS_SHITENNO_NJ,
    STRM_BGM_VS_CHAMP_NJ,
    STRM_BGM_VS_LEGEND01_NJ,
    STRM_BGM_VS_LEGEND02_NJ,
    STRM_BGM_VS_REDGREEN,
    STRM_BGM_VS_BROYAL,
    STRM_BGM_VS_SECRET_01,
    // momiji追加分 mstextと対応している
    STRM_BGM_VS_TRAINER_MJ,
    STRM_BGM_MJ_M02,
    STRM_BGM_MJ_VS07,
    STRM_BGM_MJ_VS08,
    STRM_BGM_MJ_VS09,
    STRM_BGM_GAME_SYNC,
    STRM_BGM_MJ_VS01,
    STRM_BGM_MJ_VS02,
    STRM_BGM_MJ_VS03,
    STRM_BGM_MJ_VS04,
    STRM_BGM_MJ_VS05,
    STRM_BGM_MJ_VS06,
    STRM_BGM_MJ_VSDIVEZONE02,
    STRM_BGM_MJ_VSDIVEZONE01,
    STRM_BGM_MJ_VSDIVEZONE08,
    STRM_BGM_MJ_VSDIVEZONE06,
    STRM_BGM_MJ_VSDIVEZONE07,
    STRM_BGM_MJ_VSDIVEZONE03,
    STRM_BGM_MJ_VSDIVEZONE05,
    STRM_BGM_MJ_VSDIVEZONE04,
    STRM_BGM_MJ_VSDIVEZONE11,
    STRM_BGM_MJ_VSDIVEZONE09,
    STRM_BGM_MJ_VSDIVEZONE10,
    STRM_BGM_MJ_VSDIVEZONE12,
    STRM_BGM_MJ_VSDIVEZONE13,
    STRM_BGM_MJ_VSDIVEZONE14,
    STRM_BGM_MJ_VSDIVEZONE16,
  };

}

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(battle)

//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
DebugBattleProc::DebugBattleProc( void ) :
  m_sysHeapBase( NULL ),
  m_devHeapBase( NULL ),
  m_sysHeap( NULL ),
  m_devHeap( NULL ),
  m_seq(0),
  m_sub_seq(0),
  m_page(0),
  m_pointItem( ITEM_P1_1 ),
  m_currentItemId( ITEM_MAX ),
  m_prevsave_misc( NULL )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
DebugBattleProc::~DebugBattleProc( )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief パラメータセットアップ
 * @brief heapSys  使用するシステムヒープ
 * @brief heapDev  使用するデバイスヒープ
 */
//-----------------------------------------------------------------------------
void DebugBattleProc::SetupParam( gfl2::heap::HeapBase* heapSys, gfl2::heap::HeapBase* heapDev )
{
  m_sysHeapBase = heapSys;
  m_devHeapBase = heapDev;
}

//-----------------------------------------------------------------------------
/**
  * @brief   PROCの初期化を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
  */
//-----------------------------------------------------------------------------
gfl2::proc::Result DebugBattleProc::InitFunc( gfl2::proc::Manager* pManager )
{
  if( ( m_sysHeapBase == NULL ) ||
      ( m_devHeapBase == NULL ) ) {
    GFL_ASSERT(0);
  }

  switch( m_seq )
  {
  case 0:
    this->initialize();
    if( m_uiButton->IsHold( gfl2::ui::BUTTON_L ) )
    {
      ++m_seq;
      return gfl2::proc::RES_CONTINUE;
    }
    else
    {
      break;
    }

  case 1:
    {
      bool  bSucceed = false;
      if( loadingProc( &m_sub_seq, &bSucceed ) )
      {
        if( bSucceed ) {
          this->LoadAllItems();
          this->redrawCurrentPageItems();
        }
        break;
      }
      else
      {
        return gfl2::proc::RES_CONTINUE;
      }
    }
  }

  m_seq = 0;
  m_sub_seq = 0;
  m_isDrawEnable = true;
  return gfl2::proc::RES_FINISH;
}

/**
 * @brief 初期化処理
 */
void DebugBattleProc::initialize( void )
{
  this->CreateHeap();

  u32 totalSize = m_devHeap->GetTotalAllocatableSize();

  this->SetupUiSystem();
  this->SetupPokemonContainer();
  
  m_saveData  = GFL_NEW(m_sysHeap) DebugBattleSaveData(m_sysHeap);
  m_strData   = GFL_NEW(m_sysHeap) gfl2::str::MsgData( print::GetMessageArcId_Debug(), GARC_debug_message_d_battle_DAT, m_sysHeap, gfl2::str::MsgData::LOAD_FULL );
  m_strBuf    = GFL_NEW(m_sysHeap) gfl2::str::StrBuf( 512, m_sysHeap );
  m_strBufTmp = GFL_NEW(m_sysHeap) gfl2::str::StrBuf( 512, m_sysHeap );
  m_wordSet   = GFL_NEW(m_sysHeap) print::WordSet( m_sysHeap );

  // ゲームデータをメンバーとして保持しておく（便利のため）
  m_gameData = this->GetGameManager()->GetGameData();

  {
    Savedata::Misc* misc = m_gameData->GetMisc();
    u32 misc_size = misc->GetDataSize();
    m_prevsave_misc = GflHeapAllocMemory( m_sysHeap, misc_size );
  }

#if 0
  m_netAutoMatch = NULL;
  m_netHandle    = NULL;
  m_bLoaded = false;
#endif

  this->initGraphicSystem();
  this->CreateRenderingPipeLine( m_sysHeap, m_devHeap );
  this->init2D();

  this->LoadAllItems();
  this->redrawCurrentPageItems();

  u32 curSize = m_devHeap->GetTotalAllocatableSize();
  GFL_PRINT("[DebugBtl] used %08x bytes device Heap!!\n", totalSize - curSize);

  //フィールドと条件を揃えるためにBGMを鳴らしておく by iwasawa
  Sound::StartBGMReq( STRM_BGM_T01 );

  this->StartFadeIn();
}


/**
 * @brief デバッグバトルで使用するヒープを作成する
 */
void DebugBattleProc::CreateHeap( void )
{
  if( m_sysHeap == NULL ) {
    m_sysHeap = GFL_CREATE_HEAP( m_sysHeapBase, HEAPID_DEBUG_BATTLE, HEAP_SIZE_SYSTEM, gfl2::heap::HEAP_TYPE_EXP, false );
  }

  this->CreateDeviceHeap();
}

/**
 * @brief デバイスヒープを作成する
 */
void DebugBattleProc::CreateDeviceHeap( void )
{
  if( m_devHeap == NULL ) {
    m_devHeap = GFL_CREATE_HEAP( m_devHeapBase->GetLowerHandle(), HEAPID_DEBUG_BATTLE_DEVICE, HEAP_SIZE_DEVICE,  gfl2::heap::HEAP_TYPE_EXP, false ); // バトルプロセスを呼ぶ前に削除するので、後方確保する
  }
}


/**
 * @brief Uiシステムをセットアップする
 */
void DebugBattleProc::SetupUiSystem( void )
{
  m_uiMan = this->GetGameManager()->GetUiDeviceManager();
  m_uiButton = m_uiMan->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  m_uiKey = m_uiMan->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STANDARD );
  m_uiTp = m_uiMan->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
}


/**
 * @brief ゲームデータをセットアップする
 */
void DebugBattleProc::SetupGameData( void )
{
  if( m_itemValue[ ITEM_SW_SAVE_DONT_CLEAR ] )
  {
    return;
  }

  // バトルデバッグを便利にするため、全アイテムを100個ずつ持たせる
  m_gameData->SetDebugBattleData( m_sysHeap, static_cast<bool>( m_itemValue[ ITEM_SW_ZUKAN_DONT_CLEAR ] ) );

  // 主人公の名前を設定
  Savedata::MyStatus* myStatus = m_gameData->GetPlayerStatus();
  myStatus->SetDebugName( false );
}


/**
 * @brief ポケモンデータを保持しておくコンテナをセットアップする
 */
void DebugBattleProc::SetupPokemonContainer( void )
{
  m_pokeData        = GFL_NEW( m_sysHeap ) pml::pokepara::PokemonParam( m_sysHeap, MONSNO_NULL, 1, 0 );
  m_clipPokeData    = GFL_NEW( m_sysHeap ) pml::pokepara::PokemonParam( m_sysHeap, MONSNO_NULL, 1, 0 );
  m_tmpPokeData     = GFL_NEW( m_sysHeap ) pml::pokepara::PokemonParam( m_sysHeap, MONSNO_NULL, 1, 0 );
  m_intrudePokeData = GFL_NEW( m_sysHeap ) pml::pokepara::PokemonParam( m_sysHeap, MONSNO_NULL, 1, 0 );
  for( u32 i=0; i<GFL_NELEMS(m_reinforcePokeData); ++i )
  {
    m_reinforcePokeData[i] = GFL_NEW( m_sysHeap ) pml::pokepara::PokemonParam( m_sysHeap, MONSNO_NULL, 1, 0 );
  }

  for( u32 i=0; i<GFL_NELEMS(m_party); ++i ) {
    m_party[i] = GFL_NEW(m_sysHeap) pml::PokeParty( m_sysHeap );
  }
  m_playerParty = this->GetGameManager()->GetGameData()->GetPlayerParty();
}

/**
 * @biref 全項目をロードする
 */
void DebugBattleProc::LoadAllItems( void )
{
  for(int i=0; i<ITEM_MAX; ++i){
    this->loadItem( (DBItemID)i );
  }
}



//---------------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//---------------------------------------------------------------------------
gfl2::proc::Result DebugBattleProc::EndFunc( gfl2::proc::Manager* )
{
  // 描画中は解放しない
  if( ( m_lytWk != NULL ) &&
      ( m_lytWk->IsDrawing() ) ) {
    return gfl2::proc::RES_CONTINUE;
  }

  this->quit2D();
  this->quitGraphicSystem();
  this->DeleteRenderingPipeLine();

  GflHeapSafeFreeMemory( m_prevsave_misc );

  GFL_SAFE_DELETE( m_wordSet );
  GFL_SAFE_DELETE( m_strBufTmp );
  GFL_SAFE_DELETE( m_strBuf );
  GFL_SAFE_DELETE( m_strData );
  GFL_SAFE_DELETE( m_saveData );

  for(u32 i=0; i<GFL_NELEMS(m_party); ++i) {
    GFL_SAFE_DELETE( m_party[i] );
  }

  for( u32 i=0; i<GFL_NELEMS(m_reinforcePokeData); ++i )
  {
    GFL_SAFE_DELETE( m_reinforcePokeData[i] );
  }
  GFL_SAFE_DELETE( m_intrudePokeData );
  GFL_SAFE_DELETE( m_tmpPokeData );
  GFL_SAFE_DELETE( m_clipPokeData );
  GFL_DELETE( m_pokeData );

  this->DeleteHeap();

  Sound::EndBGMReq();

  return gfl2::proc::RES_FINISH;
}


/**
 * @brief ヒープを削除する
 */
void DebugBattleProc::DeleteHeap( void )
{
  this->DeleteDeviceHeap();
  gfl2::heap::Manager::DeleteHeap( m_sysHeap );
}

/**
 * @brief デバイスヒープを削除する
 */
void DebugBattleProc::DeleteDeviceHeap( void )
{
  gfl2::heap::Manager::DeleteHeap( m_devHeap );
  m_devHeap = NULL;
}





//---------------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//---------------------------------------------------------------------------
gfl2::proc::Result DebugBattleProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  enum {
    SEQ_ROOT,
    SEQ_TO_POKEMAKE_START,
    SEQ_TO_POKEMAKE,
    SEQ_RET_POKEMAKE,
    SEQ_RET_POKEMAKE_WAIT,
    SEQ_BATTLE_COMM_START,
    SEQ_BATTLE_COMM_WAIT,
    SEQ_TO_BATTLE_START,
    SEQ_TO_BATTLE,
    SEQ_BATTLE_COMM_END,
    SEQ_BATTLE_COMM_END2,
    SEQ_BATTLE_COMM_END3,
    SEQ_BATTLE_COMM_END_WAIT,
    SEQ_RET_BATTLE,
    SEQ_RET_BATTLE_FADE,
    SEQ_SAVING,
    SEQ_LOADING,
    SEQ_LOAD_FAILED,
    SEQ_CHANGE_PAGE_NEXT,
    SEQ_CHANGE_PAGE_PREV,
    SEQ_QUIT,
  };


  switch( m_seq )
  {
  case SEQ_ROOT:
    {
      CtrlResult result = this->keyControl();
      switch( result )
      {
      case CTRL_RESULT_QUIT:
        m_seq = SEQ_QUIT;
        return gfl2::proc::RES_CONTINUE;

      case CTRL_RESULT_PAGE_PREV:
        m_seq = SEQ_CHANGE_PAGE_PREV;
        return gfl2::proc::RES_CONTINUE;

      case CTRL_RESULT_PAGE_NEXT:
        m_seq = SEQ_CHANGE_PAGE_NEXT;
        return gfl2::proc::RES_CONTINUE;

      case CTRL_RESULT_SAVE:
        m_sub_seq = 0;
        m_seq = SEQ_SAVING;
        return gfl2::proc::RES_CONTINUE;

      case CTRL_RESULT_LOAD:
      case CTRL_RESULT_LOAD_AND_RUN:
        m_sub_seq = 0;
        m_seq = SEQ_LOADING;
        return gfl2::proc::RES_CONTINUE;

      case CTRL_RESULT_RESET_LANG:
        resetLanguage();
        Sound::PlaySE( SEQ_SE_DECIDE1 );
        return gfl2::proc::RES_CONTINUE;

      case CTRL_RESULT_POKE_CUT:
      case CTRL_RESULT_POKE_COPY:
      case CTRL_RESULT_POKE_PASTE:
        this->clipDataCtrl( result );
        return gfl2::proc::RES_CONTINUE;

      case CTRL_RESULT_POKE_CREATE:
        m_seq = SEQ_TO_POKEMAKE_START;
        return gfl2::proc::RES_CONTINUE;

      case CTRL_RESULT_BATTLE:
        if( this->isBattleCommMode() )
        {
#ifdef  GF_PLATFORM_CTR

          if( m_itemValue[ITEM_BTLMODE] == ROYAL_COMM )
          {//ロイヤルはWifi
            NetLib::NijiNetworkSystem::InitializeWifiNetwork();
            {
              gfl2::heap::HeapBase*  heapDev  = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
              NetLib::Wifi::WifiConnectRunner::E_EXEC_MODE mode = NetLib::Wifi::WifiConnectRunner::E_MODE_CONNECT_AND_CREATE_NEXID;
              NetLib::Wifi::WifiConnectRunner* pWifiConnect = GFL_NEW( heapDev ) NetLib::Wifi::WifiConnectRunner( mode );

              while(true)
              {
                NetLib::Wifi::WifiConnectRunner::E_RET_CODE ret = pWifiConnect->Update();
                if( ret != NetLib::Wifi::WifiConnectRunner::E_RET_CONTINUE )
                {
                  break;
                }
                GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->Dispatch();
                gfl2::thread::ThreadStatic::CurrentSleep(1);
              }

              GFL_SAFE_DELETE( pWifiConnect );
            }
          }
          else
          {//ロイヤル以外はローカル
            NetLib::NijiNetworkSystem::InitializeLocalP2PNetwork();
          }

          NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

          if( getCommConnectPlayerNum() == 2 )
          {
            pP2pConnectionManager->Initialize( NetLib::P2P::NIJI_P2P_JF_QUICK_2 );
          }
          else
          {
            pP2pConnectionManager->Initialize( NetLib::P2P::NIJI_P2P_RNDFREE_B );
          }

          if( m_itemValue[ITEM_BTLMODE] == ROYAL_COMM )
          {//ロイヤルはWifi
            // WiFiマッチング用条件
            gflnet2::nex::NexMatchMakingClient::NexMatchMakingRuleList list;
            gflnet2::nex::NexMatchMakingRule rule1( 0, 111, 111, 111 );
            gflnet2::nex::NexMatchMakingRule rule2( 1, 222, 222, 222 );
            list.push_back( &rule1 );
            list.push_back( &rule2 );
            pP2pConnectionManager->ConnectStartWifi( gflnet2::p2p::AUTO , &list );
          }
          else
          {//ロイヤル以外はローカル
            pP2pConnectionManager->ConnectStart( gflnet2::p2p::AUTO );
          }
          m_seq = SEQ_BATTLE_COMM_START;
#endif
        }
        else
        {
          m_seq = SEQ_TO_BATTLE_START;
        }
        return gfl2::proc::RES_CONTINUE;
      }
    }
    break;

   // バトル通信マッチ
  case SEQ_BATTLE_COMM_START:
    {
#ifdef  GF_PLATFORM_CTR
      NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
      if( pP2pConnectionManager->IsConnectSuccess() )
      {
        GFL_PRINT("[DEBUG_BTL] AutoMatch...接続しました\n");
        gfl2::heap::HeapBase*  pSysHeap    = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
        m_netHandle = GFL_NEW(pSysHeap) gflnet2::p2p::NetGame();

        m_seq = SEQ_BATTLE_COMM_WAIT;
      }
#endif
    }
    break;

  case SEQ_BATTLE_COMM_WAIT:
#ifdef  GF_PLATFORM_CTR
    if( m_netHandle->GetConnectionCount() == getCommConnectPlayerNum() )
    {
      GFL_PRINT("[DEBUG_BTL] %d 人の接続を確認しました\n", getCommConnectPlayerNum());
      GFL_PRINT("[DEBUG_BTL] 自分のID %d\n" , m_netHandle->GetNetID() );
      //ホスト等の設定
      m_itemValue[ITEM_COMMMODE] = m_netHandle->GetNetID();

      m_seq = SEQ_TO_BATTLE_START;
    }
#endif
    break;


  case SEQ_TO_BATTLE_START:
    m_isDrawEnable = false;
    m_seq = SEQ_TO_BATTLE;
    break;

  case SEQ_TO_BATTLE:
    if( !( m_lytWk->IsDrawing() ) )
    {
      this->quit2D();
      this->quitGraphicSystem();
      this->DeleteRenderingPipeLine();
      this->DeleteDeviceHeap();
      {
        this->SetupGameData();
        BtlCompetitor competitor = this->setupBtlSetupParam();
        this->StartBattleBgm( competitor );
        this->CallBattleProc( pProcManager );

        if( this->isBattleCommMode() )
        {
          m_seq = SEQ_BATTLE_COMM_END;
        }
        else
        {
          m_seq = SEQ_RET_BATTLE;
        }
      }
    }
    break;

  case SEQ_BATTLE_COMM_END:
#ifdef  GF_PLATFORM_CTR
    GFL_PRINT("[DBGBTL]バトルから復帰\n");
    if( m_netHandle ){
      GFL_DELETE( m_netHandle );
      m_netHandle = NULL;
      GFL_PRINT("[DBGBTL]NetHandle削除しました\n");
    }
    {
      gfl2::heap::HeapBase*  pSysHeap    = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_netGame = GFL_NEW(pSysHeap) gflnet2::p2p::NetGame();
      GFL_PRINT("[DEBUG_BTL] 接続を待ちます\n");
      m_netGame->Initialize();

      m_seq = SEQ_BATTLE_COMM_END2;
    }
#endif
    break;

  case SEQ_BATTLE_COMM_END2:
#ifdef  GF_PLATFORM_CTR
    if(m_netGame->IsInitialize())
    {
      m_netGame->TimingStart(123);
      m_seq = SEQ_BATTLE_COMM_END3;
      mDisconnect = 30*30;
    }
#endif
    break;

  case SEQ_BATTLE_COMM_END3:
#ifdef  GF_PLATFORM_CTR
    mDisconnect--;
    if(m_netGame->IsTimingEnd(123) || (mDisconnect < 0)){
      m_netGame->Finalize();
      NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

      pP2pConnectionManager->DisconnectStart();
      GFL_PRINT("[DBGBTL]AutoMatch...終了準備開始\n");
      m_seq = SEQ_BATTLE_COMM_END_WAIT;
    }
#endif
    break;

  case SEQ_BATTLE_COMM_END_WAIT:
    {
#ifdef  GF_PLATFORM_CTR
      NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);

      if( pP2pConnectionManager->IsDisconnectSuccess() )
      {
        GFL_PRINT("[DBGBTL]AutMatch終了しました\n");

        if( m_netGame ){
          GFL_DELETE( m_netGame );
          m_netGame = NULL;
          GFL_PRINT("[DBGBTL]NetHandle削除しました\n");
        }

        pP2pConnectionManager->Terminate();
        GFL_PRINT("[DBGBTL] オートマッチ破棄します\n");

        NetLib::Wifi::ForceDisconnectWifi();

        NetLib::NijiNetworkSystem::TerminateNetwork();
        m_seq = SEQ_RET_BATTLE;
      }
#endif
    }

    break;

  case SEQ_RET_BATTLE:
    if( m_itemValue[ITEM_RECMODE] == REC_REC ){
      this->backupBattleRecordData( m_itemValue[ITEM_RECIDX] );
    }
    this->recoverGraphicSystems();
    this->redrawCurrentPageItems();
    this->cleanupBtlSetupParam();
    this->EndBattleBgm();
    this->StartFadeIn();
    m_seq = SEQ_RET_BATTLE_FADE;
    m_isDrawEnable = true;
    break;

  case SEQ_RET_BATTLE_FADE:
    if( this->IsFadeFinished() ) {
      m_seq = 0;
    }
    break;


  case SEQ_TO_POKEMAKE_START:
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestDefaultOut( gfl2::Fade::DISP_DOUBLE );
    m_isDrawEnable = false;
    m_seq = SEQ_TO_POKEMAKE;
    break;

  case SEQ_TO_POKEMAKE:
    if( !( m_lytWk->IsDrawing() ) &&
        GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsEnd(gfl2::Fade::DISP_DOUBLE) != false )
    {
      this->quit2D();
      this->quitGraphicSystem();
      this->DeleteRenderingPipeLine();
      this->DeleteDeviceHeap();
      this->CallPokeMakeProc( pProcManager );
      m_seq = SEQ_RET_POKEMAKE;
    }
    break;

  case SEQ_RET_POKEMAKE:
    m_isDrawEnable = true;
    this->StoreMakePokeWithSaveData();
    this->recoverGraphicSystems();
    this->redrawCurrentPageItems();
    this->setSelectPane( m_pointItem );

    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
    m_seq = SEQ_RET_POKEMAKE_WAIT;
    break;

  case SEQ_RET_POKEMAKE_WAIT:
    if( GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsEnd(gfl2::Fade::DISP_DOUBLE) != false ){
      m_seq = 0;
    }    
    break;


  case SEQ_SAVING:
    if( savingProc( &m_sub_seq ) )
    {
      m_seq = SEQ_ROOT;
    }
    break;

  case SEQ_LOADING:
    {
      bool  bSucceed = false;
      if( loadingProc( &m_sub_seq, &bSucceed ) )
      {
        if( bSucceed )
        {
          this->LoadAllItems();
          this->redrawCurrentPageItems();
        }
        m_bLoaded = true;
        m_seq = SEQ_ROOT;
      }
    }
    break;


  case SEQ_CHANGE_PAGE_NEXT:
    m_isDrawEnable = false;
    if( !( m_lytWk->IsDrawing() ) )
    {
      this->ChangePage( 1 );
      m_isDrawEnable = true;
      m_seq = SEQ_ROOT;
    }
    break;

  case SEQ_CHANGE_PAGE_PREV:
    m_isDrawEnable = false;
    if( !( m_lytWk->IsDrawing() ) )
    {
      this->ChangePage( -1 );
      m_isDrawEnable = true;
      m_seq = SEQ_ROOT;
    }
    break;

  case SEQ_QUIT:
    m_isDrawEnable = false;
    if( !( m_lytWk->IsDrawing() ) )
    {
      return gfl2::proc::RES_FINISH;
    }
    break;

  }

  if( m_isDrawEnable )
  {
    if( m_lytSys ){
      m_lytSys->UpdateAll();
    }
    if( m_lytWk ) {
      m_lytWk->Update();
    }
    if( m_renderingPipeLine ) {
      m_renderingPipeLine->Update();
      m_renderingPipeLine->GetLayoutRenderPath()->ClearDrawableLytWk();
      m_renderingPipeLine->GetLayoutRenderPath()->AddDrawableLytWk( m_lytWk, gfl2::lyt::DISPLAY_UPPER );
    }
  }


  return gfl2::proc::RES_CONTINUE;
}

/**
 * @brief デバッグ設定に対して、をロムの言語設定を反映させる
 */
void DebugBattleProc::resetLanguage( void )
{
  for( u32 i=0; i<POKEPARAM_NUM; ++i )
  {
    m_saveData->GetPokeParam( m_pokeData, i );
    resetPokeLanguage( m_pokeData );
    m_saveData->SetPokeParam( m_pokeData, i );
  }
}

/**
 * @brief 指定したポケモンを、現在のロムの言語設定に変更する
 * @param[in,out] 操作対象のパラメータ
 */
void DebugBattleProc::resetPokeLanguage( pml::pokepara::PokemonParam* poke )
{
  poke->SetDefaultNickName();
  poke->SetLangId( System::GetLang() );
}

/**
 * @brief 戦闘BGMの再生を開始する
 * @param competitor  対戦相手
 */
void DebugBattleProc::StartBattleBgm( BtlCompetitor competitor )
{
  u32 bgmNo = ( competitor == BTL_COMPETITOR_WILD )? STRM_BGM_VS_NORAPOKE_NJ : STRM_BGM_VS_TRAINER_NJ;

  {
    u32 bgmIndex = m_itemValue[ ITEM_BGM ];
    if( bgmIndex < GFL_NELEMS(BGM_TABLE) )
    {
      bgmNo = BGM_TABLE[ bgmIndex ];
    }
  }

  GFL_PRINT("[DEBUG_BTL] バトル用BGM開始 stat=%d ->", Sound::GetPushedBGMCount() );
  Sound::StartBattleBGMReq( bgmNo );
  GFL_PRINT("%d\n", Sound::GetPushedBGMCount() );
}

/**
 * @brief 戦闘BGMを終了する
 */
void DebugBattleProc::EndBattleBgm()
{
  Sound::EndBattleBGMReq();
}

void DebugBattleProc::StartFadeIn()
{
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA);
}

/**
 * @brief フェードが終了したか？
 * @retval true  終了した
 * @retval false  終了していない
 */
bool DebugBattleProc::IsFadeFinished( void ) const
{
  return GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd(gfl2::Fade::DISP_UPPER);
}


/**
 * @brief デバッグポケモン作成プロセスコールする
 */
void DebugBattleProc::CallPokeMakeProc( gfl2::proc::Manager* procManager )
{
  if( m_pokeData->GetLangId() == POKEMON_LANG_NULL )
  {
    m_pokeData->SetLangId(System::GetLang());
  }
  m_debugPokeParam.poke = m_pokeData;
  m_debugPokeParam.defaultPowerUpDesc = &m_defaultPowerUpDesc;
  GameSys::CallDebugPokeMakeProc( &m_debugPokeParam );
}

/**
 * @brief 作成したポケモンをセーブデータに格納する
 */
void DebugBattleProc::StoreMakePokeWithSaveData()
{
  m_saveData->SetPokeParam( m_pokeData, m_modifyPokeIndex );
  m_saveData->SetDefaultPowerUpDesc( m_modifyPokeIndex, m_defaultPowerUpDesc );
}


/**
 * @brief バトルプロセスをコールする
 */
void DebugBattleProc::CallBattleProc( gfl2::proc::Manager* procManager )
{
  btl::BATTLE_PROC_PARAM procParam;
  procParam.pAppMemory = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  procParam.pAppDeviceMemory = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
  procParam.pGameManager = this->GetGameManager();
  procParam.pSetupParam = &m_btlSetupParam;
  GameSys::CallBattleProc( &procParam );
}


//---------------------------------------------------------------------------
/**
 * @brief	PROCの描画処理
 * @param pManager   プロセス管理マネージャー
 * @param displayNo  描画対象画面
 * 登録したPROCによる、フレームバッファへの描画を実行します。
 * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
 */
//---------------------------------------------------------------------------
void DebugBattleProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  if( !( m_isDrawEnable ) ||
       ( m_renderingPipeLine == NULL ) ) {
    return;
  }
  if( displayNo == gfl2::gfx::CtrDisplayNo::DOWN ) {
    return;
  }
  gfl2::gfx::DisplayDeviceHandle handle = System::GflUse::m_HandleList[ displayNo ];
  gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( handle );
  gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );
  u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( handle );
  u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( handle );
  m_renderingPipeLine->SetRenderTarget( pBackBuffer );
  m_renderingPipeLine->SetDepthStencil( pDepthBuffer );
  m_renderingPipeLine->SetDisplaySize( displayWidth, displayHeight );
  m_renderingPipeLine->Execute();
}



void DebugBattleProc::recoverGraphicSystems( void )
{
  this->CreateDeviceHeap();
  this->initGraphicSystem();
  this->CreateRenderingPipeLine( m_sysHeap, m_devHeap );
  this->init2D();
}




//------------------------------------------------------------------------------
/**
 *  データのセーブ・ロード
 */
//------------------------------------------------------------------------------

/**
 *  save
 *  @retval  true で終了
 */
bool DebugBattleProc::savingProc( u32* seq )
{
  enum {
    SEQ_START = 0,
    SEQ_CONFIRM,
    SEQ_START_SAVE,
    SEQ_SAVING,
    SEQ_AFTER_SAVE,
    SEQ_CLEANUP,
  };

  gfl2::lyt::LytPane*    pWinPane  = m_lytWk->GetPane( PANENAME_BTLDEBUG_PANE_SAVE_WIN );
  gfl2::lyt::LytTextBox* pTextPane = m_lytWk->GetTextBoxPane( PANENAME_BTLDEBUG_PANE_TEXT_SAVEINFO );

  switch( *seq ){
  case SEQ_START:
    m_strData->GetString( DBGF_CONFIRM_SAVE, *m_strBufTmp );
    m_wordSet->RegisterNumber( 0, m_itemValue[ITEM_SAVE_SLOT], 1, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
    m_wordSet->Expand( m_strBuf, m_strBufTmp );
    pTextPane->SetString( m_strBuf->GetPtr() );
    pWinPane->SetVisible( true );
    (*seq)++;
    break;

  case SEQ_CONFIRM:
    if( m_uiButton->IsTrigger(gfl2::ui::BUTTON_A) ){
      (*seq) = SEQ_START_SAVE;
    }
    else if( m_uiButton->IsTrigger(gfl2::ui::BUTTON_B) ){
      (*seq) = SEQ_CLEANUP;
    }
    break;

  case SEQ_START_SAVE:
    for(int i=0; i<ITEM_MAX; ++i){
      this->saveItem( (DBItemID)i );
    }
    m_saveData->StartSave( m_itemValue[ITEM_SAVE_SLOT] );
    (*seq) = SEQ_SAVING;
    /* fallthru */
  case SEQ_SAVING:
    if( m_saveData->WaitSave() )
    {
      m_strData->GetString( DBGF_FINISH_SAVE, *m_strBuf );
      pTextPane->SetString( m_strBuf->GetPtr() );
      (*seq) = SEQ_AFTER_SAVE;
    }
    break;
  case SEQ_AFTER_SAVE:
    if( m_uiButton->IsTrigger(gfl2::ui::BUTTON_A|gfl2::ui::BUTTON_B) ){
      (*seq) = SEQ_CLEANUP;
    }
    break;
  case SEQ_CLEANUP:
    pWinPane->SetVisible( false );
    return true;
  }

  return false;
}


/**
 *  load
 *  @retval  true で終了
 */
bool DebugBattleProc::loadingProc( u32* seq, bool* bSucceed )
{
  gfl2::lyt::LytPane*    pWinPane  = m_lytWk->GetPane( PANENAME_BTLDEBUG_PANE_SAVE_WIN );
  gfl2::lyt::LytTextBox* pTextPane = m_lytWk->GetTextBoxPane( PANENAME_BTLDEBUG_PANE_TEXT_SAVEINFO );

  switch( *seq ){
  case 0:
    m_saveData->StartLoad( m_itemValue[ITEM_SAVE_SLOT] );
    ++(*seq);
    /* fallthru */
  case 1:
    if( m_saveData->WaitLoad(bSucceed) )
    {
      if( *bSucceed ){
        return true;
      }
      else{
        *seq = 2;
      }
    }
    break;

  // ロード失敗
  case 2:
    m_strData->GetString( DBGF_FAIL_LOAD, *m_strBuf );
    pTextPane->SetString( m_strBuf->GetPtr() );
    pWinPane->SetVisible( true );
    ++(*seq);
    break;
  case 3:
    if( m_uiButton->IsTrigger(gfl2::ui::BUTTON_A|gfl2::ui::BUTTON_B) ){
      pWinPane->SetVisible( false );
      return true;
    }
    break;
  }
  return false;
}

//------------------------------------------------------------------------------
/**
 *  録画データのセーブ・ロード
 */
//------------------------------------------------------------------------------

// 録画データセーブ
void DebugBattleProc::backupBattleRecordData( u32 slotIdx )
{
  Regulation::PRESET regulation_preset;
  switch( m_btlSetupParam.rule )
  {
  case BTL_RULE_SINGLE:
    regulation_preset = ( m_btlSetupParam.competitor == BTL_COMPETITOR_INST ) ? ( Regulation::PRESET_BATTLE_TREE_NORMAL_S ) : ( Regulation::PRESET_SINGLE_NORMAL );
    break;  

  case BTL_RULE_DOUBLE:
    regulation_preset = ( m_btlSetupParam.competitor == BTL_COMPETITOR_INST ) ? ( Regulation::PRESET_BATTLE_TREE_NORMAL_D ) : 
                        ( m_btlSetupParam.multiMode == BTL_MULTIMODE_NONE ) ? ( Regulation::PRESET_BATTLE_TREE_NORMAL_D ) : 
                        ( Regulation::PRESET_MULTI_NORMAL );
    break;

  case BTL_RULE_ROYAL:
    regulation_preset = ( m_btlSetupParam.competitor == BTL_COMPETITOR_INST ) ? ( Regulation::PRESET_ROYAL_FACILITY_NORMAL ) : ( Regulation::PRESET_ROYAL_NORMAL );
    break;

  default:
    return;
  }

  ExtSavedata::BattleRecorderSaveData* recData = GFL_NEW_LOW(m_sysHeap)  ExtSavedata::BattleRecorderSaveData( m_sysHeap->GetLowerHandle(), GFL_SINGLETON_INSTANCE( GameSys::GameManager ) );
  if( recData )
  {
    Regulation* reg = GFL_NEW( m_sysHeap->GetLowerHandle() ) Regulation( m_sysHeap->GetLowerHandle() );
    if( reg )
    {
      reg->LoadData( regulation_preset );

      if( recData->CreateSaveData( &m_btlSetupParam, reg ) )
      {
        const ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA*  backupData = recData->GetBattleRecData();
        if( m_saveData->SaveRecordData(slotIdx, backupData, sizeof(ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA)) )
        {
          GFL_PRINT("[DebugBtl] 録画データを slot %d にセーブしました\n", slotIdx );
        }
        else
        {
          GFL_PRINT("[DebugBtl] 録画データのセーブに失敗しました!!!\n");
        }
      }
      else
      {
        GFL_PRINT("[DebugBtl] 録画データの生成に失敗しました!!!\n");
      }

      GFL_DELETE( reg );
    }

    GFL_DELETE recData;
  }
}


// 録画データロード
bool DebugBattleProc::loadRecordData( BATTLE_SETUP_PARAM* dst, u32 slotIdx )
{
  bool result = false;
  ExtSavedata::BattleRecorderSaveData*  recData = GFL_NEW_LOW(m_sysHeap)  ExtSavedata::BattleRecorderSaveData( m_sysHeap->GetLowerHandle(), GFL_SINGLETON_INSTANCE( GameSys::GameManager ) );
  if( recData )
  {
    u32 dataSize = sizeof(ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA);
    ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA* dataPtr = (ExtSavedata::BattleRecorderSaveData::BATTLE_REC_DATA*)(GflHeapAllocMemory( m_sysHeap->GetLowerHandle(), dataSize ));
    if( m_saveData->LoadRecordData(slotIdx, dataPtr, dataSize) )
    {
      GFL_PRINT("[DebugBtl] 録画データ %d をロードできた\n", slotIdx );
      if( recData->CreateBattleSetupParamDebug( dst, dataPtr, m_sysHeap->GetLowerHandle() ) )
      {
        GFL_PRINT("[DebugBtl] BATTLE_SETUP_PARAM が構築できた\n");
        result = true;

        if( m_itemValue[ITEM_RECMODE] == REC_BTL0 || m_itemValue[ITEM_RECMODE] == REC_BTL1 )
        {
          u8 targetClientID = (m_itemValue[ITEM_RECMODE] == REC_BTL0)? 0 : 1;
          if( BTL_SETUP_ConvertToRecordFightMode( dst,  targetClientID, m_gameData, m_sysHeap ) ){
            GFL_PRINT("[DebugBtl] コンバートできたよ\n");
            setupParty( m_btlSetupParam.party[0], 0 );
          }else{
            GFL_PRINT("[DebugBtl] コンバート失敗\n");
            result = false;
          }
        }
      }
      else{
        GFL_PRINT("[DebugBtl] BATTLE_SETUP_PARAM の構築に失敗した\n");
      }
    }

    GFL_DELETE( recData );
    GflHeapSafeFreeMemory( dataPtr );
  }
  return result;
}



//------------------------------------------------------------------------------
/**
 *  ポケモンデータのカット、コピー、ペースト
 */
//------------------------------------------------------------------------------
void DebugBattleProc::clipDataCtrl( CtrlResult result )
{
  switch( result ){
  case CTRL_RESULT_POKE_CUT:
    // カットしたデータを保存
    m_saveData->GetPokeParam( m_clipPokeData, m_modifyPokeIndex );
    m_saveData->GetDefaultPowerUpDesc( m_modifyPokeIndex, &m_clipDefaultPowerUpDesc );
    // カットされたデータをクリア
    m_pokeData->Clear();
    DEFAULT_POWERUP_DESC_Clear( &m_defaultPowerUpDesc );
    m_saveData->SetPokeParam( m_pokeData, m_modifyPokeIndex );
    m_saveData->SetDefaultPowerUpDesc( m_modifyPokeIndex, m_defaultPowerUpDesc );
    // 描画
    this->redrawItem( m_modifyPokeItemNo );
    this->redrawClipPoke( m_clipPokeData );
    break;

  case CTRL_RESULT_POKE_COPY:
    m_saveData->GetPokeParam( m_pokeData,     m_modifyPokeIndex );
    m_saveData->GetDefaultPowerUpDesc( m_modifyPokeIndex, &m_defaultPowerUpDesc );
    if( !m_clipPokeData->IsNull() ){
      m_saveData->SetPokeParam( m_clipPokeData, m_modifyPokeIndex );
      m_saveData->SetDefaultPowerUpDesc( m_modifyPokeIndex, m_clipDefaultPowerUpDesc );
    }
    if( !m_pokeData->IsNull() ){
      m_clipPokeData->CopyFrom( *m_pokeData );
      DEFAULT_POWERUP_DESC_Copy( &m_clipDefaultPowerUpDesc, m_defaultPowerUpDesc );
    }
    this->redrawItem( m_modifyPokeItemNo );
    this->redrawClipPoke( m_clipPokeData );
    break;

  case CTRL_RESULT_POKE_PASTE:
    if( !m_clipPokeData->IsNull() )
    {
      m_saveData->SetPokeParam( m_clipPokeData, m_modifyPokeIndex );
      m_saveData->SetDefaultPowerUpDesc( m_modifyPokeIndex, m_clipDefaultPowerUpDesc );
      this->redrawItem( m_modifyPokeItemNo );
    }
    break;
  }
}





//------------------------------------------------------------------------------
/**
 *  ページ毎レイアウトデータインデックス
 */
//------------------------------------------------------------------------------
const DebugBattleProc::PAGE_PARAM   DebugBattleProc::PageParamTable[] = {
  { LYTRES_DEBUG_BATTLE_BTLDEBUG_BFLYT,      PANENAME_BTLDEBUG_PANE_SEL_WIN,    ITEM_P1_1            },
  { LYTRES_DEBUG_BATTLE_BTLDEBUG_P2_BFLYT,   PANENAME_BTLDEBUG_P2_PANE_SEL_WIN, ITEM_SW_MUST_ADD,    },
  { LYTRES_DEBUG_BATTLE_BTLDEBUG_P3_BFLYT,   PANENAME_BTLDEBUG_P3_PANE_SEL_WIN, ITEM_SW_SIREN_BATTLE },
  { LYTRES_DEBUG_BATTLE_BTLDEBUG_P4_BFLYT,   PANENAME_BTLDEBUG_P4_PANE_SEL_WIN, ITEM_FUNC_RESET_LANG },
};

//------------------------------------------------------------------------------
/**
 *
 */
//------------------------------------------------------------------------------
const DebugBattleProc::ITEM_PARAM  DebugBattleProc::ItemTable[ ITEM_MAX ] = 
{
  //                                    page   cursor                                               left                           right                         up                            down                          itemType        itemArg                        strStartIdx
  /* ITEM_P1_1                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P1_1,                    ITEM_P4_1,                     ITEM_P2_1,                    ITEM_STAMP_COUNT,             ITEM_P1_2,                    DBITEM_POKEMON, POKEPARAM_PARTY_1_POKE_1,      0,                          },
  /* ITEM_P1_2                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P1_2,                    ITEM_P4_2,                     ITEM_P2_2,                    ITEM_P1_1,                    ITEM_P1_3,                    DBITEM_POKEMON, POKEPARAM_PARTY_1_POKE_2,      0,                          },
  /* ITEM_P1_3                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P1_3,                    ITEM_P4_3,                     ITEM_P2_3,                    ITEM_P1_2,                    ITEM_P1_4,                    DBITEM_POKEMON, POKEPARAM_PARTY_1_POKE_3,      0,                          },
  /* ITEM_P1_4                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P1_4,                    ITEM_P4_4,                     ITEM_P2_4,                    ITEM_P1_3,                    ITEM_P1_5,                    DBITEM_POKEMON, POKEPARAM_PARTY_1_POKE_4,      0,                          },
  /* ITEM_P1_5                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P1_5,                    ITEM_P4_5,                     ITEM_P2_5,                    ITEM_P1_4,                    ITEM_P1_6,                    DBITEM_POKEMON, POKEPARAM_PARTY_1_POKE_5,      0,                          },
  /* ITEM_P1_6                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P1_6,                    ITEM_P4_6,                     ITEM_P2_6,                    ITEM_P1_5,                    ITEM_BTLMODE,                 DBITEM_POKEMON, POKEPARAM_PARTY_1_POKE_6,      0,                          },
  /* ITEM_P2_1                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P2_1,                    ITEM_P1_1,                     ITEM_P3_1,                    ITEM_STAMP_COUNT,             ITEM_P2_2,                    DBITEM_POKEMON, POKEPARAM_PARTY_2_POKE_1,      0,                          },
  /* ITEM_P2_2                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P2_2,                    ITEM_P1_2,                     ITEM_P3_2,                    ITEM_P2_1,                    ITEM_P2_3,                    DBITEM_POKEMON, POKEPARAM_PARTY_2_POKE_2,      0,                          },
  /* ITEM_P2_3                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P2_3,                    ITEM_P1_3,                     ITEM_P3_3,                    ITEM_P2_2,                    ITEM_P2_4,                    DBITEM_POKEMON, POKEPARAM_PARTY_2_POKE_3,      0,                          },
  /* ITEM_P2_4                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P2_4,                    ITEM_P1_4,                     ITEM_P3_4,                    ITEM_P2_3,                    ITEM_P2_5,                    DBITEM_POKEMON, POKEPARAM_PARTY_2_POKE_4,      0,                          },
  /* ITEM_P2_5                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P2_5,                    ITEM_P1_5,                     ITEM_P3_5,                    ITEM_P2_4,                    ITEM_P2_6,                    DBITEM_POKEMON, POKEPARAM_PARTY_2_POKE_5,      0,                          },
  /* ITEM_P2_6                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P2_6,                    ITEM_P1_6,                     ITEM_P3_6,                    ITEM_P2_5,                    ITEM_BTLMODE,                 DBITEM_POKEMON, POKEPARAM_PARTY_2_POKE_6,      0,                          },
  /* ITEM_P3_1                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P3_1,                    ITEM_P2_1,                     ITEM_P4_1,                    ITEM_LOAD,                    ITEM_P3_2,                    DBITEM_POKEMON, POKEPARAM_PARTY_3_POKE_1,      0,                          },
  /* ITEM_P3_2                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P3_2,                    ITEM_P2_2,                     ITEM_P4_2,                    ITEM_P3_1,                    ITEM_P3_3,                    DBITEM_POKEMON, POKEPARAM_PARTY_3_POKE_2,      0,                          },
  /* ITEM_P3_3                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P3_3,                    ITEM_P2_3,                     ITEM_P4_3,                    ITEM_P3_2,                    ITEM_P3_4,                    DBITEM_POKEMON, POKEPARAM_PARTY_3_POKE_3,      0,                          },
  /* ITEM_P3_4                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P3_4,                    ITEM_P2_4,                     ITEM_P4_4,                    ITEM_P3_3,                    ITEM_P3_5,                    DBITEM_POKEMON, POKEPARAM_PARTY_3_POKE_4,      0,                          },
  /* ITEM_P3_5                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P3_5,                    ITEM_P2_5,                     ITEM_P4_5,                    ITEM_P3_4,                    ITEM_P3_6,                    DBITEM_POKEMON, POKEPARAM_PARTY_3_POKE_5,      0,                          },
  /* ITEM_P3_6                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P3_6,                    ITEM_P2_6,                     ITEM_P4_6,                    ITEM_P3_5,                    ITEM_COMMMODE,                DBITEM_POKEMON, POKEPARAM_PARTY_3_POKE_6,      0,                          },
  /* ITEM_P4_1                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P4_1,                    ITEM_P3_1,                     ITEM_P1_1,                    ITEM_SAVE,                    ITEM_P4_2,                    DBITEM_POKEMON, POKEPARAM_PARTY_4_POKE_1,      0,                          },
  /* ITEM_P4_2                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P4_2,                    ITEM_P3_2,                     ITEM_P1_2,                    ITEM_P4_1,                    ITEM_P4_3,                    DBITEM_POKEMON, POKEPARAM_PARTY_4_POKE_2,      0,                          },
  /* ITEM_P4_3                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P4_3,                    ITEM_P3_3,                     ITEM_P1_3,                    ITEM_P4_2,                    ITEM_P4_4,                    DBITEM_POKEMON, POKEPARAM_PARTY_4_POKE_3,      0,                          },
  /* ITEM_P4_4                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P4_4,                    ITEM_P3_4,                     ITEM_P1_4,                    ITEM_P4_3,                    ITEM_P4_5,                    DBITEM_POKEMON, POKEPARAM_PARTY_4_POKE_4,      0,                          },
  /* ITEM_P4_5                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P4_5,                    ITEM_P3_5,                     ITEM_P1_5,                    ITEM_P4_4,                    ITEM_P4_6,                    DBITEM_POKEMON, POKEPARAM_PARTY_4_POKE_5,      0,                          },
  /* ITEM_P4_6                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_P4_6,                    ITEM_P3_6,                     ITEM_P1_6,                    ITEM_P4_5,                    ITEM_COMMMODE,                DBITEM_POKEMON, POKEPARAM_PARTY_4_POKE_6,      0,                          },
  /* ITEM_POKE_INTRUDE             */ {    2,  PANENAME_BTLDEBUG_P3_PANE_INTRD_POKE_VALUE,          ITEM_SW_VS_NUSI,               ITEM_SW_VS_NUSI,              ITEM_SW_INTRUDE_FORCE_APPEAR, ITEM_REINFORCE_COUNT_1,       DBITEM_POKEMON, POKEPARAM_INTRUDE,             0,                          },
  /* ITEM_POKE_REINFORCE_1         */ {    2,  PANENAME_BTLDEBUG_P3_PANE_REIN_POKE_VALUE_00,        ITEM_REINFORCE_COUNT_1,        ITEM_REINFORCE_COND_1,        ITEM_SW_REINFORCE_ENABLE,     ITEM_POKE_REINFORCE_2,        DBITEM_POKEMON, POKEPARAM_REINFORCE_1,         0,                          },
  /* ITEM_POKE_REINFORCE_2         */ {    2,  PANENAME_BTLDEBUG_P3_PANE_REIN_POKE_VALUE_01,        ITEM_REINFORCE_COUNT_2,        ITEM_REINFORCE_COND_2,        ITEM_POKE_REINFORCE_1,        ITEM_POKE_REINFORCE_3,        DBITEM_POKEMON, POKEPARAM_REINFORCE_2,         0,                          },
  /* ITEM_POKE_REINFORCE_3         */ {    2,  PANENAME_BTLDEBUG_P3_PANE_REIN_POKE_VALUE_02,        ITEM_REINFORCE_COUNT_3,        ITEM_REINFORCE_COND_3,        ITEM_POKE_REINFORCE_2,        ITEM_POKE_REINFORCE_4,        DBITEM_POKEMON, POKEPARAM_REINFORCE_3,         0,                          },
  /* ITEM_POKE_REINFORCE_4         */ {    2,  PANENAME_BTLDEBUG_P3_PANE_REIN_POKE_VALUE_03,        ITEM_REINFORCE_COUNT_4,        ITEM_REINFORCE_COND_4,        ITEM_POKE_REINFORCE_3,        ITEM_SW_VS_ULTRA_BEAST,       DBITEM_POKEMON, POKEPARAM_REINFORCE_4,         0,                          },
  //                                    page   cursor                                               left                           right                         up                            down                          itemType        itemArg                        strStartIdx                 
  /* ITEM_BTLMODE                  */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_BTLMODE,                 ITEM_COMMMODE,                 ITEM_COMMMODE,                ITEM_P1_5,                    ITEM_MSGSPEED,                DBITEM_STR,     BTLMODE_MAX,                   DBGF_ITEM_TYPE01,           },
  /* ITEM_MSGSPEED                 */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_MSGSPEED,                ITEM_EFFENABLE,                ITEM_EFFENABLE,               ITEM_BTLMODE,                 ITEM_CHANGERULE,              DBITEM_STR,     MSG_SPEED_MAX,                 DBGF_ITEM_MSGSPD_SLOW,      },
  /* ITEM_CHANGERULE               */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_CHANGERULE,              ITEM_RECIDX,                   ITEM_BTLHOUSE_ENABLE,         ITEM_MSGSPEED,                ITEM_STAMP_COUNT,             DBITEM_STR,     CHANGERULE_MAX,                DBGF_RULE_1,                },
  /* ITEM_STAMP_COUNT              */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_BADGECOUNT,              ITEM_RECIDX,                   ITEM_RECMODE,                 ITEM_CHANGERULE,              ITEM_P1_1,                    DBITEM_NUM,     6,                             0,                          },
  /* ITEM_COMMMODE                 */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_COMMMODE,                ITEM_BTLMODE,                  ITEM_BTLMODE,                 ITEM_P4_6,                    ITEM_EFFENABLE,               DBITEM_STR,     COMM_MODE_MAX,                 DBGF_ITEM_COMM_PARENT,      },
  /* ITEM_EFFENABLE                */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_EFFENABLE,               ITEM_MSGSPEED,                 ITEM_MSGSPEED,                ITEM_COMMMODE,                ITEM_BTLHOUSE_ENABLE,         DBITEM_STR,     2,                             DBGF_ITEM_WAZAEFF_ON,       },
  /* ITEM_BTLHOUSE_ENABLE          */ {    0,  PANENAME_BTLDEBUG_PANE_BTLHOUSEENABLE,               ITEM_CHANGERULE,               ITEM_CHANGERULE,              ITEM_EFFENABLE,               ITEM_RECMODE,                 DBITEM_STR,     2,                             DBGF_ITEM_STR_OFF,          },
  /* ITEM_RECMODE                  */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_RECMODE,                 ITEM_CHANGERULE,               ITEM_RECIDX,                  ITEM_BTLHOUSE_ENABLE,         ITEM_LOAD,                    DBITEM_STR,     REC_MODE_MAX,                  DBGF_ITEM_REC_OFF,          },
  /* ITEM_RECIDX                   */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_RECIDX,                  ITEM_RECMODE,                  ITEM_STAMP_COUNT,             ITEM_BTLHOUSE_ENABLE,         ITEM_SAVE,                    DBITEM_NUM,     4,                             0,                          },
  /* ITEM_LOAD                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_LOAD,                    ITEM_STAMP_COUNT,              ITEM_SAVE,                    ITEM_RECMODE,                 ITEM_P4_1,                    DBITEM_LABEL,   0,                             0,                          },
  /* ITEM_SAVE                     */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_SAVE,                    ITEM_LOAD,                     ITEM_SAVE_SLOT,               ITEM_RECIDX,                  ITEM_P4_1,                    DBITEM_LABEL,   0,                             0,                          },
  /* ITEM_SAVE_SLOT                */ {    0,  PANENAME_BTLDEBUG_PANE_TEXT_SAVEINDEX,               ITEM_SAVE,                     ITEM_STAMP_COUNT,             ITEM_RECIDX,                  ITEM_P4_1,                    DBITEM_NUM,     DebugBattleSaveData::SLOT_MAX, 0,                          }, 
  /* ITEM_TIME_GAME_M              */ {    1,  PANENAME_BTLDEBUG_P2_PANE_TIME_GAMEM,                ITEM_AI_BASE,                  ITEM_SW_MUST_ADD,             ITEM_SW_ZUKAN_DONT_CLEAR,     ITEM_TIME_GAME_S,             DBITEM_NUM,     121,                           0,                          },
  /* ITEM_TIME_GAME_S              */ {    1,  PANENAME_BTLDEBUG_P2_PANE_TIME_GAMES,                ITEM_AI_STRONG,                ITEM_SW_MUST_TOK,             ITEM_TIME_GAME_M,             ITEM_TIME_CLIENT_M,           DBITEM_NUM,     60,                            0,                          }, 
  /* ITEM_TIME_CLIENT_M            */ {    1,  PANENAME_BTLDEBUG_P2_PANE_TIME_CLIENTM,              ITEM_AI_EXPERT,                ITEM_SW_MUST_ITEM,            ITEM_TIME_GAME_S,             ITEM_TIME_CLIENT_S,           DBITEM_NUM,     121,                           0,                          },
  /* ITEM_TIME_CLIENT_S            */ {    1,  PANENAME_BTLDEBUG_P2_PANE_TIME_CLIENTS,              ITEM_AI_DOUBLE,                ITEM_SW_MUST_CRITICAL,        ITEM_TIME_CLIENT_M,           ITEM_TIME_COMMAND,            DBITEM_NUM,     60,                            0,                          }, 
  /* ITEM_TIME_COMMAND             */ {    1,  PANENAME_BTLDEBUG_P2_PANE_TIME_COMMAND,              ITEM_AI_ALLOWANCE,             ITEM_SW_MUST_CRITICAL,        ITEM_TIME_CLIENT_S,           ITEM_DEFAULT_WEATHER,         DBITEM_NUM,     240,                           0,                          },
  /* ITEM_BACKGRND_NEAR            */ {    1,  PANENAME_BTLDEBUG_P2_PANE_BACKGRND,                  ITEM_SW_DISABLE_CRITICAL,      ITEM_SW_MEGARING_OFF,         ITEM_BGM,                     ITEM_BACKGRND_FAR,            DBITEM_STR,     btl::bg::BGSYS_NEAR_TYPE_MAX,  DBGF_BG_NEAR_000,           },
  /* ITEM_BACKGRND_FAR             */ {    1,  PANENAME_BTLDEBUG_P2_PANE_BG_FAR_VALUE,              ITEM_SW_ZUKAN_DONT_CLEAR,      ITEM_SW_ZENRYOKURING_OFF,     ITEM_BACKGRND_NEAR,           ITEM_SW_RESOURCE_HIO,         DBITEM_STR,     btl::bg::BGSYS_FAR_TYPE_MAX,   DBGF_BG_FAR_000,            },
  /* ITEM_BGM                      */ {    1,  PANENAME_BTLDEBUG_P2_PANE_BGM_VALUE,                 ITEM_SW_AI_CTRL,               ITEM_SW_MEGARING_OFF,         ITEM_AI_POKE_CHANGE,          ITEM_BACKGRND_NEAR,           DBITEM_STR,     GFL_NELEMS(BGM_TABLE),         DBGF_BGM_00,                },
  /* ITEM_REINFORCE_COND_1         */ {    2,  PANENAME_BTLDEBUG_P3_PANE_REIN_COND_VALUE_00,        ITEM_POKE_REINFORCE_1,         ITEM_REINFORCE_COUNT_1,       ITEM_POKE_INTRUDE,            ITEM_REINFORCE_COND_2,        DBITEM_STR,     REINFORCE_CONDITION_NUM,       DBGF_ITEM_REINFORCE_COND_0, },
  /* ITEM_REINFORCE_COND_2         */ {    2,  PANENAME_BTLDEBUG_P3_PANE_REIN_COND_VALUE_01,        ITEM_POKE_REINFORCE_2,         ITEM_REINFORCE_COUNT_2,       ITEM_REINFORCE_COND_1,        ITEM_REINFORCE_COND_3,        DBITEM_STR,     REINFORCE_CONDITION_NUM,       DBGF_ITEM_REINFORCE_COND_0, },
  /* ITEM_REINFORCE_COND_3         */ {    2,  PANENAME_BTLDEBUG_P3_PANE_REIN_COND_VALUE_02,        ITEM_POKE_REINFORCE_3,         ITEM_REINFORCE_COUNT_3,       ITEM_REINFORCE_COND_2,        ITEM_REINFORCE_COND_4,        DBITEM_STR,     REINFORCE_CONDITION_NUM,       DBGF_ITEM_REINFORCE_COND_0, },
  /* ITEM_REINFORCE_COND_4         */ {    2,  PANENAME_BTLDEBUG_P3_PANE_REIN_COND_VALUE_03,        ITEM_POKE_REINFORCE_4,         ITEM_REINFORCE_COUNT_4,       ITEM_REINFORCE_COND_3,        ITEM_SW_VS_ULTRA_BEAST,       DBITEM_STR,     REINFORCE_CONDITION_NUM,       DBGF_ITEM_REINFORCE_COND_0, },
  /* ITEM_REINFORCE_COUNT_1        */ {    2,  PANENAME_BTLDEBUG_P3_PANE_REIN_COUNT_VALUE_00,       ITEM_REINFORCE_COND_1,         ITEM_POKE_REINFORCE_1,        ITEM_POKE_INTRUDE,            ITEM_REINFORCE_COUNT_2,       DBITEM_NUM,     10,                            0,                          },
  /* ITEM_REINFORCE_COUNT_2        */ {    2,  PANENAME_BTLDEBUG_P3_PANE_REIN_COUNT_VALUE_01,       ITEM_REINFORCE_COND_2,         ITEM_POKE_REINFORCE_2,        ITEM_REINFORCE_COUNT_1,       ITEM_REINFORCE_COUNT_3,       DBITEM_NUM,     10,                            0,                          },
  /* ITEM_REINFORCE_COUNT_3        */ {    2,  PANENAME_BTLDEBUG_P3_PANE_REIN_COUNT_VALUE_02,       ITEM_REINFORCE_COND_3,         ITEM_POKE_REINFORCE_3,        ITEM_REINFORCE_COUNT_2,       ITEM_REINFORCE_COUNT_4,       DBITEM_NUM,     10,                            0,                          },
  /* ITEM_REINFORCE_COUNT_4        */ {    2,  PANENAME_BTLDEBUG_P3_PANE_REIN_COUNT_VALUE_03,       ITEM_REINFORCE_COND_4,         ITEM_POKE_REINFORCE_4,        ITEM_REINFORCE_COUNT_3,       ITEM_SW_VS_ULTRA_BEAST,       DBITEM_NUM,     10,                            0,                          },
  //                                    page   cursor                                               left                           right                         up                            down                          itemType        itemArg                        strStartIdx                 
  /* ITEM_SW_MUST_ADD              */ {    1,  PANENAME_BTLDEBUG_P2_PANE_DBGSW_MUSTADD,             ITEM_TIME_GAME_M,              ITEM_AI_BASE,                 ITEM_SW_SAVE_DONT_CLEAR,      ITEM_SW_MUST_TOK,             DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_MUST_TOK              */ {    1,  PANENAME_BTLDEBUG_P2_PANE_DBGSW_MUSTTOK,             ITEM_TIME_GAME_S,              ITEM_AI_STRONG,               ITEM_SW_MUST_ADD,             ITEM_SW_MUST_ITEM,            DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_MUST_ITEM             */ {    1,  PANENAME_BTLDEBUG_P2_PANE_DBGSW_MUSTITEM,            ITEM_TIME_COMMAND,             ITEM_AI_EXPERT,               ITEM_SW_MUST_TOK,             ITEM_SW_MUST_CRITICAL,        DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_MUST_CRITICAL         */ {    1,  PANENAME_BTLDEBUG_P2_PANE_DBGSW_MUSTCRIT,            ITEM_AI_DOUBLE,                ITEM_AI_DOUBLE,               ITEM_SW_MUST_ITEM,            ITEM_SW_MUST_CRITICAL_CAPTURE,DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_MUST_CRITICAL_CAPTURE */ {    1,  PANENAME_BTLDEBUG_P2_PANE_FLG_MUST_CRETICALCAP,      ITEM_TIME_COMMAND,             ITEM_AI_ALLOWANCE,            ITEM_SW_MUST_CRITICAL,        ITEM_SW_HP_EVER,              DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_HP_EVER               */ {    1,  PANENAME_BTLDEBUG_P2_PANE_DBGSW_HPEVER,              ITEM_TIME_COMMAND,             ITEM_AI_ROYAL,                ITEM_SW_MUST_CRITICAL_CAPTURE,ITEM_SW_PP_EVER,              DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_PP_EVER               */ {    1,  PANENAME_BTLDEBUG_P2_PANE_DBGSW_PPEVER,              ITEM_AI_ROYAL,                 ITEM_AI_INTRUDE,              ITEM_SW_HP_EVER,              ITEM_SW_HIT_100,              DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_HIT_100               */ {    1,  PANENAME_BTLDEBUG_P2_PANE_DBGSW_HIT100,              ITEM_AI_INTRUDE,               ITEM_AI_ITEM,                 ITEM_SW_PP_EVER,              ITEM_SW_DAMAGE_RAND_DISABLE,  DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_DAMAGE_RAND_DISABLE   */ {    1,  PANENAME_BTLDEBUG_P2_PANE_DBGSW_DMGRANDOFF,          ITEM_SW_SKY_BTL,               ITEM_AI_POKE_CHANGE,          ITEM_SW_HIT_100,              ITEM_SW_STARTEFF_SKIP,        DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_STARTEFF_SKIP         */ {    1,  PANENAME_BTLDEBUG_P2_PANE_DBGSW_INEFFSKIP,           ITEM_SW_SAKASA_BTL,            ITEM_AI_POKE_CHANGE,          ITEM_SW_DAMAGE_RAND_DISABLE,  ITEM_SW_AI_CTRL,              DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_AI_CTRL               */ {    1,  PANENAME_BTLDEBUG_P2_PANE_DBGSW_AICTRL,              ITEM_SW_MUST_CAPTURE,          ITEM_BGM,                     ITEM_SW_STARTEFF_SKIP,        ITEM_SW_DISABLE_CRITICAL,     DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_DISABLE_CRITICAL      */ {    1,  PANENAME_BTLDEBUG_P2_PANE_DBGSW_DISCRIT,             ITEM_SW_GAKUSYU,               ITEM_BACKGRND_NEAR,           ITEM_SW_AI_CTRL,              ITEM_SW_ZUKAN_DONT_CLEAR,     DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_INTRUDE_FORCE_CALL    */ {    2,  PANENAME_BTLDEBUG_P3_PANE_INTRD_FORCE_VALUE,         ITEM_SW_VS_NUSI,               ITEM_SW_VS_NUSI,              ITEM_SW_INTRUDE_ENABLE,       ITEM_SW_INTRUDE_FORCE_APPEAR, DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_INTRUDE_FORCE_APPEAR  */ {    2,  PANENAME_BTLDEBUG_P3_PANE_INTRD_FORCE_APPEAR_VALUE,  ITEM_SW_VS_NUSI,               ITEM_SW_VS_NUSI,              ITEM_SW_INTRUDE_FORCE_CALL,   ITEM_POKE_INTRUDE,            DBITEM_NUM,     2,                             0,                          },
  /* ITEM_AI_BASE                  */ {    1,  PANENAME_BTLDEBUG_P2_PANE_AISW_BASE,                 ITEM_SW_MUST_ADD,              ITEM_TIME_GAME_M,             ITEM_AI_POKE_CHANGE,          ITEM_AI_STRONG,               DBITEM_NUM,     2,                             0,                          },
  /* ITEM_AI_STRONG                */ {    1,  PANENAME_BTLDEBUG_P2_PANE_AISW_HARD,                 ITEM_SW_MUST_TOK,              ITEM_TIME_GAME_S,             ITEM_AI_BASE,                 ITEM_AI_EXPERT,               DBITEM_NUM,     2,                             0,                          },
  /* ITEM_AI_EXPERT                */ {    1,  PANENAME_BTLDEBUG_P2_PANE_AISW_EXPERT,               ITEM_SW_MUST_ITEM,             ITEM_TIME_CLIENT_M,           ITEM_AI_STRONG,               ITEM_AI_DOUBLE,               DBITEM_NUM,     2,                             0,                          },
  /* ITEM_AI_DOUBLE                */ {    1,  PANENAME_BTLDEBUG_P2_PANE_AISW_SUB,                  ITEM_SW_MUST_CRITICAL,         ITEM_TIME_CLIENT_S,           ITEM_AI_EXPERT,               ITEM_AI_ALLOWANCE,            DBITEM_NUM,     2,                             0,                          },
  /* ITEM_AI_ALLOWANCE             */ {    1,  PANENAME_BTLDEBUG_P2_PANE_AISW_GAMBLE,               ITEM_SW_MUST_CRITICAL_CAPTURE, ITEM_TIME_COMMAND,            ITEM_AI_DOUBLE,               ITEM_AI_ROYAL,                DBITEM_NUM,     2,                             0,                          },
  /* ITEM_AI_ROYAL                 */ {    1,  PANENAME_BTLDEBUG_P2_PANE_AISW_DAMAGE,               ITEM_SW_HP_EVER,               ITEM_DEFAULT_WEATHER,         ITEM_AI_ALLOWANCE,            ITEM_AI_INTRUDE,              DBITEM_NUM,     2,                             0,                          },
  /* ITEM_AI_INTRUDE               */ {    1,  PANENAME_BTLDEBUG_P2_PANE_AISW_RELAY,                ITEM_SW_PP_EVER,               ITEM_DEFAULT_WEATHER,         ITEM_AI_ROYAL,                ITEM_AI_ITEM,                 DBITEM_NUM,     2,                             0,                          },
  /* ITEM_AI_ITEM                  */ {    1,  PANENAME_BTLDEBUG_P2_PANE_AISW_DOUBLE,               ITEM_SW_HIT_100,               ITEM_SW_SKY_BTL,              ITEM_AI_INTRUDE,              ITEM_AI_POKE_CHANGE,          DBITEM_NUM,     2,                             0,                          },
  /* ITEM_AI_POKE_CHANGE           */ {    1,  PANENAME_BTLDEBUG_P2_PANE_AISW_WEATHER,              ITEM_SW_DAMAGE_RAND_DISABLE,   ITEM_SW_SAKASA_BTL,           ITEM_AI_ITEM,                 ITEM_BGM,                     DBITEM_NUM,     2,                             0,                          },
  /* ITEM_DEFAULT_WEATHER          */ {    1,  PANENAME_BTLDEBUG_P2_PANE_DEFAULT_WEATHER,           ITEM_AI_INTRUDE,               ITEM_SW_HIT_100,              ITEM_TIME_COMMAND,            ITEM_SW_SKY_BTL,              DBITEM_STR,     BTL_WEATHER_MAX,               DBGF_WEATHER_0,             },
  /* ITEM_SW_SKY_BTL               */ {    1,  PANENAME_BTLDEBUG_P2_PANE_FLG_SKYBTL,                ITEM_AI_POKE_CHANGE,           ITEM_SW_STARTEFF_SKIP,        ITEM_DEFAULT_WEATHER,         ITEM_SW_SAKASA_BTL,           DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_SAKASA_BTL            */ {    1,  PANENAME_BTLDEBUG_P2_PANE_FLG_SAKASABTL,             ITEM_AI_POKE_CHANGE,           ITEM_SW_STARTEFF_SKIP,        ITEM_SW_SKY_BTL,              ITEM_SW_MUST_CAPTURE,         DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_MUST_CAPTURE          */ {    1,  PANENAME_BTLDEBUG_P2_PANE_FLG_MUST_CAPTURE,          ITEM_AI_POKE_CHANGE,           ITEM_SW_AI_CTRL,              ITEM_SW_SAKASA_BTL,           ITEM_SW_GAKUSYU,              DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_GAKUSYU               */ {    1,  PANENAME_BTLDEBUG_P2_PANE_FLG_GAKUSYU,               ITEM_BGM,                      ITEM_SW_DISABLE_CRITICAL,     ITEM_SW_MUST_CAPTURE,         ITEM_SW_MEGARING_OFF,         DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_MEGARING_OFF          */ {    1,  PANENAME_BTLDEBUG_P2_PANE_FLG_MEGARING_OFF,          ITEM_BACKGRND_NEAR,            ITEM_SW_ZUKAN_DONT_CLEAR,     ITEM_SW_GAKUSYU,              ITEM_SW_ZENRYOKURING_OFF,     DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_ZENRYOKURING_OFF      */ {    1,  PANENAME_BTLDEBUG_P2_PANE_FLG_ZENRYOKU_OFF,          ITEM_BACKGRND_FAR,             ITEM_SW_SAVE_DONT_CLEAR,      ITEM_SW_MEGARING_OFF,         ITEM_SW_ZUKAN_DONT_CLEAR,     DBITEM_NUM,     2,                             0,                          }, 
  /* ITEM_SW_ZUKAN_DONT_CLEAR      */ {    1,  PANENAME_BTLDEBUG_P2_PANE_ZUKAN_DONTCLEAR_VAL,       ITEM_SW_MEGARING_OFF,          ITEM_BACKGRND_FAR,            ITEM_SW_DISABLE_CRITICAL,     ITEM_SW_SAVE_DONT_CLEAR,      DBITEM_NUM,     2,                             0,                          }, 
  /* ITEM_SW_SAVE_DONT_CLEAR       */ {    1,  PANENAME_BTLDEBUG_P2_PANE_SAVE_DONTCLEAR_VAL,        ITEM_SW_ZENRYOKURING_OFF,      ITEM_SW_RESOURCE_HIO,         ITEM_SW_ZUKAN_DONT_CLEAR,     ITEM_SW_MUST_ADD,             DBITEM_NUM,     2,                             0,                          }, 
  /* ITEM_SW_RESOURCE_HIO          */ {    1,  PANENAME_BTLDEBUG_P2_PANE_RESOURCEHIO_VAL,           ITEM_SW_SAVE_DONT_CLEAR,       ITEM_SW_ZENRYOKURING_OFF,     ITEM_BACKGRND_FAR,            ITEM_AI_BASE,                 DBITEM_NUM,     2,                             0,                          }, 
  /* ITEM_SW_SIREN_BATTLE          */ {    2,  PANENAME_BTLDEBUG_P3_PANE_DBGSW_SIREN,               ITEM_SW_INTRUDE_ENABLE,        ITEM_SW_INTRUDE_ENABLE,       ITEM_SW_VS_BOSS,              ITEM_SW_VS_NUSI,              DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_VS_NUSI               */ {    2,  PANENAME_BTLDEBUG_P3_PANE_DBGSW_VSNUSI,              ITEM_SW_INTRUDE_FORCE_CALL,    ITEM_SW_INTRUDE_FORCE_CALL,   ITEM_SW_SIREN_BATTLE,         ITEM_SW_REINFORCE_ENABLE,     DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_INTRUDE_ENABLE        */ {    2,  PANENAME_BTLDEBUG_P3_PANE_INTRD_FLAG_VALUE,          ITEM_SW_SIREN_BATTLE,          ITEM_SW_SIREN_BATTLE,         ITEM_REINFORCE_COND_4,        ITEM_SW_INTRUDE_FORCE_CALL,   DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_REINFORCE_ENABLE      */ {    2,  PANENAME_BTLDEBUG_P3_PANE_REIN_FLAG_VALUE,           ITEM_SW_REINFORCE_ENABLE,      ITEM_SW_REINFORCE_ENABLE,     ITEM_SW_VS_NUSI,              ITEM_POKE_REINFORCE_1,        DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_VS_ULTRA_BEAST        */ {    2,  PANENAME_BTLDEBUG_P3_PANE_UB_VSUB_VALUE,             ITEM_SW_VS_ULTRA_BEAST,        ITEM_SW_VS_ULTRA_BEAST,       ITEM_POKE_REINFORCE_4,        ITEM_SW_VS_BOSS,              DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_VS_BOSS               */ {    2,  PANENAME_BTLDEBUG_P3_PANE_UB_VSBOSS_VALUE,           ITEM_SW_VS_BOSS,               ITEM_SW_VS_BOSS,              ITEM_SW_VS_ULTRA_BEAST,       ITEM_SW_SIREN_BATTLE,         DBITEM_NUM,     2,                             0,                          },
  /* ITEM_SW_FUNC_WAZA_CHECK_MODE  */ {    3,  PANENAME_BTLDEBUG_P4_PANE_WAZACHECK_VAL,             ITEM_SW_FUNC_WAZA_CHECK_MODE,  ITEM_SW_FUNC_WAZA_CHECK_MODE, ITEM_FUNC_RESET_LANG,         ITEM_FUNC_BTLFES_FIELD,       DBITEM_NUM,     2,                             0,                          },
  /* ITEM_FUNC_RESET_LANG          */ {    3,  PANENAME_BTLDEBUG_P4_PANE_TEXT_LANG,                 ITEM_FUNC_RESET_LANG,          ITEM_FUNC_RESET_LANG,         ITEM_SW_FUNC_WAZA_CHECK_MODE, ITEM_SW_FUNC_WAZA_CHECK_MODE, DBITEM_LABEL,   0,                             0,                          },
  /* ITEM_FUNC_BTLFES_FIELD        */ {    3,  PANENAME_BTLDEBUG_P4_PANE_BTLFES_FIELD_VAL,          ITEM_FUNC_BTLFES_FIELD,        ITEM_FUNC_BTLFES_FIELD,       ITEM_SW_FUNC_WAZA_CHECK_MODE, ITEM_FUNC_BTLFES_FLAG,        DBITEM_STR,     BATTLE_FES_FIELD_MAX,          DBGF_BTLFES_FIELD_00,       },
  /* ITEM_FUNC_BTLFES_FLAG         */ {    3,  PANENAME_BTLDEBUG_P4_PANE_BTLFES_FLAG_VAL,           ITEM_FUNC_BTLFES_FLAG,         ITEM_FUNC_BTLFES_FLAG,        ITEM_FUNC_BTLFES_FIELD,       ITEM_FUNC_RESET_LANG,         DBITEM_NUM,     2,                             0,                          },
  //                                    page   cursor                                               left                           right                         up                            down                          itemType        itemArg                        strStartIdx                 
};




const DebugBattleProc::LABEL_PARAM DebugBattleProc::LabelTable[] = 
{
  /* じぶん                */ { 0, PANENAME_BTLDEBUG_PANE_TEXTBOX_00,              DBGF_LABEL_FRIEND1               },
  /* あいて１              */ { 0, PANENAME_BTLDEBUG_PANE_TEXTBOX_01,              DBGF_LABEL_ENEMY1                },
  /* みかた                */ { 0, PANENAME_BTLDEBUG_PANE_TEXTBOX_03,              DBGF_LABEL_FRIEND2               },
  /* あいて２              */ { 0, PANENAME_BTLDEBUG_PANE_TEXTBOX_02,              DBGF_LABEL_ENEMY2                },
  /* モード                */ { 0, PANENAME_BTLDEBUG_PANE_TEXTBOX_04,              DBGF_LABEL_TYPE                  },
  /* MSG                   */ { 0, PANENAME_BTLDEBUG_PANE_TEXTBOX_05,              DBGF_LABEL_MSG                   },
  /* ルール                */ { 0, PANENAME_BTLDEBUG_PANE_TEXTBOX_06,              DBGF_LABEL_RULE                  },
  /* スタンプ              */ { 0, PANENAME_BTLDEBUG_PANE_TEXTBOX_07,              DBGF_LABEL_BADGE                 },
  /* つうしん              */ { 0, PANENAME_BTLDEBUG_PANE_TEXTBOX_09,              DBGF_LABEL_COMM                  },
  /* エフェクト            */ { 0, PANENAME_BTLDEBUG_PANE_TEXTBOX_08,              DBGF_LABEL_WAZAEFF               },
  /* バトルハウスモード    */ { 0, PANENAME_BTLDEBUG_PANE_TEXTBOX_12,              DBGF_LABEL_SUBWAY                },
  /* ろくが                */ { 0, PANENAME_BTLDEBUG_PANE_TEXTBOX_10,              DBGF_LABEL_RECORD                },
  /* デバッグスイッチ      */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXTBOX_03,           DBGF_LABEL_DEBUG_SWITCH          },
  /* 必ず追加効果          */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_PH_05,           DBGF_LABEL_MUST_TUIKA            },
  /* 必ずとくせい          */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P1_04,           DBGF_LABEL_MUST_TOKU             },
  /* 必ずアイテム          */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P1_03,           DBGF_LABEL_MUST_ITEM             },
  /* 必ず急所              */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P1_02,           DBGF_LABEL_MUST_CRITICAL         },
  /* HPへらない            */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P1_01,           DBGF_LABEL_HP_CONST              },
  /* PPへらない            */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P1_00,           DBGF_LABEL_PP_CONST              },
  /* 命中100               */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P1_06,           DBGF_LABEL_HIT100PER             },
  /* ダメランダムオフ      */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P1_07,           DBGF_LABEL_DMGRAND_OFF           },
  /* 入場スキップ          */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P1_08,           DBGF_LABEL_SKIP_IN               },
  /* AIコントロール        */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P1_09,           DBGF_LABEL_AI_CTRL               },
  /* 急所あたらない        */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P1_05,           DBGF_LABEL_DISABLE_CRITICAL      },
  /* 内背景                */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P3_09,           DBGF_LABEL_BG_NEAR               },
  /* 外背景                */ { 1, PANENAME_BTLDEBUG_P2_PANE_BG_FAR,               DBGF_LABEL_BG_FAR                },
  /* AIスイッチ            */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXTBOX_02,           DBGF_LABEL_AI_SWITCH             },
  /* きほん                */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P2_05,           DBGF_LABEL_AI_0                  },
  /* つよめ                */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P2_04,           DBGF_LABEL_AI_1                  },
  /* エキスパート          */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P2_03,           DBGF_LABEL_AI_2                  },
  /* ダブル                */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P2_02,           DBGF_LABEL_AI_7                  },
  /* 手加減                */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P2_01,           DBGF_LABEL_AI_10                 },
  /* バトルロイヤル        */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P2_00,           DBGF_LABEL_AI_11                 },
  /* 乱入                  */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P2_06,           DBGF_LABEL_AI_12                 },
  /* 道具                  */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P2_07,           DBGF_LABEL_AI_13                 },
  /* 入れ替え              */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P2_08,           DBGF_LABEL_AI_14                 },
  /* 制限時間              */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXTBOX_00,           DBGF_LABEL_LIMIT_TIME            },
  /* しあい（分）          */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P3_05,           DBGF_LABEL_LIMIT_GAME_MIN        },
  /* しあい（秒）          */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P3_04,           DBGF_LABEL_LIMIT_GAME_SEC        },
  /* 持ち時間（分）        */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P3_12,           DBGF_LABEL_LIMIT_CLIENT_MIN      },
  /* 持ち時間（秒）        */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P3_13,           DBGF_LABEL_LIMIT_CLIENT_SEC      },
  /* コマンド（秒）        */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P3_03,           DBGF_LABEL_LIMIT_COMMAND         },
  /* 環境                  */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXTBOX_01,           DBGF_LABEL_ENVIRONMENT           },
  /* 天候                  */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P3_00,           DBGF_LABEL_WEATHER               },
  /* スカイバトル          */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P3_02,           DBGF_LABEL_SKY_BATTLE            },
  /* さかさバトル          */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P3_06,           DBGF_LABEL_SAKASA_BATTLE         },
  /* 必ず捕獲              */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P3_07,           DBGF_LABEL_MUST_CAPTURE          },
  /* 必ず捕獲クリティカル  */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P3_01,           DBGF_LABEL_MUST_CRITICAL_CAPTURE },
  /* リソースHIO           */ { 1, PANENAME_BTLDEBUG_P2_PANE_RESOURCEHIO,          DBGF_LABEL_RESOURCE_HIO          },
  /* 学習そうち            */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P3_08,           DBGF_LABEL_GAKUSYUSOUTI          },
  /* メガリングOFF         */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P3_10,           DBGF_LABEL_MEGA_LING_OFF         },
  /* ぜんりょくリングOFF   */ { 1, PANENAME_BTLDEBUG_P2_PANE_TEXT_P3_11,           DBGF_LABEL_Z_RING_OFF            },
  /* 図鑑をクリアしない    */ { 1, PANENAME_BTLDEBUG_P2_PANE_ZUKAN_DONTCLEAR_TEXT, DBGF_LABEL_ZUKAN_DONT_CLEAR      },
  /* 試練                  */ { 2, PANENAME_BTLDEBUG_P3_PANE_TEXT_CATEGORY_NUSI,   DBGF_CATEGORY_SIREN              },
  /* 試練                  */ { 2, PANENAME_BTLDEBUG_P3_PANE_TEXT_VS_SIREN,        DBGF_SIREN                       },
  /* vsぬし                */ { 2, PANENAME_BTLDEBUG_P3_PANE_TEXT_VS_NUSI,         DBGF_VS_NUSI                     },
  /* 援軍                  */ { 2, PANENAME_BTLDEBUG_P3_PANE_REIN_CATEGORY,        DBGF_CATEGORY_REINFORCE          },
  /* 援軍あり              */ { 2, PANENAME_BTLDEBUG_P3_PANE_REIN_FLAG,            DBGF_REINFORCE_FLAG              },
  /* 援軍ポケモン１        */ { 2, PANENAME_BTLDEBUG_P3_PANE_REIN_POKE_00,         DBGF_REINFORCE_POKE_00           },
  /* 援軍ポケモン２        */ { 2, PANENAME_BTLDEBUG_P3_PANE_REIN_POKE_01,         DBGF_REINFORCE_POKE_01           },
  /* 援軍ポケモン３        */ { 2, PANENAME_BTLDEBUG_P3_PANE_REIN_POKE_02,         DBGF_REINFORCE_POKE_02           },
  /* 援軍ポケモン４        */ { 2, PANENAME_BTLDEBUG_P3_PANE_REIN_POKE_03,         DBGF_REINFORCE_POKE_03           },
  /* 条件                  */ { 2, PANENAME_BTLDEBUG_P3_PANE_REIN_COND_00,         DBGF_REINFORCE_CONDITION         },
  /* 条件                  */ { 2, PANENAME_BTLDEBUG_P3_PANE_REIN_COND_01,         DBGF_REINFORCE_CONDITION         },
  /* 条件                  */ { 2, PANENAME_BTLDEBUG_P3_PANE_REIN_COND_02,         DBGF_REINFORCE_CONDITION         },
  /* 条件                  */ { 2, PANENAME_BTLDEBUG_P3_PANE_REIN_COND_03,         DBGF_REINFORCE_CONDITION         },
  /* 回数                  */ { 2, PANENAME_BTLDEBUG_P3_PANE_REIN_COUNT_00,        DBGF_REINFORCE_COUNT             },
  /* 回数                  */ { 2, PANENAME_BTLDEBUG_P3_PANE_REIN_COUNT_01,        DBGF_REINFORCE_COUNT             },
  /* 回数                  */ { 2, PANENAME_BTLDEBUG_P3_PANE_REIN_COUNT_02,        DBGF_REINFORCE_COUNT             },
  /* 回数                  */ { 2, PANENAME_BTLDEBUG_P3_PANE_REIN_COUNT_03,        DBGF_REINFORCE_COUNT             },
  /* ウルトラビースト      */ { 2, PANENAME_BTLDEBUG_P3_PANE_UB_CATEGORY,          DBGF_CATEGORY_UB                 },
  /* vsウルトラビースト    */ { 2, PANENAME_BTLDEBUG_P3_PANE_UB_VSUB,              DBGF_VS_UB                       },
  /* vs代表ビースト        */ { 2, PANENAME_BTLDEBUG_P3_PANE_UB_VSBOSS,            DBGF_VS_BOSS                     },
  /* 乱入                  */ { 2, PANENAME_BTLDEBUG_P3_PANE_INTRD_CATEGORY,       DBGF_CATEGORY_INTRUDE            },
  /* 乱入あり              */ { 2, PANENAME_BTLDEBUG_P3_PANE_INTRD_FLAG,           DBGF_INTRUDE_FLAG                },
  /* 必ず助けを呼ぶ        */ { 2, PANENAME_BTLDEBUG_P3_PANE_INTRD_FORCE,          DBGF_INTRUDE_FORCE_CALL          },
  /* 必ず助けが現れる      */ { 2, PANENAME_BTLDEBUG_P3_PANE_INTRD_FORCE_APPEAR,   DBGF_INTRUDE_FORCE_APPEAR        },
  /* 乱入ポケモン          */ { 2, PANENAME_BTLDEBUG_P3_PANE_INTRD_POKE,           DBGF_INTRUDE_POKE                },
  /* 便利機能              */ { 3, PANENAME_BTLDEBUG_P4_PANE_TEXT_CATEGORY,        DBGF_FUNC                        },
  /* ポケ名言語変換        */ { 3, PANENAME_BTLDEBUG_P4_PANE_TEXT_LANG,            DBGF_RESET_LANG                  },
  /* ワザチェックモード    */ { 3, PANENAME_BTLDEBUG_P4_PANE_WAZACHECK,            DBGF_WAZA_CHECK_MODE             },
  /* バトルエージェント環境*/ { 3, PANENAME_BTLDEBUG_P4_PANE_BTLFES_CATEGORY,      DBGF_BTLFES_ENV                  },
  /* 天候・フィールド      */ { 3, PANENAME_BTLDEBUG_P4_PANE_BTLFES_FIELD,         DBGF_BTLFES_FIELD                },
  /* バトルエージェントフラグ */ { 3, PANENAME_BTLDEBUG_P4_PANE_BTLFES_FLAG,       DBGF_BTLFES_FLAG                 },
};







//==============================================================================
/**
 *  Key 操作
 */
//==============================================================================
DebugBattleProc::CtrlResult DebugBattleProc::keyControl( void )
{
  if( m_uiButton->IsTrigger(gfl2::ui::BUTTON_R) )
  {
    return CTRL_RESULT_PAGE_NEXT;
  }
  if( m_uiButton->IsTrigger(gfl2::ui::BUTTON_L) )
  {
    return CTRL_RESULT_PAGE_PREV;
  }

  if( this->checkKeyForItemSelect() )
  {
    this->ChangeSelectItem( m_pointItem );
  }

  if( m_uiTp->IsTouchTrigger() )
  {
    if( !m_bLoaded ){
      return CTRL_RESULT_LOAD;
    }
    else{
      return CTRL_RESULT_BATTLE;
    }
  }

  CtrlResult result = CTRL_RESULT_NONE;
  result = this->checkKeyForItemCtrl();
  if( result != CTRL_RESULT_NONE )
  {
    return result;
  }

  if( (m_pointItem == ITEM_SAVE) && (m_uiButton->IsTrigger(gfl2::ui::BUTTON_A)) )
  {
    return CTRL_RESULT_SAVE;
  }
  if( (m_pointItem == ITEM_LOAD) && (m_uiButton->IsTrigger(gfl2::ui::BUTTON_A)) )
  {
    return CTRL_RESULT_LOAD;
  }
  if( (m_pointItem == ITEM_FUNC_RESET_LANG) && (m_uiButton->IsTrigger(gfl2::ui::BUTTON_A)) )
  {
    return CTRL_RESULT_RESET_LANG;
  }


  if( m_uiButton->IsTrigger(gfl2::ui::BUTTON_START) )
  {
    return m_uiButton->IsHold( gfl2::ui::BUTTON_L ) ? ( CTRL_RESULT_QUIT ) : ( CTRL_RESULT_BATTLE );
  }


  return CTRL_RESULT_NONE;
}

/**
 * @brief ページを切り替える
 * @param pageOffset  変更ページ数
 */
void DebugBattleProc::ChangePage( s32 pageOffset )
{
  u32  prev_page = m_page;

  m_page += pageOffset;
  if( m_page < 0 ){
      m_page =  GFL_NELEMS(PageParamTable) - 1;
  }
  if( m_page >= GFL_NELEMS(PageParamTable) ){
      m_page = 0;
  }

  // 前ページ内容を保存しておく
  if( m_page != prev_page )
  {
    for(u32 i=0; i<ITEM_MAX; ++i )
    {
      if( ItemTable[i].page == prev_page )
      {
        this->saveItem((DBItemID)i);
      }
    }
  }

  if( m_lytWk )
  {
    m_lytSys->RemoveLytWk( m_lytWk );
    m_lytWk  = setupLyt( m_lytResID, PageParamTable[m_page].lytDatIdx );
    m_pointItem = PageParamTable[m_page].defaultItem;
    m_currentItemType  = ItemTable[m_pointItem].itemType;
    m_currentItemArg   = ItemTable[m_pointItem].itemArg;
    m_currentItemStrID = ItemTable[m_pointItem].strStartIdx;
    this->ChangeSelectItem( m_pointItem );
    this->redrawCurrentPageItems();
  }
}

/**
 *  キーチェック→選択アイテムの変更があったかどうか返す
 */
bool DebugBattleProc::checkKeyForItemSelect( void )
{
  enum {
    KEY_NONE, KEY_L, KEY_R, KEY_U, KEY_D
  };

  u32 key = KEY_NONE;
  if( m_uiKey->IsRepeat(gfl2::ui::DIR_LEFT) ) { key = KEY_L; }
  else if( m_uiKey->IsRepeat(gfl2::ui::DIR_RIGHT) ){ key = KEY_R; }
  else if( m_uiKey->IsRepeat(gfl2::ui::DIR_UP) )   { key = KEY_U; }
  else if( m_uiKey->IsRepeat(gfl2::ui::DIR_DOWN) ) { key = KEY_D; }

  if( key != KEY_NONE )
  {
    for(u32 i=0; i<GFL_NELEMS(ItemTable); ++i)
    {
      if( i == static_cast<u32>( m_pointItem ) )
      {
        DBItemID nextIndex = m_pointItem;
        switch( key ){
        case KEY_L: nextIndex = ItemTable[i].l; break;
        case KEY_R: nextIndex = ItemTable[i].r; break;
        case KEY_U: nextIndex = ItemTable[i].u; break;
        case KEY_D: nextIndex = ItemTable[i].d; break;
        }
        if( nextIndex != m_pointItem )
        {
          m_pointItem        = nextIndex;
          m_currentItemType  = ItemTable[m_pointItem].itemType;
          m_currentItemArg   = ItemTable[m_pointItem].itemArg;
          m_currentItemStrID = ItemTable[m_pointItem].strStartIdx;
          return true;
        }
        break;
      }
    }
  }

  return false;

}






/**
 * @brief 編集・カット・コピー・ペースト対象のポケモンIndexとItemIDをメンバ保存
 */
void DebugBattleProc::set_modifypoke_params( void )
{
  m_modifyPokeIndex  = m_currentItemArg;
  m_modifyPokeItemNo = (DBItemID)(ITEM_P1_1 + m_modifyPokeIndex);
}


//-------------------------------------------------------
/**
 *  キーチェック→選択アイテムのコントロール
 */
//-------------------------------------------------------
DebugBattleProc::CtrlResult  DebugBattleProc::checkKeyForItemCtrl( void )
{
  switch( m_currentItemType ){
  case DBITEM_POKEMON:
    if( m_uiButton->IsTrigger(gfl2::ui::BUTTON_A) )
    {
      set_modifypoke_params();
      m_saveData->GetPokeParam( m_pokeData, m_modifyPokeIndex );
      m_saveData->GetDefaultPowerUpDesc( m_modifyPokeIndex, &m_defaultPowerUpDesc );
      return CTRL_RESULT_POKE_CREATE;
    }
    if( m_uiButton->IsTrigger(gfl2::ui::BUTTON_X) )
    {
      set_modifypoke_params();
      return CTRL_RESULT_POKE_CUT;
    }
    if( m_uiButton->IsTrigger(gfl2::ui::BUTTON_Y) )
    {
      set_modifypoke_params();
      return CTRL_RESULT_POKE_COPY;
    }
    if( m_uiButton->IsTrigger(gfl2::ui::BUTTON_B) )
    {
      set_modifypoke_params();
      return CTRL_RESULT_POKE_PASTE;
    }
    break;

  case DBITEM_NUM:
  case DBITEM_STR:
    if( m_uiButton->IsTrigger(gfl2::ui::BUTTON_A) )
    {
      if( ++(m_itemValue[m_pointItem]) >= ItemTable[m_pointItem].itemArg ){
        m_itemValue[m_pointItem] = 0;
      }
      redrawItem( (DBItemID)m_pointItem );
    }
    else if( m_uiButton->IsTrigger(gfl2::ui::BUTTON_B) )
    {
      if( --(m_itemValue[m_pointItem]) < 0 ){
        m_itemValue[m_pointItem] = ItemTable[m_pointItem].itemArg - 1;
      }
      redrawItem( (DBItemID)m_pointItem );
    }
    break;
  }
  return CTRL_RESULT_NONE;
}

//-------------------------------------------------------
/**
 *  現ページのアイテムを全て再描画
 */
//-------------------------------------------------------
void DebugBattleProc::redrawCurrentPageItems( void )
{
  for(u32 i=0; i<ITEM_MAX; ++i )
  {
    if( ItemTable[i].page == m_page )
    {
      this->redrawItem( (DBItemID)(i) );
    }
  }

  if( m_page == 0 ){
    this->redrawClipPoke( m_clipPokeData );
  }

  for( u32 i=0; i<GFL_NELEMS(LabelTable); ++i )
  {
    if( LabelTable[i].page == m_page )
    {
      gfl2::lyt::LytTextBox* pPane = m_lytWk->GetTextBoxPane( LabelTable[i].paneID );
      m_strData->GetString( LabelTable[i].strID, *m_strBuf );
      print::SetStringForLytTextBox( pPane, m_strBuf );
    }
  }
}





//-------------------------------------------------------
/**
 *  アイテム再描画
 */
//-------------------------------------------------------
void DebugBattleProc::redrawItem( DBItemID item )
{
  gfl2::lyt::LytTextBox*  pPane = m_lytWk->GetTextBoxPane( ItemTable[item].cur );

  switch( ItemTable[item].itemType ){
  case DBITEM_POKEMON:
    {
      int idx = ItemTable[item].itemArg;
      m_saveData->GetPokeParam( m_pokeData, idx );
      this->SetPokeName( item, m_pokeData );
    }
    break;

  case DBITEM_STR:
    m_strData->GetString( ItemTable[item].strStartIdx + m_itemValue[item], *m_strBuf );
    print::SetStringForLytTextBox( pPane, m_strBuf );
    break;

  case DBITEM_NUM:
    print::StrNumber( m_strBuf, m_itemValue[item], 3, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU );
    pPane->SetString( m_strBuf->GetPtr() );
    break;
  }
}

void DebugBattleProc::loadItem( DBItemID item )
{
  switch( item ){
  case ITEM_BTLMODE:
  case ITEM_MSGSPEED:
  case ITEM_CHANGERULE:
  case ITEM_STAMP_COUNT:
  case ITEM_COMMMODE:
  case ITEM_EFFENABLE:
  case ITEM_BTLHOUSE_ENABLE:
  case ITEM_RECMODE:
  case ITEM_RECIDX:
  case ITEM_TIME_GAME_M:     
  case ITEM_TIME_GAME_S:     
  case ITEM_TIME_CLIENT_M:   
  case ITEM_TIME_CLIENT_S:   
  case ITEM_TIME_COMMAND: 
  case ITEM_DEFAULT_WEATHER:
  case ITEM_BACKGRND_NEAR:
  case ITEM_BACKGRND_FAR:
  case ITEM_BGM:
  case ITEM_REINFORCE_COND_1:
  case ITEM_REINFORCE_COND_2:
  case ITEM_REINFORCE_COND_3:
  case ITEM_REINFORCE_COND_4:
  case ITEM_REINFORCE_COUNT_1:
  case ITEM_REINFORCE_COUNT_2:
  case ITEM_REINFORCE_COUNT_3:
  case ITEM_REINFORCE_COUNT_4:
    m_itemValue[item] = m_saveData->GetParam( item );
    break;

  default:
    if( (u32)(item) >= ITEM_SW_MUST_ADD && (u32)(item) < ITEM_MAX ){
      m_itemValue[item] = m_saveData->GetParam( item );
    }
  }
}

void DebugBattleProc::saveItem( DBItemID item )
{
  switch( item ){
  case ITEM_BTLMODE:
  case ITEM_MSGSPEED:
  case ITEM_CHANGERULE:
  case ITEM_STAMP_COUNT:
  case ITEM_COMMMODE:
  case ITEM_EFFENABLE:
  case ITEM_BTLHOUSE_ENABLE:
  case ITEM_RECMODE:
  case ITEM_RECIDX:
  case ITEM_TIME_GAME_M:     
  case ITEM_TIME_GAME_S:     
  case ITEM_TIME_CLIENT_M:   
  case ITEM_TIME_CLIENT_S:   
  case ITEM_TIME_COMMAND: 
  case ITEM_DEFAULT_WEATHER:
  case ITEM_BACKGRND_NEAR:
  case ITEM_BACKGRND_FAR:
  case ITEM_BGM:
  case ITEM_REINFORCE_COND_1:
  case ITEM_REINFORCE_COND_2:
  case ITEM_REINFORCE_COND_3:
  case ITEM_REINFORCE_COND_4:
  case ITEM_REINFORCE_COUNT_1:
  case ITEM_REINFORCE_COUNT_2:
  case ITEM_REINFORCE_COUNT_3:
  case ITEM_REINFORCE_COUNT_4:
    m_saveData->SetParam( item, m_itemValue[item] );
    break;
  default:
    if( (u32)(item) >= ITEM_SW_MUST_ADD && (u32)(item) < ITEM_MAX ){
      m_saveData->SetParam( item, m_itemValue[item] );
    }
    break;
  }
}

/**
 *  通信が必要なバトルモードが選ばれているか判定
 */
bool DebugBattleProc::isBattleCommMode( void )
{
  BtlMode  mode = (BtlMode)(m_itemValue[ITEM_BTLMODE]);
  BtlRule rule;
  BtlCompetitor competitor;
  BtlMultiMode  multiMode;
  bool          bComm;
  modeToParams( mode, &competitor, &rule, &multiMode, &bComm );
  return ((competitor == BTL_COMPETITOR_COMM) || bComm);
}

/**
 *  通信対戦するプレイヤー数を返す
 */
int  DebugBattleProc::getCommConnectPlayerNum( void )
{
  if( m_itemValue[ITEM_BTLMODE] == DOUBLE_COMM_MULTI || 
      m_itemValue[ITEM_BTLMODE] == ROYAL_COMM )
  {
    return 4;
  }
  return 2;
}

/**
 *  MODE->BtlParams
 */
void DebugBattleProc::modeToParams( 
  BtlMode mode,
  BtlCompetitor* competitor, 
  BtlRule* rule, 
  BtlMultiMode* multiMode, 
  bool* bComm )
{
  const struct {
    u8 rule;
    u8 competitor;
    u8 num_partys;
    u8 multi_mode;
    u8 bComm;
  }paramTbl[ BTLMODE_MAX ] = {
    /* SINGLE_WILD              */ { BTL_RULE_SINGLE,   BTL_COMPETITOR_WILD,     2, BTL_MULTIMODE_NONE,     false  },
    /* SINGLE_TRAINER           */ { BTL_RULE_SINGLE,   BTL_COMPETITOR_TRAINER,  2, BTL_MULTIMODE_NONE,     false  },
    /* SINGLE_COMM              */ { BTL_RULE_SINGLE,   BTL_COMPETITOR_COMM,     1, BTL_MULTIMODE_NONE,     true   },
    /* DOUBLE_TRAINER           */ { BTL_RULE_DOUBLE,   BTL_COMPETITOR_TRAINER,  2, BTL_MULTIMODE_NONE,     false  },
    /* DOUBLE_TRAINER_TAG       */ { BTL_RULE_DOUBLE,   BTL_COMPETITOR_TRAINER,  2, BTL_MULTIMODE_P_AA,     false  },
    /* DOUBLE_TRAINER_MULTI     */ { BTL_RULE_DOUBLE,   BTL_COMPETITOR_TRAINER,  2, BTL_MULTIMODE_PA_AA,    false  },
    /* DOUBLE_COMM              */ { BTL_RULE_DOUBLE,   BTL_COMPETITOR_COMM,     1, BTL_MULTIMODE_NONE,     true   },
    /* DOUBLE_COMM_MULT   I     */ { BTL_RULE_DOUBLE,   BTL_COMPETITOR_COMM,     1, BTL_MULTIMODE_PP_PP,    true   },
    /* DOUBLE_COMM_AI_MULTI     */ { BTL_RULE_DOUBLE,   BTL_COMPETITOR_INST,     2, BTL_MULTIMODE_PP_AA,    true   },
    /* ROYAL_TRAINER            */ { BTL_RULE_ROYAL,    BTL_COMPETITOR_TRAINER,  4, BTL_MULTIMODE_NONE,     false  },
    /* ROYAL_COMM               */ { BTL_RULE_ROYAL,    BTL_COMPETITOR_COMM,     4, BTL_MULTIMODE_P_P_P_P,  true   },
  };

  *rule       = (BtlRule)(paramTbl[mode].rule);
  *multiMode  = (BtlMultiMode)(paramTbl[mode].multi_mode);
  *bComm      = ( paramTbl[mode].bComm != 0 );

  *competitor = (BtlCompetitor)(paramTbl[mode].competitor);

  if( *competitor == BTL_COMPETITOR_TRAINER )
  {
    if( m_itemValue[ITEM_BTLHOUSE_ENABLE] )
    {
      GFL_PRINT("対戦者タイプをバトルハウスモードへ切り替え\n");
      *competitor = BTL_COMPETITOR_INST;
    }
  }
}


/**
 *  modify した ポケモンパラメータ群をパーティにセット
 */
void DebugBattleProc::setupParty( pml::PokeParty* pDst, u32 tr_idx, u32 memberMax )
{
  u32 pm_idx     = tr_idx * PARTY_MEMBER_MAX;
  u32 pm_idx_end = pm_idx + PARTY_MEMBER_MAX;
  u32 count = 0;

  pDst->Clear();

  while(pm_idx < pm_idx_end)
  {
    m_saveData->GetPokeParam( m_pokeData, pm_idx++ );
    if( !m_pokeData->IsNull() )
    {
      pDst->AddMember( *m_pokeData );
      ++count;
      if( (memberMax != 0)
      &&  (count >= memberMax) ){
        break;
      }
    }
  }
}

void DebugBattleProc::setupDefaultPowerUpDesc( BATTLE_SETUP_PARAM* setupParam, u32 tr_idx )
{
  u32 pm_idx     = tr_idx * PARTY_MEMBER_MAX;

  for( u32 i=0; i<6; ++i )
  {
    PartyParam*         partyParam  = &( setupParam->partyParam[ tr_idx ] );
    DefaultPowerUpDesc* powerUpDesc = &( partyParam->defaultPowerUpDesc[ i ] );
    m_saveData->GetDefaultPowerUpDesc( pm_idx++, powerUpDesc );
  }
}

/**
 * @brief 見せ合いデータをセットアップする
 * @param[in,out]  セットアップ対象のインスタンス
 */
void DebugBattleProc::setupMiseaiData( BATTLE_SETUP_PARAM* setupParam )
{
  for( u32 clientIndex=0; clientIndex<BTL_CLIENT_NUM; ++clientIndex )
  {
    pml::PokeParty* party = setupParam->party[ clientIndex ];

    if( party == NULL )
    {
      setupParam->miseaiData[ clientIndex ] = NULL;
      continue;
    }

    setupParam->miseaiData[ clientIndex ] = &m_miseaiData[ clientIndex ];
    setupParam->miseaiData[ clientIndex ]->partyFullMember = party;

    // 選出メンバーをクリア
    for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i )
    {
      setupParam->miseaiData[ clientIndex ]->memberSelectedIndex[ i ] = -1;
    }

    // 全メンバーを先頭から順番に選出したことにする
    u32 memberNum = party->GetMemberCount();
    for( u32 memberIndex=0; memberIndex<memberNum; ++memberIndex )
    {
      setupParam->miseaiData[ clientIndex ]->memberSelectedIndex[ memberIndex ] = memberIndex;
    }
  }
}


BtlCompetitor DebugBattleProc::setupBtlSetupParam( void )
{
  m_isRecordDataLoaded = false;

  if( m_itemValue[ITEM_RECMODE] == REC_PLAY || m_itemValue[ITEM_RECMODE] == REC_BTL0 || m_itemValue[ITEM_RECMODE] == REC_BTL1 )
  {
    GFL_PRINT("[DebugBtl] レコードモード : %d \n", m_itemValue[ITEM_RECMODE] );
    if( loadRecordData(&m_btlSetupParam, m_itemValue[ITEM_RECIDX]) )
    {
      m_isRecordDataLoaded = true;
      return m_btlSetupParam.competitor;
    }
    GFL_PRINT("録画データ SLOT:%d のロードに失敗しました!!\n", m_itemValue[ITEM_RECIDX] );
  }

  // スタンプ
  {
    // 変更前のMiscを記憶しておく
    Savedata::Misc* misc = m_gameData->GetMisc();
    const void* misc_data = misc->GetData();
    const u32 misc_size = misc->GetDataSize();
    gfl2::std::MemCopy( misc_data, m_prevsave_misc, misc_size );

    static const Savedata::TrainerPassStampID StampList[] =
    { 
      Savedata::STAMP_FLAG_ISLAND_01_CLEAR,
      Savedata::STAMP_FLAG_ISLAND_02_CLEAR,
      Savedata::STAMP_FLAG_ISLAND_03_CLEAR,
      Savedata::STAMP_FLAG_ISLAND_04_CLEAR,
      Savedata::STAMP_FLAG_ISLAND_ALL_CLEAR,
    };

    // スタンプを削除
    for( u32 i=0; i<GFL_NELEMS(StampList); ++i )
    {
      misc->ResetTrainerPassStamp( StampList[i] );
    }

    // 指定された個数のスタンプをセット
    u32 stampNum = m_itemValue[ ITEM_STAMP_COUNT ];
    stampNum = gfl2::math::Min( stampNum, GFL_NELEMS(StampList) );
    for( u32 i=0; i<stampNum; ++i )
    {
      misc->SetTrainerPassStamp( StampList[i] );
    }
  }

  {
    Savedata::ConfigSave* config = m_gameData->GetConfig();
    m_prevsave_msgSpeed   = config->GetTalkingSpeed();
    m_prevsave_effectOn   = config->GetBattleAnimation();
    m_prevsave_irekaeRule = config->GetBattleRule();

    Savedata::ConfigSave::BATTLE_ANIMATION  animVisible = (m_itemValue[ITEM_EFFENABLE] == 0)?
          Savedata::ConfigSave::BATTLE_ANIMATION_VISIBLE : Savedata::ConfigSave::BATTLE_ANIMATION_UNVISIBLE;
    config->SetBattleAnimation( animVisible );
    config->SetTalkingSpeed( (Savedata::ConfigSave::MSG_SPEED)( m_itemValue[ITEM_MSGSPEED] ) );
  }

  BtlMode  mode = (BtlMode)(m_itemValue[ITEM_BTLMODE]);
  BtlRule rule;
  BtlCompetitor competitor;
  BtlMultiMode  multiMode;
  bool          bComm;

  modeToParams( mode, &competitor, &rule, &multiMode, &bComm );

  BATTLE_SETUP_FIELD_SITUATION_Init( &m_btlFieldSit );
  m_btlFieldSit.weather = (BtlWeather)(m_itemValue[ ITEM_DEFAULT_WEATHER ]);


  switch( competitor )
  {
  case BTL_COMPETITOR_WILD:    ///< 野生
    this->setup_sub_wild( rule );
    break;

  case BTL_COMPETITOR_TRAINER: ///< ゲーム内トレーナー
    this->setup_sub_standAloneTr( rule, mode, competitor );
    break;
  case BTL_COMPETITOR_COMM:    ///< 通信対戦
    this->setup_sub_commTr( rule, mode, competitor, multiMode );
    
    break;
  case BTL_COMPETITOR_INST:    ///< バトル施設トレーナー
    if( !bComm ){
      this->setup_sub_standAloneTr( rule, mode, competitor );
    }else{
      this->setup_sub_commTr( rule, mode, competitor, multiMode );
    }
    break;
  }


  // 背景
  if( rule == BTL_RULE_ROYAL )
  {
    m_btlSetupParam.fieldSituation.bgComponent.groudnType = btl::bg::PLATE_BTLROYAL;
    m_btlSetupParam.fieldSituation.bgComponent.nearType = btl::bg::NEAR_NONE;
    m_btlSetupParam.fieldSituation.bgComponent.farType = btl::bg::FAR_NONE;
  }
  else
  {
    btl::bg::bgsys_near_typeID nearTypeId = static_cast<btl::bg::bgsys_near_typeID>( m_itemValue[ ITEM_BACKGRND_NEAR ] );
    btl::bg::bgsys_far_typeID farTypeId = static_cast<btl::bg::bgsys_far_typeID>( m_itemValue[ ITEM_BACKGRND_FAR ] );
    btl::GetBgComponentData( farTypeId, nearTypeId, &m_btlSetupParam.fieldSituation.bgComponent );
  }

  setupCommonParams( mode, competitor, rule );
  setupDefaultPowerUpDesc( &m_btlSetupParam, 0 );
  setupDefaultPowerUpDesc( &m_btlSetupParam, 1 );
  setupDefaultPowerUpDesc( &m_btlSetupParam, 2 );
  setupDefaultPowerUpDesc( &m_btlSetupParam, 3 );
  setupMiseaiData( &m_btlSetupParam );

#if 0
  {
    xy_system::bg::FarTypeId  farID = (xy_system::bg::FarTypeId)( m_itemValue[ITEM_BACKGRND] + 1 );
    if( farID == xy_system::bg::FAR_TYPE_WCS2 )
    {
      // ステータスフラグをWCS決勝に設定
      BATTLE_PARAM_SetBtlStatusFlag( &m_btlSetupParam, BTL_STATUS_FLAG_WCS_FINAL );
    }
  }
#endif
  
  return competitor;
}

/**
 * 野戦戦のサブ設定
 */
void DebugBattleProc::setup_sub_wild( BtlRule rule )
{
  u8 enemy_max = 1;

  switch( rule ){
    case BTL_RULE_DOUBLE:  enemy_max = 2; break;
  }
  setupParty( m_playerParty, 0 );
  setupParty( m_party[1], 1, enemy_max );

  BATTLE_SETUP_Wild(
    &m_btlSetupParam, 
    GFL_SINGLETON_INSTANCE( GameSys::GameManager ),
    m_party[1], 
    &m_btlFieldSit,
    (BtlRule)rule,
    static_cast<BattleEffectId>(0),
    NULL,
    m_sysHeap );
}

/**
 * スタンドアローンのトレーナー戦サブ設定
 */
void DebugBattleProc::setup_sub_standAloneTr( BtlRule rule, BtlMode mode, BtlCompetitor competitor )
{
  setupParty( m_playerParty, 0 );
  // @fix MMCat Contact[154] デバッグバトルでバトルロイヤルを行うとアサートが表示される件について
  trainer::TrainerID TRAINER_ID_FRIEND = static_cast<trainer::TrainerID>(22);
  trainer::TrainerID TRAINER_ID_ENEMY1 = static_cast<trainer::TrainerID>(1);
  trainer::TrainerID TRAINER_ID_ENEMY2 = static_cast<trainer::TrainerID>(16);

  if( m_itemValue[ITEM_SW_VS_BOSS] ) {
    TRAINER_ID_ENEMY1 = trainer::TRID_BEAST_01;
  }

  switch( mode )
  {
  case DOUBLE_TRAINER_TAG:
    BTL_SETUP_Trainer_Tag(
      &m_btlSetupParam,
      GFL_SINGLETON_INSTANCE( GameSys::GameManager ),
      TRAINER_ID_ENEMY1, TRAINER_ID_ENEMY2,
      &m_btlFieldSit, (BtlRule)rule, m_sysHeap );
    setupParty( m_btlSetupParam.party[1], 1 );
    setupParty( m_btlSetupParam.party[3], 3 );
    m_btlSetupParam.tr_data[ 1 ]->SetAIBit( calc_ai_bit() );
    break;

  case DOUBLE_TRAINER_MULTI:
  case ROYAL_TRAINER:
    BTL_SETUP_Trainer_Multi( 
      &m_btlSetupParam, 
      GFL_SINGLETON_INSTANCE( GameSys::GameManager ),
      TRAINER_ID_FRIEND,
      TRAINER_ID_ENEMY1, 
      TRAINER_ID_ENEMY2,
      &m_btlFieldSit, (BtlRule)rule, m_sysHeap );
    setupParty( m_btlSetupParam.party[1], 1 );
    setupParty( m_btlSetupParam.party[2], 2 );
    setupParty( m_btlSetupParam.party[3], 3 );
    m_btlSetupParam.tr_data[ 1 ]->SetAIBit( calc_ai_bit() );
    m_btlSetupParam.tr_data[ 2 ]->SetAIBit( calc_ai_bit() );
    m_btlSetupParam.tr_data[ 3 ]->SetAIBit( calc_ai_bit() );
    break;

  default:
    BTL_SETUP_Trainer( 
      &m_btlSetupParam,
      GFL_SINGLETON_INSTANCE( GameSys::GameManager ),
      TRAINER_ID_ENEMY1,
      &m_btlFieldSit, (BtlRule)rule, m_sysHeap );
    setupParty( m_btlSetupParam.party[1], 1 );
    m_btlSetupParam.tr_data[ 1 ]->SetAIBit( calc_ai_bit() );
    break;
  }

  // バトルエージェント設定
  if (m_itemValue[ITEM_FUNC_BTLFES_FLAG]) {
    // 仲間登録
    m_btlSetupParam.playerStatus[BTL_CLIENT_PARTNER] = GFL_NEW(m_sysHeap) Savedata::MyStatus();
    m_btlSetupParam.playerStatus[BTL_CLIENT_ENEMY2] = GFL_NEW(m_sysHeap) Savedata::MyStatus();

    // 名前保管用
    gfl2::str::StrBuf *tempStr = GFL_NEW_LOW(m_sysHeap) gfl2::str::StrBuf(System::STRLEN_PLAYER_NAME + System::EOM_LEN, m_sysHeap);
    // プリセットNPCの名前男女分メッセージデータ
    gfl2::str::MsgData* pMsgData = GFL_NEW_LOW(m_sysHeap) gfl2::str::MsgData(print::GetMessageArcId(), GARC_message_jf_window_DAT, m_sysHeap->GetLowerHandle());

    const u8 sex = m_btlSetupParam.playerStatus[BTL_CLIENT_PLAYER]->GetSex();
    pMsgData->GetString((1 + sex), *tempStr);
    gfl2::str::STRCODE name[::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN];
    tempStr->PutStr(name, ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN);
    GFL_SAFE_DELETE(pMsgData);

    m_btlSetupParam.playerStatus[BTL_CLIENT_PARTNER]->CopyFrom(*m_btlSetupParam.playerStatus[BTL_CLIENT_PLAYER]);
    m_btlSetupParam.playerStatus[BTL_CLIENT_PARTNER]->SetNameString(tempStr);
    m_btlSetupParam.playerStatus[BTL_CLIENT_PARTNER]->SetMegaRing(true);

    m_btlSetupParam.playerStatus[BTL_CLIENT_ENEMY2]->CopyFrom(*m_btlSetupParam.playerStatus[BTL_CLIENT_PLAYER]);
    m_btlSetupParam.playerStatus[BTL_CLIENT_ENEMY2]->SetNameString(tempStr);
    // メガリングなし
    GFL_SAFE_DELETE(tempStr);

    //勝利時メッセージ
    const int MsgID = 525;
    int offsetSex = (sex == PM_MALE) ? 0 : MsgID;
    m_btlSetupParam.tr_data[BTL_CLIENT_PLAYER] = GFL_NEW(m_sysHeap)BSP_TRAINER_DATA(m_sysHeap);
    m_btlSetupParam.tr_data[BTL_CLIENT_PLAYER]->SetMsgID(0, offsetSex, 0);

    m_btlSetupParam.tr_data[BTL_CLIENT_PARTNER] = GFL_NEW(m_sysHeap)BSP_TRAINER_DATA(m_sysHeap);
    m_btlSetupParam.tr_data[BTL_CLIENT_PARTNER]->SetMsgID(0, offsetSex, 0);

    m_btlSetupParam.tr_data[BTL_CLIENT_ENEMY2] = GFL_NEW(m_sysHeap)BSP_TRAINER_DATA(m_sysHeap);
    m_btlSetupParam.tr_data[BTL_CLIENT_ENEMY2]->SetMsgID(0, offsetSex, 0);

    // バトル施設モードONの必要あり(勝利演出が見れない)
    if (competitor != BTL_COMPETITOR_INST)
    {
      GFL_ASSERT_MSG(0, "NEED! BattleHouseMode ON");
      competitor = BTL_COMPETITOR_INST;
    }
  }

  m_btlSetupParam.competitor = competitor;

}

/**
 * 通信時のトレーナー戦サブ設定
 */
void DebugBattleProc::setup_sub_commTr( BtlRule rule, BtlMode mode, BtlCompetitor competitor, BtlMultiMode multiMode )
{
#ifdef  GF_PLATFORM_CTR
  // @fix MMCat Contact[154] デバッグバトルでバトルロイヤルを行うとアサートが表示される件について
  static const trainer::TrainerID TRAINER_ID_FRIEND = static_cast<trainer::TrainerID>(22);
  static const trainer::TrainerID TRAINER_ID_ENEMY1 = static_cast<trainer::TrainerID>(1);
  static const trainer::TrainerID TRAINER_ID_ENEMY2 = static_cast<trainer::TrainerID>(16);

  u8 master = 0;
  if( m_uiButton->IsHold(gfl2::ui::BUTTON_Y)){
    master = 1;
  }

  // 立ち位置：おやは常に 0
  u8 commPos = master;
  // 子機側はマルチモードに応じて
  if( m_itemValue[ITEM_COMMMODE] != COMM_PARENT )
  {
    switch( multiMode ){
    case BTL_MULTIMODE_NONE:  // 1vs1の時、子機側なら CommPos = 1
    default:
      commPos = !master;
      break;
    case BTL_MULTIMODE_PP_PP: // 通常マルチの時
    case BTL_MULTIMODE_P_P_P_P:// バトルロイヤルマルチの時
      commPos = 1 + (m_itemValue[ITEM_COMMMODE] - COMM_CHILD_1);
      break;
    case BTL_MULTIMODE_PP_AA: // 通信AIマルチの時
      commPos = 2;
      break;
    }
  }

  GFL_PRINT("[DEBUGBTL] My commPos = %d\n", commPos);
  setupParty( m_playerParty, 0 );
  BATTLE_SETUP_Comm( 
    &m_btlSetupParam, 
    this->GetGameManager(),
    &m_btlFieldSit, BTL_COMM_WIRELESS, commPos, m_sysHeap );
  m_btlSetupParam.rule = rule;

  switch( multiMode )
  {
  case BTL_MULTIMODE_PP_PP:
    GFL_PRINT("通信マルチ用の設定に変更します\n");
    BATTLE_SETUP_ToMultiMode( &m_btlSetupParam );
    break;

  case BTL_MULTIMODE_PP_AA:
    GFL_PRINT("通信AIマルチ用の設定に変更します\n");
    BATTLE_SETUP_ToAIMultiMode( &m_btlSetupParam, m_gameData, TRAINER_ID_ENEMY1, TRAINER_ID_ENEMY2, competitor, m_sysHeap );
    // 本来はこちら（↓）を呼ぶべきだが、内部で gameManager->GetBattleInst() を呼び出す都合上、デバッグバトルから呼び出すのが難しい。
    //        BATTLE_SETUP_ToAIMultiModeForBattleHouse( &m_btlSetupParam, mpGameProcManager->GetGameManager(), m_sysHeap );
    setupParty( m_btlSetupParam.party[1], 1 );
    setupParty( m_btlSetupParam.party[3], 3 );
    break;
  }

  BATTLE_PARAM_SetBtlStatusFlag( &m_btlSetupParam, BTL_STATUS_FLAG_OTEIRE_DISABLE );//お手入れ禁止
#endif
}


void DebugBattleProc::cleanupBtlSetupParam( void )
{
  if( m_isRecordDataLoaded )
  {
    ExtSavedata::BattleRecorderSaveData* recData = GFL_NEW_LOW(m_sysHeap)  ExtSavedata::BattleRecorderSaveData( m_sysHeap->GetLowerHandle(), GFL_SINGLETON_INSTANCE( GameSys::GameManager ) );
    recData->ClearBattleSetupParam( &m_btlSetupParam );
    GFL_SAFE_DELETE( recData );

    m_isRecordDataLoaded = false;
  }
  else
  {
    BATTLE_SETUP_Clear( &m_btlSetupParam );
  }

  // Miscを復元する
  {
    Savedata::Misc* misc = m_gameData->GetMisc();
    misc->SetData( m_prevsave_misc );
  }

  {
    Savedata::ConfigSave* config = m_gameData->GetConfig();
    config->SetBattleAnimation( m_prevsave_effectOn );
    config->SetTalkingSpeed( m_prevsave_msgSpeed );
    config->SetBattleRule( m_prevsave_irekaeRule );
  }
}


void DebugBattleProc::setupCommonParams( BtlMode mode, BtlCompetitor competitor, BtlRule rule )
{
  // 各種デバッグフラグを設定
  BTL_SETUP_ResetDebugFlag( &m_btlSetupParam );
  for(u32 i=0; i<BTL_DEBUGFLAG_MAX; ++i){
    if( m_itemValue[i+ITEM_SW_MUST_ADD] )
    {
      BTL_SETUP_SetDebugFlag( &m_btlSetupParam, (BtlDebugFlag)i );
    }
  }

  // 試合時間制限を設定
  u32 gameLimitSec = m_itemValue[ITEM_TIME_GAME_M]*60 + m_itemValue[ITEM_TIME_GAME_S];
  u32 clientLimitSec = m_itemValue[ITEM_TIME_CLIENT_M]*60 + m_itemValue[ITEM_TIME_CLIENT_S];
  BATTLE_PARAM_SetTimeLimit( &m_btlSetupParam, gameLimitSec, clientLimitSec, m_itemValue[ITEM_TIME_COMMAND] );

  // 特殊モードを設定
  if( m_itemValue[ITEM_SW_SKY_BTL] ){
    BATTLE_PARAM_SetSkyBattle( &m_btlSetupParam );
  }
  if( m_itemValue[ITEM_SW_SAKASA_BTL] ){
    BATTLE_PARAM_SetSakasaBattle( &m_btlSetupParam );
  }
  if( m_itemValue[ITEM_SW_MUST_CAPTURE] ){
    BATTLE_PARAM_SetMustCaptureMode( &m_btlSetupParam );
  }
  if( m_itemValue[ITEM_SW_VS_ULTRA_BEAST] ) {
    BATTLE_PARAM_SetBtlStatusFlag( &m_btlSetupParam, BTL_STATUS_FLAG_VS_ULTRA_BEAST );
    BATTLE_PARAM_SetBattleEffect( &m_btlSetupParam, BATTLE_EFFECT_WILD_SPPOKE5 );
  }
  if( m_itemValue[ITEM_SW_VS_BOSS] ) {
    BATTLE_PARAM_SetBtlStatusFlag( &m_btlSetupParam, BTL_STATUS_FLAG_VS_BOSS );
    BATTLE_PARAM_SetBattleEffect( &m_btlSetupParam, BATTLE_EFFECT_TR_CEOBEAST );
  }
  if( m_itemValue[ITEM_SW_VS_NUSI] ) {
    BATTLE_PARAM_SetBtlStatusFlag( &m_btlSetupParam, BTL_STATUS_FLAG_VS_NUSI );
    BATTLE_PARAM_SetBattleEffect( &m_btlSetupParam, BATTLE_EFFECT_WILD_SPPOKE1 );
  }
  if( m_itemValue[ITEM_SW_SIREN_BATTLE] ) {
    BATTLE_PARAM_SetBtlStatusFlag( &m_btlSetupParam, BTL_STATUS_FLAG_SIREN_BATTLE );
  }
  if( m_itemValue[ITEM_SW_FUNC_WAZA_CHECK_MODE] ) {
    m_btlSetupParam.btlvDebugParam.mWazaCheckMode = true;
  }
  if (m_itemValue[ITEM_FUNC_BTLFES_FLAG]) {
    BATTLE_PARAM_SetBtlStatusFlag(&m_btlSetupParam, BTL_STATUS_FLAG_BATTLE_FES);
    BATTLE_PARAM_SetBtlStatusFlag(&m_btlSetupParam, BTL_STATUS_FLAG_OTEIRE_DISABLE);
    m_btlSetupParam.btlFesFieldType = static_cast<BtlFesField>(m_itemValue[ITEM_FUNC_BTLFES_FIELD]);
  }
  m_btlSetupParam.bGakusyuSouti = m_itemValue[ITEM_SW_GAKUSYU];
  m_btlSetupParam.btlvDebugParam.isHioBgResource = m_itemValue[ITEM_SW_RESOURCE_HIO];
  m_btlSetupParam.btlvDebugParam.isHioEffResource = m_itemValue[ITEM_SW_RESOURCE_HIO];
  m_btlSetupParam.btlvDebugParam.isDebugBattle=1;
  
  Savedata::MyStatus* playerStatus = m_gameData->GetPlayerStatus();
  if( playerStatus )
  {
    GFL_PRINT("[DebugBtl] playerStatus を書き換える\n");
    playerStatus->SetMegaRing( !(m_itemValue[ITEM_SW_MEGARING_OFF]) );
    playerStatus->SetZenryokuRing( !(m_itemValue[ITEM_SW_ZENRYOKURING_OFF]) );
  }
  else{
    GFL_PRINT("[DebugBtl] playerStatus 取得不可\n");
  }

  // 録画バッファ
  if( m_itemValue[ITEM_RECMODE] == REC_REC ){
    GFL_PRINT("[DebugBtl] RecBuffer つくるよ mode=%d\n", m_itemValue[ITEM_RECMODE]);
    BATTLE_PARAM_AllocRecBuffer( &m_btlSetupParam, m_sysHeap );
  }else{
    GFL_PRINT("[DebugBtl] RecBuffer つくらない mode=%d\n", m_itemValue[ITEM_RECMODE]);
  }


  // 乱入
  m_btlSetupParam.intrudeParam.intrudeEnableFlag = ( m_itemValue[ ITEM_SW_INTRUDE_ENABLE ] != 0 );

  if( m_itemValue[ ITEM_SW_INTRUDE_ENABLE ] )
  {
    m_saveData->GetPokeParam( m_intrudePokeData, POKEPARAM_INTRUDE );
    if( m_intrudePokeData->GetMonsNo() != MONSNO_NULL )
    {
      m_btlSetupParam.intrudeParam.intrudePokeParam = m_intrudePokeData;
    }
  }

  // 援軍
  m_btlSetupParam.reinforceParam.reinforceEnableFlag = ( m_itemValue[ ITEM_SW_REINFORCE_ENABLE ] != 0 );

  if( m_itemValue[ ITEM_SW_REINFORCE_ENABLE ] )
  {
    for( u32 i=0; i<MAX_REINFORCE_POKE_NUM; ++i )
    {
      m_saveData->GetPokeParam( m_reinforcePokeData[i], POKEPARAM_REINFORCE_1 + i );
      if( m_reinforcePokeData[i]->GetMonsNo() != MONSNO_NULL )
      {
        m_btlSetupParam.reinforceParam.reinforcePoke[i].condition      = static_cast<ReinforceCondition>( m_itemValue[ ITEM_REINFORCE_COND_1 + i ] );
        m_btlSetupParam.reinforceParam.reinforcePoke[i].maxAppearCount = m_itemValue[ ITEM_REINFORCE_COUNT_1 + i ];
        m_btlSetupParam.reinforceParam.reinforcePoke[i].pokeParam->CopyFrom( *m_reinforcePokeData[i] );
      }
    }
  }
}


u32 DebugBattleProc::calc_ai_bit( void ) const
{
  enum {
    ITEM_AI_START = ITEM_AI_BASE,
    ITEM_AI_END   = ITEM_AI_POKE_CHANGE,
    ITEM_AI_COUNT = (ITEM_AI_END - ITEM_AI_START + 1),
  };

  u32 ai_bit = 0;

  for(u32 i=0; i<ITEM_AI_COUNT; ++i)
  {
    if( m_itemValue[ ITEM_AI_START + i ] ){
      ai_bit |= (1 << i);
    }
  }

  return ai_bit;
}












// ここからView関連コード










/**
 *  GraphicsSystem 初期化
 */
void DebugBattleProc::initGraphicSystem( void )
{
  m_sysAllocator   = GFL_NEW(m_sysHeap)  gfl2::heap::NwAllocator(m_sysHeap);
  m_devAllocator   = GFL_NEW(m_devHeap)  gfl2::heap::NwAllocator(m_devHeap);
}

/**
 * @brief レンダリングパイプラインを生成する
 */
void DebugBattleProc::CreateRenderingPipeLine( gfl2::heap::HeapBase* systemHeap, gfl2::heap::HeapBase* deviceHeap )
{
  m_renderingPipeLine = GFL_NEW( systemHeap ) AppRenderingPipeLine( deviceHeap, 512, 256 );
}

/**
 *  GraphicsSystem 終了
 */
void DebugBattleProc::quitGraphicSystem( void )
{
  if( m_devAllocator ){
    GFL_DELETE  m_devAllocator;
    m_devAllocator = NULL;
  }
  if( m_sysAllocator ){
    GFL_DELETE  m_sysAllocator;
    m_sysAllocator = NULL;
  }
}


/**
 * @brief レンダリングパイプラインを削除する
 */
void DebugBattleProc::DeleteRenderingPipeLine( void )
{
  GFL_SAFE_DELETE( m_renderingPipeLine );
}

/**
 *  2Dリソース初期化
 */
void DebugBattleProc::init2D( void )
{
  // レイアウトシステム生成
  {
    const gfl2::lyt::LytSysSetUp SetUp = {
	    GFL_SINGLETON_INSTANCE(print::SystemFont)->GetRectDrawerShaderRes(),
      10, // lyt_resource_max
      10  // lyt_wk_max
    };

    m_lytSys = GFL_NEW(m_sysHeap) gfl2::lyt::LytSys( &SetUp, m_sysAllocator, m_devAllocator, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetDrawingLytWkManager() );

    // リソース読み込み
    app::util::FileAccessor::FileOpenSync( ARCID_DEBUG_BATTLE, m_sysHeap );
    app::util::FileAccessor::FileLoadSync( ARCID_DEBUG_BATTLE, GARC_debug_battle_debug_battle_APPLYT, &m_lytDataBuffer, m_sysHeap, false, 128 );
    app::util::FileAccessor::FileCloseSync( ARCID_DEBUG_BATTLE );

    app::util::AppLytAccessor* accessor = GFL_NEW_LOW( m_sysHeap ) app::util::AppLytAccessor();
    accessor->SetAppLytBuf( m_lytDataBuffer );
    m_lytResID = m_lytSys->LoadLytRes( NULL, m_devAllocator, accessor->GetLytDat() );
    GFL_DELETE( accessor );

    // レイアウトワーク生成
    m_lytWk  = setupLyt( m_lytResID, PageParamTable[m_page].lytDatIdx );

    this->ChangeSelectItem( m_pointItem );
  }
}

/**
 *  レイアウトワーク生成
 */
gfl2::lyt::LytWk* DebugBattleProc::setupLyt( gfl2::lyt::LytMultiResID lytResID, u32 lytDatID )
{
  // レイアウトワーク生成
  gfl2::lyt::LytWkSetUp setup = {
    gfl2::math::Vector3(0,0,0),
    gfl2::math::Vector2(1.0f,1.0f),
    gfl2::math::Vector3(0,0,0),
    0,
    0,
  };

  gfl2::lyt::LytWk* lytWk = m_lytSys->CreateLytWk( &lytResID, lytDatID, NULL, 0, 0, &setup );
  lytWk->SetAutoDrawFlag( false );
  return lytWk;
}

/**
 *  2Dリソース破棄
 */
void DebugBattleProc::quit2D( void )
{
  if( m_lytSys )
  {
    if( m_lytWk )
    {
      m_lytSys->RemoveLytWk( m_lytWk );
      m_lytWk = NULL;
    }

    GFL_DELETE  m_lytSys;
    m_lytSys = NULL;
  }

  GflHeapSafeFreeMemory( m_lytDataBuffer );
}




/**
 * @brief 選択中の項目を変更する
 * @param nextItem  新しく選択する項目
 */
void DebugBattleProc::ChangeSelectItem( DBItemID nextItem )
{
  if( nextItem == m_currentItemId ) {
    return;
  }
  this->resetSelectPane( m_currentItemId );
  this->setSelectPane( nextItem );
  m_currentItemId = nextItem;
}


/**
 * @brief 選択からはずれたPaneの描画色を変更
 * @param itemId  非選択状態にする項目
 */
void DebugBattleProc::resetSelectPane( DBItemID itemId )
{
  gfl2::lyt::LytTextBox* pPane = this->GetTextBoxPane( itemId );
  if( pPane == NULL ) {
    return;
  }
  nw::ut::Color8 colorU( 154, 154, 154, 255 );
  nw::ut::Color8 colorD( 0, 0, 0, 255 );
  pPane->SetTextColor( colorU, colorD );
}


/**
 * @brief 選択されたPaneの描画色を変更
 * @param itemId  選択状態にする項目
 */
void DebugBattleProc::setSelectPane( DBItemID itemId )
{
  gfl2::lyt::LytTextBox* pPane = this->GetTextBoxPane( itemId );
  if( pPane == NULL ) {
    return;
  }
  nw::ut::Color8 color( 255, 255, 255, 255 );
  gfl2::lyt::LytPane* pWinPane = m_lytWk->GetPane( PageParamTable[m_page].lytSelPaneIdx );
  nw::lyt::Size  paneSize = pPane->GetSize();
  nw::math::VEC3 panePos  = pPane->GetTranslate();
  pWinPane->SetSize( paneSize );
  pWinPane->SetTranslate( panePos );
  pPane->SetTextColor( color, color );
}


/**
 * @brief 指定した項目のテキストボックスを取得する
 * @param itemId  テキストボックスを取得したい項目
 */
gfl2::lyt::LytTextBox* DebugBattleProc::GetTextBoxPane( DBItemID itemId )
{
  if( ITEM_MAX <= itemId ) {
    return NULL;
  }
  return m_lytWk->GetTextBoxPane( ItemTable[ itemId ].cur );
}


/**
 * @brief ClipBoardのポケモン名を更新
 * @param pokePara  名前を表示するポケモン
 */
void DebugBattleProc::redrawClipPoke( pml::pokepara::PokemonParam* pokePara )
{
  gfl2::lyt::LytTextBox*  pPane = m_lytWk->GetTextBoxPane( PANENAME_BTLDEBUG_PANE_TEXT_POKECLIP );
  this->redrawPokeNamePane( pPane, pokePara );
}


/**
 * @brief ポケモン名用のPaneを更新
 * @param pPane     表示先のペイン
 * @param pokePara  名前を表示するポケモン
 */
void DebugBattleProc::redrawPokeNamePane( gfl2::lyt::LytTextBox* pPane, pml::pokepara::PokemonParam* pokePara )
{
  if( pPane == NULL )
  {
    return;
  }

  if( pokePara->IsNull() ){
    m_strData->GetString( DBGF_ITEM_POKENAME_BLANK, *m_strBuf );
  }else{
    MonsNo  monsno = pokePara->GetMonsNo();
    pml::personal::GetMonsName( m_strBuf, monsno );
  }
  pPane->SetString( m_strBuf->GetPtr() );
}


/**
 * @brief ポケモンの名前をセットする
 * @param itemId    操作対象の項目
 * @param pokePara  表示するポケモン
 */
void DebugBattleProc::SetPokeName( DBItemID itemId, pml::pokepara::PokemonParam* pokePara )
{
  gfl2::lyt::LytTextBox*  pPane = this->GetTextBoxPane( itemId );
  this->redrawPokeNamePane( pPane, pokePara );
}



GFL_NAMESPACE_END(battle)
GFL_NAMESPACE_END(debug)

#endif // PM_DEBUG
