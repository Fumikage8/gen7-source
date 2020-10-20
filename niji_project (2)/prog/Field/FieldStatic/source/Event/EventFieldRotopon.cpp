//=============================================================================
/**
 * @file    EventFieldRotopon.cpp
 * @brief   ロトポン用イベント
 * @author  Hiroyuki Nakamura
 * @date    2016.12.08
 */
//=============================================================================

// module
#include "Field/FieldStatic/include/Event/EventFieldRotopon.h"
// momiji
#include "Field/FieldStatic/include/Event/EventXMenuListener.h"
#include "App/FieldMenu/include/FieldMenuSubWindowProc.h"
#include "FieldScript/include/EventScriptCall.h"
// resource
#include "niji_conv_header/field/script/inc/field_event.inc"


GFL_NAMESPACE_BEGIN(Field)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
EventFieldRotopon::EventFieldRotopon( gfl2::heap::HeapBase * heap )
  : GameSys::GameEvent( heap )
  , m_fieldMenuProc( NULL )
  , m_eventXMenuListener( NULL )
  , m_itemNo( ITEM_DUMMY_DATA )
  , m_mainSeq( 0 )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
EventFieldRotopon::~EventFieldRotopon()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    StartEvent
 * @brief   イベント起動
 * @date    2016.12.08
 *
 * @param   gmgr  ゲームマネージャクラス
 *
 * @return  EventFieldRotoponクラス
 */
//-----------------------------------------------------------------------------
EventFieldRotopon * EventFieldRotopon::StartEvent( GameSys::GameManager * gmgr )
{
  GameSys::GameEventManager * ev_man = gmgr->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, EventFieldRotopon >( ev_man );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetupParam
 * @brief   パラメータ設定
 * @date    2016.12.08
 *
 * @param   proc      Proc
 * @param   listener  リスナー
 * @param   item      アイテム番号
 */
//-----------------------------------------------------------------------------
void EventFieldRotopon::SetupParam( App::FieldMenu::FieldMenuSubWindowProc * proc, EventXMenuListener * listener, u32 item )
{
  m_fieldMenuProc = proc;
  m_eventXMenuListener = listener;
  m_itemNo = item;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @date    2016.12.08
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @retval  true  = 起動してよい
 * @retval  false = 起動しない
   */
//-----------------------------------------------------------------------------
bool EventFieldRotopon::BootChk( GameSys::GameManager * gmgr )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @date    2016.12.08
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void EventFieldRotopon::InitFunc( GameSys::GameManager * gmgr)
{
  m_fieldMenuProc->SetRequest( Field::SubScreen::IN_REQ_ID_START_ROTOPON );
}

//-----------------------------------------------------------------------------
/**
 * @func    MainFunc
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2016.12.08
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @return  イベント制御関数の戻り値
 */
//-----------------------------------------------------------------------------
GameSys::GMEVENT_RESULT EventFieldRotopon::MainFunc( GameSys::GameManager * gmgr )
{
  switch( m_mainSeq )
  {
  case 0:
    if( m_fieldMenuProc->GetRequest() == Field::SubScreen::OUT_REQ_ID_ROTOPON_ITEM_EVENT )
    {
      Field::EventScriptCall::CallScript(
        gmgr,
        SCRID_FLD_EV_ROTOPON,
        NULL,
        NULL,
        m_itemNo );
//      m_fieldMenuProc->Suspend();
      m_mainSeq++;
    }
    break;

  case 1:
//    m_fieldMenuProc->Resume();
    m_fieldMenuProc->SetRequest( Field::SubScreen::IN_REQ_ID_ROTOPON_ITEM_EVENT_RETURN );
    m_mainSeq++;
    break;

  case 2:
    if( m_fieldMenuProc->GetRequest() == Field::SubScreen::OUT_REQ_ID_END_ROTOPON )
    {
//      m_eventXMenuListener->SetXMenuEnd();
      return GameSys::GMEVENT_RES_FINISH;
    }
    break;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2016.12.08
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void EventFieldRotopon::EndFunc( GameSys::GameManager * gmgr )
{
}


GFL_NAMESPACE_END(Field)
