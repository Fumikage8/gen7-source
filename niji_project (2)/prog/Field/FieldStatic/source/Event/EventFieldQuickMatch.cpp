//=================================================================================================
/**
 * @file    EventFieldQuickMatch.cpp
 * @brief   フィールドでレポート書くためのイベント
 * @author  k.ohno
 * @date    2015.8.18
 */
//=================================================================================================

#include "Field/FieldStatic/include/Event/EventAppCall.h"

// gflib2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


#include "Field/FieldStatic/include/Event/EventFieldQuickMatch.h"
#include "Field/FieldStatic/include/Event/EventXMenu.h"

#include "App/FieldMenu/include/FieldMenuSubWindowProc.h"
#include "App\FieldMenu\include\FieldMenuFrame.h"
#include "Field/FieldStatic/include/Launcher/FieldApplicationLauncher.h"


GFL_NAMESPACE_BEGIN( Field )


//----------------------------------------------------------------------------
/**
 *	@brief  イベント起動
 *
 *	@param  p_geventman ゲームイベントマネージャ
 *	@param  initwork    初期化情報
 */
//-----------------------------------------------------------------------------
void EventFieldQuickMatch::StartEvent( GameSys::GameManager* p_gameman )
{
  GameSys::GameEventManager * p_geventman = p_gameman->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldQuickMatch>( p_geventman );
}

//--------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------
EventFieldQuickMatch::EventFieldQuickMatch( gfl2::heap::HeapBase* heap ) :
GameSys::GameEvent( heap )
{
  m_heap = heap;
}


//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
EventFieldQuickMatch::~EventFieldQuickMatch()
{
}




void EventFieldQuickMatch::SetInit( App::FieldMenu::FieldMenuSubWindowProc* pProc,EventXMenuListener* pListener  )
{
  m_pFieldMenuProc = pProc;
  mpListener=pListener;
}



//--------------------------------------------------------------
// bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
//--------------------------------------------------------------
void EventFieldQuickMatch::InitFunc( GameSys::GameManager* game_manager )
{
  m_mainSeq=0;
  m_pFieldMenuProc->SetRequest(Field::SubScreen::IN_REQ_ID_QUICKMATCH_OPEN);
}

//--------------------------------------------------------------
// イベントを動かしてよいかどうかの判定関数 純粋仮想関数
//--------------------------------------------------------------
bool EventFieldQuickMatch::BootChk( GameSys::GameManager* gmgr )
{
  return true;
}


//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventFieldQuickMatch::MainFunc( GameSys::GameManager* game_manager )
{
  switch(m_mainSeq){
  case 0:
    if(m_pFieldMenuProc->GetRequest() == Field::SubScreen::OUT_REQ_ID_QUICKMATCH_CLOSE)
    {
      m_mainSeq++;
      if(mpListener){
        mpListener->SetXMenuEnd();
      }
      return GameSys::GMEVENT_RES_FINISH;
    }
    else if(m_pFieldMenuProc->GetRequest() == Field::SubScreen::OUT_REQ_ID_QUICKMATCH_KEEP_MENU)
    {
      m_mainSeq++;
      return GameSys::GMEVENT_RES_FINISH;
    }
    else if(m_pFieldMenuProc->GetRequest() == Field::SubScreen::OUT_REQ_ID_QUICKMATCH_CALL   ){
      if(mpListener){
        mpListener->SetNextFunction(Field::SubScreen::OUT_REQ_ID_QUICKMATCH_CALL);
      }
      else{
        {
          b32 dummy;
          if(Field::Application::CallEvent(Field::SubScreen::OUT_REQ_ID_QUICKMATCH_CALL, game_manager, &dummy, m_pFieldMenuProc->GetFieldMenuFrame())){
          }
        }
      }
      m_mainSeq++;
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
void EventFieldQuickMatch::CallAppProc( GameSys::GameManager* game_manager )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  アプリケーションからの戻り値をチェック
 */
//-----------------------------------------------------------------------------
void EventFieldQuickMatch::RetAppProc( GameSys::GameManager* game_manager )
{
}


//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
void EventFieldQuickMatch::EndFunc( GameSys::GameManager* game_manager )
{
}


/* ------------------------------------------------------------------------- */
/**
 * @brief フィールドをコントロールするか切り替える
 *
 * @param is_control フィールドコントロールフラグ
 */
/* ------------------------------------------------------------------------- */
void EventFieldQuickMatch::SetFieldControl( bool is_control )
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
void EventFieldQuickMatch::SetBgmPushPopFlag(bool is_push )
{
  m_is_bgm_push = is_push;
}




GFL_NAMESPACE_END( Field )

