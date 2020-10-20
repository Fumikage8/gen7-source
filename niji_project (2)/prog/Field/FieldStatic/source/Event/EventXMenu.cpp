//=================================================================================================
/**
 * @file    EventXMenu.cpp
 * @brief   Ｘボタンを押して
 * @author  obata_toshihiro   k.ohno
 * @date    2011.06.29   2015.6.9
 */
//=================================================================================================

#if 1
#include "Field/FieldStatic/include/Event/EventAppCall.h"

// gflib2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "Field/FieldStatic/include/Event/EventXMenu.h"
#include "Field/FieldStatic/include/Event/EventFieldReport.h"
#include "Field/FieldStatic/include/Event/EventFieldQuickMatch.h"
#include "Field/FieldStatic/include/Event/EventRizaadonRide.h"
#include "Field/FieldStatic/include/Subscreen/FieldSubscreen.h"

#include "App\FieldMenu\include\FieldMenuFrame.h"
#include "App\FieldMenu\include\FieldMenuSubWindowProc.h"
#include "Field/FieldStatic/include/Launcher/FieldApplicationLauncher.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"

#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/GameData.h>

#include <NetStatic/NetEvent/include/QuickMatchEvent.h>

#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalEvent/JoinFestaPersonalEvent.h"

GFL_NAMESPACE_BEGIN( Field )

//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
EventXMenu::EventXMenu( gfl2::heap::HeapBase* heap ) :
GameSys::GameEvent( heap )
{
  m_heap = heap;
}


//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
EventXMenu::~EventXMenu()
{
}




void EventXMenu::SetFieldMenuProc( App::FieldMenu::FieldMenuSubWindowProc* pProc  )
{
  m_pFieldMenuProc = pProc;
}


void EventXMenu::SetFieldMenuProc( App::FieldMenu::FieldMenuSubWindowProc* pProc, Field::SubScreen::FieldMenuOutRequestID reqno  )
{
  m_pFieldMenuProc = pProc;
  m_ReqNo = reqno;
}


/* -------------------------------------------------------------------------*/
/**
 * @brief ライドを呼び出す部分
 */
/* -------------------------------------------------------------------------*/

GameSys::GameEvent* eventcall_pokeride_256(GameSys::GameManager* p_gman, Savedata::FieldMenu::RideID rideno, Field::RIDE_POKEMON_ID pokeID)
{
  // Newフラグを落とす
  GameSys::GameData * gd = p_gman->GetGameData();
  gd->GetFieldMenu()->ResetRideNewFlag( rideno );

  //空を飛ぶ
  if( pokeID == RIDE_RIZAADON ){
    Field::EventRizaadonRide* pEvent = GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, Field::EventRizaadonRide>( p_gman->GetGameEventManager() );
    //pEvent->SetPokemonRideOnID( p_gman, pokeID );
    return pEvent;
  }

  if ( pokeID != Field::EventPokemonRideTool::GetPokemonRideOnID(p_gman) )
  {
    Field::EventPokemonRideOn* pEvent = GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, Field::EventPokemonRideOn>( p_gman->GetGameEventManager() );
    pEvent->SetPokemonRideOnID( p_gman, pokeID );
    return pEvent;
  }
  else
  {
    Field::EventPokemonRideOff* pEvent = GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, Field::EventPokemonRideOff>( p_gman->GetGameEventManager() );
    return pEvent;
  }
}



//--------------------------------------------------------------
// bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
//--------------------------------------------------------------
void EventXMenu::InitFunc( GameSys::GameManager* game_manager )
{
  mReportEnd = false;

  m_mainSeq=0;
  switch(m_ReqNo){
  case Field::SubScreen::OUT_REQ_ID_MENU_OPEN:     //!< メニューを開く ※フィールド処理 
    m_pFieldMenuProc->SetRequest(Field::SubScreen::IN_REQ_ID_MENU_OPEN);
    game_manager->GetFieldmap()->GetEffectManager()->RequestEndPlaceName();   // 地名非表示

    // ジョインフェスタのパーソナルイベント抽選時間更新
    // memo:ジョインフェスタアイコンに「！」を出すために必要な処理なので、Xメニュー起動のタイミングで更新する
    NetAppLib::JoinFesta::JoinFestaPersonalEvent::UpdateLotteryTime();
    break;
  case Field::SubScreen::OUT_REQ_ID_RIDE_OPEN:         //!< ライドポケモンメニュー
    m_pFieldMenuProc->SetRequest( Field::SubScreen::IN_REQ_ID_RIDE_OPEN ); 
    game_manager->GetFieldmap()->GetEffectManager()->RequestEndPlaceName();   // 地名非表示
    break;
  }
}

//--------------------------------------------------------------
// イベントを動かしてよいかどうかの判定関数 純粋仮想関数
//--------------------------------------------------------------
bool EventXMenu::BootChk( GameSys::GameManager* gmgr )
{
  return true;
}


//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventXMenu::MainFunc( GameSys::GameManager* game_manager )
{
  b32 dummy;
  if(mID==Field::SubScreen::OUT_REQ_ID_QUICKMATCH_CALL){

    {
      if(Field::Application::CallEvent(Field::SubScreen::OUT_REQ_ID_QUICKMATCH_CALL, game_manager, &dummy, m_pFieldMenuProc->GetFieldMenuFrame())){
        m_mainSeq++;
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetFieldMenuOpen(App::FieldMenu::FieldMenuFrame::START_TYPE_NORMAL);
        return GameSys::GMEVENT_RES_FINISH;
      }
    }

  }
  if(mReportEnd){
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetFieldMenuOpen(App::FieldMenu::FieldMenuFrame::START_TYPE_NORMAL);
    return GameSys::GMEVENT_RES_FINISH;
  }
  switch(m_mainSeq){
  case 0:
    if(m_pFieldMenuProc->GetRequest() == Field::SubScreen::OUT_REQ_ID_REPORT)
    {
      EventFieldReport* pEv = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldReport>( game_manager->GetGameEventManager() );
      pEv->SetInit( m_pFieldMenuProc, this );
      m_mainSeq++;
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetFieldMenuOpen(App::FieldMenu::FieldMenuFrame::START_TYPE_MENU_OPEN);
      return GameSys::GMEVENT_RES_CONTINUE;
    }
    else if(m_pFieldMenuProc->GetRequest() == Field::SubScreen::OUT_REQ_ID_NET_PLAY)
    {
      EventFieldQuickMatch* pEv = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldQuickMatch>( game_manager->GetGameEventManager() );
      pEv->SetInit( m_pFieldMenuProc, this );
      m_mainSeq++;
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetFieldMenuOpen(App::FieldMenu::FieldMenuFrame::START_TYPE_MENU_OPEN);
      return GameSys::GMEVENT_RES_CONTINUE;
    }
    else if(m_pFieldMenuProc->GetRequest() == Field::SubScreen::OUT_REQ_ID_MENU_CLOSE)
    {
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetFieldMenuOpen(App::FieldMenu::FieldMenuFrame::START_TYPE_NORMAL);
      return GameSys::GMEVENT_RES_FINISH;
    }
    else if(m_pFieldMenuProc->GetRequest() == Field::SubScreen::OUT_REQ_ID_RIDE_CLOSE)
    {
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetFieldMenuOpen(App::FieldMenu::FieldMenuFrame::START_TYPE_NORMAL);
      return GameSys::GMEVENT_RES_FINISH;
    }
    else if(m_pFieldMenuProc->GetRequest() == Field::SubScreen::OUT_REQ_ID_RIDE_DECIDE)
    {
      ////----
      Savedata::FieldMenu::RideID rideno = m_pFieldMenuProc->GetRideID();
      eventcall_pokeride_256( game_manager, rideno, m_pFieldMenuProc->GetFieldRideID(rideno) );
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetFieldMenuOpen(App::FieldMenu::FieldMenuFrame::START_TYPE_NORMAL);
      return GameSys::GMEVENT_RES_FINISH;
    }
    else if(m_pFieldMenuProc->GetRequest() == Field::SubScreen::OUT_REQ_ID_JOIN_FESTA)
    {
      if(Field::Application::CallEvent(m_pFieldMenuProc->GetRequest(), game_manager, &dummy, m_pFieldMenuProc->GetFieldMenuFrame())){
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetFieldMenuOpen(App::FieldMenu::FieldMenuFrame::START_TYPE_NORMAL);
        return GameSys::GMEVENT_RES_FINISH;
      }
    }
    else if(m_pFieldMenuProc->GetRequest() == Field::SubScreen::OUT_REQ_ID_RIDE_INFO )
    {
      if( Field::Application::CallEvent(Field::SubScreen::OUT_REQ_ID_RIDE_INFO,game_manager,&dummy,m_pFieldMenuProc->GetFieldMenuFrame()) )
      {
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetFieldMenuOpen(App::FieldMenu::FieldMenuFrame::START_TYPE_RIDE_OPEN);
//        return GameSys::GMEVENT_RES_FINISH;
        m_mainSeq++;
        return GameSys::GMEVENT_RES_CONTINUE;
      }
    }
    else{
      switch(m_pFieldMenuProc->GetRequest()){
      case Field::SubScreen::OUT_REQ_ID_POKELIST:          //!< ポケモンリストk
      case Field::SubScreen::OUT_REQ_ID_ZUKAN:             //!< 図鑑k
      case Field::SubScreen::OUT_REQ_ID_BAG:               //!< バッグk
      case Field::SubScreen::OUT_REQ_ID_ALBUM:             //!< マイアルバム
      case Field::SubScreen::OUT_REQ_ID_TR_CARD:           //!< トレーナーカードk
      case Field::SubScreen::OUT_REQ_ID_POKE_PLANET:       //!< ポケプラネットk
      case Field::SubScreen::OUT_REQ_ID_POKE_PARURE:       //!< ポケパルレk
      case Field::SubScreen::OUT_REQ_ID_BATTLE_VIDEO:      //!< バトルビデオ
      case Field::SubScreen::OUT_REQ_ID_QR_READ:           //!< QR読み込み
      case Field::SubScreen::OUT_REQ_ID_POKEFINDER:        //!< ポケファインダー
      case Field::SubScreen::OUT_REQ_ID_CONFIG:            //!< 設定
        if(Field::Application::CallEvent(m_pFieldMenuProc->GetRequest(), game_manager, &dummy, m_pFieldMenuProc->GetFieldMenuFrame())){
          m_mainSeq++;
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetFieldMenuOpen(App::FieldMenu::FieldMenuFrame::START_TYPE_MENU_OPEN);
          return GameSys::GMEVENT_RES_CONTINUE;
        }
        break;
      case Field::SubScreen::OUT_REQ_ID_REPORT:            //!< レポート
      case Field::SubScreen::OUT_REQ_ID_RIDE_OPEN:         //!< ライドポケモンのリストを開く
      case Field::SubScreen::OUT_REQ_ID_RIDE_CLOSE:        //!< ライドポケモンのリストを閉じる（キャンセル）
      case Field::SubScreen::OUT_REQ_ID_RIDE_DECIDE:       //!< ライドポケモンのリストで選択された
      case Field::SubScreen::OUT_REQ_ID_RIDE_POKEMON:      //!< ライドポケモン
        if(Field::Application::CallEvent(m_pFieldMenuProc->GetRequest(), game_manager, &dummy, m_pFieldMenuProc->GetFieldMenuFrame())){
          m_mainSeq++;
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->SetFieldMenuOpen(App::FieldMenu::FieldMenuFrame::START_TYPE_MENU_OPEN);
          return GameSys::GMEVENT_RES_CONTINUE;
        }
        break;
      }
    }
    break;
  case 1:
    m_mainSeq = 0;
    m_pFieldMenuProc->Resume();
    if(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldMenuOpen()==App::FieldMenu::FieldMenuFrame::START_TYPE_NORMAL){
      return GameSys::GMEVENT_RES_FINISH;
    }
    return GameSys::GMEVENT_RES_CONTINUE; 
  }
  return GameSys::GMEVENT_RES_CONTINUE;
}


/* -------------------------------------------------------------------------*/
/**
 * @brief アプリケーションの呼び出し
 *
 * @param game_manager  ゲームマネージャ
 */
/* -------------------------------------------------------------------------*/
void EventXMenu::CallAppProc( GameSys::GameManager* game_manager )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  アプリケーションからの戻り値をチェック
 */
//-----------------------------------------------------------------------------
void EventXMenu::RetAppProc( GameSys::GameManager* game_manager )
{
}


//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
void EventXMenu::EndFunc( GameSys::GameManager* game_manager )
{
  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetSubScreen()->SetEventXMenu(NULL);
  OHNO_PRINT("SetEventXMenuを呼びました\n");
}

//--------------------------------------------------------------
/**
 * @brief フィールドからアプリケーションに切り替える際 / アプリケーションからフィールドに戻る際、
 *        フェードアウトを行うかどうかを設定する。
 */
//--------------------------------------------------------------
//void EventXMenu::SetFadeMode(FadeMode mode)
//{
  //m_fadeMode = mode;
//}


/* ------------------------------------------------------------------------- */
/**
 * @brief フィールドをコントロールするか切り替える
 *
 * @param is_control フィールドコントロールフラグ
 */
/* ------------------------------------------------------------------------- */
void EventXMenu::SetFieldControl( bool is_control )
{
  m_field_control = is_control;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief フィールドCloseのタイプ
 *
 * @param fieldCloseType FieldCloseType型
 *
 * @note  この関数を呼び出さない場合のデフォルトはFIELD_CLOSE_RESIDENT_APP_KEEP
 */
/* ------------------------------------------------------------------------- */
//void EventXMenu::SetFieldCloseType( FieldCloseType fieldCloseType )
//{
  //m_fieldCloseType = fieldCloseType;
//}

/* -------------------------------------------------------------------------*/
/**
 * @brief アプリ切り替え時にBGMのプッシュ/ポップを行うか設定する
 *
 * @param is_push trueのときBGMをプッシュ/ポップする
 */
/* -------------------------------------------------------------------------*/
void EventXMenu::SetBgmPushPopFlag(bool is_push )
{
  m_is_bgm_push = is_push;
}


void EventXMenu::SetXMenuEnd(void)
{
  mReportEnd=true;
}


void EventXMenu::SetNextFunction(Field::SubScreen::FieldMenuOutRequestID id)
{
  mID = id;
}



GFL_NAMESPACE_END( Field )
#endif
