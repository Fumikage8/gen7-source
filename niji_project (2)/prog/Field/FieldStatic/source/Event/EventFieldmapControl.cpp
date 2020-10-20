//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EventFieldmapControl.cpp
 *  @brief  フィールドマップ管理
 *  @author tomoya takahashi
 *  @date   2015.04.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "EventFieldmapControl.h"
#include "GameSys/include/DllProc.h"

#include "GameSys/include/GameProcManager.h"

#include <Fade/include/gfl2_FadeManager.h>

#include "Field/FieldRo/include/Event/FieldEventEntranceIn.h"
#include "Field/FieldRo/include/Event/FieldEventEntranceOut.h"


#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldStatic/include/Time/EvTime.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficManager.h"
#include "Field/FieldStatic/include/Berry/FieldBerryManager.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"

#include "Field/FieldStatic/include/Event/FieldEventPokemonRideTool.h"

#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountManager.h"

#include "niji_conv_header/field/FieldGimmickTypes.h"

#include "FieldScript/include/FieldScriptSystem.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/FieldProc.h"

// poke_tool
#include "PokeTool/include/PokeToolForm.h"

#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"

//下画面
#include "Field/FieldRo/include/Subscreen/FieldSubScreenProcManager.h"
#include "NetApp/JoinFestaSubScreen/include/JoinFestaSubScreenProc.h" // ジョインフェスタ下画面

// ロトムパワー用
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"

#if PM_DEBUG
#include "Field/FieldStatic/include/Debug/FieldDebugHeapLog.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

#define DEBUG_TICK_DRAW (0)

#else

#define DEBUG_TICK_DRAW (0)

#endif // PM_DEBUG

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
 *  @brief コンストラクタ
 */
UpdateMapData::UpdateMapData() : 
  m_Seq(0)
{
  m_Data.p_gman = NULL;
  m_Data.bUpdateEvTime = false;
  m_Data.location.Initialize();
}

/**
 *  @brief デストラクタ
 */
UpdateMapData::~UpdateMapData()
{
}

/**
 *  @brief 更新パラメータ設定
 */
void UpdateMapData::Setup( const UpdateMapData::Desc & desc )
{
  m_Data = desc;
}

/**
 *  @brief 更新
 */
bool UpdateMapData::Update( void )
{
  enum
  {
    SEQ_OLD_DELETE,               // 過去ゾーン、ワールド、エリアの情報を削除する。
    SEQ_RESIDENT_INITIALIZE,
    SEQ_RESIDENT_INITIALIZE_WAIT,
    SEQ_WORLD_INITIALIZE,
    SEQ_WORLD_INITIALIZE_WAIT,
    SEQ_AREA_INITIALIZE,
    SEQ_AREA_INITIALIZE_WAIT,
    SEQ_SETUP,
    SEQ_FINISH,
  };

  //-----------------------------------------------
  // Tick計測ワーク定義
  enum
  {
    TICK_START = SEQ_FINISH,
    TICK_MAX,
  };
  GFL_DEBUG_TICK_SETUP(UpdateMapData,TICK_MAX);



  switch( m_Seq )
  {
  case SEQ_OLD_DELETE:
    {
      GFL_DEBUG_TICK_SET_FLAG(UpdateMapData,DEBUG_TICK_DRAW);
      GFL_DEBUG_TICK_RESET(UpdateMapData);

      GFL_DEBUG_TICK_START(UpdateMapData, SEQ_OLD_DELETE, "UpdateMapData SEQ_OLD_DELETE");

      // 断片化回避のため、配置データをここで削除
      if( !isContinueMode() )
      {
        OldDeleteJump();

        // ジョインフェスタの状態更新
        JoinFestaControl();
      }
      else
      {
        OldDeleteContinue();
      }
#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->Clear();
#endif 

      GFL_DEBUG_TICK_END(UpdateMapData, SEQ_OLD_DELETE);
      GFL_DEBUG_TICK_START(UpdateMapData, SEQ_RESIDENT_INITIALIZE, "UpdateMapData SEQ_RESIDENT_INITIALIZE");
      ++m_Seq;
    }
    // break; through

  case SEQ_RESIDENT_INITIALIZE:
    {
      FieldResident * pResident = m_Data.p_gman->GetFieldResident();
      // 生成済みならスキップ
      if( pResident->IsInitialized() == false )
      {
        gfl2::heap::HeapBase* pAppHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE); 
        gfl2::heap::HeapBase* pAppHeapDevice = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE); 
        pResident->Initialize( pAppHeap, pAppHeapDevice, m_Data.p_gman->GetAsyncFileManager(), *m_Data.p_gman->GetGameData() );

        ++m_Seq;
        GFL_DEBUG_TICK_END(UpdateMapData, SEQ_RESIDENT_INITIALIZE);
        GFL_DEBUG_TICK_START(UpdateMapData, SEQ_RESIDENT_INITIALIZE_WAIT, "UpdateMapData SEQ_RESIDENT_INITIALIZE_WAIT");
      }
      else
      {
        GFL_DEBUG_TICK_END(UpdateMapData, SEQ_RESIDENT_INITIALIZE);
        GFL_DEBUG_TICK_START(UpdateMapData, SEQ_WORLD_INITIALIZE, "UpdateMapData SEQ_WORLD_INITIALIZE");
        m_Seq =SEQ_WORLD_INITIALIZE; 
        break;
      }
    }
    //break; through

  case SEQ_RESIDENT_INITIALIZE_WAIT:
    {
      FieldResident * pResident = m_Data.p_gman->GetFieldResident();
      bool resident_result = pResident->InitializeWait();

      if( (resident_result == false) )
      {
        break;
      } 
      ++m_Seq;

      GFL_DEBUG_TICK_END(UpdateMapData, SEQ_RESIDENT_INITIALIZE_WAIT);
      GFL_DEBUG_TICK_START(UpdateMapData, SEQ_WORLD_INITIALIZE, "UpdateMapData SEQ_WORLD_INITIALIZE");
    }
    // break;through

  case SEQ_WORLD_INITIALIZE:
    {
      World* pWorld = m_Data.p_gman->GetGameData()->GetFieldWorld();

      // ワールドIDが更新されているかチェック
      if( pWorld->GetWorldID() != m_Data.location.worldId )
      {
        pWorld->Initialize( m_Data.location.worldId, m_Data.p_gman->GetAsyncFileManager() );

        GFL_DEBUG_TICK_END(UpdateMapData, SEQ_WORLD_INITIALIZE);
        GFL_DEBUG_TICK_START(UpdateMapData, SEQ_WORLD_INITIALIZE_WAIT, "UpdateMapData SEQ_WORLD_INITIALIZE_WAIT");
        ++m_Seq;
      }
      else
      {
        GFL_DEBUG_TICK_END(UpdateMapData, SEQ_WORLD_INITIALIZE);
        GFL_DEBUG_TICK_START(UpdateMapData, SEQ_AREA_INITIALIZE, "UpdateMapData SEQ_AREA_INITIALIZE");
        m_Seq = SEQ_AREA_INITIALIZE;
        break;
      }
    }
    // break; through

  case SEQ_WORLD_INITIALIZE_WAIT:
    {
      World* pWorld = m_Data.p_gman->GetGameData()->GetFieldWorld();
      bool result = pWorld->InitializeWait();
      if( (result == false) )
      {
        break;
      }

      ++m_Seq;

      GFL_DEBUG_TICK_END(UpdateMapData, SEQ_WORLD_INITIALIZE_WAIT);
      GFL_DEBUG_TICK_START(UpdateMapData, SEQ_AREA_INITIALIZE, "UpdateMapData SEQ_AREA_INITIALIZE");
    }
    // break; through


  case SEQ_AREA_INITIALIZE:
    {
      //エリア情報読み込みリクエスト
      FieldResident * pResident = m_Data.p_gman->GetFieldResident();
      World* pWorld = m_Data.p_gman->GetGameData()->GetFieldWorld();
      Area* pArea = m_Data.p_gman->GetGameData()->GetFieldArea();
      AreaID areaID = pWorld->GetIDZoneInfo( static_cast<ZoneID>(m_Data.location.zoneId) ).m_AreaID;

      // エリアIDが更新されているか
      if( pArea->GetAreaID() != areaID )
      {
        gfl2::heap::HeapBase* pAppHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE); 
#if PM_DEBUG
        Debug::DebugHeapLog::GetInstance()->AddLog( pAppHeap, "AreaLoadStart" );
        Debug::DebugHeapLog::GetInstance()->Push();
#endif 
        Area::Mode mode = GetAreaMode( m_Data.p_gman->GetGameData(), pWorld, areaID );
        pArea->Initialize( areaID, pAppHeap, pResident->GetFieldDeviceHeap(), m_Data.p_gman->GetAsyncFileManager(), mode );

        GFL_DEBUG_TICK_END(UpdateMapData, SEQ_AREA_INITIALIZE);
        GFL_DEBUG_TICK_START(UpdateMapData, SEQ_AREA_INITIALIZE_WAIT, "UpdateMapData SEQ_AREA_INITIALIZE_WAIT");
        ++m_Seq;
      }
      else
      {
        m_Seq = SEQ_SETUP;
        break;
      }
    }
    //break; through

  case SEQ_AREA_INITIALIZE_WAIT:
    {
      FieldResident * pResident = m_Data.p_gman->GetFieldResident();
      Area* pArea = m_Data.p_gman->GetGameData()->GetFieldArea();
      bool result = pArea->InitializeWait();
      if( (result == false) )
      {
        break;
      }

#if PM_DEBUG
        gfl2::heap::HeapBase* pAppHeap = pResident->GetFieldDeviceHeap();
        Debug::DebugHeapLog::GetInstance()->Pop();
        Debug::DebugHeapLog::GetInstance()->AddLog( pAppHeap, "AreaLoadEnd" );
#endif 
      ++m_Seq;
      GFL_DEBUG_TICK_END(UpdateMapData, SEQ_AREA_INITIALIZE_WAIT);
      GFL_DEBUG_TICK_START(UpdateMapData, SEQ_SETUP, "UpdateMapData SEQ_SETUP");
    }
    // break; through


  case SEQ_SETUP:
    {
      SetupGameData(!IsZoneChange());
      ++m_Seq;
      GFL_DEBUG_TICK_END(UpdateMapData, SEQ_SETUP);
    }
    // break; through


  case SEQ_FINISH:
#if defined(DEBUG_ONLY_FOR_takahashi_tomoya)
#if DEBUG_TICK_DRAW
    gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
    GFL_DEBUG_TICK_DUMP_ALL(UpdateMapData);
    gfl2::debug::ResetPrintFlag();
#endif
#endif
    return true;


  default:
    GFL_ASSERT_STOP(0);
    break;
  }


  return false;
}

/**
 *  @brief 現状の状況からプレイヤーRide状態を設定
 *  @fix GFNMCat[702] PcRideControlTypeごとのライド変換処理をコーディング
 */
void UpdateMapData::ControlPlayerRide( GameSys::GameManager* p_gman, const Field::Location& location, PcRideControlType pcRideControl )
{
  MoveModel::FieldMoveModelManager * pMoveModelMan = p_gman->GetGameData()->GetFieldCharaModelManager();
  MoveModel::FieldMoveModelPlayer * pPlayer = static_cast<MoveModel::FieldMoveModelPlayer*>(pMoveModelMan->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

  void* pZoneData = p_gman->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( location.GetZoneID() );
  GFL_ASSERT_STOP(pZoneData);
  ZoneDataAccessor zoneDataAccessor( pZoneData );

  // 今のライド状態取得
  RIDE_POKEMON_ID now_pokemon_id = EventPokemonRideTool::GetPokemonRideOnID( pPlayer, true );


  // @caution デバッグ前に再度調査

  switch( pcRideControl )
  {
  case PC_RIDECONTROL_CLEAR:       ///< 必ずクリア
    // ライドに乗っていたら必ずクリア
    if( now_pokemon_id < RIDE_POKEMON_NUM )
    {
      // ライド状態を解消
      pPlayer->InitRideMoveModelHeader();
      pMoveModelMan->TerminateMoveModelWorkResource( Field::MoveModel::FIELD_MOVE_MODEL_RIDE_POKEMON );
      now_pokemon_id = RIDE_NO_POKEMON;
    }
    break;
  case PC_RIDECONTROL_CONNECT:     ///< コネクトタイプ
    // 次行くマップの各種フラグチェック
    if( now_pokemon_id < RIDE_POKEMON_NUM )
    {
      if( !zoneDataAccessor.IsPokeRideRunEnable() )
      {
        // ライド状態を解消
        pPlayer->InitRideMoveModelHeader();
        pMoveModelMan->TerminateMoveModelWorkResource( Field::MoveModel::FIELD_MOVE_MODEL_RIDE_POKEMON );
        now_pokemon_id = RIDE_NO_POKEMON;
      }
    }
    break;
  case PC_RIDECONTROL_ESCAPE:      ///< 穴抜けモード
    {
      // 脱出時対応
      // LocationのformNoを確認し、RIDE_POKEMON_IDが入っていた場合の処理
      if( location.GetFormNo() < RIDE_POKEMON_NUM )
      {
        b32 is_change = false;

        RIDE_POKEMON_ID location_ride = static_cast<RIDE_POKEMON_ID>(location.GetFormNo());

        // 波乗り状態で進入しており、波乗り状態に今なってない場合
        if( EventPokemonRideTool::IsNaminori( location_ride ) )
        {
          if( !EventPokemonRideTool::IsNaminori( now_pokemon_id ) )
          {
            is_change = true;
          }
        }
        // 地面馬で進入しており、今地面馬に乗っていない場合
        else if( (location_ride == RIDE_ROBA2) && (now_pokemon_id != RIDE_ROBA2) )
        {
          is_change = true;
        }

        if(is_change)
        {
          Field::MoveModel::FieldMoveModelHeaderWork     ride_headerWork;
          Field::MoveModel::FieldMoveModelHeaderResource ride_headerResource;

          EventPokemonRideTool::CreatePokemonRideWorld( static_cast<RIDE_POKEMON_ID>(location.GetFormNo()), &ride_headerWork, &ride_headerResource );

          pPlayer->SetRideMoveModelHeader( ride_headerWork, ride_headerResource );
        }
      }
      // 洞窟の入り口にライド乗っていない状態で復帰するケース
      else
      {
        // ライド状態を解消
        pPlayer->InitRideMoveModelHeader();
        pMoveModelMan->TerminateMoveModelWorkResource( Field::MoveModel::FIELD_MOVE_MODEL_RIDE_POKEMON );
        now_pokemon_id = RIDE_NO_POKEMON;
      }
    }
    break;

  case PC_RIDECONTROL_CONTINUE:    ///< 続きからモード
    {
      GFL_ASSERT( isContinueMode() );

      // ゾーンチェンジしていたらクリア
      if( IsZoneChange() )
      {
        // ライド状態を解消
        pPlayer->InitRideMoveModelHeader();
        pMoveModelMan->TerminateMoveModelWorkResource( Field::MoveModel::FIELD_MOVE_MODEL_RIDE_POKEMON );
        now_pokemon_id = RIDE_NO_POKEMON;
      }
    }
    break;

  default:
    GFL_ASSERT( m_Data.pc_ride_control < PC_RIDECONTROL_MAX );
    break;
  }
}


//------------------------------------------------------------------------------
/**
 * @func        SetupGameData
 * @brief       GameDataの更新.
 * @author      N.Takeda
 * @date        2015/09/14(月) 21:35:40
 * @note        2015/10/16 コンティニュー時か？　ゾーンが変わったときか？を取れるように変更
 *
 * @return      なし (none)
 */
//------------------------------------------------------------------------------
void UpdateMapData::SetupGameData(bool is_continue_and_not_zonechange)
{
  FieldResident * pResident = m_Data.p_gman->GetFieldResident();
  Area* pArea = m_Data.p_gman->GetGameData()->GetFieldArea();
  World* pWorld = m_Data.p_gman->GetGameData()->GetFieldWorld();
  bool isJoinFesta = m_Data.p_gman->GetGameData()->IsNowJoinFestaFieldData();

#if PM_DEBUG
  // FieldResidentにデバッグ用とのグローバルなZONEIDを設定
  pResident->DebugSetZone( m_Data.location.zoneId );
#endif

  // JoinFestaゾーンではないのに、GameDataのデータがJoinFestaモードの場合、処理を停止
  if(isJoinFesta)
	{
		GFL_ASSERT_STOP_MSG(ZoneDataAccessor::IsJoinFestaZone(m_Data.location.zoneId), "GameData mode is jf. but ZoneID is not jf.");
	}
	

  // @fix GFNMCat[4513] ビーストルザミーネに負けたとき、復帰処理後もロトム図鑑が眠ったまま復帰する
  m_Data.p_gman->GetGameData()->SetStartLocation( &m_Data.location );  // 最初にプレイヤー座標を更新

  // プレイヤーライド状態 操作
  ControlPlayerRide( m_Data.p_gman, m_Data.location, m_Data.pc_ride_control );

  if( m_Data.bUpdateEvTime )
  {
    // !!ゲーム内時間の更新!!
    Field::EvTime::Update( m_Data.p_gman->GetGameData( ) );
    // @fix MMCat[231][226] マップジャンプ時にEvTimeの更新が行われてしまっているために仕方なく実装
    if (Field::EvTime::IsUpdateMinuteEvent())
    {
      m_Data.p_gman->GetRotomPowerManager()->UpdateMinuteEvent();
    }
  }


  //ゾーンスクリプト初期化
  {
    Field::FieldScript::FieldScriptSystem* pFieldScriptSystem = m_Data.p_gman->GetFieldScriptSystem();
    gfl2::fs::BinLinkerAccessor* pBinLinker = pArea->GetAccessor( Area::AREA_DATA_IDX_ZONE_SCRIPT_RESOURCE );
    int index = pWorld->GetZoneIndexInArea(static_cast<ZoneID>(m_Data.location.zoneId));
    pFieldScriptSystem->InitializeZoneScript( pBinLinker->GetData(index), pBinLinker->GetDataSize(index) , static_cast<ZoneID>(m_Data.location.zoneId) );

    //セーブ復帰時はフラグをたてる
    if ( isContinueMode() )
    {
      pFieldScriptSystem->SetContinueRecoverFlag(true);
    }
    //ゾーン　フラグワーク初期化スクリプトコール
    if(!is_continue_and_not_zonechange) // マップジャンプ　＆　コンティニューでゾーンが変わっているときのみ実行
    {
      if( isJoinFesta == false ){
        pFieldScriptSystem->ClearZoneLocalWork();
      }

      pFieldScriptSystem->CallZoneChange(
        pArea->GetAccessor( Area::AREA_DATA_IDX_INIT_SCRIPT_RESOURCE ), index, 
        static_cast<ZoneID>(m_Data.location.zoneId), SCR_ZONEINIT_WALK_FLAGS, pResident->GetFieldDeviceHeap() );
      
      WarpDataTools   *pWarpData = m_Data.p_gman->GetGameData()->GetWarpDataTools();
      pWarpData->UpdateArrive( m_Data.p_gman->GetGameData(), static_cast<ZoneID>(m_Data.location.zoneId) );
    }
  }

#if PM_DEBUG
  Debug::DebugHeapLog::GetInstance()->AddLog( pResident->GetFieldDeviceHeap(), "ZoneScriptInitializeEnd" );
#endif 


  // ギミックエンカウント初期化
  InitializeGimmickEncountManager( m_Data.p_gman, m_Data.location);

  if(!is_continue_and_not_zonechange) // マップジャンプ　＆　コンティニューでゾーンが変わっているときのみ実行
  {
    // ギミックエンカウント抽選
    if( pResident->GetGimmickEncountManager() ) pResident->GetGimmickEncountManager()->LotteryZoneGimmickFlag( m_Data.location.zoneId);
  }

#if PM_DEBUG
  Debug::DebugHeapLog::GetInstance()->AddLog( pResident->GetFieldDeviceHeap(), "GimmickEncountInitializeEnd" );
#endif 
  if(!is_continue_and_not_zonechange) // マップジャンプ　＆　コンティニューでゾーンが変わっているときのみ実行
  {

    // きのみ配置ステータス更新
    if( m_Data.p_gman->GetGameData()->GetFieldBerryManager() ) m_Data.p_gman->GetGameData()->GetFieldBerryManager()->UpdateBerrysStatusToExist();

    // 釣りスポット復活
    if( m_Data.p_gman->GetGameData()->GetFishingSpotData()) m_Data.p_gman->GetGameData()->GetFishingSpotData()->ResetFishingSpot();

    // ドロップアイテム情報リセット
    if( m_Data.p_gman->GetGameData()->GetFieldItemManager()) m_Data.p_gman->GetGameData()->GetFieldItemManager()->TerminateDropItemData();

    // 隠しアイテムの復活処理
    if( !isJoinFesta )
    {
      m_Data.p_gman->GetGameData()->GetEventWork()->ClearReviveItemFlags();
    }
  }

  // 配置データ初期化
  InitializePlacementDataManager( m_Data.p_gman, m_Data.location );

#if PM_DEBUG
  Debug::DebugHeapLog::GetInstance()->AddLog( pResident->GetFieldDeviceHeap(), "PlacementDataInitializeEnd" );
#endif 

  u16 warp_id;
  warp_id = m_Data.p_gman->GetGameData()->GetWarpDataTools()->SearchByRoomID( m_Data.location.zoneId, m_Data.p_gman->GetGameData() );
  //HOSAKA_PRINT("Set WarpID=%d\n", warp_id );
  if ( warp_id )
  {
    m_Data.p_gman->GetGameData()->SetWarpID( warp_id );
  }
  
#if PM_DEBUG
  Debug::DebugHeapLog::GetInstance()->AddLog( pResident->GetFieldDeviceHeap(), "SetWarpIDEnd" );
#endif 

  // ポケセン誕生日用BGMフラグを操作
  {
    //Alias
    ZoneDataLoader* pZoneDataLoader = m_Data.p_gman->GetGameData()->GetFieldZoneDataLoader();
    ZoneDataAccessor zoneDataAccessor( pZoneDataLoader->GetZoneData( m_Data.location.GetZoneID() ) );

    //リセット
    u32 work_old = m_Data.p_gman->GetGameData()->GetEventWork()->GetEventWork( WK_SYS_POKECEN_BIRTH );
    m_Data.p_gman->GetGameData()->GetEventWork()->SetEventWork( WK_SYS_POKECEN_BIRTH, 0 );

    // ポケセン判定
    if(zoneDataAccessor.IsPokemonCenter())
    {
      //ポケセンの誕生日演出用にEvTime（というかSYS_FLAG_BIRTHDAY）を更新
      //- 主に「誕生日にセーブし、誕生日を過ぎてからロードした際」の状態に対応するため
      //- 上の方でもEvTime::Updateを呼んでいるが、他所への影響を考えて独自に行う
      Field::EvTime::Update( m_Data.p_gman->GetGameData( ) );
      // @fix MMCat[231][226] マップジャンプ時にEvTimeの更新が行われてしまっているために仕方なく実装
      if (Field::EvTime::IsUpdateMinuteEvent())
      {
        m_Data.p_gman->GetRotomPowerManager()->UpdateMinuteEvent();
      }

      // 誕生日判定
      // BTS662 誕生日：イベントが重なるとポケセン誕生日にBGMが無音になる 対処
      // GFBTS151 誕生日：ゲームオーバー時は抑制
      // BTS2288【シーケンス：誕生日にゲームオーバーになった際のポケセンBGM不具合】対処
      if ( m_Data.p_gman->GetGameData()->GetEventWork()->CheckEventFlag( SYS_FLAG_BIRTHDAY ) ) 
      {
        if( m_Data.p_gman->GetGameData()->GetEventWork()->CheckEventFlag( SYS_FLAG_BD_POKECEN_FIRST ) == false )
        {
          //GFNMCat[1887]:誕生日イベント：前日にセーブ、当日にロードすると、BGMが流れない
          //- 起動時は誕生日判定をスキップする
          //- すでに誕生日演出後であれば演出の維持のためスキップしない
          bool is_skip = isContinueMode();

          if(! is_skip){
            FieldScript::FieldScriptSystem*	p_script = m_Data.p_gman->GetFieldScriptSystem();

            // 初回：リザーブやシーン発動優先
            if( p_script->IsReserveScriptSetting() == false &&
                p_script->IsEnableSceneScript() == false)
            {
              m_Data.p_gman->GetGameData()->GetEventWork()->SetEventWork( WK_SYS_POKECEN_BIRTH, 1 ); // 無音
            }
          }
        }
        else
        {
          // 二回目以降：イベントとかぶってもBGMを変える
          m_Data.p_gman->GetGameData()->GetEventWork()->SetEventWork( WK_SYS_POKECEN_BIRTH, 3 ); // オルゴール
        }
      }
    }

    // フェス（マップ遷移を行うアプリ）判定
    // GFNMCat[1372] 誕生日イベントの前にフェスサークルに行って戻るとBGMが鳴らない
    //  原因：フェスサークルは遷移時も復帰時もこの関数が呼ばれてしまい、値が想定外となる
    //  修正：フェスサークルから戻ってきた時にWorkの値が以前と同じになるようにする
    //  備考：フェスサークル内でさらに遷移してこの関数が呼ばれる事もあるので注意
    {
      u32 work_offset = 100;

      //フェスへの遷移
      if(isJoinFesta)
      {
        if(work_old < work_offset){
          //遷移
          //- 「以前の値＋オフセット」にする事で「通常の値ではない事」の確認と「復帰すべき値」の取得を可能にしておく
          m_Data.p_gman->GetGameData()->GetEventWork()->SetEventWork( WK_SYS_POKECEN_BIRTH, work_old + work_offset );
        }else{
          //内部遷移
          //- 「以前の値＋オフセット」を維持し続ける
          m_Data.p_gman->GetGameData()->GetEventWork()->SetEventWork( WK_SYS_POKECEN_BIRTH, work_old );
        }
      }

      //フェスからの復帰
      if(! isJoinFesta)
      {
        if(work_offset <= work_old){
          //復帰
          //- 「以前の値＋オフセット」からオフセットを抜く事で以前の値を再設定
          m_Data.p_gman->GetGameData()->GetEventWork()->SetEventWork( WK_SYS_POKECEN_BIRTH, work_old - work_offset );
        }
      }
    }
  }
}


/**
 *  @brief PlacementDataManagerの初期化共通処理
 */
void UpdateMapData::InitializePlacementDataManager( GameSys::GameManager* p_gman, const Field::Location& location )
{
    World* pWorld = p_gman->GetGameData()->GetFieldWorld();
    Area* pArea = p_gman->GetGameData()->GetFieldArea();
    FieldResident* pResident = p_gman->GetFieldResident();

  // 配置初期化
  {
    PlacementDataManager* pPlacementDataMan = pResident->GetPlacementDataManager();
    PlacementDataManager::Desc desc;


    // 生成
    desc.pCollisionSceneForTerrain                = pResident->GetCollisionScene( FieldResident::COLLISION_SCENE_TERRAIN );
    desc.pCollisionSceneForStaticModel            = pResident->GetCollisionScene( FieldResident::COLLISION_SCENE_STATIC_MODEL );
    desc.pCollisionSceneForEventPos               = pResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_POS );
    desc.pCollisionSceneForEventIntrPos           = pResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_INTRPOS );
    desc.pCollisionSceneForEventTalk              = pResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_TALK );
    desc.pCollisionSceneForEventPush              = pResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_PUSH );
    desc.pCollisionSceneForEncount                = pResident->GetCollisionScene( FieldResident::COLLISION_SCENE_ENCOUNT );
    desc.pCollisionSceneForTrainerEye             = pResident->GetCollisionScene( FieldResident::COLLISION_SCENE_TRAINER_EYE );
    desc.pCollisionSceneForGimmickEncount         = pResident->GetCollisionScene( FieldResident::COLLISION_SCENE_GIMMICK_ENCOUNT );
    desc.pCollisionSceneForGimmickEncountAction   = pResident->GetCollisionScene( FieldResident::COLLISION_SCENE_GIMMICK_ENCOUNT_ACTION );
    desc.pCollisionSceneForEventFinder            = pResident->GetCollisionScene( FieldResident::COLLISION_SCENE_EVENT_FINDER );
    desc.pCollisionSceneForFishingPos             = pResident->GetCollisionScene( FieldResident::COLLISION_SCENE_FISHING_POS );
    desc.pPlacementData                           = pArea->GetAccessor( Area::AREA_DATA_IDX_PLACEMENTDATA );
    desc.cpEventWork                              = p_gman->GetGameData()->GetEventWork();
    desc.pMoveModelManager                        = p_gman->GetGameData()->GetFieldCharaModelManager();
    desc.pStaticModelManager                      = pResident->GetStaticModelManager();
    desc.pEnvSoundManager                         = pResident->GetEnvSoundManager();
    desc.pGimmickEncountManager                   = pResident->GetGimmickEncountManager();
    desc.pFieldResident                           = pResident;
    desc.pItemManager                             = p_gman->GetGameData()->GetFieldItemManager();
    desc.pBerryManager                            = p_gman->GetGameData()->GetFieldBerryManager();
    desc.pFishingSpot                             = p_gman->GetGameData()->GetFishingSpotData();
    desc.pWorld                                   = pWorld;
    desc.pArea                                    = pArea;
    desc.pHeap                                    = pResident->GetFieldDeviceHeap();
    desc.cpMapJumpParam                           = pResident->GetResource( BL_IDX_FIELD_RESIDENT__MAPJUMP_DATA_DAT );
    desc.mapJumpParamSize                         = pResident->GetResourceSize( BL_IDX_FIELD_RESIDENT__MAPJUMP_DATA_DAT );
    pPlacementDataMan->Initialize( desc );

    // △動作モデル生成
    pPlacementDataMan->CreateAreaMoveModelWork();
    pPlacementDataMan->CreateAreaTrainerWork();
    pPlacementDataMan->CreateZoneModelWork( location.zoneId );
    pPlacementDataMan->CreateZoneTrainerWork( location.zoneId );
    pPlacementDataMan->CreateZoneGimmickEncount( location.zoneId );
    pPlacementDataMan->CreateZoneBerryPointWork(location.zoneId);
    pPlacementDataMan->CreateZoneFishingSpotWork(location.zoneId);
    pPlacementDataMan->CreateZonePushRockWork(location.zoneId);
  }
}


/**
 *  @brief GimmickEncountManagerの初期化共通処理
 */
void UpdateMapData::InitializeGimmickEncountManager( GameSys::GameManager* p_gman, const Field::Location& location )
{
  FieldResident* pResident = p_gman->GetFieldResident();
  GimmickEncount::Manager* pGimmickEncountManager = pResident->GetGimmickEncountManager();
  if(pGimmickEncountManager)
  {
    GimmickEncount::Manager::Description desc;
    desc.pEventWork = p_gman->GetGameData()->GetEventWork();
    desc.pWorld     = p_gman->GetGameData()->GetFieldWorld();
    desc.pArea      = p_gman->GetGameData()->GetFieldArea();
    desc.pResident  = p_gman->GetFieldResident();

    pGimmickEncountManager->Initialize( desc);
  }
}


/**
 *  @brief 過去の情報破棄　マップジャンプ用
 */
void UpdateMapData::OldDeleteJump(void)
{
  World* pWorld = m_Data.p_gman->GetGameData()->GetFieldWorld();
  Area* pArea = m_Data.p_gman->GetGameData()->GetFieldArea();
  FieldResident * pResident = m_Data.p_gman->GetFieldResident();

  // Zone由来の物  必ず破棄
  if( pResident->IsInitialized() )
  {
    // 配置
    PlacementDataManager* pPlacementDataMan = pResident->GetPlacementDataManager();

    pPlacementDataMan->DeleteZoneModel();
    pPlacementDataMan->DeleteZoneTrainer();
    pPlacementDataMan->DeleteZoneGimmickEncount();
    pPlacementDataMan->DeleteZoneBerryPointWork();
    pPlacementDataMan->DeleteAreaMoveModelWork();
    pPlacementDataMan->DeleteAreaTrainerWork();
    pPlacementDataMan->DeleteZoneFishingSpotWork();
    pPlacementDataMan->DeleteZonePushRockWork();
    pPlacementDataMan->Terminate();

  }
  else{
    //全滅・空を飛ぶ Script[MapChangeNoFieldコマンド]時はResidentが無い状態でここにくる。動作モデルを破棄しないと積みあがっていくので削除
    OldDeleteMoveModel(true);

    // FieldDLLが無い状態なので読み込んでおく
    pResident->InitializeFieldDLL( m_Data.p_gman->GetAsyncFileManager() );
  }

  // 往来
  {
    Field::TrafficManager *pTrafficManager = m_Data.p_gman->GetGameData()->GetFieldTrafficManager();
    if( pTrafficManager ) pTrafficManager->DeleteZoneActor();
  }

  // 念のため旧ゾーンのデータが残っているNPCがいたら破棄する。
  OldDeleteMoveModel(false, MoveModel::FIELD_MOVE_MODEL_NPC_START, true ); // GFNMCat[3575] 案件

  // ゾーンスクリプト(フラグクリア)
  {
    Field::FieldScript::FieldScriptSystem* pFieldScriptSystem = m_Data.p_gman->GetFieldScriptSystem();
    pFieldScriptSystem->FinalizeZoneScript();
  }


  // World, Areaは変える必要が無ければ変えない
  if( pWorld->GetWorldID() != m_Data.location.worldId )
  {
    // 全て削除
    pArea->Terminate();
    pWorld->Terminate();
  }
  else
  {
    AreaID areaID = pWorld->GetIDZoneInfo( static_cast<ZoneID>(m_Data.location.zoneId) ).m_AreaID;
    // エリアIDが更新されているか
    if( pArea->GetAreaID() != areaID )
    {
      pArea->Terminate();
    }
  }
}

/**
 *  @brief 過去の情報破棄　コンティニュー用
 */
void UpdateMapData::OldDeleteContinue(void)
{
  //動作モデルの情報破棄
  OldDeleteMoveModel(false);
}


/**
 *  @brief 過去の情報破棄　動作モデル用
 */
void UpdateMapData::OldDeleteMoveModel(b32 is_force, int start, b32 is_assert)
{
  // ゾーンの更新発生
  if(IsZoneChange())
  {
    // 過去マップのセーブ復帰させたデータをクリアする。
    // 特定ゾーンで生成した動作モデルの破棄
    Field::MoveModel::FieldMoveModelManager* pMoveModelManager = m_Data.p_gman->GetGameData()->GetFieldCharaModelManager();

    for( int i=start; i<MoveModel::FIELD_MOVE_MODEL_MAX; ++i  )
    {
      MoveModel::FieldMoveModel * pCharaModel = pMoveModelManager->GetFieldMoveModel( i );

      if( pCharaModel->IsSetupWork() && ((pCharaModel->GetZoneId() != m_Data.location.zoneId) || (is_force)) )
      {
        if( is_assert )
        {
          GFL_RELEASE_PRINT( "下記アサートは、セーブしていたゾーンのキャラクターの配置データの削除が発生したことを通知するものです。\n" );
          GFL_RELEASE_PRINT( "セーブ復帰後の動作に支障が出ている可能性があります。 配置データは一度マップジャンプをすることできれいな状態になります。\n" );
          GFL_RELEASE_PRINT( "シナリオ進行が問題なく継続できるかは、イベント設計チームもしくは、イベントプログラムチームに問い合わる必要があります。\n" );
          GFL_RELEASE_PRINT( "もし、作業に支障なければセーブデータをクリアしてください。\n" );
          GFL_ASSERT_MSG(0, "remain MoveModel eventID %d zoneID %d", pCharaModel->GetEventId(), pCharaModel->GetZoneId() );
        }

        // ジャンプ後ZoneIDと一致しないデータは破棄する。
        pMoveModelManager->TerminateMoveModelWorkResource( static_cast<MoveModel::FIELD_MOVE_MODEL_ID>(i) );

      }
    }
  }
}


/**
 *  @brief コンティニューもーどか？ */
bool UpdateMapData::isContinueMode() const
{
  switch( m_Data.mode )
  {
  case MODE_CONTINUE_ZONECHANGE:       ///< コンティニュー  ゾーン変動
  case MODE_CONTINUE_NO_ZONECHANGE:    ///< コンティニュー  ゾーン変動なし
    return true;
  }

  return false;
}

/**
 *  @brief コンティニューでゾーンが変更されているか？
 */
bool UpdateMapData::IsZoneChange() const
{
  switch( m_Data.mode )
  {
  case MODE_CONTINUE_ZONECHANGE:       ///< コンティニュー  ゾーン変動
  case MODE_MAPJUMP:       
  case MODE_JFOPEN:
    return true;
  }

  return false;
}


void UpdateMapData::JoinFestaControl()
{
  if( m_Data.mode == MODE_JFCLOSE )
  {
    // JoinFestaモード終了　
    m_Data.p_gman->GetGameData()->EndJoinFestaFieldData();
  }
  else if( m_Data.mode == MODE_JFOPEN )
  {
    // JoinFestaモード フィールドClose完了
    m_Data.p_gman->GetGameData()->SetJoinFestaManagerSwitch(true);


    // 乗っていたら、
    // 主人公をライドからおろす
    MoveModel::FieldMoveModelManager * pMoveModelMan = m_Data.p_gman->GetGameData()->GetFieldCharaModelManager();
    MoveModel::FieldMoveModelPlayer * pPlayer = static_cast<MoveModel::FieldMoveModelPlayer*>(pMoveModelMan->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

    // 今のライド状態取得
    RIDE_POKEMON_ID now_pokemon_id = EventPokemonRideTool::GetPokemonRideOnID( pPlayer, true );

    if( now_pokemon_id < RIDE_POKEMON_NUM )
    {
      // ライド状態を解消
      pPlayer->InitRideMoveModelHeader();
      pMoveModelMan->TerminateMoveModelWorkResource( Field::MoveModel::FIELD_MOVE_MODEL_RIDE_POKEMON );
    }
  }
}

// エリアのモード取得
Area::Mode UpdateMapData::GetAreaMode( const GameSys::GameData* cp_gdata, Field::World* pWorld, AreaID areaId )
{
  Area::Mode mode = Area::MODE_NORMAL;
  if( cp_gdata->IsNowJoinFestaFieldData() )
  {
    mode =  Area::MODE_JF;
  }
  else if( pWorld->IsAreaDisplayBattlePokemonUsingAreaHeap( areaId ) )
  {
    mode =  Area::MODE_BATTLEMODEL;
  }
  else if( pWorld->IsAreaDisplayShopUIUsingAreaHeap( areaId ) )
  {
    mode =  Area::MODE_MIDDLE;
  }

  return mode;
}


/** 
 *  @brief コンストラクタ
 */
RecoverMapData::RecoverMapData() : 
  m_Seq(0)
{
  m_Data.location.Initialize();
  m_Data.p_gman = NULL;
}

/** 
 *  @brief デストラクタ
 */
RecoverMapData::~RecoverMapData()
{
}


/**
 *  @brief 更新パラメータ設定
 */
void RecoverMapData::Setup( const RecoverMapData::Desc & desc )
{
  m_Data = desc;
}

/**
 *  @brief 更新
 */
bool RecoverMapData::Update( void )
{
  enum
  {
    SEQ_RESIDENT_CREATE,
    SEQ_AREA_LOAD,
    SEQ_SETUP,
    SEQ_FINISH,
  };

  //-----------------------------------------------
  // Tick計測ワーク定義
  enum
  {
    TICK_START = SEQ_FINISH,
    TICK_MAX,
  };
  GFL_DEBUG_TICK_SETUP(RecoverMapData,TICK_MAX);

  switch( m_Seq ){
  // アプリ復帰用処理
  case SEQ_RESIDENT_CREATE:
    {
      GFL_DEBUG_TICK_SET_FLAG(RecoverMapData,DEBUG_TICK_DRAW);
      GFL_DEBUG_TICK_RESET(RecoverMapData);

      GFL_DEBUG_TICK_START(RecoverMapData, SEQ_RESIDENT_CREATE, "RecoverMapData SEQ_RESIDENT_CREATE");

      World* pWorld = m_Data.p_gman->GetGameData()->GetFieldWorld();
      Area* pArea = m_Data.p_gman->GetGameData()->GetFieldArea();
      FieldResident* pFieldResident = m_Data.p_gman->GetFieldResident();

#if PM_DEBUG
      Debug::DebugHeapLog::GetInstance()->Clear();
#endif 

      gfl2::heap::HeapBase* pAppHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE); 
      gfl2::heap::HeapBase* pAppHeapDevice = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE); 
      pFieldResident->Initialize( pAppHeap, pAppHeapDevice, m_Data.p_gman->GetAsyncFileManager(), *m_Data.p_gman->GetGameData() );

      AreaID areaId = pWorld->GetIDZoneInfo( static_cast<ZoneID>(m_Data.location.zoneId) ).m_AreaID;
      Area::Mode mode = UpdateMapData::GetAreaMode( m_Data.p_gman->GetGameData(), pWorld, areaId );
      if( pArea->GetAreaID() != areaId ) // FM_CLOSE_MODE_APP_KEEPRES対応　pAreaにデータが存在していたらそのまま使う。
      {
        pArea->Initialize( areaId, pAppHeapDevice, pFieldResident->GetFieldDeviceHeap(), m_Data.p_gman->GetAsyncFileManager(), mode );
      }

      GFL_DEBUG_TICK_END(RecoverMapData, SEQ_RESIDENT_CREATE);
      GFL_DEBUG_TICK_START(RecoverMapData, SEQ_AREA_LOAD, "RecoverMapData SEQ_AREA_LOAD");
      ++m_Seq;
    }
    //break; through

  case SEQ_AREA_LOAD:
    {
      Area* pArea = m_Data.p_gman->GetGameData()->GetFieldArea();
      FieldResident* pFieldResident = m_Data.p_gman->GetFieldResident();

      bool result = pArea->InitializeWait();
      bool resident_result = pFieldResident->InitializeWait();
      if( (result == false) || (resident_result == false) )
      {
        break;
      }
      ++m_Seq;

      GFL_DEBUG_TICK_END(RecoverMapData, SEQ_AREA_LOAD);
      GFL_DEBUG_TICK_START(RecoverMapData, SEQ_SETUP, "RecoverMapData SEQ_SETUP");
    }
    //break; through

  case SEQ_SETUP:
    {
      // ギミックエンカウントマネージャー初期化
      UpdateMapData::InitializeGimmickEncountManager( m_Data.p_gman, m_Data.location );

      // 配置データ初期化
      UpdateMapData::
       InitializePlacementDataManager( m_Data.p_gman, m_Data.location );

      ++m_Seq;
      GFL_DEBUG_TICK_END(RecoverMapData, SEQ_SETUP);
    }
    //break; through

  case SEQ_FINISH:
#if defined(DEBUG_ONLY_FOR_takahashi_tomoya)
#if DEBUG_TICK_DRAW
    gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
    GFL_DEBUG_TICK_DUMP_ALL(RecoverMapData);
    gfl2::debug::ResetPrintFlag();
#endif
#endif
    return true;

  default:
    GFL_ASSERT_STOP(0);
    break;
  }


  return false;
}


//-----------------------------------------------------------------------------
/**
 *          共通
*/
//-----------------------------------------------------------------------------
bool FldMapCtrlCallback_FadeInSetupDefault
  ( void* p_work, GameSys::GameManager* p_gman )
{
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
  return true;
}

bool FldMapCtrlCallback_FadeInSetupDefaultUpperOnly
  ( void* p_work, GameSys::GameManager* p_gman )
{
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA );
  return true;
}

bool FldMapCtrlCallback_FadeOutSetupDefault
  ( void* p_work, GameSys::GameManager* p_gman )
{
  gfl2::math::Vector4 start_col( 0, 0, 0, 0.0f );
  gfl2::math::Vector4 end_col( 0, 0, 0, 255.0f );

  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, &start_col, &end_col );
  return true;
}

bool FldMapCtrlCallback_FadeOutSetupDefaultUpperOnly
  ( void* p_work, GameSys::GameManager* p_gman )
{
  gfl2::math::Vector4 start_col( 0, 0, 0, 0.0f );
  gfl2::math::Vector4 end_col( 0, 0, 0, 255.0f );

  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, &start_col, &end_col );
  return true;
}

bool FldMapCtrlCallback_FadeOutSetupDefaultLowerOnly
  ( void* p_work, GameSys::GameManager* p_gman )
{
  gfl2::math::Vector4 start_col( 0, 0, 0, 0.0f );
  gfl2::math::Vector4 end_col( 0, 0, 0, 255.0f );

  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_LOWER, gfl2::Fade::FADE_TYPE_ALPHA, &start_col, &end_col );
  return true;
}

bool FldMapCtrlCallback_FadeInSetupBlackFade
  ( void* p_work, GameSys::GameManager* p_gman )
{
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, Field::FieldScript::FADE_DEFAULT_SYNC );
  return true;
}

bool FldMapCtrlCallback_FadeOutSetupBlackFade
  ( void* p_work, GameSys::GameManager* p_gman )
{
  gfl2::math::Vector4 start_col( 0, 0, 0, 0.0f );
  gfl2::math::Vector4 end_col( 0, 0, 0, 255.0f );

  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, &start_col, &end_col,  Field::FieldScript::FADE_DEFAULT_SYNC );
  return true;
}

bool FldMapCtrlCallback_FadeInSetupBlackFadeUpperOnly
  ( void* p_work, GameSys::GameManager* p_gman )
{
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, Field::FieldScript::FADE_DEFAULT_SYNC );
  return true;
}

bool FldMapCtrlCallback_FadeOutSetupBlackFadeUpperOnly
  ( void* p_work, GameSys::GameManager* p_gman )
{
  gfl2::math::Vector4 start_col( 0, 0, 0, 0.0f );
  gfl2::math::Vector4 end_col( 0, 0, 0, 255.0f );

  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, &start_col, &end_col,  Field::FieldScript::FADE_DEFAULT_SYNC );
  return true;
}

bool FldMapCtrlCallback_FadeIsEndDefault
  ( void* p_work, GameSys::GameManager* p_gman )
{
  return GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd( gfl2::Fade::DISP_UPPER );
}

bool FldMapCtrlCallback_FadeIsEndLowerOnly
  ( void* p_work, GameSys::GameManager* p_gman )
{
  return GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd( gfl2::Fade::DISP_LOWER );
}

// ロトムフェードイン
bool FldMapCtrlCallback_RotomFadeInSetup
  ( void* p_work, GameSys::GameManager* p_gman )
{
  Field::SubScreen::FieldSubScreenProcManager* pFieldSubScreenProcManager = p_gman->GetFieldmap()->GetFieldSubScreenManager(); 
  NetApp::JoinFestaSubScreen::JoinFestaSubScreenProc* pJoinFestaSubScreenProc = pFieldSubScreenProcManager->GetJoinFestaSubScreenProc();
  GFL_ASSERT(pFieldSubScreenProcManager);
  if( pFieldSubScreenProcManager != NULL )
  {
    pJoinFestaSubScreenProc->RotomFadeInStart();
  }
  return true;
}

bool FldMapCtrlCallback_RotomFadeIsEnd
  ( void* p_work, GameSys::GameManager* p_gman )
{
  Field::SubScreen::FieldSubScreenProcManager* pFieldSubScreenProcManager = p_gman->GetFieldmap()->GetFieldSubScreenManager(); 
  NetApp::JoinFestaSubScreen::JoinFestaSubScreenProc* pJoinFestaSubScreenProc = pFieldSubScreenProcManager->GetJoinFestaSubScreenProc();
  GFL_ASSERT(pFieldSubScreenProcManager);
  if( pFieldSubScreenProcManager != NULL )
  {
    return pJoinFestaSubScreenProc->IsRotomFadeInEnd();
  }
  return true;
}



/**
 *  @brief キャプチャフェードアウト 開始
 */
bool FldMapCtrlCallback_FadeOutSetupCaptureBlackFade
  ( void* p_work, GameSys::GameManager* p_gman )
{
  gfl2::math::Vector4 start_col( 0, 0, 0, 0.0f );
  gfl2::math::Vector4 end_col( 0, 0, 0, 255.0f );

  // Captureありでブラックフェードを開始
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, &start_col, &end_col,  Field::FieldScript::FADE_DEFAULT_SYNC, true );
  return true;
}



/* -------------------------------------------------------------------------*/
/**
 * @brief エントランスインイベントをセット
 *
 * @param p_work  イベント用ワーク
 * @param p_gman  ゲームマネージャ
 *
 * @return セットに成功したらtrue
 */
/* -------------------------------------------------------------------------*/
bool FldMapCtrlCallback_FadeInSetupEntranceIn
  ( void* p_work, GameSys::GameManager* p_gman )
{
  const EventEntranceIn::Desc* p_desc = reinterpret_cast<const EventEntranceIn::Desc*>(p_work);

  // エントランスインをセット
  EventEntranceIn* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventEntranceIn >( p_gman->GetGameEventManager( ) );
  p_event->SetData( *p_desc );

  return true;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief エントランスアウトイベントをセット
 *
 * @param p_work  イベントワーク
 * @param p_gman  ゲームマネージャ
 *
 * @return セットに成功したらtrue
 */
/* -------------------------------------------------------------------------*/
bool FldMapCtrlCallback_FadeInSetupEntranceOut
  ( void* p_work, GameSys::GameManager* p_gman )
{
  const EventEntranceOut::Desc* p_desc = reinterpret_cast<const EventEntranceOut::Desc*>(p_work);

  // エントランスアウトをセット
  EventEntranceOut* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventEntranceOut>( p_gman->GetGameEventManager( ) );
  p_event->SetData( *p_desc );

  return true;
}

bool FldMapCtrlCallback_FadeInSetupCustom
  ( void* p_work, GameSys::GameManager* p_gman )
{
  FieldmapControlFadeParam* param = reinterpret_cast<FieldmapControlFadeParam*>(p_work);

  if( param->type==gfl2::Fade::FADE_TYPE_NONE)
  {
    return true;
  }
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(param->disp, param->type, param->sync );
  return true;
}

bool FldMapCtrlCallback_FadeOutSetupCustom
  ( void* p_work, GameSys::GameManager* p_gman )
{
  FieldmapControlFadeParam* param = reinterpret_cast<FieldmapControlFadeParam*>(p_work);

  if( param->type==gfl2::Fade::FADE_TYPE_NONE)
  {
    return true;
  }
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(param->disp, param->type, &param->startCol, &param->endCol,  param->sync );
  return true;
}

bool FldMapCtrlCallback_FadeIsEndCustom
  ( void* p_work, GameSys::GameManager* p_gman )
{
  FieldmapControlFadeParam* param = reinterpret_cast<FieldmapControlFadeParam*>(p_work);

  if( param->type==gfl2::Fade::FADE_TYPE_NONE)
  {
    return true;
  }

  return GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsEnd( param->disp );
}


/************************************************************************/
/*    FieldmapOpen                                                      */
/************************************************************************/

#if PM_DEBUG
int EventFieldmapOpen::m_s_DebugCheckVramClear = true;
#endif

EventFieldmapOpen::EventFieldmapOpen( gfl2::heap::HeapBase* heap ) :
  GameSys::GameEvent(heap),
  m_Seq(0)
{
  this->SetFadeCallBackDefault();
}
EventFieldmapOpen::~EventFieldmapOpen()
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
bool EventFieldmapOpen::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventFieldmapOpen::InitFunc(GameSys::GameManager* gmgr)
{
#if GFL_HEAP_DEBUG
// フィールドに帰ってくるタイミングで反転する。
#if defined(GF_PLATFORM_CTR)
#if PM_DEBUG
  if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::ENABLE_MEMORY_DANGLING_CHECK ) )
  {
    if( gfl2::heap::CTRExpHeap::GetDebugReverseAllocFlag() ){
      gfl2::heap::CTRExpHeap::SetDebugReverseAllocFlag(false);
    }else{
      gfl2::heap::CTRExpHeap::SetDebugReverseAllocFlag(true);
    }
  }
  else
  {
    gfl2::heap::CTRExpHeap::SetDebugReverseAllocFlag(false);
  }
#endif // PM_DEBUG
#endif // defined(GF_PLATFORM_CTR)
#endif // GFL_HEAP_DEBUG

#if PM_DEBUG
  if( m_s_DebugCheckVramClear )
  {
    System::nijiAllocator::DebugCheckApplicationVramClear();  ///フィールドに戻ってきた際にVRAMがLEAKしていないかチェック
  }
#endif // PM_DEBUG

#if FIELD_CTR_PROFILING_INIT
      FIELD_CTR_PROFILING_START;
#endif

  gmgr->SetVSyncSkipFlag( true ); // 処理高速化のためVSYNC待ちしない

  FieldResident* pFieldResident = gmgr->GetFieldResident();
  if( pFieldResident->IsInitialized() )
  {
    m_Seq = SEQ_CALL_FIELDPROC;
  }
  else
  {
    m_Seq = SEQ_RECOVER_INITIALIZE;
  }

      
  //フィールド生成前にケルディオのしんぴの剣チェック
  PokeTool::form::UpdateKerudhioForm( gmgr->GetGameData() );


#if PM_DEBUG
#if defined(GF_PLATFORM_CTR)
  Field::Debug::DebugTypes::s_DebugFieldStartTick = nn::os::Tick::GetSystemCurrent();
#endif

  // ファイルシステムのデバッグログを削除
#if GFL_FS_DEBUG_LOG
  gmgr->GetAsyncFileManager()->ClearFsDebugLogArcFileInfo();
#endif

#endif




}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventFieldmapOpen::MainFunc(GameSys::GameManager* gmgr)
{

  switch( m_Seq ){
  // アプリ復帰用処理
  case SEQ_RECOVER_INITIALIZE:
    {
      RecoverMapData::Desc desc;
      desc.p_gman = gmgr;
      desc.location = m_Desc.playerLocation;
      m_RecoverMapData.Setup(desc);
      ++m_Seq;
    }
    //break; through

  case SEQ_RECOVER_INITIALIZE_WAIT:
    {
      if( m_RecoverMapData.Update() == false )
      {
        break;
      }
      ++m_Seq;
    }
    break;

  // マップジャンプはここから。
  case SEQ_CALL_FIELDPROC:
    // FieldmapProcを生成
    if( gmgr->GetProcManager()->IsMainHandle() == false )
    {
      // FieldProcを起動
      GameSys::CallFieldProc(&m_Desc);
      TOMOYA_PRINT( "☆FieldProc Call\n" );
    }
    else
    {
      // プロックに対して、再初期化を通知する。
      FieldProc * pFieldProc = reinterpret_cast<FieldProc*>(gmgr->GetProcManager()->GetBaseProcess());
      pFieldProc->RequestReturnInitialize(&m_Desc);
      TOMOYA_PRINT( "☆FieldProc RequestReturnInitialize\n" );
    }

    ++m_Seq;
    break;
    
  case SEQ_WAIT_READY:
    {
      // 初期化の完了まち
      if( (gmgr->GetFieldmap()==NULL) || !gmgr->GetFieldmap()->IsReady() )
      {
        break;
      }

      if (!gmgr->GetFieldmap()->InitializeFieldStaffrollReady())
      {
        break;
      }
      
      // フェード設定
      if(m_fadeProc){
        if(m_fadeProc( m_fadeWork, gmgr) == false){ break; }
        // 何かしら設定したのであれば、1フレーム待つ。
        // (イベントをここで設定する可能性がある。)
        ++m_Seq;
        return GameSys::GMEVENT_RES_CONTINUE_DIRECT;
      }

      ++m_Seq;
    }
    //break through

  case SEQ_WAIT_FADE_WAIT:
    {
      //エンディング用モジュールの準備待ち（モジュールがない場合はスルー）
      if (!(gmgr->GetFieldmap()->InitializeFieldStaffrollReadyWait()))
      {
        break;
      }
      // フェード待ち
      if(m_fadeIsEndProc){
        if( m_fadeIsEndProc(m_fadeWork, gmgr) == false ){ break; }
      }
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
void EventFieldmapOpen::EndFunc(GameSys::GameManager* gmgr)
{
  // ヌケがあった場合のため念のためここでも復帰
  gmgr->SetVSyncSkipFlag( false ); // VSYNC待ち復帰

}

//--------------------------------------------------------------
/**
 * @brief   フェードコールバック設定
 */
//--------------------------------------------------------------
void EventFieldmapOpen::SetFadeCallBack
  (FLD_MAPCTRL_CALLBACK_FUNC fadeProc, FLD_MAPCTRL_CALLBACK_FUNC fadeIsEndProc, void  * fadeWork)
{
  m_fadeProc = fadeProc;
  m_fadeIsEndProc = fadeIsEndProc;
  m_fadeWork = fadeWork;
}

void EventFieldmapOpen::SetFadeCallBackDefault(void)
{
  m_fadeProc = FldMapCtrlCallback_FadeInSetupDefault;
  m_fadeIsEndProc = FldMapCtrlCallback_FadeIsEndDefault;
  m_fadeWork = NULL;
}

void EventFieldmapOpen::SetFadeCallBackDefaultUpperOnly(void)
{
  m_fadeProc = FldMapCtrlCallback_FadeInSetupDefaultUpperOnly;
  m_fadeIsEndProc = FldMapCtrlCallback_FadeIsEndDefault;
  m_fadeWork = NULL;
}

void EventFieldmapOpen::SetFadeCallBackCustom(
  gfl2::Fade::DISP disp, gfl2::Fade::FADE_TYPE type, u32 sync /*= gfl2::Fade::FADE_DEFAULT_SYNC*/)
{
  m_fadeProc = FldMapCtrlCallback_FadeInSetupCustom;
  m_fadeIsEndProc = FldMapCtrlCallback_FadeIsEndCustom;
  m_fadeWork = &m_fadeParam;

  m_fadeParam.type = type;
  m_fadeParam.disp = disp;
  m_fadeParam.startCol.Set(0.0f,0.0f,0.0f,0.0f);
  m_fadeParam.endCol.Set(0.0f,0.0f,0.0f,255.0f);
  m_fadeParam.sync = sync;
}

//--------------------------------------------------------------
/**
  * @brief   ジョインフェスタ専用フェード設定（ロトムフェード）
*/
//--------------------------------------------------------------
void EventFieldmapOpen::SetFadeCallBackJfOpen(void)
{
  m_fadeProc = FldMapCtrlCallback_RotomFadeInSetup;
  m_fadeIsEndProc = FldMapCtrlCallback_RotomFadeIsEnd;
  m_fadeWork = NULL;
}

/************************************************************************/
/*    FieldmapClose                                                     */
/************************************************************************/
EventFieldmapClose::EventFieldmapClose( gfl2::heap::HeapBase* heap ) : 
  GameSys::GameEvent(heap)
{
  this->SetFadeCallBackDefault();
  m_Desc.closeType = FM_CLOSE_MODE_MAX;
  m_Desc.isSkyboxKeep = false;
}
EventFieldmapClose::~EventFieldmapClose()
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
bool EventFieldmapClose::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventFieldmapClose::InitFunc(GameSys::GameManager* gmgr)
{

}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventFieldmapClose::MainFunc(GameSys::GameManager* gmgr)
{
  enum{
    SEQ_FIELD_FADE,
    SEQ_FIELD_FADE_WAIT,
    SEQ_FIELD_CLOSE,
    SEQ_FIELD_CLOSE_WAIT,
    SEQ_FIELD_RESIDENT_DELETE00,
    SEQ_FIELD_RESIDENT_DELETE01,

    SEQ_MAX
  };

  //-----------------------------------------------
  // Tick計測ワーク定義
  enum
  {
    TICK_START = SEQ_MAX,
    TICK_TOTAL,
    TICK_RESIDENT_DELETE_AREA,
    TICK_RESIDENT_DELETE_RESIDENT,
    TICK_RESIDENT_DELETE_BGM,
    TICK_MAX,
  };
  GFL_DEBUG_TICK_SETUP(Terminate,TICK_MAX);


  switch(this->GetSeqNo()){
  case SEQ_FIELD_FADE:
    GFL_DEBUG_TICK_SET_FLAG(Terminate,DEBUG_TICK_DRAW);
    GFL_DEBUG_TICK_RESET(Terminate);

    GFL_DEBUG_TICK_START(Terminate, TICK_TOTAL, "EventFieldClose TOTAL");

    GFL_DEBUG_TICK_START(Terminate, SEQ_FIELD_FADE, "EventFieldClose SEQ_FIELD_FADE");

    // フェード設定
    if(m_fadeProc){
      if(m_fadeProc( m_fadeWork, gmgr) == false){ break; }

      // ここは1フレーム待つ。
      // (イベントをここで設定する可能性がある。)
      this->AddSeqNo();

      GFL_DEBUG_TICK_END(Terminate, SEQ_FIELD_FADE);
      GFL_DEBUG_TICK_START(Terminate, SEQ_FIELD_FADE_WAIT, "EventFieldClose SEQ_FIELD_FADE_WAIT");
      return GameSys::GMEVENT_RES_CONTINUE_DIRECT;
    }

    // そもそも設定するものがないときはthrough
    this->AddSeqNo();
    GFL_DEBUG_TICK_END(Terminate, SEQ_FIELD_FADE);
    GFL_DEBUG_TICK_START(Terminate, SEQ_FIELD_FADE_WAIT, "EventFieldClose SEQ_FIELD_FADE_WAIT");
    //break through

  case SEQ_FIELD_FADE_WAIT:
    // フェード待ち
    if(m_fadeIsEndProc){
      if( m_fadeIsEndProc(m_fadeWork, gmgr) == false ){ break; }
    }

    //エンディング用モジュールの開放準備リクエスト(モジュールがない場合は何もしない)
    gmgr->GetFieldmap()->TerminateFieldStaffrollReady();
    this->AddSeqNo();
    GFL_DEBUG_TICK_END(Terminate, SEQ_FIELD_FADE_WAIT);
    GFL_DEBUG_TICK_START(Terminate, SEQ_FIELD_CLOSE, "EventFieldClose SEQ_FIELD_CLOSE");
    //break through

  case SEQ_FIELD_CLOSE:
    //エンディング用モジュールの開放準備待ち（モジュールがない場合はスルー）
    if ( !(gmgr->GetFieldmap()->TerminateFieldStaffrollReadyWait()) )
    {
      break;
    }

    if( m_Desc.isSkyboxKeep)
    {
      gmgr->GetFieldmap()->RequestKeepSkyboxResource();
    }

    // ダングリングチェック機能
#if GFL_HEAP_DEBUG
#if defined(GF_PLATFORM_CTR)
#if PM_DEBUG
    if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::ENABLE_MEMORY_DANGLING_CHECK ) )
    {
      // ヒープを再生成するために必ず完全破棄する。
      m_Desc.closeType = FM_CLOSE_MODE_APP;
    }
#endif // PM_DEBUG
#endif  // defined(GF_PLATFORM_CTR)
#endif  // GFL_HEAP_DEBUG

    gmgr->GetFieldmap()->FieldmapCloseReq( m_Desc.closeType );
    GFL_DEBUG_TICK_END(Terminate, SEQ_FIELD_CLOSE);
    GFL_DEBUG_TICK_START(Terminate, SEQ_FIELD_CLOSE_WAIT, "EventFieldClose SEQ_FIELD_CLOSE_WAIT");
    this->AddSeqNo();
    break;
    
  case SEQ_FIELD_CLOSE_WAIT:
    {
      if( gmgr->GetFieldmap() != NULL )
      {
        break;
      }
      else
      {
        switch( m_Desc.closeType ){
        case FM_CLOSE_MODE_APP:             //アプリジャンプ用
        case FM_CLOSE_MODE_APP_KEEPRES:     //アプリジャンプ用
          
          // FieldProcを破棄する。
          {
            FieldProc * pFieldProc = reinterpret_cast<FieldProc*>(gmgr->GetProcManager()->GetBaseProcess());
            pFieldProc->RequestTerminate();
            TOMOYA_PRINT( "☆FieldProc RequestTerminate\n" );
          }

          this->AddSeqNo(); // RESIDENT_DELETEへ
          GFL_DEBUG_TICK_END(Terminate, SEQ_FIELD_CLOSE_WAIT);
          GFL_DEBUG_TICK_START(Terminate, SEQ_FIELD_RESIDENT_DELETE00, "EventFieldClose SEQ_FIELD_RESIDENT_DELETE00");
          break;

        case FM_CLOSE_MODE_MAPJUMP: //マップジャンプ用
        case FM_CLOSE_MODE_JFOPEN:  //ジョインフェスタオープン
        case FM_CLOSE_MODE_JFCLOSE: //ジョインフェスタクローズ
          return GameSys::GMEVENT_RES_FINISH;
         
        default:
          GFL_ASSERT_STOP(0);
          break;
        }
      }
    }
    // break through
  // マップジャンプはここまで

  // アプリ復帰用処理
  case SEQ_FIELD_RESIDENT_DELETE00:

    TOMOYA_PRINT( "FieldmapControl DELETE\n" );
    //FIELD_CTR_PROFILING_START

    GFL_DEBUG_TICK_START(Terminate, TICK_RESIDENT_DELETE_AREA, "EventFieldClose RESIDENT_DELETE_AREA");
    // Area破棄
    if( m_Desc.closeType != FM_CLOSE_MODE_APP_KEEPRES )
    {
      Area* pArea = gmgr->GetGameData()->GetFieldArea();
      pArea->Terminate();
    }
    GFL_DEBUG_TICK_END(Terminate, TICK_RESIDENT_DELETE_AREA);
    this->AddSeqNo();
    GFL_DEBUG_TICK_END(Terminate, SEQ_FIELD_RESIDENT_DELETE00);
    GFL_DEBUG_TICK_START(Terminate, SEQ_FIELD_RESIDENT_DELETE01, "EventFieldClose SEQ_FIELD_RESIDENT_DELETE01");
    break;

  case SEQ_FIELD_RESIDENT_DELETE01:

    // プロセスの終了待ち。
    if( gmgr->GetProcManager()->IsMainHandle() )
    {
      break;
    }

    GFL_DEBUG_TICK_START(Terminate, TICK_RESIDENT_DELETE_RESIDENT, "EventFieldClose RESIDENT_DELETE_RESIDENT");
    // FieldResident破棄
    {
      FieldResident* pFieldResident = gmgr->GetFieldResident();
      pFieldResident->Terminate();
    }
    GFL_DEBUG_TICK_END(Terminate, TICK_RESIDENT_DELETE_RESIDENT);

    GFL_DEBUG_TICK_START(Terminate, TICK_RESIDENT_DELETE_BGM, "EventFieldClose RESIDENT_DELETE_BGM");
    {
      //BGMのフォーカスモード解除。マップジャンプ時はココではなくEventMapJumpのより適切なタイミングで処理している
      FieldSound* pFieldSound = gmgr->GetGameData()->GetFieldSound();
      pFieldSound->ResetTrainerFocusMode( Sound::BGM_FADE_VERY_FAST );
    }
    GFL_DEBUG_TICK_END(Terminate, TICK_RESIDENT_DELETE_BGM);

    GFL_DEBUG_TICK_END(Terminate, SEQ_FIELD_RESIDENT_DELETE01);

    GFL_DEBUG_TICK_END(Terminate, TICK_TOTAL);

#ifdef DEBUG_ONLY_FOR_takahashi_tomoya
#if DEBUG_TICK_DRAW
    gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
    GFL_DEBUG_TICK_DUMP_ALL(Terminate);
    gfl2::debug::ResetPrintFlag();
#endif
#endif

    // プロファイル終了
    //FIELD_CTR_PROFILING_END

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
void EventFieldmapClose::EndFunc(GameSys::GameManager* gmgr)
{
  //フィールド破棄後にケルディオのしんぴの剣チェック
  PokeTool::form::UpdateKerudhioForm( gmgr->GetGameData() );
}
//--------------------------------------------------------------
/**
 * @brief   フェードコールバック設定
 */
//--------------------------------------------------------------
void EventFieldmapClose::SetFadeCallBack
  (FLD_MAPCTRL_CALLBACK_FUNC fadeProc, FLD_MAPCTRL_CALLBACK_FUNC fadeIsEndProc, void * fadeWork)
{
  m_fadeProc = fadeProc;
  m_fadeIsEndProc = fadeIsEndProc;
  m_fadeWork = fadeWork;
}

void EventFieldmapClose::SetFadeCallBackDefault(void)
{
  m_fadeProc = FldMapCtrlCallback_FadeOutSetupDefault;
  m_fadeIsEndProc = FldMapCtrlCallback_FadeIsEndDefault;
  //m_fadeIsEndProc = NULL;
  m_fadeWork = NULL;
}

void EventFieldmapClose::SetFadeCallBackDefaultUpperOnly(void)
{
  m_fadeProc = FldMapCtrlCallback_FadeOutSetupDefaultUpperOnly;
  m_fadeIsEndProc = FldMapCtrlCallback_FadeIsEndDefault;
  //m_fadeIsEndProc = NULL;
  m_fadeWork = NULL;
}

void EventFieldmapClose::SetFadeCallBackDefaultLowerOnly(void)
{
  m_fadeProc = FldMapCtrlCallback_FadeOutSetupDefaultLowerOnly;
  m_fadeIsEndProc = FldMapCtrlCallback_FadeIsEndLowerOnly;
  //m_fadeIsEndProc = NULL;
  m_fadeWork = NULL;
}

void EventFieldmapClose::SetFadeCallBackCaptureBlackFade(void)
{
  m_fadeProc = FldMapCtrlCallback_FadeOutSetupCaptureBlackFade;
  m_fadeIsEndProc = NULL;
  m_fadeWork = NULL;
}

void EventFieldmapClose::SetFadeCallBackCustom(
  gfl2::Fade::DISP disp, gfl2::Fade::FADE_TYPE type, const gfl2::math::Vector4& start_col, const gfl2::math::Vector4& end_col, u32 sync /*= gfl2::Fade::FADE_DEFAULT_SYNC*/)
{
  m_fadeProc = FldMapCtrlCallback_FadeOutSetupCustom;
  m_fadeIsEndProc = FldMapCtrlCallback_FadeIsEndCustom;
  m_fadeWork = &m_fadeParam;

  m_fadeParam.type = type;
  m_fadeParam.disp = disp;
  m_fadeParam.startCol = start_col;
  m_fadeParam.endCol = end_col;
  m_fadeParam.sync = sync;
}


//----------------------------------------------------------------------------
/**
 *  @brief 破棄タイプ 
 */
//-----------------------------------------------------------------------------
void EventFieldmapClose::SetDesc( const Desc& desc )
{
  m_Desc = desc;
}


GFL_NAMESPACE_END(Field);
