//=================================================================================================
/**
 * @file    JoinFestaCloseEvent.cpp
 * @brief ジョインフェスタからフィールドに遷移するイベント
 * @date 2015.09.11
 * @author ichiraku_katsuhiko
 */
//=================================================================================================
#include "NetStatic/NetEvent/include/JoinFestaCloseEvent.h"

// gflib2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "GameSys/include/GameEventManager.h" // GAMESYSTEM_EVENT_CALL
#include "NetStatic/NetLib/include/NijiNetworkSystem.h" // NijiNetworkSystem
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
// field
#include "Field/FieldStatic/include/Event/EventMapJump.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"
#include "Field\FieldStatic\include\Launcher\FieldApplicationLauncher.h"
#include <niji_conv_header/field/zone/zone_id.h>
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountManager.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldStatic/include/FieldWorld.h"
#include "FieldScript/include/FieldScriptSystem.h"

// エラーマネージャー
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

// save
#include "Savedata/include/JoinFestaDataSave.h"

// インターネット接続イベント
#include "NetApp/Connection/include/ConnectionAppParam.h"
#include "NetStatic/NetEvent/include/ConnectionEvent.h"

//　ホームスリープ制御
#include "System/include/HomeSleepAndPowerSystem.h"


GFL_NAMESPACE_BEGIN( NetEvent )

//--------------------------------------------------------------
/**
 * @brief   ジョインフェスタからフィールドに遷移するイベント呼び出し
 * 
 * @param   pGameManager  ゲームマネージャ
 * @param   isNoFade      フェードの有無(デフォルトあり）
 */
//--------------------------------------------------------------
void JoinFestaCloseEvent::StartEvent( GameSys::GameManager* pGameManager, const bool isNoFade )
{
#if defined(GF_PLATFORM_CTR)
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->LeaveJoinFesta();
#endif

  JoinFestaCloseEvent* p_event;
  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaCloseEvent>( pGameManager->GetGameEventManager() );

  // 遷移前のゾーン座標に戻る。
  Field::ZoneID zoneId = pGameManager->GetGameData()->GetStartLocation( true )->GetZoneID(); // 主人公の家の前

  EventMapJumpCore::Desc desc;
  desc.p_gdata = pGameManager->GetGameData();
  desc.location.worldId  = desc.p_gdata->GetFieldZoneDataLoader()->GetZoneWorldId(zoneId);
  desc.location.zoneId   = zoneId;
  desc.nowZoneID         = pGameManager->GetFieldmap()->GetZoneID();
  desc.location.position = pGameManager->GetGameData()->GetStartLocation( true )->GetPosition();
  desc.pcRideControlType = Field::UpdateMapData::PC_RIDECONTROL_CONNECT;// @fix GFNMCat[702]
  desc.dispPlaceName = false; // NMCat[1413]

  // マップ遷移タイプ
  desc.evMapJumpType = isNoFade ? Field::EventMapJumpCore::EV_MAPJUMP_NO_EFFECT : Field::EventMapJumpCore::EV_MAPJUMP_BLACK_FADE_DOUBLE;

  // データ更新モード
  desc.updateMapDataMode = Field::UpdateMapData::MODE_JFCLOSE;

  p_event->SetDesc( desc );
}

// コンストラクタ
JoinFestaCloseEvent::JoinFestaCloseEvent( gfl2::heap::HeapBase* pHeap ) :
    Field::EventMapJumpCore( pHeap )
{
  GFL_PRINT("JoinFestaCloseEvent 開始\n");
  if( NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
  {// インターネット接続中だった場合はイベント中ホームスリープ禁止
    System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_NETAPP_ACC );
  }
}

// デストラクタ
JoinFestaCloseEvent::~JoinFestaCloseEvent()
{
  GFL_PRINT("JoinFestaCloseEvent 終了\n");

  // @fix NMCat[3478]:インターネット切断のエラー発生対応
  {
    NetLib::Error::NijiNetworkErrorManager* pErrorManager = GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager);

    if( pErrorManager->IsError() )
    {
      // 共通通信エラー対処
      pErrorManager->CommonNetworkErrorHandling();
    }

    // 通信エラークリア
    pErrorManager->Clear();
  }


  //ホームスリープ禁止解除
  System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_NETAPP_ACC );
}

//--------------------------------------------------------------
/**
 * @brief   フィールド破棄直後に呼び出される関数
 * @param   gmgr       ゲームマネージャークラス
 * @note    フィールド遷移時に、何からの処理を挟みたい場合等にオーバライドして下さい
 */
//--------------------------------------------------------------
// @ overrride
void JoinFestaCloseEvent::OnFieldClose( GameSys::GameManager* gmgr )
{
  // インターネット接続中だった場合は切断する
  if( NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )
  {
    // インターネット切断モード
    NetApp::Connection::APP_PARAM connectionAppParam;
    connectionAppParam.bootMode = NetApp::Connection::BOOT_MODE_INTERNET_DISCONNECTION;
    NetEvent::Connection::ConnectionEvent::StartEvent( gmgr, connectionAppParam );
  }

  // @fix NMCat[253]：お店注意喚起メッセージフラグをリセット
  Savedata::JoinFestaDataSave* pSave = gmgr->GetGameData()->GetJoinFestaData();
  pSave->SetIsSkipMessageAirToy( false );
  pSave->SetIsSkipMessageStall( false );
  pSave->SetIsSkipMessageHorrorHouse( false );
}

GFL_NAMESPACE_END( NetEvent )
