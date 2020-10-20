//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EventGameStart.h
 *  @brief  ゲーム開始イベント
 *  @author tomoya takahashi
 *  @date   2015.04.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldStatic/include/Event/EventGameStart.h"
#include "EventFieldmapControl.h"

#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldStatic/include/Time/EvTime.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include "FieldScript/include/FieldScriptSystem.h"

#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"
#include "Field/FieldRo/source/Debug/FieldDebugEventAutoAllMapJump.h"
#include "Field/FieldRo/source/Debug/FieldDebugEventBattleInstTest.h"

#include "Debug/Launcher/include/LauncherProc.h"
#include "Debug/StartMenu/include/StartLauncher.h"
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/GameManager.h>
#include "GameSys/include/PlayTimeManager.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"

#include "niji_conv_header/field/zone/zone_id.h"
#include "niji_conv_header/field/world/world_id.h"

// Traffic
#include "Field/FieldRo/include/Traffic/FieldTrafficManager.h"


//debug
#include "Savedata/include/EventWorkDef.h"

#include "Savedata/include/QRReaderSave.h"
#include "Savedata/include/JoinFestaDataSave.h"

#include "PokeTool/include/PokeToolForm.h"
#include "Trainer/Trainer/include/TrainerPairData.h"

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "Field/FieldStatic/include/Debug/DebugStartSettingAccessor.h"

#if PM_DEBUG
#include "Field/FieldStatic/include/Debug/FieldDebugHeapLog.h"
#define DEBUG_HEAP_LOG_RECORD_MAX  (100)
#endif



GFL_NAMESPACE_BEGIN(Field);

// ゲーム開始ゾーンID
static const u32 FIRST_ZONEID( Z_F0102EV04_S0101 );

// debug開始で降り立つゾーン
static const u32 DEBUG_START_ZONE( Z_F0102_S0201 );  // 主人公の家の前

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

//-----------------------------------------------------------------------------
/**
 *          EventGameStart
*/
//-----------------------------------------------------------------------------

/**
 *  @brief  ゲーム開始イベント　CALL生成
 */
GameSys::GameEvent* EventGameStart::CallEventGameStart(const Description& desc)
{
  GameSys::GameEvent* p_event = NULL;

  switch( desc.mode )
  {
  case GAMEINIT_MODE_FIRST:              ///< はじめからはじめる。
    GameInit( desc.gmgr);
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventGameStart::EventFirst>( desc.gmgr->GetGameEventManager() );
    break;

  case GAMEINIT_MODE_CONTINUE:           ///< つづきからはじめる。
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventGameStart::EventContinueBase>( desc.gmgr->GetGameEventManager() );
    break;

#if PM_DEBUG

  case GAMEINIT_MODE_DEBUG_DEFAULTNAME:        ///< デバッグ起動(デフォルト名で開始る
    GameInitDebug( desc.gmgr, GameSys::GameData::DEBUG_DATA_TYPE_DEFAULTNAME, true);
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventGameStart::EventDebugDefaultName>( desc.gmgr->GetGameEventManager() );
    break;

  case GAMEINIT_MODE_DEBUG_FIRST:        ///< デバッグデータではじめからはじめる
    GameInitDebug( desc.gmgr, GameSys::GameData::DEBUG_DATA_TYPE_DEFAULTNAME, true);
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventGameStart::EventDebugFirst>( desc.gmgr->GetGameEventManager() );
    break;

  case GAMEINIT_MODE_DEBUG_ALLMAPJUMP:        ///< フィールドを開始し、全マップジャンプを自動で実行する。
    GameInitDebug( desc.gmgr, GameSys::GameData::DEBUG_DATA_TYPE_DEFAULTNAME, true);
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventGameStart::EventDebugAllMapJump>( desc.gmgr->GetGameEventManager() );
    break;

  case GAMEINIT_MODE_DEBUG_BATTLE_INST_TEST:        ///< フィールドを開始し、バトル施設のテストを行なう
    CaptureTutorialDllTest();
    GameInitDebug( desc.gmgr, GameSys::GameData::DEBUG_DATA_TYPE_DEFAULTNAME, true);
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventGameStart::EventDebugBattleInst>( desc.gmgr->GetGameEventManager() );
    break;

  case GAMEINIT_MODE_DEBUG_NETTEST:        ///< デバッグデータではじめからはじめる&通信テスト用の初期データ
    GameInitDebug( desc.gmgr, GameSys::GameData::DEBUG_DATA_TYPE_NETWORK_RAND, true);
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventGameStart::EventDebugFirst>( desc.gmgr->GetGameEventManager() );
    break;
  case GAMEINIT_MODE_DEBUG_FIRST_TOP_PFM_DRESS:        ///< デバッグデータではじめからはじめる&最高負荷の着せ替え
    GameInitDebug(desc.gmgr, GameSys::GameData::DEBUG_DATA_TYPE_TOP_PFM_DRESS, true);
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventGameStart::EventDebugFirst>(desc.gmgr->GetGameEventManager());
    break;
#endif
  }

#if PM_DEBUG
  Debug::DebugHeapLog* pLog = Debug::DebugHeapLog::Create( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG ), DEBUG_HEAP_LOG_RECORD_MAX );

#endif

  return p_event;
}

/**
 *  @brief  ゲーム開始イベント　CHANGE生成
 */
GameSys::GameEvent* EventGameStart::ChangeEventGameStart(const Description& desc)
{
  GameSys::GameEvent* p_event = NULL;

  switch( desc.mode )
  {
  case GAMEINIT_MODE_FIRST:              ///< はじめからはじめる。
    GameInit( desc.gmgr);
    p_event = GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, EventGameStart::EventFirst>( desc.gmgr->GetGameEventManager() );
    break;

  case GAMEINIT_MODE_CONTINUE:           ///< つづきからはじめる。
    p_event = GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, EventGameStart::EventContinueBase>( desc.gmgr->GetGameEventManager() );
    break;

#if PM_DEBUG
  case GAMEINIT_MODE_DEBUG_DEFAULTNAME:        ///< デバッグ起動(デフォルト名で開始る
    GameInitDebug( desc.gmgr, GameSys::GameData::DEBUG_DATA_TYPE_DEFAULTNAME, true);
    p_event = GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, EventGameStart::EventDebugDefaultName>( desc.gmgr->GetGameEventManager() );
    break;

  case GAMEINIT_MODE_DEBUG_FIRST:        ///< デバッグデータではじめからはじめる
    GameInitDebug( desc.gmgr, GameSys::GameData::DEBUG_DATA_TYPE_DEFAULTNAME, true);
    p_event = GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, EventGameStart::EventDebugFirst>( desc.gmgr->GetGameEventManager() );
    break;

  case GAMEINIT_MODE_DEBUG_ALLMAPJUMP:        ///< フィールドを開始し、全マップジャンプを自動で実行する。
    GameInitDebug( desc.gmgr, GameSys::GameData::DEBUG_DATA_TYPE_DEFAULTNAME, true);
    p_event = GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, EventGameStart::EventDebugAllMapJump>( desc.gmgr->GetGameEventManager() );
    break;

  case GAMEINIT_MODE_DEBUG_BATTLE_INST_TEST:        ///< フィールドを開始し、バトル施設のテストを行なう
    CaptureTutorialDllTest();
    GameInitDebug( desc.gmgr, GameSys::GameData::DEBUG_DATA_TYPE_DEFAULTNAME, true);
    p_event = GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, EventGameStart::EventDebugBattleInst>( desc.gmgr->GetGameEventManager() );
    break;
#endif
  }

#if PM_DEBUG
  Debug::DebugHeapLog* pLog = Debug::DebugHeapLog::Create( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG ), DEBUG_HEAP_LOG_RECORD_MAX );
#endif

  return p_event;
}




//--------------------------------------------------------------
/**
 * @brief   ゲームデータ初期化
 */
//--------------------------------------------------------------
void EventGameStart::GameInit( GameSys::GameManager* p_gman )
{
  gfl2::heap::HeapBase* p_devHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

  GameSys::GameData* p_gdata = p_gman->GetGameData();
  //GameSys::GameEventManager* p_eventman = p_gman->GetGameEventManager();

  gfl2::ui::Button * button = p_gman->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD);
  gfl2::ui::Button * button_stick_emu = p_gman->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::CROSSKEY_STICK_EMU);

  // 最初から始めるデータを設定
  p_gdata->SetFirstData( p_devHeap );

  // ボタンモードの設定
  Savedata::ConfigSave * p_config = p_gdata->GetConfig();
  p_config->SetButtonMode( p_config->GetButtonMode(), button, button_stick_emu);

  //ゲーム開始初回のみの時間イベント更新
  Field::EvTime::DeviceUpdateForFirstDay(p_gman->GetGameData());

  // ゲーム開始スクリプト呼び出し(NewGame時に一回のみ実行する)
  {
    gfl2::heap::HeapBase* p_ev_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
    FieldScript::FieldScriptSystem* p_script = p_gman->GetFieldScriptSystem();
    p_script->CallGameInit( p_ev_heap, FieldScript::GAME_INIT_TYPE_INIT );
  }
}


//--------------------------------------------------------------
/**
 * @brief   デバッグ開始データ初期化
 */
//--------------------------------------------------------------
#if PM_DEBUG
void EventGameStart::GameInitDebug( GameSys::GameManager* p_gman, GameSys::GameData::DebugDataType gamedata_debug_type, bool b_ev_debug )
{
  gfl2::heap::HeapBase* p_devHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

  GameSys::GameData* p_gdata = p_gman->GetGameData();
  //GameSys::GameEventManager* p_eventman = p_gman->GetGameEventManager();

  gfl2::ui::Button * button = p_gman->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD);
  gfl2::ui::Button * button_stick_emu = p_gman->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::CROSSKEY_STICK_EMU);

  int sex = PM_FEMALE;
  
  // 設定ファイルによる上書き
  {
    const Field::Debug::StartSettingAccessor* pDebugStartSettingAccessor = p_gman->GetGameData()->GetDebugStartSettingAccessor();
    if( pDebugStartSettingAccessor->HasStartSex() )
    {
      sex = pDebugStartSettingAccessor->GetStartSex();
    }
  }

  // Lボタン押下で反転
  if( button->IsHold( gfl2::ui::BUTTON_L ) )
  {
    sex = ( sex == PM_MALE ) ? PM_FEMALE : PM_MALE;
  }

  // ゲーム起動時にセーブデータが適応されているので、動作モデルをすべてきれいに削除する。
  p_gdata->GetFieldCharaModelManager()->TerminateMoveModelResource();
  p_gdata->GetFieldCharaModelManager()->TerminateMoveModelWork();
  p_gdata->GetFieldCharaModelManager()->TerminateSubobject();
  
  // この中でデバッグ時のポケモンやアイテムをセット
  p_gdata->SetDebugData( gamedata_debug_type, p_devHeap, sex );

  // ボタンモードの設定
  Savedata::ConfigSave * p_config = p_gdata->GetConfig();
  p_config->SetButtonMode( p_config->GetButtonMode(), button, button_stick_emu);

  //ゲーム開始初回のみの時間イベント更新
  Field::EvTime::DeviceUpdateForFirstDay(p_gman->GetGameData());

  // ゲーム開始スクリプト呼び出し(NewGame時に一回のみ実行する)
  {
    gfl2::heap::HeapBase* p_ev_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
    FieldScript::FieldScriptSystem* p_script = p_gman->GetFieldScriptSystem();
    p_script->CallGameInit( p_ev_heap, FieldScript::GAME_INIT_TYPE_DEBUG );
  }

#if 0
  //ゲーム開始初回のみの時間イベント更新
  Field::EvTime::UpdateForFirstDay(p_gdata);
#endif
}
#endif

#if PM_DEBUG
//捕獲チュートリアルDLLサイズテスト
void EventGameStart::CaptureTutorialDllTest(void)
{
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager *pRoManager = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::heap::HeapBase * pDllLoad = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD );


  struct DllData
  {
    bool isLow; // メモリの後方確保を行なうか
    char* dllName;
  };

  // Roモジュール
  static const int RO_MODULE_NUM = 5;
  const DllData RO_MODULES[RO_MODULE_NUM] =
  {
    {false,"CaptureTutorial.cro"},
    {false,"Battle.cro"},
    {false,"Object.cro"},//Object,Background,SeqEditorはmomijiで統合されました。
    //{false,"SeqEditor.cro"},
    //{false,"Background.cro"},
    {true,"Bag.cro"},
    {true,"WazaOshie.cro"},
  };
  gfl2::ro::Module* pModules[RO_MODULE_NUM];


  for( u32 i = 0; i < RO_MODULE_NUM; ++i)
  {
    if( RO_MODULES[i].isLow )
    {
      pModules[i] = pRoManager->LoadModuleLower( pGameManager->GetAsyncFileManager(), RO_MODULES[i].dllName, pDllLoad);
    }else{
      pModules[i] = pRoManager->LoadModule( pGameManager->GetAsyncFileManager(), RO_MODULES[i].dllName, pDllLoad);
    }
    pRoManager->StartModule( pModules[i],false);
    GFL_RELEASE_PRINT( "load %s\n", RO_MODULES[i].dllName );
    pDllLoad->Dump();
  }
  GFL_RELEASE_PRINT( "CaptureTutorial StartModule end\n" );
  GFL_RELEASE_PRINT( "write heap free size  when had loaded dll free[0x%x] allocatable[0x%x]\n", pDllLoad->GetTotalFreeSize(), pDllLoad->GetTotalAllocatableSize() );
  pDllLoad->Dump();

  for( u32 i = 0; i < RO_MODULE_NUM; ++i)
  {
    pRoManager->DisposeModule( pModules[i] );
  }
  GFL_RELEASE_PRINT( "CaptureTutorial DisposeModule end\n" );
}
#endif





/************************************************************************/
/*                                                                      */
/************************************************************************/
//-----------------------------------------------------------------------------
/**
 *          EventFirstBase
*/
//-----------------------------------------------------------------------------
EventGameStart::EventFirstBase::EventFirstBase( gfl2::heap::HeapBase* heap ) : 
  GameSys::GameEvent(heap)
{

}

EventGameStart::EventFirstBase::~EventFirstBase()
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
bool EventGameStart::EventFirstBase::BootChk(GameSys::GameManager* gmgr)
{
  // 1つ前のProcが終了するのを待つ。(Laucher対応用)
  if( gmgr->GetProcManager()->IsMainHandle() )
  {
    return false;
  }

  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventGameStart::EventFirstBase::InitFunc(GameSys::GameManager* gmgr)
{
  m_pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );
  this->initStartLocation(gmgr);
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventGameStart::EventFirstBase::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_SYSTEM_INITIALIZE,
    SEQ_SYSTEM_INITIALIZE_WAIT,
    SEQ_SYSTEM_INITIALIZE_UPDATE,
    SEQ_FIELD_OPEN,
    SEQ_FIELD_OPEN_WAIT,
  };

  switch(this->GetSeqNo())
  {
  case SEQ_SYSTEM_INITIALIZE: ///< システム初期化
    {
      FieldResident * pResident = GFL_NEW(m_pHeap) FieldResident();
      gmgr->SetFieldResident( pResident );
      pResident->InitializeFieldDLL( gmgr->GetAsyncFileManager() );

      gmgr->GetFieldScriptSystem()->MsgDataNameList_Initialize();
      gmgr->GetGameData()->GetFieldSound()->Initialize(gmgr,m_pHeap);
      gmgr->GetGameData()->GetWarpDataTools()->Initialize(gmgr,m_pHeap);
      gmgr->GetGameData()->GetTrainerPairData()->Initialize(gmgr,m_pHeap);

      // ゲーム開始時に必ず実行する処理
      {
        // 往来
        Field::TrafficManager * pTrafficManager = GFL_NEW(m_pHeap) Field::TrafficManager();

        pTrafficManager->Initialize( m_pHeap, gmgr->GetGameData()->GetFieldCharaModelManager() );
        gmgr->GetGameData()->SetFieldTrafficManager( pTrafficManager );
      }

      this->AddSeqNo();
    }
    return GameSys::GMEVENT_RES_CONTINUE_DIRECT;

  case SEQ_SYSTEM_INITIALIZE_WAIT:
    {
      if( !gmgr->GetGameData()->GetFieldSound()->InitializeWait( gmgr ) ||
        !gmgr->GetFieldScriptSystem()->MsgDataNameList_Update()  ||
        !gmgr->GetGameData()->GetWarpDataTools()->InitializeWait( gmgr )  ||
        !gmgr->GetGameData()->GetTrainerPairData()->InitializeWait( gmgr )){
        break;
      }
      UpdateMapData::Desc desc;
      desc.p_gman = gmgr;
      desc.bUpdateEvTime = false;
      desc.location = m_StartLocation;
      desc.mode = UpdateMapData::MODE_MAPJUMP;
      desc.pc_ride_control = UpdateMapData::PC_RIDECONTROL_CLEAR; // @fix GFNMCat[702]
      m_UpdateMapData.Setup(desc);
      this->AddSeqNo();
    }
    return GameSys::GMEVENT_RES_CONTINUE_DIRECT;

  case SEQ_SYSTEM_INITIALIZE_UPDATE:
    {
      if( m_UpdateMapData.Update() == false )
      {
        break;
      } 

      this->AddSeqNo();
    }
    return GameSys::GMEVENT_RES_CONTINUE_DIRECT;

  case SEQ_FIELD_OPEN:           ///< フィールド開始
    {
      FieldmapDesc desc;
      desc.openMode       = FM_OPEN_MODE_INIT;
      desc.playerLocation = m_StartLocation;  ///< 出現先指定
     
      //現在地点をフィールドサウンドモジュールに通知
      gmgr->GetGameData()->GetFieldSound()->SetCurrentZoneID( m_StartLocation.zoneId );
      //BGMの初回呼び出し
      gmgr->GetGameData()->GetFieldSound()->StartFirstFieldBGM();

      EventFieldmapOpen* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen>( gmgr->GetGameEventManager() );
      p_event->SetDesc(desc);
      this->SetFadeinModeFunc(p_event);

      this->AddSeqNo();
    }
    break;
  case SEQ_FIELD_OPEN_WAIT:      ///< フィールド開始まち
    //プレイタイムのカウント開始
    gmgr->GetPlayTimeManager()->Start();
    NetLib::NijiNetworkSystem::StartBeaconSystem();//ビーコンシステム起動
    GFL_RELEASE_PRINT( "!!!!FieldmapOpen Success!!!!\n" );
#if PM_DEBUG
    {
      // デバッグ用DLLLOADヒープの残量表示
      gfl2::heap::HeapBase * pDllLoad = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DLL_LOAD );
      GFL_RELEASE_PRINT( "write heap free size  when had initialized field. free[0x%x] allocatable[0x%x]\n", pDllLoad->GetTotalFreeSize(), pDllLoad->GetTotalAllocatableSize() );
    }
#endif
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
void EventGameStart::EventFirstBase::EndFunc(GameSys::GameManager* gmgr)
{

}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 */
//--------------------------------------------------------------
void EventGameStart::EventFirstBase::SetFadeinModeFunc(EventFieldmapOpen* p_event)
{

}

/************************************************************************/
/*                                                                      */
/************************************************************************/
//-----------------------------------------------------------------------------
/**
 *          EventFirst
*/
//-----------------------------------------------------------------------------
EventGameStart::EventFirst::EventFirst( gfl2::heap::HeapBase* heap ) : 
  EventFirstBase(heap)
{
}
EventGameStart::EventFirst::~EventFirst()
{
}

/**
 *  @brief  開始ロケーションの設定
 */
void EventGameStart::EventFirst::initStartLocation(GameSys::GameManager* gmgr)
{
  m_StartLocation.zoneId  = FIRST_ZONEID;

  // ゲーム開始ロケーション作成
  {
    m_StartLocation.worldId = gmgr->GetGameData()->GetFieldZoneDataLoader()->GetZoneWorldId( m_StartLocation.zoneId );
    m_StartLocation.position = gfl2::math::Vector3(0.0f,0.0f,0.0f);
    m_StartLocation.quaternion = gfl2::math::Quaternion(0,0,0,1);

    ZoneDataAccessor zoneDataAccessor( gmgr->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( m_StartLocation.zoneId ) );
    zoneDataAccessor.GetStartPosition( &m_StartLocation.position );
  }
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 */
//--------------------------------------------------------------
void EventGameStart::EventFirst::SetFadeinModeFunc(EventFieldmapOpen* p_event)
{
  p_event->SetFadeCallBack(NULL,NULL,NULL);
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
#if PM_DEBUG
//-----------------------------------------------------------------------------
/**
 *          EventDebugFirst
*/
//-----------------------------------------------------------------------------

b32 EventGameStart::EventDebugFirst::m_IsSetStartInfo( false );
s32 EventGameStart::EventDebugFirst::m_StartZone( 0 );
gfl2::math::Vector3 EventGameStart::EventDebugFirst::m_StartPositon( 0.0f, 0.0f, 0.0f );
gfl2::math::Quaternion EventGameStart::EventDebugFirst::m_StartQuaternion( 0.0f, 0.0f, 0.0f, 1.0f );



EventGameStart::EventDebugFirst::EventDebugFirst( gfl2::heap::HeapBase* heap ) : 
  EventFirstBase(heap)
{
}
EventGameStart::EventDebugFirst::~EventDebugFirst()
{
}

/**
 *  @brief  開始ロケーションの設定
 */
void EventGameStart::EventDebugFirst::initStartLocation(GameSys::GameManager* gmgr)
{
  // 外部指定されたゾーンを使う
  if( m_IsSetStartInfo )
  {
    m_StartLocation.zoneId = m_StartZone;
    m_StartLocation.worldId = gmgr->GetGameData()->GetFieldZoneDataLoader()->GetZoneWorldId( m_StartLocation.zoneId );
    m_StartLocation.position = m_StartPositon;
    m_StartLocation.quaternion = m_StartQuaternion;
  }
  else
  // ゲーム開始ロケーション作成
  {
    const Field::Debug::StartSettingAccessor* pDebugStartSettingAccessor = gmgr->GetGameData()->GetDebugStartSettingAccessor();
    
    m_StartLocation.zoneId  = DEBUG_START_ZONE;

    // 設定ファイルによる上書き
    if( pDebugStartSettingAccessor->HasStartZone() )
    {
      m_StartLocation.zoneId = pDebugStartSettingAccessor->GetStartZone();
    }

    m_StartLocation.worldId = gmgr->GetGameData()->GetFieldZoneDataLoader()->GetZoneWorldId( m_StartLocation.zoneId );
    m_StartLocation.position = gfl2::math::Vector3(0.0f,0.0f,0.0f);
    m_StartLocation.quaternion = gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f);

    ZoneDataAccessor zoneDataAccessor( gmgr->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( m_StartLocation.zoneId ) );
    zoneDataAccessor.GetStartPosition( &m_StartLocation.position );
    
    // 設定ファイルによる上書き
    if( pDebugStartSettingAccessor->HasStartPosition() )
    {
      pDebugStartSettingAccessor->GetStartPosition( &m_StartLocation.position );
    }
  }
}

void EventGameStart::EventDebugFirst::SetStartInfo( s32 zoneId, const gfl2::math::Vector3& rPosition, const gfl2::math::Quaternion& rQuaternion )
{
  m_IsSetStartInfo = true;
  m_StartZone = zoneId;
  m_StartPositon = rPosition;
  m_StartQuaternion = rQuaternion;
}


//-----------------------------------------------------------------------------
/**
 *          EventDebugDefaultName
*/
//-----------------------------------------------------------------------------
EventGameStart::EventDebugDefaultName::EventDebugDefaultName( gfl2::heap::HeapBase* heap ) : 
  EventFirstBase(heap)
{
}
EventGameStart::EventDebugDefaultName::~EventDebugDefaultName()
{
}

/**
 *  @brief  開始ロケーションの設定
 */
void EventGameStart::EventDebugDefaultName::initStartLocation(GameSys::GameManager* gmgr)
{
  m_StartLocation.zoneId  = DEBUG_START_ZONE;

  // ゲーム開始ロケーション作成
  {
    m_StartLocation.worldId = gmgr->GetGameData()->GetFieldZoneDataLoader()->GetZoneWorldId( m_StartLocation.zoneId );
    m_StartLocation.position = gfl2::math::Vector3(0.0f,0.0f,0.0f);
    m_StartLocation.quaternion = gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f);

    ZoneDataAccessor zoneDataAccessor( gmgr->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( m_StartLocation.zoneId ) );
    zoneDataAccessor.GetStartPosition( &m_StartLocation.position );
  }
}


//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventGameStart::EventDebugDefaultName::EndFunc(GameSys::GameManager* gmgr)
{
  GFL_PRINT( "EventGameStart DebugDefaultName Success\n" );
}



//-----------------------------------------------------------------------------
/**
 *          EventDebugAllMapJump
*/
//-----------------------------------------------------------------------------
EventGameStart::EventDebugAllMapJump::EventDebugAllMapJump( gfl2::heap::HeapBase* heap ) : 
  EventFirstBase(heap)
{
}
EventGameStart::EventDebugAllMapJump::~EventDebugAllMapJump()
{
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventGameStart::EventDebugAllMapJump::EndFunc(GameSys::GameManager* gmgr)
{
  DebugEventAutoAllMapJump * p_event = GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, DebugEventAutoAllMapJump>( gmgr->GetGameEventManager() );
  DebugEventAutoAllMapJump::InitializeDescription initDesc;
  p_event->SetDesc( initDesc );
}


/**
 *  @brief  開始ロケーションの設定
 */
void EventGameStart::EventDebugAllMapJump::initStartLocation(GameSys::GameManager* gmgr)
{
  m_StartLocation.zoneId  = DEBUG_START_ZONE;

  // ゲーム開始ロケーション作成
  {
    m_StartLocation.worldId = gmgr->GetGameData()->GetFieldZoneDataLoader()->GetZoneWorldId( m_StartLocation.zoneId );
    m_StartLocation.position = gfl2::math::Vector3(0.0f,0.0f,0.0f);
    m_StartLocation.quaternion= gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f);

    ZoneDataAccessor zoneDataAccessor( gmgr->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( m_StartLocation.zoneId ) );
    zoneDataAccessor.GetStartPosition( &m_StartLocation.position );
  }
}

//-----------------------------------------------------------------------------
/**
 *          EventDebugAllMapJump
*/
//-----------------------------------------------------------------------------
EventGameStart::EventDebugBattleInst::EventDebugBattleInst( gfl2::heap::HeapBase* heap ) : 
  EventFirstBase(heap)
{
}
EventGameStart::EventDebugBattleInst::~EventDebugBattleInst()
{
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventGameStart::EventDebugBattleInst::EndFunc(GameSys::GameManager* gmgr)
{
  DebugEventBattleInstTest * p_event = GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, DebugEventBattleInstTest>( gmgr->GetGameEventManager() );
  DebugEventBattleInstTest::InitializeDescription initDesc;
  p_event->SetDesc( initDesc );
}


/**
 *  @brief  開始ロケーションの設定
 */
void EventGameStart::EventDebugBattleInst::initStartLocation(GameSys::GameManager* gmgr)
{
  m_StartLocation.zoneId  = DEBUG_START_ZONE;

  // ゲーム開始ロケーション作成
  {
    m_StartLocation.worldId = gmgr->GetGameData()->GetFieldZoneDataLoader()->GetZoneWorldId( m_StartLocation.zoneId );
    m_StartLocation.position = gfl2::math::Vector3(0.0f,0.0f,0.0f);
    m_StartLocation.quaternion= gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f);

    ZoneDataAccessor zoneDataAccessor( gmgr->GetGameData()->GetFieldZoneDataLoader()->GetZoneData( m_StartLocation.zoneId ) );
    zoneDataAccessor.GetStartPosition( &m_StartLocation.position );
  }
}


#endif


//--------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//--------------------------------------------------------------
EventGameStart::EventContinueBase::EventContinueBase( gfl2::heap::HeapBase* heap ) : 
  GameSys::GameEvent(heap)
{
}

//--------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//--------------------------------------------------------------
EventGameStart::EventContinueBase::~EventContinueBase()
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
bool EventGameStart::EventContinueBase::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}


//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventGameStart::EventContinueBase::InitFunc(GameSys::GameManager* gmgr)
{
  m_pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );
  this->initStartLocation(gmgr);

  EventWork* p_ev_work = gmgr->GetGameData()->GetEventWork();

  // RTCの変更チェック
  {
    Savedata::GameTime* p_time = gmgr->GetGameData()->GetGameTime();
    if( !p_time->IsIdentifyRTCOffset() ){
      // 変わっている。
      // ペナルティモードON
      p_time->SetPenaltyTime();
      
      //各種ペナルティ処理の実行
      {
        // 再戦施設 案内状・挑戦状フラグリスト
        // 連番である保障はないとして処理しておく
        /*
        static u16 dueling_works_[] =
        {
          WK_SYS_DUELING_ANNAI,
          WK_SYS_DUELING_ANNAI_SILVER,
          WK_SYS_DUELING_ANNAI_GOLD,
          WK_SYS_DUELING_TYOUSEN,
          WK_SYS_DUELING_TYOUSEN_BLUE,
          WK_SYS_DUELING_TYOUSEN_RED,
          WK_SYS_DUELING_TYOUSEN_BK
        };
        */

        //タイムフラグ逆ペナルティ版のクリア
        p_ev_work->ClearTimeFlagsPenaltyInverse(); //ペナルティに関係なく0時またぎでもクリアされる

        /*
        //再戦施設 案内状・挑戦状のクリア
        for ( int i = 0; i < GFL_NELEMS(dueling_works_); ++i )
        {
          p_ev_work->ClearEventWorks( dueling_works_[i], dueling_works_[i] ); // コードを安全に利用するため範囲1ずつ消していく
        }
        */

        //シェイミのフォルムをランドフォルムに戻す
        PokeTool::form::ChangeSheimiFormNormal( gmgr->GetGameData()->GetPlayerParty() );

        /*  QRReader: FieldScan  */
        {
          gmgr->GetGameData()->GetQRReaderSave()->OnModifiedRTCOffset();    /*  RTCOffset変更通知  */
        }
        
        /// ペナルティリセット.
        Field::EvTime::PenaltyTimeSet( gmgr->GetGameData() );

        // ジョインフェスタペナルティ処理
        gmgr->GetGameData()->GetJoinFestaData()->PenaltyTimeUpdate();
        GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->PenaltyTimeUpdate();

      }      
      TOMOYA_PRINT( "ペナルティON\n" );
    }
    p_time->UpdateRTCOffet(); // RTCオフセット更新

  }
}


//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventGameStart::EventContinueBase::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_SYSTEM_INITIALIZE,
    SEQ_SYSTEM_INITIALIZE_WAIT,
    SEQ_SYSTEM_INITIALIZE_UPDATE,
    SEQ_FIELD_OPEN,
    SEQ_FIELD_OPEN_WAIT,
  };

  switch(this->GetSeqNo())
  {
  case SEQ_SYSTEM_INITIALIZE: ///< システム初期化
    {

      FieldResident * pResident = GFL_NEW(m_pHeap) FieldResident();
      gmgr->SetFieldResident( pResident );
      pResident->InitializeFieldDLL( gmgr->GetAsyncFileManager() );

      gmgr->GetFieldScriptSystem()->MsgDataNameList_Initialize();
      gmgr->GetGameData()->GetFieldSound()->Initialize(gmgr,m_pHeap);
      gmgr->GetGameData()->GetWarpDataTools()->Initialize(gmgr,m_pHeap);
      gmgr->GetGameData()->GetTrainerPairData()->Initialize(gmgr,m_pHeap);


      // ゲーム開始時に必ず実行する処理
      {
        // 往来
        Field::TrafficManager * pTrafficManager = GFL_NEW(m_pHeap) Field::TrafficManager();

        pTrafficManager->Initialize( m_pHeap, gmgr->GetGameData()->GetFieldCharaModelManager() );
        gmgr->GetGameData()->SetFieldTrafficManager( pTrafficManager );
      }

      this->AddSeqNo();
    }
    return GameSys::GMEVENT_RES_CONTINUE_DIRECT;

  case SEQ_SYSTEM_INITIALIZE_WAIT:
    {
      if( !gmgr->GetGameData()->GetFieldSound()->InitializeWait( gmgr ) ||
        !gmgr->GetFieldScriptSystem()->MsgDataNameList_Update()  ||
        !gmgr->GetGameData()->GetWarpDataTools()->InitializeWait( gmgr )  ||
        !gmgr->GetGameData()->GetTrainerPairData()->InitializeWait( gmgr )){
        break;
      }
      UpdateMapData::Desc desc;
      desc.p_gman = gmgr;
      desc.bUpdateEvTime = false;
      desc.location = m_StartLocation;
      desc.mode = m_UpdateMapDataMode;
      desc.pc_ride_control = UpdateMapData::PC_RIDECONTROL_CONTINUE; // @fix GFNMCat[702]
      m_UpdateMapData.Setup(desc);
      this->AddSeqNo();
    }
    return GameSys::GMEVENT_RES_CONTINUE_DIRECT;

  case SEQ_SYSTEM_INITIALIZE_UPDATE:
    {
      if( m_UpdateMapData.Update() == false )
      {
        break;
      } 

      this->AddSeqNo();
    }
    return GameSys::GMEVENT_RES_CONTINUE_DIRECT;

  case SEQ_FIELD_OPEN:           ///< フィールド開始
    {
      FieldmapDesc desc;
      if( m_UpdateMapDataMode == UpdateMapData::MODE_CONTINUE_ZONECHANGE ) {
        //ZoneChangeしていた場合はInitモードでOpen
        desc.openMode = FM_OPEN_MODE_INIT;
      }else{
        desc.openMode = FM_OPEN_MODE_APP_RECOVER;
      }
      desc.playerLocation = m_StartLocation;  ///< 出現先指定
     
      //現在地点をフィールドサウンドモジュールに通知
      gmgr->GetGameData()->GetFieldSound()->SetCurrentZoneID( m_StartLocation.zoneId );
      //BGMの初回呼び出し
      gmgr->GetGameData()->GetFieldSound()->StartFirstFieldBGM();

      EventFieldmapOpen* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen>( gmgr->GetGameEventManager() );
      p_event->SetDesc(desc);
      this->SetFadeinModeFunc(p_event);

      this->AddSeqNo();
    }
    break;
  case SEQ_FIELD_OPEN_WAIT:      ///< フィールド開始まち
    //プレイタイムのカウント開始
    gmgr->GetPlayTimeManager()->Start();
    NetLib::NijiNetworkSystem::StartBeaconSystem();//ビーコンシステム起動
    GFL_PRINT( "!!!!FieldmapOpen Success!!!!\n" );
#if PM_DEBUG
    {
      // デバッグ用DLLLOADヒープの残量表示
      gfl2::heap::HeapBase * pDllLoad = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DLL_LOAD );
      GFL_RELEASE_PRINT( "write heap free size  when had initialized field free[0x%x] allocatable[0x%x]\n", pDllLoad->GetTotalFreeSize(), pDllLoad->GetTotalAllocatableSize() );
    }
#endif
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
void EventGameStart::EventContinueBase::EndFunc(GameSys::GameManager* gmgr)
{
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventGameStart::EventContinueBase::SetFadeinModeFunc(EventFieldmapOpen* p_event)
{
}


/**
 *  @brief  復帰ロケーションの設定
 */
void EventGameStart::EventContinueBase::initStartLocation(GameSys::GameManager* gmgr) 
{
  GameSys::GameData* p_gdata = gmgr->GetGameData();
  Location::SaveData loc_svdata;

  if( p_gdata->GetSituation()->HasWarpContinueRequest() ){
    //SpecialLocationへの復帰
    p_gdata->GetSpecialLocation()->Get(&loc_svdata);
    m_UpdateMapDataMode = UpdateMapData::MODE_CONTINUE_ZONECHANGE;
    p_gdata->GetSituation()->SetWarpContinueRequest( false );  //コンティニュー時マップ遷移リクエストを破棄
  }else{
    //通常座標復帰
    m_UpdateMapDataMode = UpdateMapData::MODE_CONTINUE_NO_ZONECHANGE;
    p_gdata->GetStartLocation()->Get(&loc_svdata);
  }
  
  m_StartLocation.Set( loc_svdata );
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
EventGameClose::EventGameClose( gfl2::heap::HeapBase* heap ) : 
  GameSys::GameEvent(heap)
{

}
EventGameClose::~EventGameClose()
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
bool EventGameClose::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventGameClose::InitFunc(GameSys::GameManager* gmgr)
{
  m_pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_RESIDENT_DEVICE );


  // @note ゲーム開始ロケーション作成はゾーンローダーが必要だったのでMainFuncに移動  150505 ikuta_junya
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventGameClose::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_FIELD_CLOSE,
    SEQ_SYSTEM_TERMINATE,
  };

  switch(this->GetSeqNo())
  {
  case SEQ_FIELD_CLOSE:
    {
      EventFieldmapClose* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapClose>( gmgr->GetGameEventManager() );
      EventFieldmapClose::Desc desc;
      desc.closeType = FM_CLOSE_MODE_APP;
      p_event->SetDesc( desc );
      this->AddSeqNo();
    }
    break;

  case SEQ_SYSTEM_TERMINATE:

    // Area破棄
    {
      Area* pArea = gmgr->GetGameData()->GetFieldArea();
      pArea->Terminate();
    }

    {
      FieldResident* pFieldResident = gmgr->GetFieldResident();
      // 一時的にFieldDLL読み込み
      pFieldResident->InitializeFieldDLL( gmgr->GetAsyncFileManager() );

       // TrafficManagerの破棄
      Field::TrafficManager * pTrafficManager = gmgr->GetGameData()->GetFieldTrafficManager();
      GFL_SAFE_DELETE( pTrafficManager );
      gmgr->GetGameData()->SetFieldTrafficManager(NULL);

      // 一時的にFieldDLL破棄
      pFieldResident->TerminateFieldDLL();
    }

    // FieldResident破棄
    {
      FieldResident* pFieldResident = gmgr->GetFieldResident();
      GFL_SAFE_DELETE( pFieldResident );
      gmgr->SetFieldResident(NULL);
    }


    // World破棄
    {
      World* pWorld = gmgr->GetGameData()->GetFieldWorld();
      pWorld->Terminate();
    }
    //視線トレーナーペアデータ破棄
    gmgr->GetGameData()->GetTrainerPairData()->Terminate();
    //WarpData破棄
    gmgr->GetGameData()->GetWarpDataTools()->Terminate();
    //FieldSound破棄
    gmgr->GetGameData()->GetFieldSound()->Terminate();
    //NameListデータ破棄 FNMCat[4999]デバッグ機能ResetField後に、再度ゲームを始めようとすると停止.
    gmgr->GetFieldScriptSystem()->MsgDataNameList_Terminate();

#if PM_DEBUG
    // 常駐ヒープ監視終了
    gmgr->GetGameEventManager()->SetDebugChkHeapData(false);
#endif


#if PM_DEBUG
    // ランチャープロセス呼び出し
    {
      GameSys::GameProcManager* proc_manager = gmgr->GetProcManager();
      debug::launcher::LauncherProc::CallParam LauncherParam;
      debug::launcher::LauncherProc::InitCallParam( &LauncherParam );
      debug::launcher::LauncherProc * launcher = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( proc_manager, &LauncherParam );
      launcher->CreateStartListData<debug::startmenu::StartLauncher>();
    }
#endif // PM_DEBUG

    GFL_PRINT( "===========================================\n" );
    GFL_PRINT( "    安全にリソースのコンバートが可能です。" );
    GFL_PRINT( "    Fieldを選択してリスタート \n" );
    GFL_PRINT( "===========================================\n" );
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
void EventGameClose::EndFunc(GameSys::GameManager* gmgr)
{

}
GFL_NAMESPACE_END(Field);
