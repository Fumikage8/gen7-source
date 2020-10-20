//=================================================================================================
/**
 * @file    JoinFestaOpenEvent.cpp
 * @brief フィールドからジョインフェスタに遷移するイベント
 * @date 2015.09.11
 * @author ichiraku_katsuhiko
 */
//=================================================================================================
#include "NetStatic/NetEvent/include/JoinFestaOpenEvent.h"

// gflib2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "GameSys/include/GameEventManager.h" // GAMESYSTEM_EVENT_CALL
#include "NetStatic/NetLib/include/NijiNetworkSystem.h" // NijiNetworkSystem
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalEvent/JoinFestaPersonalEvent.h" // PersonalEvent
// アトラクションマネージャ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/Event/EventMapJump.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"
#include <niji_conv_header/field/zone/zone_id.h>
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountManager.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldStatic/include/FieldWorld.h"
#include "FieldScript/include/FieldScriptSystem.h"


GFL_NAMESPACE_BEGIN( NetEvent )

//--------------------------------------------------------------
/**
 * @brief   ジョインフェスタからフィールドに遷移するイベント呼び出し
 * 
 * @param   pGameManager  ゲームマネージャ
 * @param   isCall
 * @param   isRoom        城内かどうか(false:城外 true:城内）
 * @param   pPosition     遷移後の主人公位置(NULLでデフォルト位置）
 * @param   isNoFade      フェードの有無(デフォルトあり）
 */
//--------------------------------------------------------------
GameSys::GameEvent* JoinFestaOpenEvent::StartEvent( GameSys::GameManager* pGameManager, b32 isCall,
                                                    const bool isRoom, gfl2::math::Vector3* pPosition,
                                                    const bool isNoFade )
{
  JoinFestaOpenEvent* p_event;
  if( isCall )
  {
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaOpenEvent>( pGameManager->GetGameEventManager() );
  }
  else
  {
    p_event = GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, JoinFestaOpenEvent>( pGameManager->GetGameEventManager() );
  }

  Field::ZoneID zoneId = isRoom ? Z_F0601JF0102 : Z_F0601JF0101; // ジョインフェスタに飛ぶ

  Field::EventMapJumpCore::Desc desc;
  desc.p_gdata = pGameManager->GetGameData();
  desc.location.worldId = desc.p_gdata->GetFieldZoneDataLoader()->GetZoneWorldId(zoneId);
  desc.location.zoneId  = zoneId;
  desc.nowZoneID        = pGameManager->GetFieldmap()->GetZoneID();
  desc.dispPlaceName    = false;

  // 飛んだあとの主人公位置
  if( pPosition == NULL )
  {
    // デフォルトの開始座標を取得
    Field::ZoneDataAccessor zoneDataAccessor( desc.p_gdata->GetFieldZoneDataLoader()->GetZoneData( zoneId ) );
    zoneDataAccessor.GetStartPosition( &desc.location.position );
  }
  else
  {
    desc.location.position.Set( *pPosition );
  }

  // マップ遷移タイプ
  // ジョインフェスタ遷移時専用フェード（ロトムフェード）
  // メモ：ロトムフェードインはフェードの仕組みで行えないのでロトムフェードインだけは自動で行う
  desc.evMapJumpType = isNoFade ? Field::EventMapJumpCore::EV_MAPJUMP_JF_OPEN2 : Field::EventMapJumpCore::EV_MAPJUMP_JF_OPEN;

  desc.updateMapDataMode = Field::UpdateMapData::MODE_JFOPEN;

  p_event->SetDesc( desc );

  return p_event;
}

// コンストラクタ
JoinFestaOpenEvent::JoinFestaOpenEvent( gfl2::heap::HeapBase* pHeap ) :
Field::EventMapJumpCore( pHeap )
{
  GFL_PRINT("JoinFestaOpenEvent 開始\n");
}

// デストラクタ
JoinFestaOpenEvent::~JoinFestaOpenEvent()
{
  GFL_PRINT("JoinFestaOpenEvent 終了\n");
}

void JoinFestaOpenEvent::OnInitialize( GameSys::GameManager * gmgr )
{
  //gfl2::heap::HeapBase* pEventDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

  // ※↓順番変えない

  // イベント発動抽選時間更新
  NetAppLib::JoinFesta::JoinFestaPersonalEvent::UpdateLotteryTime();

  // フィールドからジョインフェスタに飛ぶ際に、フィールドに出現するパーソナルリストを作成する
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->CreateFieldPersonalList();

  // 人物イベント抽選もリスト構築時に行う
  NetAppLib::JoinFesta::JoinFestaPersonalEvent::PersonalEventActuate();

  // アトラクション初期化
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaAttractionManager)->ClearAttraction();

  // JoinFestaモード フィールドClose完了
  gmgr->GetGameData()->SetJoinFestaManagerSwitch(false);
  // JoinFestaモード開始
  gmgr->GetGameData()->StartJoinFestaFieldData();
#if defined(GF_PLATFORM_CTR)
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->EnterJoinFesta();
#else

#if PM_DEBUG
  // Win版でも自分のデータ構築のみしておく
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* myData = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();
    myData->SetupMyself( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData() );
  }
#endif

#endif
}


void JoinFestaOpenEvent::OnFieldClose( GameSys::GameManager* gmgr )
{
}


GFL_NAMESPACE_END( NetEvent )
