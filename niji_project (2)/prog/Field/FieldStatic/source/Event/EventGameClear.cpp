//=============================================================================
/**
 *	@file	  EventGameClear.cpp
 *	@brief  イベント：ゲームクリアの流れ
 *	@author Miyuki Iwasawa	
 *	@date	  2016.01.20
 */
//=============================================================================
#include <str/include/gfl2_StrBuf.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// pml
#include <pml/include/pmlib.h>

#include "Savedata/include/MyStatus.h"
#include "Savedata/include/ConfigSave.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"
#include "System/include/PokemonVersion.h"
#include "Sound/include/Sound.h"

#include "Field/FieldRo/include/Fieldmap.h"

#include "Field/FieldStatic/include/Event/EventProcessCall.h"
#include "Field/FieldStatic/include/Time/EvTime.h"

#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"
#include "Field/FieldStatic/include/Event/EventMapJump.h"
#include "Field/FieldStatic/include/Event/EventGameClear.h"

#include "FieldScript/include/EventScriptCall.h" // for CallScript
#include "FieldScript/include/FieldScriptSystem.h"

#include "App/AppEvent/include/FieldDemo/FieldDemoEvent.h"
#include "App/GameClearSave/include/GameClearSaveProc.h"
#include "App/EndRoll/include/EndRollProc.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/script_event_data.gaix>
#include <arc_index/demo_seq_script.gaix>
#include "PokeTool/include/PokeToolRibbon.h"

#include "niji_conv_header/field/zone/zone_id.h"
#include "niji_reference_files/script/FieldPawnTypes.h"


namespace Field {

//-----------------------------------------------------------------------------
/*
 *					コールバック関数
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/*
 *					実装
*/
//-----------------------------------------------------------------------------

//--------------------------------------------------------------
//--------------------------------------------------------------
EventGameClear * EventGameClear::Create( GameSys::GameManager * p_gman, bool isAlreadyClear )
{
  GameSys::GameEventManager* p_evman = p_gman->GetGameEventManager( );
  gfl2::heap::HeapBase * heap= p_evman->GetHeap();
  EventGameClear::EventParam param;

  // イベントを作成
  EventGameClear* p_event = GFL_NEW( heap ) EventGameClear( heap );

  gfl2::std::MemClear( &param, sizeof(EventGameClear::EventParam));

  param.isAlreadyClear = isAlreadyClear;

  p_event->SetParam( param );
  return p_event;
}

//--------------------------------------------------------------
/*
 *  ゲームクリア時セーブデータ更新 NMCat[1007]ステータス変更のタイミングを整理
 *  
 *  GAME_CLEAR_SAVE_TYPE_DENDOUIRI_1ST：初回殿堂入りタイミングでの更新
 *    初回チャンピオン戦直後の殿堂入りデモコールタイミング
 *    
 *    この後、エンディングまでの各種イベントでパーティメンバの更新などがあるため
 *    殿堂入りメンバーの記録などは、ここで行う
 *   
 *   
 *  GAME_CLEAR_SAVE_TYPE_ENDING：エンディング時の更新
 *    エンディングデモの直前の更新
 *    
 *    チャンピオン戦後に各種シナリオデモを通り抜けるので、
 *    ゲームクリアスクリプトや復帰ポイント記録は、このタイミングで行う
 *    nijiでは初回クリア時の1回しか通らない
 *  
 *    初回の周回タイミングがここ
 *    
 *  
 *  GAME_CLEAR_SAVE_TYPE_DENDOUIRI_BOUEI：2回目以降の殿堂入りでの更新
 *    防衛戦後の殿堂入りデモコールタイミング
 *    
 *    2回目以降の周回タイミングがここ
 */
//--------------------------------------------------------------
void EventGameClear::SetGameClearStatus( GameSys::GameManager * p_gman, gfl2::heap::HeapBase * pTempHeap, GameClearSaveType type  )
{
  FieldScript::FieldScriptSystem* p_script = p_gman->GetFieldScriptSystem();
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::Record * p_record = p_gdata->GetRecordSaveData();
  pml::PokeParty * p_party = p_gdata->GetPlayerParty();
  Field::EventWork* p_evwork = p_gdata->GetEventWork();

  //殿堂入り記録タイミング
  bool isDendouiri = ( type == GAME_CLEAR_SAVE_TYPE_DENDOUIRI_1ST || type == GAME_CLEAR_SAVE_TYPE_DENDOUIRI_BOUEI );

  //ゲーム周回タイミング
  bool isRoundUpdate =  ( type == GAME_CLEAR_SAVE_TYPE_ENDING || type == GAME_CLEAR_SAVE_TYPE_DENDOUIRI_BOUEI );

  //---------------------------------------------------------------
  //ゲーム周回タイミング処理
  if( isRoundUpdate )
  {
    //ゲームクリアスクリプトコール
    p_script->CallGameInit( pTempHeap, FieldScript::GAME_INIT_TYPE_GAMECLEAR );

    //ゲームクリアフラグを立てる
    p_gdata->GetEventWork()->SetEventFlag( SYS_FLAG_GAME_CLEAR );
  }

  //---------------------------------------------------------------
  //殿堂入りタイミングで行う処理
  if( isDendouiri )
  {
    { //クリア時にチャンプリボンを貰える処理(タマゴ以外全部)
      for( int i = 0; i < p_party->GetMemberCount(); ++i )
      {
        pml::pokepara::PokemonParam* pp = p_party->GetMemberPointer( i );
        if( !pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
        {
          pp->SetRibbon( PokeTool::RIBBON_NO_NIJI_CROWN );
        }
      }
    }
  }

  //---------------------------------------------------------------
  //エンディングで行う処理(nijiでは初回の1回のみしかエンディングを通らない)
  if ( type == GAME_CLEAR_SAVE_TYPE_ENDING )
  {
    // 初回殿堂入りの場合のみ呼ばれる処理
    {
      Field::Location location;

      //初回は自宅へ
      location.zoneId  = Z_F0102R0101_01;
      location.position.x = 2317;
      location.position.y = 0; //Yは吸着
      location.position.z = 2154;
      location.worldId = p_gdata->GetFieldZoneDataLoader()->GetZoneWorldId(location.zoneId);

      p_gdata->SetSpecialLocation( &location );
      p_gdata->GetSituation()->SetWarpContinueRequest( true );  //コンティニュー時マップ遷移リクエスト
    }

    //クリアタイムをレコードに登録
    Savedata::PlayTime * p_playtime = p_gdata->GetPlayTime();
    u32 time = p_playtime->GetPlayHour() * 10000
      + p_playtime->GetPlayMinute() * 100
      + p_playtime->GetPlaySecond();
    Savedata::SetRecord( Savedata::Record::RECID_CLEAR_TIME, time );

    //クリアゲーム時間を記録
    Field::EvTime::SetGameClearDeviceDateTime( p_gdata );
  }

  //---------------------------------------------------------------
  //初回殿堂入りタイミングで行う処理
  if ( type == GAME_CLEAR_SAVE_TYPE_DENDOUIRI_1ST )
  {
    //パーティを記録
    p_evwork->SetDendouiriParty(DENDOUIRI_PARTY_TYPE_1ST,p_party);  //初回だけの記録
    p_evwork->SetDendouiriParty(DENDOUIRI_PARTY_TYPE_LAST,p_party);
#if 0
    //手持ちにソルガレオ（ルナアーラ）が含まれていたかで特定イベントの発言が変化するのでフラグ設定
    {
      //対象のポケモン
      u32 trg_mons_no = MONSNO_GEKKOU;//ルナアーラ

      if(System::GetVersion() == VERSION_SUN2){
        trg_mons_no = MONSNO_NIKKOU;//ソルガレオ
      }

      //所持しているか
      for( int i = 0; i < p_party->GetMemberCount(); ++i )
      {
        pml::pokepara::PokemonParam* pp = p_party->GetMemberPointer( i );

        //タマゴでない該当ポケモンならフラグOn
        if( !pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) &&
          pp->GetMonsNo() == trg_mons_no )
        {
          p_evwork->SetEventFlag( FV_DENDOU_DENSETU );
          break;
        }
      }
    }
#endif //実はnijiでも必要なかった部分。momijiでコメントアウト
  }
  //---------------------------------------------------------------
  //殿堂入り/防衛タイミングで行う処理
  if ( type == GAME_CLEAR_SAVE_TYPE_DENDOUIRI_BOUEI )
  {
    //パーティを記録
    p_evwork->SetDendouiriParty(DENDOUIRI_PARTY_TYPE_LAST,p_party);

    //レコード：防衛回数カウントアップ
    Savedata::AddRecord( Savedata::Record::RECID_DENDOU_CNT , 1 );
    u32 dendou_cnt = Savedata::GetRecord( Savedata::Record::RECID_DENDOU_CNT );

    //ポケモンダイアリー：殿堂入り情報（現在の防衛回数）
    PokeDiary::Set( PDID_COUNT_DENDOU, dendou_cnt );
  }
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void EventGameClear::SetParam( const EventGameClear::EventParam & evParam )
{
  mEventParam = evParam;
}

//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
EventGameClear::EventGameClear( gfl2::heap::HeapBase* heap ) : 
  GameSys::GameEvent( heap ),
  mSeq( 0 )
{
}


//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
EventGameClear::~EventGameClear()
{
}



//--------------------------------------------------------------
// イベントを動かしてよいかどうかの判定関数 純粋仮想関数
//--------------------------------------------------------------
bool EventGameClear::BootChk( GameSys::GameManager* gmar )
{
  GFL_UNUSED( gmar );
  return true;
}



//--------------------------------------------------------------
// bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
//--------------------------------------------------------------
void EventGameClear::InitFunc( GameSys::GameManager* gmar )
{
  GFL_UNUSED( gmar );

  mSeqTable = DecideSequenceTable( gmar );
}


//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventGameClear::MainFunc( GameSys::GameManager* p_gman )
{
  gfl2::ui::DeviceManager* pDeviceManager = p_gman->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  u32 key = pButton->GetTrigger() && (gfl2::ui::BUTTON_A|gfl2::ui::BUTTON_B);
  GameSys::GameData * p_gdata = p_gman->GetGameData();

  GameSys::GameProcManager* proc_manager = p_gman->GetProcManager();
  switch( mSeqTable[mSeq] ) {
  case FIELD_CLOSE:
    {
      EventFieldmapClose* event;
      EventFieldmapClose::Desc desc;
 
      //サウンドをフェードアウトしてPause
      Sound::EndBGMReq( Sound::BGM_FADE_SLOW );

      event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapClose > ( p_gman->GetGameEventManager( ) );
      desc.closeType = FM_CLOSE_MODE_APP; // メモリを全破棄
      
      //ScriptでFadeOutしているため、プログラム側は何もしない
      event->SetFadeCallBack( NULL, NULL, NULL );
      event->SetDesc(desc);
      SetNextSeq();
    }
    break;

  case SAVE_START:
    if( !p_gdata->IsSaveLock() )
    {
      app::game_clear_save::APP_PARAM param;
      GameSys::CallGameClearSaveProc( &param );
    }
    SetNextSeq();
    break;

  case SAVE_WAIT:
    if ( p_gman->IsProcessExists() == false )
    {
      SetNextSeq();
    }
    break;

  case SCENE_PART_ENDLOOP_START:
    GameSys::CallEndRollProc( NULL );
    SetNextSeq();
    break;

  case SCENE_PART_ENDLOOP_WAIT:
    if ( p_gman->IsProcessExists() == false )
    {
      SetNextSeq();
    }
    break;

  case END:
    return GameSys::GMEVENT_RES_FINISH;

  case SOFT_RESET:
    {
      System::GflUse::SetSoftResetRequest();
    }
    return GameSys::GMEVENT_RES_FINISH;


  default:
    GFL_ASSERT(0);
    break;
  }
   
  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
void EventGameClear::EndFunc( GameSys::GameManager* gmar )
{
  GFL_UNUSED( gmar );
}


//--------------------------------------------------------------
//  次のシーケンスに進める
//--------------------------------------------------------------
void EventGameClear::SetNextSeq( void )
{
  IWASAWA_PRINT("GameClear Seq(%2d --> %2d)\n", mSeq, mSeq + 1 );
  ++ mSeq;
}

//--------------------------------------------------------------
//  ゲームクリアの流れを決める
//--------------------------------------------------------------
const EventGameClear::Sequence * EventGameClear::DecideSequenceTable( GameSys::GameManager* gmar ) 
{
  static const Sequence FirstClearTable[] =
  { //初回
    FIELD_CLOSE,
    SAVE_START,
    SAVE_WAIT,
    SCENE_PART_ENDLOOP_START,
    SCENE_PART_ENDLOOP_WAIT,
    SOFT_RESET,
  };

#if PM_DEBUG
  static const Sequence DebugClearTable[] =
  {
    FIELD_CLOSE,
    SAVE_START,
    SAVE_WAIT,
    END,
  };
  gfl2::ui::DeviceManager* pDeviceManager = gmar->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if (pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    return DebugClearTable;
  }
#endif
  if ( mEventParam.isAlreadyClear == false )
  {
    return FirstClearTable;
  }
  else
  {
    return FirstClearTable;
  }
}


} // namespace Field 

