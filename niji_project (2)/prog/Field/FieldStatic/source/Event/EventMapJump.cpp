//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EventMapJumpCore.cpp
 *  @brief  マップ遷移イベント
 *  @author takahashi tomoya
 *  @date   2015.04.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldStatic/include/Event/EventMapJump.h"

#include "./EventFieldmapControl.h"

#include "GameSys/include/GameData.h"
#include "GameSys/include/NijiTime.h"


#include "Field/FieldStatic/include/FieldWorld.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include "Field/FieldStatic/include/Event/FieldEventPokemonRideTool.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "Field/FieldStatic/include/FieldAttribute.h"

#include "niji_conv_header/field/warpdata/FieldWarpDataType.h"

// Debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

// ふれあい
#include "Savedata/include/EventWork.h"
#include "niji_conv_header/field/flagwork/FlagDefine.h"

GFL_NAMESPACE_BEGIN(Field);

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          実装
*/
//-----------------------------------------------------------------------------

/**
 *  @brief マップジャンプ　ゾーンのデフォルト位置にジャンプする
 */
GameSys::GameEvent* EventMapJump::CallMapJumpDefaultPos( GameSys::GameManager* p_man, ZoneID zoneId )
{
  return CallMapJumpDefaultPos( p_man, zoneId, EventMapJumpCore::EV_MAPJUMP_DEFAULT );
}

GameSys::GameEvent* EventMapJump::CallMapJumpDefaultPos( GameSys::GameManager* p_man, ZoneID zoneId, EventMapJumpCore::EventMapJumpType type)
{
  return CreateMapJumpDefaultPos( p_man, zoneId, EventMapJumpCore::EV_MAPJUMP_DEFAULT, true );
}

GameSys::GameEvent* EventMapJump::CreateMapJumpDefaultPos( GameSys::GameManager* p_man, ZoneID zoneId, EventMapJumpCore::EventMapJumpType type, bool isCall /*=false*/)
{
  EventMapJumpCore::Desc desc;
  desc.p_gdata = p_man->GetGameData();
  desc.location.worldId = desc.p_gdata->GetFieldZoneDataLoader()->GetZoneWorldId(zoneId);
  desc.location.zoneId  = zoneId;
  desc.nowZoneID        = p_man->GetFieldmap()->GetZoneID();
  ZoneDataAccessor zoneDataAccessor(desc.p_gdata->GetFieldZoneDataLoader()->GetZoneData(zoneId) );
  zoneDataAccessor.GetStartPosition(&desc.location.position); // デフォルトの開始座標を取得

  // マップ遷移タイプ
  desc.evMapJumpType  = type;
  desc.pcRideControlType = UpdateMapData::PC_RIDECONTROL_CONNECT;// @fix GFNMCat[702]

  EventMapJumpCore* p_event = NULL;
  if( isCall )
  {
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventMapJumpCore>( p_man->GetGameEventManager() );
  }else{
    GameSys::GameEventManager* p_evman = p_man->GetGameEventManager( );
    p_event = GFL_NEW( p_evman->GetHeap( ) ) EventMapJumpCore( p_evman->GetHeap( ) );  
  }
  p_event->SetDesc(desc);
  return p_event;
}

/**
 *  @brief マップジャンプ　ゾーンの特定位置にジャンプする
 */
EventMapJumpCore* EventMapJump::CallMapJumpPos( GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos )
{
  return CallMapJumpPos( p_man, zoneId, pos, EventMapJumpCore::EV_MAPJUMP_DEFAULT );
}

EventMapJumpCore* EventMapJump::CallMapJumpPos( GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos, EventMapJumpCore::EventMapJumpType type, bool isDispPlaceName /*=true*/ )
{
  return CreateMapJumpPos( p_man, zoneId, pos, type, isDispPlaceName, true );
}

EventMapJumpCore* EventMapJump::CreateMapJumpPos( GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos, EventMapJumpCore::EventMapJumpType type, bool isDispPlaceName /*=true*/, bool isCall /*=false*/ )
{
  EventMapJumpCore::Desc desc;
  desc.p_gdata = p_man->GetGameData();
  desc.location.worldId = desc.p_gdata->GetFieldZoneDataLoader()->GetZoneWorldId(zoneId);
  desc.location.zoneId  = zoneId;
  desc.location.position=pos;
  desc.nowZoneID        = p_man->GetFieldmap()->GetZoneID();
  desc.dispPlaceName = isDispPlaceName;

  // マップ遷移タイプ
  desc.evMapJumpType  = type;

  desc.pcRideControlType = UpdateMapData::PC_RIDECONTROL_CONNECT;// @fix GFNMCat[702]

  EventMapJumpCore* p_event = NULL;
  if( isCall )
  {
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventMapJumpCore>( p_man->GetGameEventManager() );
  }else{
    GameSys::GameEventManager* p_evman = p_man->GetGameEventManager( );
    p_event = GFL_NEW( p_evman->GetHeap( ) ) EventMapJumpCore( p_evman->GetHeap( ) );  
  }
  p_event->SetDesc(desc);
  return p_event;
}

/**
 *  @brief 脱出マップジャンプ
 */
GameSys::GameEvent* EventMapJump::CallMapJumpEscape( GameSys::GameManager* p_man, EventMapJumpCore::EventMapJumpType type )
{
  EventMapJumpCore::Desc desc;

  const Field::Location* cp_location = p_man->GetGameData()->GetEscapeLocation();

  desc.p_gdata        = p_man->GetGameData();
  desc.location       = *cp_location;
  desc.nowZoneID      = p_man->GetFieldmap()->GetZoneID();
  desc.dispPlaceName  = true;

  // マップ遷移タイプ
  desc.evMapJumpType  = type;

  desc.pcRideControlType = UpdateMapData::PC_RIDECONTROL_ESCAPE;// @fix GFNMCat[702]

  EventMapJumpCore* p_event = NULL;
  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventMapJumpCore>( p_man->GetGameEventManager() );
  p_event->SetDesc(desc);
  return p_event;
}


/**
 *  @brief マップジャンプ　マップジャンプ配置情報を使用してジャンプする
 */
GameSys::GameEvent* EventMapJump::CallMapJumpPlacement( GameSys::GameManager* p_man, const FieldMapJumpAccessor::MapJumpData* cp_mapjump )
{
  return CallMapJumpPlacement( p_man, cp_mapjump, EventMapJumpCore::EV_MAPJUMP_DEFAULT );
}

GameSys::GameEvent* EventMapJump::CallMapJumpPlacement( GameSys::GameManager* p_man, const FieldMapJumpAccessor::MapJumpData* cp_mapjump, EventMapJumpCore::EventMapJumpType type )
{
  return CreateMapJumpPlacement( p_man, cp_mapjump, EventMapJumpCore::EV_MAPJUMP_DEFAULT, true );
}

GameSys::GameEvent* EventMapJump::CreateMapJumpPlacement( GameSys::GameManager* p_man, const FieldMapJumpAccessor::MapJumpData* cp_mapjump, EventMapJumpCore::EventMapJumpType type, bool isCall /*=false*/ )
{
  EventMapJumpCore::Desc desc;
  desc.p_gdata = p_man->GetGameData();

  // 接続情報を保存
  desc.mapjump = *cp_mapjump;
  u32 nextZoneID  = cp_mapjump->nextZoneID;

  // 今のZONEID
  desc.nowZoneID        = p_man->GetFieldmap()->GetZoneID();

  // マップ遷移タイプ
  desc.evMapJumpType  = EventMapJumpCore::EV_MAPJUMP_ENTRANCE;

  desc.pcRideControlType = UpdateMapData::PC_RIDECONTROL_CONNECT;// @fix GFNMCat[702]

  // 復帰先の設定(座標は仮）
  desc.location.worldId = desc.p_gdata->GetFieldZoneDataLoader()->GetZoneWorldId(nextZoneID);
  desc.location.zoneId  = nextZoneID;
  desc.location.position= gfl2::math::Vector3(0,0,0);

  // エントランスIN
  {
    PlacementDataManager* pPlacementDataMan = p_man->GetFieldResident()->GetPlacementDataManager();
    const PlacementMapJumpData::MapJumpParam* cpParam = pPlacementDataMan->GetMapJumpParam( cp_mapjump->mapJumpType );

    // エントランスINの演出を確定
    desc.entranceIn.mapJumpType = cp_mapjump->mapJumpType;
    desc.entranceIn.eventID     = cp_mapjump->eventID;
    desc.entranceIn.nowZoneID   = cp_mapjump->zoneID;
    desc.entranceIn.nextZoneID  = cp_mapjump->nextZoneID;
    desc.entranceIn.targetPos.Set( cp_mapjump->jumpPosition[0], cp_mapjump->jumpPosition[1], cp_mapjump->jumpPosition[2] );
    desc.entranceIn.quaternion.Set( cp_mapjump->quaternion[0], cp_mapjump->quaternion[1], cp_mapjump->quaternion[2], cp_mapjump->quaternion[3] );
    desc.entranceIn.isExistenceCameraAnime = true;
    desc.entranceIn.connectCenterPosition.Set(cp_mapjump->position[0], cp_mapjump->position[1], cp_mapjump->position[2]);
    desc.entranceIn.cameraAnimeIndex = cp_mapjump->inCamera;

    if(cpParam){
      desc.entranceIn.playSE      = cpParam->se_label;
    }else{
      desc.entranceIn.playSE      = PlacementMapJumpData::SE_LABEL_NULL;
    }

    // 出現座標の中心を0 外を100%とした場合の%を求める
    FieldMapJumpAccessor mapJumpAccessor;
    void * pData = pPlacementDataMan->GetZonePlacementData( cp_mapjump->zoneID, PlacementDataManager::PACK_IDX_MAPJUMP );
    gfl2::math::Vector3 playerPos = p_man->GetFieldmap()->GetPlayerPosition();
    GFL_ASSERT_STOP(pData); // ありえない
    mapJumpAccessor.Initialize( pData );
    desc.mapJumpPosPercent = mapJumpAccessor.GetCollisionCenterWidthPercentage( *cp_mapjump, playerPos );

    // エントランスOUT　設定できるところ設定
    desc.entranceOut.nowZoneID  = cp_mapjump->zoneID;
    desc.entranceOut.nextZoneID  = cp_mapjump->nextZoneID;
    desc.entranceOut.eventID     = cp_mapjump->nextEventID;
  }

  // イベントコール
  EventMapJumpCore* p_event = NULL;
  if( isCall )
  {
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventMapJumpCore>( p_man->GetGameEventManager() );
  }else{
    GameSys::GameEventManager* p_evman = p_man->GetGameEventManager( );
    p_event = GFL_NEW( p_evman->GetHeap( ) ) EventMapJumpCore( p_evman->GetHeap( ) );  
  }
  p_event->SetDesc(desc);
  return p_event;
}

/**
 *  @brief マップジャンプ　ゾーンの特定位置にジャンプする アローラ時間反転Ver
 */
GameSys::GameEvent* EventMapJump::CallMapJumpOtherWorldTime( GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos, GameSys::AlolaTime::AlolaOtherWorldType otherworld_type )
{
  return CreateMapJumpOtherWorldTime( p_man, zoneId, pos, otherworld_type, EventMapJumpCore::EV_MAPJUMP_DEFAULT, true, true );
}

GameSys::GameEvent* EventMapJump::CallMapJumpOtherWorldTime( GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos, GameSys::AlolaTime::AlolaOtherWorldType otherworld_type, EventMapJumpCore::EventMapJumpType type )
{
  return CreateMapJumpOtherWorldTime( p_man, zoneId, pos, otherworld_type, EventMapJumpCore::EV_MAPJUMP_DEFAULT, true, true );
}

EventMapJumpCore* EventMapJump::CreateMapJumpOtherWorldTime( GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos, GameSys::AlolaTime::AlolaOtherWorldType otherworld_type, EventMapJumpCore::EventMapJumpType type, bool isDispPlaceName /*=true*/, bool isCall /*=false*/ )
{
  EventMapJumpCore::Desc desc;
  desc.p_gdata = p_man->GetGameData();
  desc.location.worldId = desc.p_gdata->GetFieldZoneDataLoader()->GetZoneWorldId(zoneId);
  desc.location.zoneId  = zoneId;
  desc.location.position=pos;
  desc.nowZoneID        = p_man->GetFieldmap()->GetZoneID();
  desc.dispPlaceName = isDispPlaceName;

  // マップ遷移タイプ
  desc.evMapJumpType  = type;

  desc.pcRideControlType = UpdateMapData::PC_RIDECONTROL_CLEAR;// @fix GFNMCat[702]

  EventMapJumpOtherWorldTime* p_event = NULL;
  if( isCall )
  {
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventMapJumpOtherWorldTime>( p_man->GetGameEventManager() );
  }else{
    GameSys::GameEventManager* p_evman = p_man->GetGameEventManager( );
    p_event = GFL_NEW( p_evman->GetHeap( ) ) EventMapJumpOtherWorldTime( p_evman->GetHeap( ) );  
  }
  p_event->SetDesc(desc);
  p_event->SetOtherWorldType( otherworld_type );

  return p_event;
}



//==============================================================================
EventMapJumpCore::EventMapJumpCore( gfl2::heap::HeapBase* heap ) : 
  GameSys::GameEvent(heap),
  m_pWorld(NULL),
  m_pArea( NULL ),            ///< Coverity.12004
  m_Desc()
{

}

EventMapJumpCore::~EventMapJumpCore()
{

}

//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool EventMapJumpCore::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventMapJumpCore::InitFunc(GameSys::GameManager* gmgr)
{
#if PM_DEBUG
  if( m_Desc.evMapJumpType == EV_MAPJUMP_JF_OPEN && m_Desc.updateMapDataMode != UpdateMapData::MODE_JFOPEN )
  {
    GFL_ASSERT_MSG(0,"MAPJUMP_JF_OPENがジョインフェスタ以外で使用された\n");
  }
  if( m_Desc.evMapJumpType == EV_MAPJUMP_JF_OPEN2 && m_Desc.updateMapDataMode != UpdateMapData::MODE_JFOPEN )
  {
    GFL_ASSERT_MSG(0,"MAPJUMP_JF_OPEN2がジョインフェスタ以外で使用された\n");
  }
#endif

  // イベント初期化直前の独自処理
  this->OnInitialize(gmgr);

  m_pWorld = gmgr->GetGameData()->GetFieldWorld();
  m_pArea = gmgr->GetGameData()->GetFieldArea();
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventMapJumpCore::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_FIELD_CLOSE,          ///< フィールド終了
    SEQ_FIELD_CLOSE_WAIT,     ///< フィールド終了まち

     
    SEQ_MAPUPDATE_INITIALIZE,       ///< マップ情報更新
    SEQ_MAPUPDATE_INITIALIZE_WAIT,  ///< マップ情報更新待ち

    SEQ_FIELD_OPEN,           ///< フィールド開始
    SEQ_FIELD_OPEN_WAIT,      ///< フィールド開始まち
  };

  switch(this->GetSeqNo())
  {
  case SEQ_FIELD_CLOSE:          ///< フィールド終了
    {
      // ゾーンIDの保存
      gmgr->GetGameData()->GetSituation()->SetLastZoneID( gmgr->GetFieldmap()->GetZoneID() );

      // 遷移前のロケーションを保存
      saveEscapeLocation(gmgr);


      EventFieldmapClose* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapClose>( gmgr->GetGameEventManager() );
      EventFieldmapClose::Desc desc;
      desc.closeType = getFmCloseMode();
      p_event->SetDesc( desc );

      // 演出タイプを設定
      switch( m_Desc.evMapJumpType ){
      case EV_MAPJUMP_DEFAULT:     ///< ブラックフェード(短)
        p_event->SetFadeCallBackDefaultUpperOnly();
        break;
      case EV_MAPJUMP_BLACK_FADE:     ///< ブラックフェード(イベント演出用)
        p_event->SetFadeCallBack( FldMapCtrlCallback_FadeOutSetupBlackFadeUpperOnly, FldMapCtrlCallback_FadeIsEndDefault, NULL );
        break;
      case EV_MAPJUMP_NO_EFFECT:     ///< 演出なし
        p_event->SetFadeCallBack( NULL, NULL, NULL );
        break;
      case EV_MAPJUMP_ENTRANCE:      ///< エントランス系演出あり
        p_event->SetFadeCallBack( FldMapCtrlCallback_FadeInSetupEntranceIn, NULL, &m_Desc.entranceIn );
        break;
      case EV_MAPJUMP_BLACK_FADE_DOUBLE:
        p_event->SetFadeCallBackDefault();
        break;
      case EV_MAPJUMP_JF_OPEN:       ///< ジョインフェスタ遷移時専用フェード（フェードアウト有り、ロトムフェードイン有り）
        p_event->SetFadeCallBackDefault();
        break;
      case EV_MAPJUMP_JF_OPEN2:      ///< ジョインフェスタ遷移時専用フェード（フェードアウト無し、ロトムフェードイン有り）
        p_event->SetFadeCallBack( NULL, NULL, NULL );
        break;
      default:
        break;
      }

      this->AddSeqNo();
    }
    break;
  case SEQ_FIELD_CLOSE_WAIT:     ///< フィールド終了まち
    {
      // フィールドクローズ終了後独自処理
      this->OnFieldClose( gmgr );
      this->AddSeqNo();
    }
    // break;through

  case SEQ_MAPUPDATE_INITIALIZE:     ///< マップデータ更新開始
    {
      UpdateMapData::Desc desc;
      desc.p_gman = gmgr;
      desc.location = m_Desc.location;
      desc.bUpdateEvTime = m_Desc.bUpdateEvTime;
      desc.mode = m_Desc.updateMapDataMode;
      desc.pc_ride_control = m_Desc.pcRideControlType;// @fix GFNMCat[702]
      m_UpdateMapData.Setup( desc );

      this->AddSeqNo();
    }
    // break;through

  case SEQ_MAPUPDATE_INITIALIZE_WAIT: ///< エリア情報読み込みまち
    {
      if( m_UpdateMapData.Update() == false )
      {
        break;
      }

      //ゾーンBGMの再生準備
      {
        Field::FieldSound* pFieldSound = gmgr->GetGameData()->GetFieldSound();
        pFieldSound->SetCurrentZoneID(m_Desc.location.zoneId);
        if( !m_Desc.bgmKeepFlag ){
          pFieldSound->PrepareNextFieldBGM();
        }
      }

      // 接続先の情報を取得
      if( m_Desc.evMapJumpType == EV_MAPJUMP_ENTRANCE )
      {
        FieldResident* pFieldResident = gmgr->GetFieldResident();
        PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();


        // ここでOUTの座標を確定
        FieldMapJumpAccessor mapJumpAccessor;
        const FieldMapJumpAccessor::MapJumpData* cp_mapjump = NULL;

        void * pData = pPlacementDataManager->GetZonePlacementData( m_Desc.location.zoneId, PlacementDataManager::PACK_IDX_MAPJUMP );
        GFL_ASSERT_STOP(pData); // ありえない
        mapJumpAccessor.Initialize( pData );

        for( u32 i=0; i<mapJumpAccessor.GetDataMax(); ++i ){
          const FieldMapJumpAccessor::MapJumpData* check_data;
          check_data = mapJumpAccessor.GetData(i);
          if( check_data->eventID == m_Desc.mapjump.nextEventID ){
            cp_mapjump = check_data;
            break;
          }
        }

        GFL_ASSERT_STOP_MSG( cp_mapjump, "MapJump 飛び先のイベントデータが見つかりません。" );
        m_Desc.entranceOut.mapJumpType = cp_mapjump->mapJumpType;
        m_Desc.entranceOut.connectCenterPosition = gfl2::math::Vector3( cp_mapjump->position[0], cp_mapjump->position[1], cp_mapjump->position[2] );
        m_Desc.location.position= gfl2::math::Vector3( cp_mapjump->jumpPosition[0], cp_mapjump->jumpPosition[1], cp_mapjump->jumpPosition[2] );
        m_Desc.entranceOut.jumpCenterPosition = m_Desc.location.position; // @fix GFNMCat[1032] 出現位置を保存
        m_Desc.location.quaternion = gfl2::math::Quaternion( cp_mapjump->quaternion[0], cp_mapjump->quaternion[1], cp_mapjump->quaternion[2], cp_mapjump->quaternion[3] );

        // 接続前の座標の割合%から出現座標を補正
        const PlacementMapJumpData::MapJumpParam* cpMapJumpParam = pPlacementDataManager->GetMapJumpParam( cp_mapjump->mapJumpType );
        if( cpMapJumpParam )
        {
          if( !cpMapJumpParam->is_pos_center )
          {
            gfl2::math::Vector3 jumpPos = gfl2::math::Vector3( cp_mapjump->jumpPosition[0], cp_mapjump->jumpPosition[1], cp_mapjump->jumpPosition[2] );
            mapJumpAccessor.GetCollisionCenterWidthPercentagePosition( *cp_mapjump, m_Desc.mapJumpPosPercent, jumpPos, &m_Desc.location.position );
          }
        }
        

        m_Desc.entranceOut.rotate         = m_Desc.location.quaternion;
        m_Desc.entranceOut.outPos      = m_Desc.location.position;
        m_Desc.entranceOut.isExistenceCameraAnime = true;             //@fix GFNMCat[1964]
        m_Desc.entranceOut.cameraAnimeIndex = cp_mapjump->outCamera;  //@fix GFNMCat[1964]
      }

      this->AddSeqNo();
    }
    // break through

  case SEQ_FIELD_OPEN:           ///< フィールド開始
    {
#if PM_DEBUG
      gfl2::math::Vector3  controlPos = m_Desc.location.GetPosition();
      const World::ZoneInfo * pZone = m_pWorld->IsHitZoneInfo( controlPos );
      if(pZone->m_ZoneID != m_Desc.location.GetZoneID())
      {
        GFL_ASSERT_MSG(0,"***PositionSettingError :: PosToZone=%d, SetZone=%d, (%f,%f,%f)\n",pZone->m_ZoneID, m_Desc.location.GetZoneID(), controlPos.x, controlPos.y, controlPos.z);
      }
#endif
      {
        FieldScript::FieldScriptSystem* p_script = gmgr->GetFieldScriptSystem();

        //SceneChangeテーブルでのフェードオプション反映
        #if PM_DEBUG
        if ( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_SCENESCRIPT ) )
        #endif //PM_DEBUG
        {
          // シーン起動スクリプト発動チェック＆RUN
          if( p_script->CheckSceneScriptFadeMode() == SCENE_CHANGE_FADE_MANUAL )
          {
            //@fix GFNMCat[2344][2345] typeがエントランスタイプの時のみセーフティをかけるのが正しい
            if( m_Desc.evMapJumpType == EV_MAPJUMP_ENTRANCE )
            {
              //@fix GFNMCat[2311] EntranceOutをスキップしちゃうので、EntranceOutで行う裏世界セーフティ処理をここで実行してしまう
              m_Desc.location.position = m_Desc.entranceOut.jumpCenterPosition;
            }
            m_Desc.evMapJumpType = EV_MAPJUMP_NO_EFFECT;
            m_Desc.dispPlaceName = false; //地名表示も抑制
          }
        }
      }
      EventMapJumpType type = m_Desc.evMapJumpType;
      FieldmapDesc desc;
      desc.openMode       = getFmOpenMode();
      desc.playerLocation = m_Desc.location;  ///< 出現先指定

      EventFieldmapOpen* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen>( gmgr->GetGameEventManager() );
      p_event->SetDesc(desc);

      // 演出タイプを設定
      switch( type ){
      case EV_MAPJUMP_DEFAULT:     ///< ブラックフェード
        p_event->SetFadeCallBackDefaultUpperOnly();
        break;
      case EV_MAPJUMP_BLACK_FADE:     ///< ブラックフェード(イベント演出用)
        p_event->SetFadeCallBack( FldMapCtrlCallback_FadeInSetupBlackFadeUpperOnly, FldMapCtrlCallback_FadeIsEndDefault, NULL );
        break;
      case EV_MAPJUMP_NO_EFFECT:     ///< 演出なし
        p_event->SetFadeCallBack( NULL, NULL, NULL );
        break;
      case EV_MAPJUMP_ENTRANCE:      ///< エントランス系演出あり
        p_event->SetFadeCallBack( FldMapCtrlCallback_FadeInSetupEntranceOut, NULL, &m_Desc.entranceOut );
        break;
      case EV_MAPJUMP_BLACK_FADE_DOUBLE:///< 両画面ブラックフェード（極短）
        p_event->SetFadeCallBackDefault();
        break;
      case EV_MAPJUMP_JF_OPEN:       ///< ジョインフェスタ遷移時専用フェード（ロトムフェード）
      case EV_MAPJUMP_JF_OPEN2:      ///< ジョインフェスタ遷移時専用フェード（フェードアウト無し、ロトムフェードイン有り）
        p_event->SetFadeCallBackJfOpen();
        break;

      default:
        break;
      }
      //ゾーンBGMの再生
      if( !m_Desc.bgmKeepFlag ){
        Field::FieldSound* pFieldSound = gmgr->GetGameData()->GetFieldSound();
        pFieldSound->PreparePlayNextFieldBGM();
      }
      this->AddSeqNo();
    }
    break;
  case SEQ_FIELD_OPEN_WAIT:      ///< フィールド開始まち
    if( gmgr->GetFieldmap() && m_Desc.dispPlaceName )
    {
      gmgr->GetFieldmap()->GetEffectManager()->RequestStartPlaceName( false );
    }
    return GameSys::GMEVENT_RES_FINISH;

  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventMapJumpCore::EndFunc(GameSys::GameManager* gmgr)
{

}

//--------------------------------------------------------------
/**
 * @brief   遷移前のロケーションを保存
 */
//--------------------------------------------------------------
void EventMapJumpCore::saveEscapeLocation(GameSys::GameManager* gmgr)
{
  // ロケーション保存ゾーンか？
  Field::ZoneDataAccessor zone_accessor( m_Desc.p_gdata->GetFieldZoneDataLoader()->GetZoneData( gmgr->GetFieldmap()->GetZoneID() ) );
  if( zone_accessor.IsEscapeSaveEnable() && m_Desc.saveEscape )
  {
    Field::Location location;
    location.Initialize();
    gmgr->GetFieldmap()->GetPlayerLocation(&location);

    // MCat[201]あなぬけのひもで脱出した座標が、接続のコリジョンにヒットしている。
    if( m_Desc.evMapJumpType == EV_MAPJUMP_ENTRANCE )
    {
      location.position.Set( m_Desc.mapjump.jumpPosition[0], m_Desc.mapjump.jumpPosition[1], m_Desc.mapjump.jumpPosition[2] );
    }
    else
    {
      // @caution POS起動のMapJumpでエントランスセーブするケースへの対処をどうするか？
      // 2016/01/29は発生する箇所なし。
      // 2016/05/26は発生する箇所なし。
    }

    RIDE_POKEMON_ID now_id = EventPokemonRideTool::GetPokemonRideOnID(gmgr);
    RIDE_POKEMON_ID set_id = RIDE_NO_POKEMON;

    // 波乗り
    if( EventPokemonRideTool::IsNaminori(now_id) )
    {
      set_id = now_id;
    }
    // 地面馬
    else if( (now_id == RIDE_MODEL_ROBA2) && Attribute::IsRoba2Ground( gmgr->GetFieldmap()->GetPlayerCharacter()->GetGroundAttributeLastSuccesful() ) )
    {
      set_id = now_id;
    }

    m_Desc.p_gdata->SetEscapeLocation( &location, set_id );
  }

  // 下画面用マップ用座標を保存か？
  if( zone_accessor.IsSubscreenOutsideMapMode() )
  {
    Savedata::FieldMenu* p_sv_menu = m_Desc.p_gdata->GetFieldMenu();
    gfl2::math::Vector3 pos = gmgr->GetFieldmap()->GetPlayerPosition();
    p_sv_menu->SetMapPosition( pos.GetX(), pos.GetZ() );
  }

}

/// Fieldmapオープンモード取得
FieldmapOpenMode EventMapJumpCore::getFmOpenMode(void) const
{
  if( m_Desc.updateMapDataMode == UpdateMapData::MODE_JFCLOSE )
  {
    // ジョインフェスタから戻るときは、リカバリーとして動く
    return FM_OPEN_MODE_APP_RECOVER;
  }
  return FM_OPEN_MODE_INIT;
}

/// Fieldmapクローズモードの取得
FieldmapCloseMode EventMapJumpCore::getFmCloseMode(void) const
{
  switch( m_Desc.updateMapDataMode )
  {
  case UpdateMapData::MODE_MAPJUMP:                   /// マップジャンプ
  case UpdateMapData::MODE_CONTINUE_ZONECHANGE:       ///< コンティニュー  ゾーン変動
  case UpdateMapData::MODE_CONTINUE_NO_ZONECHANGE:    ///< コンティニュー  ゾーン変動なし
    return FM_CLOSE_MODE_MAPJUMP;

  case UpdateMapData::MODE_JFCLOSE:                   ///< JFCLOSE
    return FM_CLOSE_MODE_JFCLOSE;

  case UpdateMapData::MODE_JFOPEN:                    ///< JFOPEN
    return FM_CLOSE_MODE_JFOPEN;

  default :
    GFL_ASSERT(0);
    break;
  }
  return FM_CLOSE_MODE_MAPJUMP;
}


//------------------------------------------------------------------------------
/**
 * @class       EventMapJumpOtherWorldTime
 * @brief       アローラ時間が12時間反転した世界へのジャンプ
 */
//------------------------------------------------------------------------------
void EventMapJumpOtherWorldTime::OnFieldClose(GameSys::GameManager* gmgr)
{
  // フラグを変更
  if( !GameSys::AlolaTime::CheckAlolaOtherWorld(gmgr->GetGameData()) )
  {
    GFL_ASSERT( m_OtherWorldType < GameSys::AlolaTime::ALOLA_OTHER_WOLRD_TYPE_MAX );
    GameSys::AlolaTime::StartAlolaOtherWorldOffset(gmgr->GetGameData(), m_OtherWorldType);
  }
  else
  {
    GFL_ASSERT( m_OtherWorldType ==  GameSys::AlolaTime::ALOLA_OTHER_WOLRD_TYPE_MAX );
    GameSys::AlolaTime::ClearAlolaOtherWorldOffset(gmgr->GetGameData());
  }
}


//------------------------------------------------------------------------------
/**
 * @func        CallMapJumpGameOver
 * @brief       CallMapJumpGameOver関数.
 * @author      N.Takeda
 * @date        2015/07/16(木) 11:41:19
 *
 * @param[in,out] GameSys::GameManager* p_man .
 *
 * @return      GameSys::GameEvent* 
 */
//------------------------------------------------------------------------------
GameSys::GameEvent* EventMapJump::CallMapJumpGameOver( GameSys::GameManager* p_man )
{
  /// 家の前
  //ZoneID                zoneId = 14;  // Z_F01C0103
  //gfl2::math::Vector3   pos(37614, -979, 26596);
  /// 家の中
  //ZoneID                zoneId = 19;  // Z_F01R0101
  //gfl2::math::Vector3   pos(2412, 1, 2145);
  
  return CallMapJumpGameOver( p_man, EventMapJumpGameOver::EV_MAPJUMP_FADE_NONE );
}

GameSys::GameEvent* EventMapJump::CallMapJumpGameOver( GameSys::GameManager* p_man, EventMapJumpGameOver::EventMapJumpType type)
{
  return CreateMapJumpGameOver( p_man, type, true );
}

//------------------------------------------------------------------------------
/**
 * @func        CreateMapJumpGameOver
 * @brief       MapJumpGameOverの作成.
 * @author      N.Takeda
 * @date        2015/07/15(水) 15:02:26
 *
 * @param[in,out] GameSys::GameManager*                   p_man   .
 * @param[in]     EventMapJumpGameOver::EventMapJumpType  type    タイプ.
 * @param[in,out] bool                                    isCall  .
 *
 * @return      GameSys::GameEvent* 
 */
//------------------------------------------------------------------------------
GameSys::GameEvent* EventMapJump::CreateMapJumpGameOver( GameSys::GameManager* p_man, EventMapJumpGameOver::EventMapJumpType type, bool isCall /*=false*/ )
{
  EventMapJumpGameOver::Desc desc;
  desc.p_gdata = p_man->GetGameData();
//  desc.location.worldId   = desc.p_gdata->GetFieldZoneDataLoader()->GetZoneWorldId(zoneId);
//  desc.location.zoneId    = zoneId;
//  desc.location.position  = pos;
  desc.nowZoneID          = 0;

  // マップ遷移タイプ
  desc.evMapJumpType  = type;

  EventMapJumpGameOver* p_event = NULL;
  if( isCall )
  {
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventMapJumpGameOver>( p_man->GetGameEventManager() );
  }else{
    GameSys::GameEventManager* p_evman = p_man->GetGameEventManager( );
    p_event = GFL_NEW( p_evman->GetHeap( ) ) EventMapJumpGameOver( p_evman->GetHeap( ) );  
  }
  p_event->SetDesc(desc);
  return p_event;
}


//==============================================================================
EventMapJumpGameOver::EventMapJumpGameOver( gfl2::heap::HeapBase* heap ) : 
  GameSys::GameEvent(heap),
  m_pWorld(NULL),
  m_pArea( NULL ),            ///< Coverity.12004
  m_Desc()
{

}

EventMapJumpGameOver::~EventMapJumpGameOver()
{

}

//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool EventMapJumpGameOver::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventMapJumpGameOver::InitFunc(GameSys::GameManager* gmgr)
{

  m_pWorld = gmgr->GetGameData()->GetFieldWorld();
  m_pArea = gmgr->GetGameData()->GetFieldArea();
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventMapJumpGameOver::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_RESIDENT_INITIALIZE,
    SEQ_RESIDENT_INITIALIZE_WAIT,


    SEQ_UPDATE_MAPDATA,       ///<

    SEQ_FIELD_OPEN,             ///< フィールド開始
    SEQ_FIELD_OPEN_WAIT,        ///< フィールド開始まち
  };

  switch(this->GetSeqNo())
  {
  case SEQ_RESIDENT_INITIALIZE:          ///< フィールド終了
    {
      FieldResident* pFieldResident = gmgr->GetFieldResident();
      
      gfl2::heap::HeapBase* pAppHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      gfl2::heap::HeapBase* pAppHeapDevice = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      pFieldResident->Initialize( pAppHeap, pAppHeapDevice, gmgr->GetAsyncFileManager(), *gmgr->GetGameData() );

      //@fix NMCat[1413] 元いたゾーンIDを保存する
      gmgr->GetGameData()->GetSituation()->SetLastZoneID( gmgr->GetGameData()->GetNowZoneID() );
    }
    this->AddSeqNo();
    break;
  case SEQ_RESIDENT_INITIALIZE_WAIT:     ///< フィールド終了まち
    {
      FieldResident* pFieldResident = gmgr->GetFieldResident();
      bool resident_result = pFieldResident->InitializeWait();
      if( resident_result == false )
      {
        break;
      }
      WarpDataTools         *pWarpData = gmgr->GetGameData()->GetWarpDataTools();
      
      ZoneID                zoneId;
      int                   warp_id = gmgr->GetGameData()->GetWarpID()-1;
      /// ワープデータから復活先を取得.
      pWarpData->GetRecoverPoint( warp_id, &zoneId, &m_Desc.location.position );
      m_Desc.location.worldId   = m_Desc.p_gdata->GetFieldZoneDataLoader()->GetZoneWorldId( zoneId );
      m_Desc.location.zoneId    = zoneId;

      // 
      UpdateMapData::Desc desc;
      desc.p_gman = gmgr;
      desc.location = m_Desc.location;
      desc.bUpdateEvTime = true;
      desc.mode = UpdateMapData::MODE_MAPJUMP;
      desc.pc_ride_control = UpdateMapData::PC_RIDECONTROL_CLEAR;// @fix GFNMCat[702]

      m_UpdateMapData.Setup(desc);
      
      this->AddSeqNo();
    }
    // break;through

  case SEQ_UPDATE_MAPDATA:       ///<
    if( m_UpdateMapData.Update() == false )
    {
      break;
    }

    //ゾーンBGMの再生準備
    {
      Field::FieldSound* pFieldSound = gmgr->GetGameData()->GetFieldSound();
      pFieldSound->SetCurrentZoneID(m_Desc.location.zoneId);
      pFieldSound->PrepareNextFieldBGM();
    }

    this->AddSeqNo();
    break;
   

  case SEQ_FIELD_OPEN:           ///< フィールド開始
    {
      FieldmapDesc desc;
      desc.openMode       = FM_OPEN_MODE_INIT;
      desc.playerLocation = m_Desc.location;  ///< 出現先指定

      EventFieldmapOpen* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen>( gmgr->GetGameEventManager() );
      p_event->SetDesc(desc);

      // 演出タイプを設定
      switch( m_Desc.evMapJumpType ){
      case EV_MAPJUMP_BLACK_FADE:     ///< ブラックフェード(イベント演出用)
        p_event->SetFadeCallBack( FldMapCtrlCallback_FadeInSetupBlackFade, FldMapCtrlCallback_FadeIsEndDefault, NULL );
        break;
      default:
        p_event->SetFadeCallBack( NULL, NULL, NULL );
        break;
      }
      //ゾーンBGMの再生
      {
        Field::FieldSound* pFieldSound = gmgr->GetGameData()->GetFieldSound();
        pFieldSound->PreparePlayNextFieldBGM();
      }
      this->AddSeqNo();
    }
    break;
  case SEQ_FIELD_OPEN_WAIT:      ///< フィールド開始まち
    return GameSys::GMEVENT_RES_FINISH;

  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventMapJumpGameOver::EndFunc(GameSys::GameManager* gmgr)
{

}


//------------------------------------------------------------------------------
/**
 * @func        CallMapJumpPosWithoutField
 * @brief       CallMapJumpPosWithoutField関数.
 * @author      Miyuki Iwasawa
 * @date        2015/11/15(月)
 *
 * @param[in,out] GameSys::GameManager* p_man .
 *
 * @return      GameSys::GameEvent* 
 */
//------------------------------------------------------------------------------
GameSys::GameEvent* EventMapJump::CallMapJumpPosWithoutField(
  GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos )
{
  return CallMapJumpPosWithoutField( p_man, zoneId, pos, EventMapJumpWithoutField::EV_MAPJUMP_BLACK_FADE );
}

GameSys::GameEvent* EventMapJump::CallMapJumpPosWithoutField(
  GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos,
  EventMapJumpWithoutField::EventMapJumpType type, bool isDispPlaceName /*= true*/, bool bgmKeepFlag /*= false*/ )
{
  return CreateMapJumpPosWithoutField( p_man, zoneId, pos, type, isDispPlaceName, bgmKeepFlag, true );
}

GameSys::GameEvent* EventMapJump::CallMapJumpPosWithoutField(
  GameSys::GameManager* p_man, const u32 warp_id, WarpType warp_type,
  EventMapJumpWithoutField::EventMapJumpType type, bool isDispPlaceName /*= true*/, bool bgmKeepFlag /*= false*/ )
{
  WarpDataTools *pWarpData = p_man->GetGameData()->GetWarpDataTools();

  ZoneID                zoneId;
  gfl2::math::Vector3   pos;

  /// ワープデータから復活先を取得.
  if (warp_type==WARP_TYPE_RECOVER)
  {
    pWarpData->GetRecoverPoint( warp_id, &zoneId, &pos );
  } 
  else
  {
    pWarpData->GetFlyPoint( warp_id, &zoneId, &pos );
  }

  EventMapJumpWithoutField* pEvent = CreateMapJumpPosWithoutField( p_man, zoneId, pos, type, isDispPlaceName, bgmKeepFlag, true );

  //NMCat[3533]「リザードン　フライト」で着地した際の自機の向きが南固定の為、着地場所によっては自機が不自然な方向を向く 
  if( warp_type == WARP_TYPE_FLY && warp_id == WP_F0401S0401_00 ){
    const f32 WP_F0401S0401_00_Y_AXIS = 82.0; //ツリーのワープポイントのみ谷君が指定した値にセットする
    gfl2::math::Quaternion rot(0,0,0,1);

    rot.RadianYXZToQuaternion(0.0, gfl2::math::ConvDegToRad(WP_F0401S0401_00_Y_AXIS), 0.0);
    pEvent->SetDeskLocationQuaternion( rot );
  }

  return pEvent;
}

//------------------------------------------------------------------------------
/**
 * @func        CreateMapJumpWithoutField
 * @brief       MapJumpWithoutFieldの作成.　フィールドが破棄された状態からマップジャンプする
 * @author      Miyuki Iwasawa
 * @date        2015/11/15(月)
 *
 * @param[in,out] GameSys::GameManager*                   p_man   .
 * @param[in]     EventMapJumpGameOver::EventMapJumpType  type    タイプ.
 * @param[in,out] bool                                    isCall  .
 *
 * @return      GameSys::GameEvent* 
 */
//------------------------------------------------------------------------------
EventMapJumpWithoutField* EventMapJump::CreateMapJumpPosWithoutField(
  GameSys::GameManager* p_man, ZoneID zoneId, const gfl2::math::Vector3& pos,
  EventMapJumpWithoutField::EventMapJumpType type, bool isDispPlaceName /*= true*/, bool bgmKeepFlag /*= false*/, bool isCall /*= false*/ )
{
  EventMapJumpWithoutField::Desc desc;
  desc.p_gdata = p_man->GetGameData();
  desc.location.worldId   = desc.p_gdata->GetFieldZoneDataLoader()->GetZoneWorldId(zoneId);
  desc.location.zoneId    = zoneId;
  desc.location.position  = pos;
  desc.nowZoneID          = p_man->GetGameData()->GetNowZoneID();
  desc.dispPlaceName      = isDispPlaceName;
  desc.bgmKeepFlag        = bgmKeepFlag;

  // マップ遷移タイプ
  desc.evMapJumpType  = type;

  EventMapJumpWithoutField* p_event = NULL;
  if( isCall )
  {
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventMapJumpWithoutField>( p_man->GetGameEventManager() );
  }else{
    GameSys::GameEventManager* p_evman = p_man->GetGameEventManager( );
    p_event = GFL_NEW( p_evman->GetHeap( ) ) EventMapJumpWithoutField( p_evman->GetHeap( ) );  
  }
  p_event->SetDesc(desc);
  return p_event;
}


//==============================================================================
EventMapJumpWithoutField::EventMapJumpWithoutField( gfl2::heap::HeapBase* heap ) : 
  GameSys::GameEvent(heap),
  m_pWorld(NULL),
  m_pArea( NULL ),            ///< Coverity.12004
  m_Desc()
{

}

EventMapJumpWithoutField::~EventMapJumpWithoutField()
{

}

//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool EventMapJumpWithoutField::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventMapJumpWithoutField::InitFunc(GameSys::GameManager* gmgr)
{

  m_pWorld = gmgr->GetGameData()->GetFieldWorld();
  m_pArea = gmgr->GetGameData()->GetFieldArea();
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventMapJumpWithoutField::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_UPDATE_MAPDATA_INIT,
    SEQ_UPDATE_MAPDATA,       ///<

    SEQ_FIELD_OPEN,             ///< フィールド開始
    SEQ_FIELD_OPEN_WAIT,        ///< フィールド開始まち
  };

  switch(this->GetSeqNo())
  {
  case SEQ_UPDATE_MAPDATA_INIT:
    {
      //@fix NMCat[1413] 元いたゾーンIDを保存する
      gmgr->GetGameData()->GetSituation()->SetLastZoneID( gmgr->GetGameData()->GetNowZoneID() );

      // 
      UpdateMapData::Desc desc;
      desc.p_gman = gmgr;
      desc.location = m_Desc.location;
      desc.bUpdateEvTime = true;
      desc.mode = UpdateMapData::MODE_MAPJUMP;
      desc.pc_ride_control = UpdateMapData::PC_RIDECONTROL_CLEAR;// @fix GFNMCat[702]

      m_UpdateMapData.Setup(desc);
      
      this->AddSeqNo();
    }
    // break;through

  case SEQ_UPDATE_MAPDATA:       ///<
    if( m_UpdateMapData.Update() == false )
    {
      break;
    }

    //ゾーンBGMの再生準備
    {
      Field::FieldSound* pFieldSound = gmgr->GetGameData()->GetFieldSound();
      pFieldSound->SetCurrentZoneID(m_Desc.location.zoneId);
      if( !m_Desc.bgmKeepFlag ){
        pFieldSound->PrepareNextFieldBGM();
      }
    }

    this->AddSeqNo();
    break;
   

  case SEQ_FIELD_OPEN:           ///< フィールド開始
    {
#if PM_DEBUG
      gfl2::math::Vector3  controlPos = m_Desc.location.GetPosition();
      const World::ZoneInfo * pZone = m_pWorld->IsHitZoneInfo( controlPos );
      if(pZone->m_ZoneID != m_Desc.location.GetZoneID())
      {
        GFL_ASSERT_MSG(0,"***PositionSettingError :: PosToZone=%d, SetZone=%d\n",pZone->m_ZoneID, m_Desc.location.GetZoneID());
      }
#endif
      FieldmapDesc desc;
      desc.openMode       = FM_OPEN_MODE_INIT;
      desc.playerLocation = m_Desc.location;  ///< 出現先指定

      EventFieldmapOpen* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen>( gmgr->GetGameEventManager() );
      p_event->SetDesc(desc);

      // 演出タイプを設定
      switch( m_Desc.evMapJumpType ){
      case EV_MAPJUMP_BLACK_FADE:     ///< ブラックフェード(イベント演出用)
        p_event->SetFadeCallBack( FldMapCtrlCallback_FadeInSetupBlackFade, FldMapCtrlCallback_FadeIsEndDefault, NULL );
        break;
      default:
        p_event->SetFadeCallBack( NULL, NULL, NULL );
        m_Desc.dispPlaceName = false; //演出ナシの場合は地名表示は抑制
        break;
      }
      //ゾーンBGMの再生
      if( !m_Desc.bgmKeepFlag ){
        Field::FieldSound* pFieldSound = gmgr->GetGameData()->GetFieldSound();
        pFieldSound->PreparePlayNextFieldBGM();
      }
      this->AddSeqNo();
    }
    break;
  case SEQ_FIELD_OPEN_WAIT:      ///< フィールド開始まち
    if( gmgr->GetFieldmap() && m_Desc.dispPlaceName )
    {
      gmgr->GetFieldmap()->GetEffectManager()->RequestStartPlaceName( false );
    }
    return GameSys::GMEVENT_RES_FINISH;

  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventMapJumpWithoutField::EndFunc(GameSys::GameManager* gmgr)
{

}

GFL_NAMESPACE_END(Field)
