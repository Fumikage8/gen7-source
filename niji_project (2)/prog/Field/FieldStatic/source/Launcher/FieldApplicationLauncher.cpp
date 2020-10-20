//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		 FieldApplicationLauncher.cpp
 *	@brief   下画面からのアプリ起動   Fieldを抜ける場合staticに配置すべきもの
           FieldSubScreenLauncherと密接な関係がある
 *	@author  k.ohno
 *	@date		 2015.06.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include "proc/include/gfl2_ProcManager.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameData.h"


#include "App\FieldMenu\include\FieldMenuFrame.h"
#include "App\FieldMenu\include\FieldMenuSubWindowProc.h"

#include "App\AppEvent\include\PokeList\PokeListEvent.h"
#include "App\AppEvent\include\Zukan\ZukanEvent.h"
#include "App\AppEvent\include\Config\ConfigEvent.h"
#include "App\AppEvent\include\Bag\BagEvent.h"
#include "App\AppEvent\include\PokeIsland\PokeIslandEvent.h"
#include "App/AppEvent/include/TownMap/TownMapEvent.h"
#include "App/AppEvent/include/PokeParure/PokeParureEvent.h"
#include "App/AppEvent/include/TrainerPass/TrainerPassEvent.h"
#include "App/AppEvent/include/FieldMenu/RidedemoEvent.h"
#include "App/AppEvent/include/FieldMenu/FieldMenuGamedata.h"
#include "App/AppEvent/include/MyAlbum/MyAlbumEvent.h"
#include "App/AppEvent/include/FinderStudio/FinderStudioCaptureEvent.h"

#include "Field/FieldStatic/include/Event/EventXMenu.h"
#include "Field/FieldStatic/include/Event/EventAppCall.h"
#include "Field/FieldStatic/include/Event/EventFieldReport.h"
#include "Field/FieldStatic/include/Event/EventFieldQuickMatch.h"
#include "Field/FieldStatic/include/Launcher/FieldApplicationLauncher.h"

#include "NetStatic/NetEvent/include/MiracleTradeEvent.h"
#include "NetStatic/NetEvent/include/GtsEvent.h"
#include "NetStatic/NetEvent/include/BattleSpotEvent.h"
#include "NetStatic/NetEvent/include/RegulationSelectEvent.h"
#include "NetApp/RegulationSelect/source/RegulationSelectDefine.h"
#include "NetStatic/NetEvent/include/GameSyncEvent.h"
#include "NetStatic/NetEvent/include/BattleVideoPlayerEvent.h"

// ジョインフェスタ
#include "Field/FieldStatic/include/Event/EventMapJump.h"
#include <niji_conv_header/field/zone/zone_id.h>
// ジョインフェスタ
#include "NetStatic/NetEvent/include/JoinFestaEventLinkEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaOpenEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaCloseEvent.h"

// network
#include "NetStatic/NetLib/include/NijiNetworkSystem.h" // NijiNetworkSystem 


#include <NetStatic/NetEvent/include/QuickMatchEvent.h>
// インターネット接続イベント
#include "NetApp/Connection/include/ConnectionAppParam.h"
#include "NetStatic/NetEvent/include/ConnectionEvent.h"

//ジョインフェスタプレイヤーリスト系
#include "NetApp/JoinFestaPlayerListShow/include/JoinFestaPlayerListShowAppParam.h"
#include "NetStatic/NetEvent/include/JoinFestaPlayerListShowEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaProfileEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaGlobalAttractionEvent.h"

//バトルビデオ再生
#include "NetStatic/NetEvent/include/BattleVideoRecordingEvent.h"
#include "NetStatic/NetEvent/include/BattleVideoPlayerEvent.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Application )


//-----------------------------------------------------------------------------
/**
 *					定数宣言
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
 */
//-----------------------------------------------------------------------------




Field::EventAppCall::CallTarget get_call_target( Field::SubScreen::FieldMenuOutRequestID req_id )
{
  struct TargetData
  {
    Field::SubScreen::FieldMenuOutRequestID req_id;
    Field::EventAppCall::CallTarget target;
  };

  // @note 定義されているものだけ。
  static const TargetData tbl[] =
  {
    { Field::SubScreen::OUT_REQ_ID_POKELIST,    Field::EventAppCall::CALL_TARGET_POKELIST },    // ポケモンリスト
    { Field::SubScreen::OUT_REQ_ID_BAG,         Field::EventAppCall::CALL_TARGET_BAG },         // バッグ
    { Field::SubScreen::OUT_REQ_ID_ZUKAN,       Field::EventAppCall::CALL_TARGET_ZUKAN },       // 図鑑
    { Field::SubScreen::OUT_REQ_ID_CONFIG,      Field::EventAppCall::CALL_TARGET_CONFIG },      // 設定画面
    { Field::SubScreen::OUT_REQ_ID_POKEFINDER,  Field::EventAppCall::CALL_TARGET_FINEDER },     // ファインダー
    { Field::SubScreen::OUT_REQ_ID_POKE_PLANET, Field::EventAppCall::CALL_TARGET_ISLAND },      // リゾート
    { Field::SubScreen::OUT_REQ_ID_TR_CARD,     Field::EventAppCall::CALL_TARGET_TRAINERPASS }, // トレーナーパス
    { Field::SubScreen::OUT_REQ_ID_QR_READ,     Field::EventAppCall::CALL_TARGET_QR_READ },     // QR読み込み
    { Field::SubScreen::OUT_REQ_ID_ALBUM,       Field::EventAppCall::CALL_TARGET_ALBUM },       // マイアルバム
  };

  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    if( tbl[i].req_id == req_id )
    {
      return tbl[i].target;
    }
  }

  return Field::EventAppCall::CALL_TARGET_NULL;
}

GameSys::GameEvent* eventcall_app(GameSys::GameManager* p_gman, Field::SubScreen::FieldMenuOutRequestID no, bool bsoundpush)
{
  EventAppCall*  pApp = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventAppCall>( p_gman->GetGameEventManager() );
  pApp->SetAppNo( no );
  pApp->SetCallTargetApplication( get_call_target(no) );
  pApp->SetBgmPushPopFlag(bsoundpush);

//  app::event::PokeListEvent::StartEvent(p_gman);
  return pApp;
}

GameSys::GameEvent* eventnext_app(GameSys::GameManager* p_gman, Field::SubScreen::FieldMenuOutRequestID no, bool bsoundpush)
{
  EventAppCall*  pApp = GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, EventAppCall>( p_gman->GetGameEventManager() );
  pApp->SetAppNo( no );
  pApp->SetCallTargetApplication( get_call_target(no) );
  pApp->SetBgmPushPopFlag(bsoundpush);

//  app::event::PokeListEvent::StartEvent(p_gman);
  return pApp;
}

void fieldmenu_reset_new( GameSys::GameManager* p_gman, Field::SubScreen::FieldMenuOutRequestID no )
{
  if( !( no >= Field::SubScreen::OUT_REQ_ID_POKELIST && no <= Field::SubScreen::OUT_REQ_ID_CONFIG ) )
  {
    return;
  }
  Savedata::FieldMenu * save = p_gman->GetGameData()->GetFieldMenu();
  save->ResetIconNew( static_cast<Savedata::FieldMenu::IconID>(no-Field::SubScreen::OUT_REQ_ID_POKELIST) );
}


/* -------------------------------------------------------------------------*/
/**
 * @brief 下画面関連のイベント起動
 *
 * @param p_menu      フィールドメニュー
 * @param p_gman      ゲームマネージャ
 * @param pStopInterestBG trueの場合、看板注目を切る
 *
 *  @retval 起動したイベント
 *  @retval NULL  特に何もおきない。
 *  @fix GFBTS[2575]:アプリを呼び出す際に、動作モデルのロックが出来ていなかった問題を修正
 */
/* -------------------------------------------------------------------------*/
GameSys::GameEvent* CallEvent(Field::SubScreen::FieldMenuOutRequestID no, GameSys::GameManager* p_gman, b32* pStopInterestBG ,App::FieldMenu::FieldMenuFrame *pFrame)
{
  GameSys::GameEvent*               p_event    = NULL;


  switch(no){
  case Field::SubScreen::OUT_REQ_ID_MENU_OPEN:     //!< メニューを開く
    break;
  case Field::SubScreen::OUT_REQ_ID_MENU_CLOSE:    //!< メニューを閉じる
    break;
  case Field::SubScreen::OUT_REQ_ID_ZUKAN:         //!< 図鑑
    if(pFrame){
      pFrame->SetRequest( Field::SubScreen::IN_REQ_ID_CALL_APP );	// FieldMenuFrameクラス
    }
  case Field::SubScreen::OUT_REQ_ID_POKELIST:      //!< ポケモンリスト
  case Field::SubScreen::OUT_REQ_ID_BAG:           //!< バッグ
  case Field::SubScreen::OUT_REQ_ID_ALBUM:         //!< マイアルバム
  case Field::SubScreen::OUT_REQ_ID_TR_CARD:       //!< トレーナーカード
  case Field::SubScreen::OUT_REQ_ID_POKE_PARURE:   //!< ポケパルレ
  case Field::SubScreen::OUT_REQ_ID_CONFIG:        //!< 設定
  case Field::SubScreen::OUT_REQ_ID_BATTLE_VIDEO:  //!< バトルビデオ
  case Field::SubScreen::OUT_REQ_ID_QR_READ:       //!< QR読み込み
    fieldmenu_reset_new( p_gman, no );
    p_event = eventcall_app(p_gman, no, false);
    break;
  case Field::SubScreen::OUT_REQ_ID_REPORT:        //!< レポート
    Field::EventFieldReport::StartEvent( p_gman );
    break;
  case Field::SubScreen::OUT_REQ_ID_NET_PLAY:      //!< 通信で遊ぶ
    fieldmenu_reset_new( p_gman, no );
    Field::EventFieldQuickMatch::StartEvent( p_gman );
    break;
  case Field::SubScreen::OUT_REQ_ID_QUICKMATCH_CALL:  //通信へ行く
    fieldmenu_reset_new( p_gman, no );
    p_event = eventnext_app(p_gman, no, false);
    break;
  case Field::SubScreen::OUT_REQ_ID_JOIN_FESTA:
    // ジョインフェスタ呼び出しイベント
    fieldmenu_reset_new( p_gman, no );
    p_event = NetEvent::JoinFestaOpenEvent::StartEvent( p_gman, false );
    break;
  case Field::SubScreen::OUT_REQ_ID_POKE_PLANET:   //!< ポケプラネット->リゾート
    fieldmenu_reset_new( p_gman, no );
    p_event = eventcall_app(p_gman,no, false);
    break;

  case Field::SubScreen::OUT_REQ_ID_TOWNMAP:        //!< タウンマップ
  case Field::SubScreen::OUT_REQ_ID_FINDER_STUDIO:  //!< ファインダースタジオ
    if(pFrame){
      pFrame->SetRequest( Field::SubScreen::IN_REQ_ID_CALL_APP );	// FieldMenuFrameクラス
    }
    p_event = eventcall_app(p_gman,no, false);
    break;

  case Field::SubScreen::OUT_REQ_ID_EVENTLIST:	//!< イベントリストを選択
  case Field::SubScreen::OUT_REQ_ID_MAIN_EVENT:	//!< メインイベントアイコンを選択　※ボタン未実装
  case Field::SubScreen::OUT_REQ_ID_SUB_EVENT:	//!< サブイベントアイコンを選択　※ボタン未実装
//    p_event = eventcall_app(p_gman,no, false);
    break;
  case Field::SubScreen::OUT_REQ_ID_POKEFINDER:    //!< ポケファインダー
    break;

//  case Field::SubScreen::OUT_REQ_ID_JOIN_FESTA:
 //   p_event = eventnext_app(p_gman, no, false);
  //  break;

  case Field::SubScreen::OUT_REQ_ID_RIDE_POKEMON:  //!< ライドポケモン
    p_event = eventcall_app(p_gman,no, false);
    break;

  case Field::SubScreen::OUT_REQ_ID_RIDE_INFO:    //!< ライドの説明画面を呼び出す
    p_event = eventcall_app( p_gman, no, false );
/*
    {
      App::Event::FieldDemoEvent * ev = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, App::Event::FieldDemoEvent>( p_gman->GetGameEventManager() );
      ev->SetupParam( 4, true, 0 );
      p_event = ev;
    }
*/
    break;
  }
  return p_event;
}






/* -------------------------------------------------------------------------*/
/**
 * @brief 下画面関連のイベント起動
 *
 * @param p_menu      フィールドメニュー
 * @param p_gman      ゲームマネージャ
 * @param pStopInterestBG trueの場合、看板注目を切る
 *
 *  @retval 起動したイベント
 *  @retval NULL  特に何もおきない。
 *  @fix GFBTS[2575]:アプリを呼び出す際に、動作モデルのロックが出来ていなかった問題を修正
 */
/* -------------------------------------------------------------------------*/
GameSys::GameEvent* CallAppEvent( Field::SubScreen::FieldMenuOutRequestID no, GameSys::GameManager* p_gman, FieldmapData* fieldmap_data )
{
  GameSys::GameEvent*               p_event    = NULL;

  switch(no){
  case Field::SubScreen::OUT_REQ_ID_POKELIST:      //!< ポケモンリスト
    app::event::PokeListEvent::StartEvent( p_gman, app::event::PokeListEvent::FRAME_ID_POKELIST_FIELD );
    break;
  case Field::SubScreen::OUT_REQ_ID_ZUKAN:         //!< 図鑑
    app::event::ZukanEvent::StartEvent( p_gman );
    break;
  case Field::SubScreen::OUT_REQ_ID_BAG:           //!< バッグ
    {
      App::Event::BagEvent * ev = App::Event::BagEvent::StartEvent( p_gman );
      ev->SetupFieldParam( &fieldmap_data->item_use_work, fieldmap_data->weather );
      p_event = ev;
    }
    break;
  case Field::SubScreen::OUT_REQ_ID_ALBUM:         //!< マイアルバム  
    App::Event::MyAlbumEvent::StartEvent( p_gman );
    break;
  case Field::SubScreen::OUT_REQ_ID_TR_CARD:       //!< トレーナーカード
    App::Event::TrainerPassEvent::StartEvent( p_gman );
    break;
  case Field::SubScreen::OUT_REQ_ID_CONFIG:        //!< 設定
    app::event::ConfigEvent::StartEvent( p_gman );
    break;
  case Field::SubScreen::OUT_REQ_ID_REPORT:        //!< レポート
    break;
  case Field::SubScreen::OUT_REQ_ID_POKE_PLANET:   //!< ポケプラネット
    App::Event::PokeIslandEvent::StartEvent( p_gman );
    break;
  case Field::SubScreen::OUT_REQ_ID_POKE_PARURE:   //!< ポケパルレ
    {
      App::Event::PokeParureEvent * ev = App::Event::PokeParureEvent::StartEvent( p_gman );
      App::Kawaigari::KawaigariProcParam param;
      pml::PokeParty * party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
      u32 pos = party->GetMarkingIndex();
      param.bgParam = fieldmap_data->battle_bg_param;
      param.pokemonParam = party->GetMemberPointer( pos );
      param.mode = App::Kawaigari::KAWAIGARI_MODE_NORMAL;
      ev->SetupParam( &param, pos );
    }
    break;
  case Field::SubScreen::OUT_REQ_ID_NET_PLAY:      //!< 通信で遊ぶ
    break;
  case Field::SubScreen::OUT_REQ_ID_QUICKMATCH_CALL:
    NetEvent::QuickMatch::QuickMatchEvent::StartEvent( p_gman );//クイックマッチアプリのイベント呼び出し
    break;
  case Field::SubScreen::OUT_REQ_ID_BATTLE_VIDEO:  //!< バトルビデオ
    {
      //  ローカル用
      NetEvent::BattleVideoPlayer::Event::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager),NetEvent::BattleVideoPlayer::Event::PLAYAER_DATA_TYPE_LOCAL );
    }
    
    break;
  case Field::SubScreen::OUT_REQ_ID_QR_READ:       //!< QR読み込み
    NetEvent::QRReader::QRReaderEvent::StartEventFromField(GFL_SINGLETON_INSTANCE(GameSys::GameManager));
    break;
  case Field::SubScreen::OUT_REQ_ID_TOWNMAP:      //!< タウンマップ
    {
      App::Event::TownMapEvent * ev = App::Event::TownMapEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager) );
      ev->SetParam( App::TownMap::MODE_TOWNMAP );
    }
    break;

  case Field::SubScreen::OUT_REQ_ID_EVENTLIST:	//!< イベントリストを選択
  case Field::SubScreen::OUT_REQ_ID_MAIN_EVENT:	//!< メインイベントアイコンを選択　※ボタン未実装
  case Field::SubScreen::OUT_REQ_ID_SUB_EVENT:	//!< サブイベントアイコンを選択　※ボタン未実装
    break;

  case Field::SubScreen::OUT_REQ_ID_POKEFINDER:    //!< ポケファインダー
    break;

  case Field::SubScreen::OUT_REQ_ID_RIDE_POKEMON:  //!< ライドポケモン
    break;

  case Field::SubScreen::OUT_REQ_ID_RIDE_INFO:    //!< ライドの説明画面を呼び出す
    {
      App::FieldMenu::PlayData * play_data = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenuPlayData();
      App::Event::RideDemoEvent * ev = App::Event::RideDemoEvent::StartEvent( p_gman );
      ev->SetupParam( static_cast<Savedata::FieldMenu::RideID>(play_data->GetSelectRideID()) );
    }
    break;

  case Field::SubScreen::OUT_REQ_ID_FINDER_STUDIO:  //!< ファインダースタジオ呼び出し
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenu()->SetRotomRequest( Savedata::FieldMenu::ROTOM_REQ_EVOLUTION );
    {
      app::event::FinderStudioCaptureEvent * ev = app::event::FinderStudioCaptureEvent::StartEvent( GFL_SINGLETON_INSTANCE(GameSys::GameManager) );
      ev->SetPlayBGM();
    }
    break;

  case Field::SubScreen::OUT_REQ_ID_MIRACLE:        //!< ミラクル交換
    NetEvent::MiracleTrade::MiracleTradeEvent::StartEvent( p_gman );
    break;
  case Field::SubScreen::OUT_REQ_ID_GTS:            //!< GTS
    NetEvent::GTS::GTSEvent::StartEvent( p_gman );
    break;
  case Field::SubScreen::OUT_REQ_ID_CHANGE_WIFI:       //!< ジョインフェスタで通信をWIFIに切り替える
    {
      // インターネット切り替えモード
      NetApp::Connection::APP_PARAM connectionAppParam;
      connectionAppParam.bootMode = NetApp::Connection::BOOT_MODE_CHANGE_INTERNET_MODE;
      NetEvent::Connection::ConnectionEvent::StartEvent( p_gman, connectionAppParam );
    }
    break;
  case Field::SubScreen::OUT_REQ_ID_CHANGE_LOCAL:      //!< ジョインフェスタで通信をローカルに切り替える
    {
      // ローカル接続モード
      NetApp::Connection::APP_PARAM connectionAppParam;
      connectionAppParam.bootMode = NetApp::Connection::BOOT_MODE_CHANGE_LOCAL_MODE;
      NetEvent::Connection::ConnectionEvent::StartEvent( p_gman, connectionAppParam );
    }
    break;
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SHOW:                  //!< プレイヤーリスト（管理閲覧モード３）
    {
      NetEvent::JoinFestaPlayerListShow::JoinFestaPlayerListShowEvent::StartEvent( p_gman, true );
    }
    break;
  case Field::SubScreen::OUT_REQ_ID_PROFILE:                           //!< プロフィールアプリ
    {
      NetEvent::JoinFestaProfile::JoinFestaProfileEvent::StartEvent( p_gman );
    }
    break;
  case Field::SubScreen::OUT_REQ_ID_BATTLE_SPOT:
    {
      NetEvent::BattleSpot::BattleSpotEventParam param;//受け渡すもの特に無し
      NetEvent::BattleSpot::BattleSpotEvent::StartEvent( p_gman , param );
    }
    break;
  case Field::SubScreen::OUT_REQ_ID_REGULATION_DOWNLOAD:
    {
      NetApp::RegulationSelect::APP_PARAM param;
      ::std::memset( &param , 0 , sizeof( param ) );
      param.bootMode = NetApp::RegulationSelect::BOOT_MODE_DOWNLOAD_P2P;
      NetEvent::RegulationSelect::RegulationSelectEvent::StartEvent( p_gman , param );
    }
    break;
  case Field::SubScreen::OUT_REQ_ID_GAME_SYNC:           //!< ゲームシンク
    {
      NetEvent::GameSync::GameSyncEvent::StartEvent( p_gman ); 
    }
    break;
  case Field::SubScreen::OUT_REQ_ID_WIFI_BTLVIDEO:       //!< インターネット版バトルビデオ
    {
      NetEvent::BattleVideoPlayer::Event::StartEvent( p_gman, NetEvent::BattleVideoPlayer::Event::PLAYAER_DATA_TYPE_INTERNET );
    }
    break;
  case Field::SubScreen::OUT_REQ_ID_GLOBAL_ATTRACTION:   //!< グローバルアトラクションアプリ
    {
      NetEvent::JoinFestaGlobalAttraction::JoinFestaGlobalAttractionEvent::StartEvent( p_gman );
    }
    break;
  }
  return p_event;
}


//-----------------------------------------------------------------------------
/**
 * @brief   フィールドからのデータを生成
 *
 * @param   gm    ゲームマネージャ
 * @param   data  生成場所
 */
//-----------------------------------------------------------------------------
void SetupFieldmapData( GameSys::GameManager * gm, FieldmapData * data )
{
  GameSys::GameData * gd = gm->GetGameData();
  Field::Fieldmap * fieldmap = gm->GetFieldmap();

  // バッグ
  Field::FieldItemUse::InitializeWork( fieldmap, data->item_use_work );
  data->weather = fieldmap->GetWeatherControl()->GetNowWeatherKind();

  { // かわいがり
    btl::BgComponentData btl_data;
    btl::GetBgComponentData( fieldmap, gd->GetFieldZoneDataLoader(), fieldmap->GetZoneID(), &btl_data );

    data->battle_bg_param.groundType = btl_data.groudnType;
    data->battle_bg_param.nearType   = btl_data.nearType;
    data->battle_bg_param.farType    = btl_data.farType;
    if( btl_data.useSkyBox != false )
    {
      data->battle_bg_param.skyType = gm->GetSkybox()->GetSkyType();
    }
    else
    {
      data->battle_bg_param.skyType = System::Skybox::Skybox::SKY_TYPE_NONE;
    }
  }
}




GFL_NAMESPACE_END( Application );
GFL_NAMESPACE_END( Field );
