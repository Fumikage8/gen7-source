#if PM_DEBUG
//==============================================================================
/**
 * @file	NetDebug.cpp
 * @brief	通信デバッグ機能
 * @author	ichiraku_katsuhiko
 * @date	2016/04/29 15:59:28
 */
// =============================================================================
#include "NetStatic/NetLib/include/Debug/NetDebug.h"

#include <base/include/gfl2_Singleton.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_DebugLogWindow.h>

#include <gflnet2/include/nex/gflnet2_NexMatchMakingClient.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// savedata
#include "Savedata/include/MyStatus.h"
#include "Savedata/include/EventWork.h"
#include "Savedata/include/JoinFestaDataSave.h"
#include "niji_conv_header/field/flagwork/WorkDefine.h"

// net
#include <NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h>
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataUtil.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include <NetStatic/NetLib/include/Validation/PokemonValidation.h>
#include "Battle/Regulation/include/RegulationScriptChecker.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

#include "NetStatic/NetEvent/include/NetworkBattleEvent.h"

#include <Debug/DebugWin/include/DebugWinSystem.h>
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

#if defined(GF_PLATFORM_CTR)

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Debug)

//==============================================================================
/**
*  @brief  
*/
//==============================================================================
NetDebug::NetDebug( gfl2::debug::DebugWinGroup* pParentGroup ) :
   m_debugDeviceManager(NULL)
  ,m_pDebugButton(NULL)
  ,m_pDataViewGroup(NULL)
  ,m_pFlagGroup(NULL)
  ,m_isDispNetLog(1)
{
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  ::Debug::DebugWin::DebugWinWork*  devWork           = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData*               pMsgData          = devWork->GetMessageData();
  gfl2::heap::HeapBase*             pHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);

  Savedata::JoinFestaDataSave* pSave = pGameManager->GetGameData()->GetJoinFestaData();
  Savedata::MyStatus* pMystatus = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus();
  Savedata::MyStatus::CORE_DATA* pCore = (Savedata::MyStatus::CORE_DATA*)pMystatus->GetData();

  /*  deviceManager  */
  m_debugDeviceManager  = pGameManager->GetUiDeviceManager();
  m_pDebugButton        = m_debugDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  // グループ作成
  // "データ確認"
  {
    m_pDataViewGroup  = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_net_data_top, pParentGroup );


    gfl2::debug::DebugWin_AddItemValueU32( m_pDataViewGroup, pHeap, L"PlayerID", &pCore->ID, pCore->ID, pCore->ID, 0);
    gfl2::debug::DebugWin_AddItemValueU8( m_pDataViewGroup, pHeap, L"RomCode", &pCore->rom_code, pCore->rom_code, pCore->rom_code, 0);
    gfl2::debug::DebugWin_AddItemValueU8( m_pDataViewGroup, pHeap, L"Sex", &pCore->sex, pCore->sex, pCore->sex, 0);
    gfl2::debug::DebugWin_AddItemValueU64( m_pDataViewGroup, pHeap, L"DataStoreID", &pCore->pssDataStoreId, pCore->pssDataStoreId, pCore->pssDataStoreId, 0);
    gfl2::debug::DebugWin_AddItemValueU64( m_pDataViewGroup, pHeap, L"NexUniqueID", &pCore->nexUniqueId, pCore->nexUniqueId, pCore->nexUniqueId, 0);
    gfl2::debug::DebugWin_AddItemValueU64( m_pDataViewGroup, pHeap, L"TransferdID", &pCore->transferdId, pCore->transferdId, pCore->transferdId, 0);
    gfl2::debug::DebugWin_AddItemValueU64( m_pDataViewGroup, pHeap, L"FirstTransFerdID", &pCore->firstTransferdId, pCore->firstTransferdId, pCore->firstTransferdId, 0);
    gfl2::debug::DebugWin_AddItemValueU32( m_pDataViewGroup, pHeap, L"PrincipalID", &pCore->principalId, pCore->principalId,pCore->principalId, 0);
    gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDataViewGroup, pHeap, L"A: ResetGameSyncID", this, DebugFunc_ResetGameSyncID, NULL );
  }

  // グループ作成
  // "フラグ"
  {
    m_pFlagGroup  = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_net_flag_top, pParentGroup );

    // 「通信ログ表示」
    gfl2::debug::DebugWin_AddItemValueU8( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_00, &m_isDispNetLog, 0, 1 )->SetChangedValueFunc( DebugFunc_DispNetLogChange, this );
    // 「通信バージョン」
    gfl2::debug::DebugWin_AddItemValueU8( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_01, &NetAppLib::JoinFesta::JoinFestaDefine::JOIN_FESTA_BEACON_VERSION, 0, 255 )->SetChangedValueFunc( DebugFunc_NetVersionChange, NULL );
    // 「交換の切断ペナルティ」
    gfl2::debug::DebugWin_AddItemValueU32( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_02, pSave->GetTradePenaltyCount(), 0, 255 );
    // 「ポケモン不正チェック：メンテナンス」
    gfl2::debug::DebugWin_AddItemValueBool( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_03, &NetLib::Validation::PokemonValidation::m_debugMaintenance );
    // 「ポケモン不正チェック：通信失敗」
    gfl2::debug::DebugWin_AddItemValueBool( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_04, &NetLib::Validation::PokemonValidation::m_debugFaileFlag );
    // 「ポケモン不正チェック：不正ポケモン」
    gfl2::debug::DebugWin_AddItemValueBool( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_05, &NetLib::Validation::PokemonValidation::m_debugInvalidPokemon );
    // 「ポケモン不正チェック：不正署名」
    gfl2::debug::DebugWin_AddItemValueBool( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_06, &NetLib::Validation::PokemonValidation::m_debugInvalidCert );
    // 「レギュレーション：不正CRC」
    gfl2::debug::DebugWin_AddItemValueBool( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_07, &Regulation::m_debugCrcBroken );
    // 「レギュレーション：不正署名」
    gfl2::debug::DebugWin_AddItemValueBool( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_08, &RegulationScriptChecker::m_debugRegulationInvalidCert );
    // 「Lボタンで通信エラー（P2P）」
    gfl2::debug::DebugWin_AddItemValueBool( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_09, &NetLib::Error::NijiNetworkErrorManager::m_debugLButtonErrorEnable );
    // 「Rボタンで通信エラー（WiFi）」
    gfl2::debug::DebugWin_AddItemValueBool( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_10, &NetLib::Error::NijiNetworkErrorManager::m_debugRButtonErrorEnable );
    // 「P2P接続で必ずエラー」
    gfl2::debug::DebugWin_AddItemValueBool( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_11, &NetLib::P2P::P2pConnectionManager::m_debugConnectError );
    // 「簡易アドレス」
    gfl2::debug::DebugWin_AddItemValueU32( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_12, &pCore->addressId, 0, 0xffffffff );
    // 「簡易アドレス（国）」//
    gfl2::debug::DebugWin_AddItemValueU8( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_15, &(((u8*)&pCore->addressId)[3]), 0, 0xff );
    // 「簡易アドレス（地域）」//
    gfl2::debug::DebugWin_AddItemValueU8( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_16, &(((u8*)&pCore->addressId)[2]), 0, 0xff );
    // 「リージョンコード」
    gfl2::debug::DebugWin_AddItemValueU8( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_13, &pCore->regionCode, 0, 0xFF );
    // 「ポケモンの言語コード」
    gfl2::debug::DebugWin_AddItemValueU8( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_14, &pCore->pokeLanguageId, 0, 0xFF );

    // 「観戦機能」
    gfl2::debug::DebugWin_AddItemValueU8( m_pFlagGroup, pHeap, pMsgData, msg_dmenu_net_flag_item_17, NetEvent::NetworkBattle::NetworkBattleEvent::GetDbgLiveRecSendFlagPrt(), 0, 1 , 1 );
  }
}

NetDebug::~NetDebug()
{
  // グループ破棄
  gfl2::debug::DebugWin_RemoveGroup( m_pFlagGroup );
  gfl2::debug::DebugWin_RemoveGroup( m_pDataViewGroup );
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ゲームシンクIDリセット
 */
/* ----------------------------------------------------------------------------------------- */
void NetDebug::DebugFunc_ResetGameSyncID( void* userWork, gfl2::debug::DebugWinItem* item )
{
  NetDebug* pThis = static_cast<NetDebug*>(userWork);
  if(pThis->m_pDebugButton->IsTrigger(gfl2::ui::BUTTON_A))
  {
    GameSys::GameManager* gman = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gman->GetGameData()->GetPlayerStatus()->SetNexUniqueID( 0 );
    gman->GetGameData()->GetSaveData()->GetGameSync()->GetGameSyncSaveDataDirect().m_registInf.m_isSigned = false;
    gman->GetGameData()->GetSaveData()->GetGameSync()->GetGameSyncSaveDataDirect().m_config.m_isUploaded = false;
    gman->GetGameData()->GetSaveData()->GetGameSync()->GetGameSyncSaveDataDirect().m_config.m_isExecuteGameSync = false;
    item->PlaySystemSound( gfl2::debug::SE_ACCEPT );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信ログ表示
 */
/* ----------------------------------------------------------------------------------------- */
void NetDebug::DebugFunc_DispNetLogChange( void* userWork )
{
  NetDebug* pThis = static_cast<NetDebug*>(userWork);
  gfl2::debug::LogWindow_SetEnable( pThis->m_isDispNetLog );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信バージョン変更
 */
/* ----------------------------------------------------------------------------------------- */
void NetDebug::DebugFunc_NetVersionChange( void* userWork )
{
  GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager )->GetMyData()->SetVersion( NetAppLib::JoinFesta::JoinFestaDefine::JOIN_FESTA_BEACON_VERSION );
#if defined(GF_PLATFORM_CTR)
  GFL_SINGLETON_INSTANCE(gflnet2::nex::NexMatchMakingClient)->SetDebugMatchmakeGroup( NetAppLib::JoinFesta::JoinFestaDefine::JOIN_FESTA_BEACON_VERSION );
#endif
}


GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(NetLib)

#endif // GF_PLATFORM_CTR

#endif // PM_DEBUG
