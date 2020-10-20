//=================================================================================================
/**
 * @file    EventFieldReport.cpp
 * @brief   フィールドでレポート書くためのイベント
 * @author  k.ohno
 * @date    2015.8.18
 */
//=================================================================================================

#include "Field/FieldStatic/include/Event/EventAppCall.h"

// gflib2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


#include "Field/FieldStatic/include/Event/EventFieldReport.h"
#include "Field/FieldStatic/include/Event/EventXMenu.h"

#include "App/FieldMenu/include/FieldMenuSubWindowProc.h"
#include "App\FieldMenu\include\FieldMenuFrame.h"
#include "Field/FieldStatic/include/Launcher/FieldApplicationLauncher.h"

// ふれあい
#include "niji_reference_files/script/FieldPokemonFureaiScriptTypes.h"
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

GFL_NAMESPACE_BEGIN( Field )


//----------------------------------------------------------------------------
/**
 *	@brief  イベント起動
 *
 *	@param  p_geventman ゲームイベントマネージャ
 *	@param  initwork    初期化情報
 */
//-----------------------------------------------------------------------------
void EventFieldReport::StartEvent( GameSys::GameManager* p_gameman )
{
  GameSys::GameEventManager * p_geventman = p_gameman->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldReport>( p_geventman );
}

//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
EventFieldReport::EventFieldReport( gfl2::heap::HeapBase* heap ) :
GameSys::GameEvent( heap )
{
  m_heap = heap;
}


//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
EventFieldReport::~EventFieldReport()
{
}




void EventFieldReport::SetInit( App::FieldMenu::FieldMenuSubWindowProc* pProc,EventXMenuListener* pListener  )
{
  m_pFieldMenuProc = pProc;
  mpListener=pListener;
}



//--------------------------------------------------------------
// bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
//--------------------------------------------------------------
void EventFieldReport::InitFunc( GameSys::GameManager* game_manager )
{
  m_mainSeq=0;
  m_pFieldMenuProc->SetRequest(Field::SubScreen::IN_REQ_ID_REPORT_OPEN);
}

//--------------------------------------------------------------
// イベントを動かしてよいかどうかの判定関数 純粋仮想関数
//--------------------------------------------------------------
bool EventFieldReport::BootChk( GameSys::GameManager* gmgr )
{
  return true;
}


//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventFieldReport::MainFunc( GameSys::GameManager* game_manager )
{
  switch(m_mainSeq){
  case 0:
    if(m_pFieldMenuProc->GetRequest() == Field::SubScreen::OUT_REQ_ID_REPORT_CLOSE)
    {
      m_mainSeq++;
      if(mpListener){
        mpListener->SetXMenuEnd();
      }

			// ふれあいリアクション通知
		  {
				Field::FieldResident* pFieldResident = game_manager->GetFieldResident();
			  Field::PlacementDataManager* pPlacementDataManager = pFieldResident->GetPlacementDataManager();
				pPlacementDataManager->RequestEventPlayerActionForFureaiActor(PokemonFureai::FPF_PLAYER_ACITON_NORMAL_Q);
		  }

      return GameSys::GMEVENT_RES_FINISH;
    }
    else if(m_pFieldMenuProc->GetRequest() == Field::SubScreen::OUT_REQ_ID_REPORT_KEEP_MENU)
    {
      m_mainSeq++;
  //    EventXMenu* pEvX = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventXMenu>( game_manager->GetGameEventManager() );
    //  pEvX->SetFieldMenuFrame( m_pFieldMenuFrame,  m_pFieldMenuFrame->GetRequest());

      return GameSys::GMEVENT_RES_FINISH;
    }
    break;
  case 1:
    return GameSys::GMEVENT_RES_FINISH; 
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
void EventFieldReport::CallAppProc( GameSys::GameManager* game_manager )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  アプリケーションからの戻り値をチェック
 */
//-----------------------------------------------------------------------------
void EventFieldReport::RetAppProc( GameSys::GameManager* game_manager )
{
}


//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
void EventFieldReport::EndFunc( GameSys::GameManager* game_manager )
{
}


/* ------------------------------------------------------------------------- */
/**
 * @brief フィールドをコントロールするか切り替える
 *
 * @param is_control フィールドコントロールフラグ
 */
/* ------------------------------------------------------------------------- */
void EventFieldReport::SetFieldControl( bool is_control )
{
  m_field_control = is_control;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief アプリ切り替え時にBGMのプッシュ/ポップを行うか設定する
 *
 * @param is_push trueのときBGMをプッシュ/ポップする
 */
/* -------------------------------------------------------------------------*/
void EventFieldReport::SetBgmPushPopFlag(bool is_push )
{
  m_is_bgm_push = is_push;
}




GFL_NAMESPACE_END( Field )

