//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		 FieldSubscreenLuncher.cpp
 *	@brief   下画面管理マネージャー
 *	@author  k.ohno
 *	@date		 2015.06.05
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field\FieldRo\include\Subscreen\FieldSubscreenLauncher.h"

#include "proc/include/gfl2_ProcManager.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameData.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"


#include "Field\FieldRo\include\Subscreen\FieldSubScreenProcManager.h"
#include "Field\FieldStatic\include\Subscreen\FieldSubScreen.h"

#include "Field\FieldRo\include\Subscreen\FieldSubScreenProc.h"


#include "App\FieldMenu\include\FieldMenuFrame.h"
#include "App\FieldMenu\include\FieldMenuSubWindowProc.h"

#include "App\AppEvent\include\PokeList\PokeListEvent.h"

#include "Field/FieldRo/include/Event/FieldEventPokemonRide.h"


#include "Field/FieldStatic/include/Event/EventXMenu.h"
#include "Field/FieldStatic/include/Event/EventFieldReport.h"
#include "Field/FieldStatic/include/Event/EventFieldQuickMatch.h"
#include "Field/FieldStatic/include/Event/EventAppCall.h"
#include "Field/FieldStatic/include/Event/EventRizaadonRide.h"
#include "Field/FieldStatic/include/Event/EventFieldRotopon.h"
#include "Field/FieldStatic/include/Launcher/FieldApplicationLauncher.h"

#include "Field/FieldStatic/include/Event/EventMapJump.h"
#include <niji_conv_header/field/zone/zone_id.h>

// ジョインフェスタ
#include "NetStatic/NetEvent/include/JoinFestaEventLinkEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaOpenEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaCloseEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaAttractionRetireEvent.h"

// network
#include "NetStatic/NetLib/include/NijiNetworkSystem.h" // NijiNetworkSystem 

// PokeFinder
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/source/EventCheck/FieldEventCheck.h"
#include "Field/FieldRo/include/TrialModel/FieldFinderUtil.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldPokeFinderActor.h"
#include "FieldScript/include/EventScriptCall.h"
#include "niji_conv_header/field/script/inc/finder_event.inc"
#include "NetStatic/NetEvent/include/FatalErrorEvent.h"

// camera
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaFocus.h"


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( SubScreen )


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



GameSys::GameEvent*  eventcall_xmenu(GameSys::GameManager* p_gman, App::FieldMenu::FieldMenuSubWindowProc *pProc, Field::SubScreen::FieldSubScreen* pSub)
{
  EventXMenu* pEvX = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventXMenu>( p_gman->GetGameEventManager() );
  pEvX->SetFieldMenuProc( pProc,  pProc->GetRequest());
  pSub->SetEventXMenu(pEvX);

  return pEvX;
}


/* -------------------------------------------------------------------------*/
/**
 * @brief セーブを呼び出す部分
 */
/* -------------------------------------------------------------------------*/
GameSys::GameEvent*  eventcall_save(GameSys::GameManager* p_gman, App::FieldMenu::FieldMenuSubWindowProc *pProc,EventXMenu* pEvX)
{
  EventFieldReport* pEv = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldReport>( p_gman->GetGameEventManager() );
  pEv->SetInit( pProc,NULL );
  return pEv;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief セーブを呼び出す部分
 */
/* -------------------------------------------------------------------------*/
GameSys::GameEvent*  eventcall_quick(GameSys::GameManager* p_gman, App::FieldMenu::FieldMenuSubWindowProc *pProc,EventXMenu* pEvX)
{
  EventFieldQuickMatch* pEv = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldQuickMatch>( p_gman->GetGameEventManager() );
  pEv->SetInit( pProc,NULL );
  return pEv;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief ポケファインダーを呼び出す部分
 */
/* -------------------------------------------------------------------------*/

GameSys::GameEvent* eventcall_pokefinder(GameSys::GameManager* p_gman)
{
  Field::Fieldmap *pFieldmap = p_gman->GetFieldmap();
  Field::FieldEventCheck *pEventCheck = pFieldmap->GetEventCheck();
  FieldPokeFinderActor *pActor = pEventCheck->GetPokeFinderActor();
  if( !pActor ){ return NULL; } // @fix GFNMCat[1595]Nullアクセス防止
  u32 nSpotID = pActor->GetSpotID();
  u32 state = 0;
#if defined(GF_PLATFORM_CTR)
  gfl2::imagedb::ImageDBAccessor::imgDbResult result;
  gfl2::heap::HeapBase *pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD );
  u32 num = Field::TrialModel::FinderUtil::CheckSDCard( &result, pHeap );
  switch( result )
  {
    // 問題ない
  case imgdb::ResultSuccess:
    state = 0;
    break;

    // エラーだが続行するもの
  case imgdb::ResultErrorInsufficientStorageSpace:
    state = 1;
    break;
  case imgdb::ResultErrorOverNumImages:
    state = 2;
    break;

    // 致命傷
  case imgdb::ResultErrorFileOperationFailure:
    state = 4;
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::FatalError::FatalErrorEvent>( p_gman->GetGameEventManager() );
    break;
  case imgdb::ResultErrorReadOnlyDbFile:
    state = 4;
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::FatalError::FatalErrorEvent>( p_gman->GetGameEventManager() );
    break;
  case imgdb::ResultErrorNotExistStorage:
    state = 4;
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::FatalError::FatalErrorEvent>( p_gman->GetGameEventManager() );
    break;
  case imgdb::ResultErrorWriteProtect:
    state = 4;
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::FatalError::FatalErrorEvent>( p_gman->GetGameEventManager() );
    break;
  case imgdb::ResultErrorNotAvailableStorage:
    state = 4;
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::FatalError::FatalErrorEvent>( p_gman->GetGameEventManager() );
    break;

    // その他SDカードエラー
  default:
    state = 3;
    break;
  }
#endif

  // 4以上は致命傷＝FatalErrorを呼び出す
  GameSys::GameEvent *pEvent = NULL;
  if( state < 4 )
  {
    pEvent = EventScriptCall::CallScript( p_gman, SCRID_FINDER_EVENT, pActor, NULL, nSpotID, state );
  }

  return pEvent;
}


/* -------------------------------------------------------------------------*/
/**
 * @brief ライドを呼び出す部分
 */
/* -------------------------------------------------------------------------*/

GameSys::GameEvent* eventcall_pokeride(GameSys::GameManager* p_gman, Savedata::FieldMenu::RideID rideno, Field::RIDE_POKEMON_ID pokeID)
{
  // ライドできるかチェック
  Field::RIDE_CHECK_RESULT rcr_result = RCR_ERROR_OTHERS;
  u32 script_id = 0;
  if( (pokeID == RIDE_RIZAADON) || (pokeID != Field::EventPokemonRideTool::GetPokemonRideOnID(p_gman)) )
  {
    rcr_result = Field::EventPokemonRideOn::IsPokemonRideOnEnable( p_gman, pokeID );  // 乗れるかチェック
    if(rcr_result != RCR_OK)
    {
      script_id = Field::EventPokemonRideOn::GetRcrScriptId( rcr_result );            // エラーだったら表示するスクリプトID取得
    }
  }
  else
  {
    rcr_result = Field::EventPokemonRideOff::IsPokemonRideOffEnable( p_gman );        // 乗れるかチェック
    if(rcr_result != RCR_OK)
    {
      script_id = Field::EventPokemonRideOff::GetRcrScriptId( rcr_result );           // エラーだったら表示するスクリプトID取得
    }
  }

  if( rcr_result == RCR_OK )
  {
    // Newフラグを落とす
    GameSys::GameData * gd = p_gman->GetGameData();
    gd->GetFieldMenu()->ResetRideNewFlag( rideno );

    //空を飛ぶ
    if( pokeID == RIDE_RIZAADON ){
      Field::EventRizaadonRide* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventRizaadonRide>( p_gman->GetGameEventManager() );
      return pEvent;
    }

    if ( pokeID != Field::EventPokemonRideTool::GetPokemonRideOnID(p_gman) )
    {
      Field::EventPokemonRideOn* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventPokemonRideOn>( p_gman->GetGameEventManager() );
      pEvent->SetPokemonRideOnID( p_gman, pokeID );
      return pEvent;
    }
    else
    {
      Field::EventPokemonRideOff* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::EventPokemonRideOff>( p_gman->GetGameEventManager() );
      return pEvent;
    }
  }
  else
  {
    // ERRORメッセージスクリプトコール
    return EventScriptCall::CallScript( p_gman, script_id, NULL, NULL );
  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief ロトポンを呼び出す部分
 */
/* -------------------------------------------------------------------------*/
GameSys::GameEvent* eventcall_rotopon( GameSys::GameManager* p_gman, App::FieldMenu::FieldMenuSubWindowProc * pProc )
{
  EventFieldRotopon * ev = EventFieldRotopon::StartEvent( p_gman );
  ev->SetupParam( pProc, NULL, pProc->GetFieldMenuFrame()->GetRotoponItem() );
  return ev;
}



/* -------------------------------------------------------------------------*/
/**
 * @brief 下画面関連のイベント起動 特別版
 *
 * @param pFieldSubScreenProcManager フィールドサブプロックマネージャ
 * @param p_gman      ゲームマネージャ
 * @param pStopInterestBG trueの場合、看板注目を切る
 *
 *  @retval true:メニュー用。プレイヤーを停止させ、下画面のSuspend呼ばすイベント起動
 *  @retval false: 通常イベント起動。プレイヤーは動いたまま、下画面のSuspend呼んでイベント起動
 */
/* -------------------------------------------------------------------------*/
int CallEventForceMenuOpen( FieldSubScreenProcManager* pFieldSubScreenProcManager,FieldSubScreen* pSub, GameSys::GameManager* p_gman, b32* pStopInterestBG )
{
  int ret = FIELDSUBSCREEN_NONE;

  // 下画面種類ごとにリクエストを処理する
  SubWindowId subWindowId = pFieldSubScreenProcManager->GetSubWindowId();

  switch( subWindowId ){
  case WIN_MENU_MAIN:        //! アイコンがたくさん並んでいる状態
    {
      App::FieldMenu::FieldMenuSubWindowProc* pProc  = pFieldSubScreenProcManager->GetFieldMenuProc();
      if(pProc){
        App::FieldMenu::FieldMenuFrame *pFrame = pProc->GetFieldMenuFrame();
        pFieldSubScreenProcManager->Resume();
        pFrame->SetRequest( Field::SubScreen::IN_REQ_ID_INPUT_ENABLE_MENU_OPEN );
        eventcall_xmenu(p_gman,pProc,pSub);
        ret = FIELDSUBSCREEN_MENU;
        pFrame->ResetRequest();
      }
    }
    break;
  }
  return ret;
}



/* -------------------------------------------------------------------------*/
/**
 * @brief 下画面関連のイベント起動
 *
 * @param pFieldSubScreenProcManager フィールドサブプロックマネージャ
 * @param p_gman      ゲームマネージャ
 * @param pStopInterestBG trueの場合、看板注目を切る
 *
 *  @return FIELDSUBSCREEN_MENU :メニュー用。プレイヤーを停止させ、下画面のSuspend呼ばすイベント起動
 *  @return FIELDSUBSCREEN_NORMAL   通常イベント起動。プレイヤー停止させ、下画面のSuspend呼んでイベント起動
 *  @retval FIELDSUBSCREEN_NONE   なにもしない
 *  @fix GFBTS[2575]:アプリを呼び出す際に、動作モデルのロックが出来ていなかった問題を修正
 */
/* -------------------------------------------------------------------------*/
int CallEvent( FieldSubScreenProcManager* pFieldSubScreenProcManager,FieldSubScreen* pSub, GameSys::GameManager* p_gman, b32* pStopInterestBG, u32* pFocusStopBit )
{
  int ret = FIELDSUBSCREEN_NORMAL;

  u32 focusStopBit = Camera::Area::Focus::EventStopBit::BGM;

  // 下画面種類ごとにリクエストを処理する
  SubWindowId subWindowId = pFieldSubScreenProcManager->GetSubWindowId();
  FieldMenuOutRequestID requestId = pFieldSubScreenProcManager->GetEventRequestID();
  switch( subWindowId ){
  case WIN_MENU_MAIN:        //! アイコンがたくさん並んでいる状態
    {
      App::FieldMenu::FieldMenuSubWindowProc* pProc  = pFieldSubScreenProcManager->GetFieldMenuProc();
      if(pProc){
        App::FieldMenu::FieldMenuFrame *pFrame = pProc->GetFieldMenuFrame();

        switch( requestId ){
        case Field::SubScreen::OUT_REQ_ID_MENU_OPEN:     //!< メニューを開く ※フィールド処理 
          eventcall_xmenu(p_gman,pProc,pSub);
          ret = FIELDSUBSCREEN_MENU;
          break;
        case Field::SubScreen::OUT_REQ_ID_MENU_CLOSE:    //!< メニューを閉じる ※フィールド処理
          break;
        case Field::SubScreen::OUT_REQ_ID_REPORT:        //!< レポート
          eventcall_save(p_gman, pProc, p_gman->GetSubScreen()->GetXMenu());
          ret = FIELDSUBSCREEN_MENU;
          break;

        case Field::SubScreen::OUT_REQ_ID_START_ROTOPON:  //!< ロトポン開始
          eventcall_rotopon( p_gman, pProc /*, p_gman->GetSubScreen()->GetXMenu()*/ );
          ret = FIELDSUBSCREEN_MENU;
          break;

        case Field::SubScreen::OUT_REQ_ID_EVENTLIST:	//!< イベントリストを選択
        case Field::SubScreen::OUT_REQ_ID_MAIN_EVENT:	//!< メインイベントアイコンを選択　※ボタン未実装
        case Field::SubScreen::OUT_REQ_ID_SUB_EVENT:	//!< サブイベントアイコンを選択　※ボタン未実装
          break;

        case Field::SubScreen::OUT_REQ_ID_TOWNMAP:
        case Field::SubScreen::OUT_REQ_ID_FINDER_STUDIO:
          Field::Application::CallEvent(pProc->GetRequest(),p_gman,pStopInterestBG, pFrame);
          break;
/*        case Field::SubScreen::OUT_REQ_ID_POKELIST:      //!< ポケモンリスト*/
        case Field::SubScreen::OUT_REQ_ID_ZUKAN:         //!< 図鑑
        case Field::SubScreen::OUT_REQ_ID_BAG:           //!< バッグ
/*        case Field::SubScreen::OUT_REQ_ID_TR_CARD:       //!< トレーナーカード*/
/*        case Field::SubScreen::OUT_REQ_ID_CONFIG:        //!< 設定*/
/*        case Field::SubScreen::OUT_REQ_ID_POKE_PARURE:   //!< ポケパルレ*/
          Field::Application::CallEvent(pProc->GetRequest(),p_gman,pStopInterestBG, pFrame);
          break;

        case Field::SubScreen::OUT_REQ_ID_POKE_PLANET:   //!< ポケプラネット
          Field::Application::CallEvent(pProc->GetRequest(),p_gman,pStopInterestBG, pFrame);
          break;

/*        case Field::SubScreen::OUT_REQ_ID_NET_PLAY:      //!< 通信で遊ぶ
          eventcall_quick(p_gman, pProc, p_gman->GetSubScreen()->GetXMenu());
          ret = FIELDSUBSCREEN_MENU;
          break;*/
        case Field::SubScreen::OUT_REQ_ID_JOIN_FESTA:    //!< ジョインフェスタ
          // ジョインフェスタ呼び出しイベント
          NetEvent::JoinFestaOpenEvent::StartEvent( p_gman, true );
          break;
/*        case Field::SubScreen::OUT_REQ_ID_BATTLE_VIDEO:  //!< バトルビデオ
        case Field::SubScreen::OUT_REQ_ID_QR_READ:       //!< QR読み込み
          Field::Application::CallEvent(pProc->GetRequest(),p_gman,pStopInterestBG, pFrame);
          return FIELDSUBSCREEN_MENU;*/
        case Field::SubScreen::OUT_REQ_ID_POKEFINDER:    //!< ポケファインダー
          if( eventcall_pokefinder(p_gman) != NULL )
          {
            ret = FIELDSUBSCREEN_NORMAL; // @fix GFNMCat[3794] 下画面を停止するためNormalに変更
          }
          else
          {
            ret = FIELDSUBSCREEN_NONE;
          }
          break;
        case Field::SubScreen::OUT_REQ_ID_RIDE_OPEN:         //!< ライドポケモンメニュー
          eventcall_xmenu(p_gman,pProc,pSub);
          ret = FIELDSUBSCREEN_MENU;
          break;
/*        case Field::SubScreen::OUT_REQ_ID_RIDE_CLOSE: //!< ライドポケモンメニュークローズ
          { // @note ここは通らないでEventXMenuの処理を通る
            Savedata::FieldMenu::RideID rideno = pFrame->GetRideID();
            if(rideno == Savedata::FieldMenu::RIDE_ID_NONE){
            }
            else{
              eventcall_pokeride(p_gman, rideno, pFrame->GetFieldRideID(rideno));
              ret = true;
            }
          }
          break;*/
        case Field::SubScreen::OUT_REQ_ID_RIDE_POKEMON:  //!< ライドポケモン ※フィールド処理
          {
            focusStopBit = 0;
            Savedata::FieldMenu::RideID rideno = pFrame->GetRideID();
            eventcall_pokeride(p_gman, rideno, pFrame->GetFieldRideID(rideno));
          }
          ret = FIELDSUBSCREEN_NORMAL;
          break;
        }
        pFrame->ResetRequest();
      }
    }
    break;

  case WIN_JOIN_FESTA:       //! ジョインフェスタ
    {
      switch( requestId ){
      case OUT_REQ_ID_JF_END:            //!< ジョインフェスタからフィールドへ戻る
        // ジョインフェスタ終了イベント
        NetEvent::JoinFestaCloseEvent::StartEvent( p_gman );
        break;
      case OUT_REQ_ID_MIRACLE:           //!< ミラクル交換
      case OUT_REQ_ID_GTS:               //!< GTS
      case OUT_REQ_ID_CHANGE_WIFI:       //!< ジョインフェスタで通信をWIFIに切り替える
      case OUT_REQ_ID_CHANGE_LOCAL:      //!< ジョインフェスタで通信をローカルに切り替える
      case OUT_REQ_ID_PLAYER_LIST_SHOW:                  //!< プレイヤーリスト（管理閲覧モード３）
      case OUT_REQ_ID_PROFILE:                           //!< プロフィールアプリ
      case OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_S:       //!< プレイヤーリスト（シングル対戦選択モード１）
      case OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_D:       //!< プレイヤーリスト（ダブル対戦選択モード１）
      case OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_M:       //!< プレイヤーリスト（マルチ対戦選択モード２）
      case OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_R:       //!< プレイヤーリスト（ロイヤル対戦選択モード２）
      case OUT_REQ_ID_PLAYER_LIST_SELECT_TRADE:          //!< プレイヤーリスト（交換選択モード１）
      case OUT_REQ_ID_BEACON_MATCHING:                   //!< ビーコンマッチングアプリ
      case OUT_REQ_ID_OPEN_LIST:                         //!< 募集一覧のリスト（ストリーミングタッチで行く）
      case OUT_REQ_ID_POKELIST:      //!< ポケモンリスト
      case OUT_REQ_ID_BAG:           //!< バッグ
      case OUT_REQ_ID_CONFIG:        //!< 設定
      case OUT_REQ_ID_BATTLE_SPOT:   //!< バトルスポット
      case OUT_REQ_ID_REGULATION_DOWNLOAD: //!< レギュレーションダウンロード
        {
          // ジョインフェスタからイベント起動する際のイベント起動
          NetEvent::JoinFestaEventLinkEvent* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::JoinFestaEventLinkEvent>( p_gman->GetGameEventManager() );
          pEvent->SetRequestID( requestId );
        }
        break;
      case OUT_REQ_ID_ATTRACTION_RETIRE:                 //!< ローカルアトラクションのリタイア選択
        {
          // アトラクションリタイアイベント
          GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, NetEvent::JoinFestaAttractionRetireEvent>( p_gman->GetGameEventManager() );
          
          // ※この後、下画面でメッセージ表示する必要があるので下画面は動いたままにする必要がある 
          ret = FIELDSUBSCREEN_MENU;
        }
        break;
      default:
        GFL_ASSERT_MSG(0,"requestID:%d error \n", requestId );
        break;
      }
    }
    break;

  case WIN_FINDER:           //! ポケファインダー 
    {
      // ポケファインダーからイベント起動したい場合に使用
    }
    break;
  }


  *pFocusStopBit = focusStopBit;
  return ret;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 下画面関連のイベント起動してよいかどうか
 * @return  OKの場合TRUE
 */
/* -------------------------------------------------------------------------*/
bool CheckEvent( FieldSubScreenProcManager* pFieldSubScreenProcManager )
{
  return pFieldSubScreenProcManager->IsEventRequest();
}





GFL_NAMESPACE_END( SubScreen )
GFL_NAMESPACE_END( Field )